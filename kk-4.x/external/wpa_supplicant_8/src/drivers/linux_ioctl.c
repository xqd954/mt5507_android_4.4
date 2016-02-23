/*
 * Linux ioctl helper functions for driver wrappers
 * Copyright (c) 2002-2010, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>

#include "utils/common.h"
#include "linux_ioctl.h"

#include "linux_wext.h"

int linux_set_iface_flags(int sock, const char *ifname, int dev_up)
{
	struct ifreq ifr;
	int ret;

	if (sock < 0)
		return -1;

	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if (ioctl(sock, SIOCGIFFLAGS, &ifr) != 0) {
		ret = errno ? -errno : -999;
		wpa_printf(MSG_ERROR, "Could not read interface %s flags: %s",
			   ifname, strerror(errno));
		return ret;
	}

	if (dev_up) {
		if (ifr.ifr_flags & IFF_UP)
			return 0;
		ifr.ifr_flags |= IFF_UP;
	} else {
		if (!(ifr.ifr_flags & IFF_UP))
			return 0;
		ifr.ifr_flags &= ~IFF_UP;
	}

	if (ioctl(sock, SIOCSIFFLAGS, &ifr) != 0) {
		ret = errno ? -errno : -999;
		wpa_printf(MSG_ERROR, "Could not set interface %s flags (%s): "
			   "%s",
			   ifname, dev_up ? "UP" : "DOWN", strerror(errno));
		return ret;
	}

	return 0;
}


int linux_iface_up(int sock, const char *ifname)
{
	struct ifreq ifr;
	int ret;

	if (sock < 0)
		return -1;

	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if (ioctl(sock, SIOCGIFFLAGS, &ifr) != 0) {
		ret = errno ? -errno : -999;
		wpa_printf(MSG_ERROR, "Could not read interface %s flags: %s",
			   ifname, strerror(errno));
		return ret;
	}

	return !!(ifr.ifr_flags & IFF_UP);
}


int linux_get_ifhwaddr(int sock, const char *ifname, u8 *addr)
{
	struct ifreq ifr;

	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(sock, SIOCGIFHWADDR, &ifr)) {
		wpa_printf(MSG_ERROR, "Could not get interface %s hwaddr: %s",
			   ifname, strerror(errno));
		return -1;
	}

	if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
		wpa_printf(MSG_ERROR, "%s: Invalid HW-addr family 0x%04x",
			   ifname, ifr.ifr_hwaddr.sa_family);
		return -1;
	}
	os_memcpy(addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

	return 0;
}


int linux_set_ifhwaddr(int sock, const char *ifname, const u8 *addr)
{
	struct ifreq ifr;

	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);
	os_memcpy(ifr.ifr_hwaddr.sa_data, addr, ETH_ALEN);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

	if (ioctl(sock, SIOCSIFHWADDR, &ifr)) {
		wpa_printf(MSG_DEBUG, "Could not set interface %s hwaddr: %s",
			   ifname, strerror(errno));
		return -1;
	}

	return 0;
}


#ifndef SIOCBRADDBR
#define SIOCBRADDBR 0x89a0
#endif
#ifndef SIOCBRDELBR
#define SIOCBRDELBR 0x89a1
#endif
#ifndef SIOCBRADDIF
#define SIOCBRADDIF 0x89a2
#endif
#ifndef SIOCBRDELIF
#define SIOCBRDELIF 0x89a3
#endif


int linux_br_add(int sock, const char *brname)
{
	if (ioctl(sock, SIOCBRADDBR, brname) < 0) {
		wpa_printf(MSG_DEBUG, "Could not add bridge %s: %s",
			   brname, strerror(errno));
		return -1;
	}

	return 0;
}


int linux_br_del(int sock, const char *brname)
{
	if (ioctl(sock, SIOCBRDELBR, brname) < 0) {
		wpa_printf(MSG_DEBUG, "Could not remove bridge %s: %s",
			   brname, strerror(errno));
		return -1;
	}

	return 0;
}


int linux_br_add_if(int sock, const char *brname, const char *ifname)
{
	struct ifreq ifr;
	int ifindex;

	ifindex = if_nametoindex(ifname);
	if (ifindex == 0)
		return -1;

	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, brname, IFNAMSIZ);
	ifr.ifr_ifindex = ifindex;
	if (ioctl(sock, SIOCBRADDIF, &ifr) < 0) {
		wpa_printf(MSG_DEBUG, "Could not add interface %s into bridge "
			   "%s: %s", ifname, brname, strerror(errno));
		return -1;
	}

	return 0;
}


int linux_br_del_if(int sock, const char *brname, const char *ifname)
{
	struct ifreq ifr;
	int ifindex;

	ifindex = if_nametoindex(ifname);
	if (ifindex == 0)
		return -1;

	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, brname, IFNAMSIZ);
	ifr.ifr_ifindex = ifindex;
	if (ioctl(sock, SIOCBRDELIF, &ifr) < 0) {
		wpa_printf(MSG_DEBUG, "Could not remove interface %s from "
			   "bridge %s: %s", ifname, brname, strerror(errno));
		return -1;
	}

	return 0;
}


int linux_br_get(char *brname, const char *ifname)
{
	char path[128], brlink[128], *pos;
	os_snprintf(path, sizeof(path), "/sys/class/net/%s/brport/bridge",
		    ifname);
	os_memset(brlink, 0, sizeof(brlink));
	if (readlink(path, brlink, sizeof(brlink) - 1) < 0)
		return -1;
	pos = os_strrchr(brlink, '/');
	if (pos == NULL)
		return -1;
	pos++;
	os_strlcpy(brname, pos, IFNAMSIZ);
	return 0;
}

/******* Mediatek Proprietary Driver Commands ********/
#ifndef SIOCIWFIRSTPRIV
#define SIOCIWFIRSTPRIV	                            0x8BE0
#endif
#define RT_PRIV_IOCTL                               (SIOCIWFIRSTPRIV + 0x01)
#define OID_GET_SET_TOGGLE                          0x8000
#define OID_GO_SOFTAP_ENABLE												0x0873

int linux_driver_mtk_set_oid(int ioctl_sock, const char *ifname, int oid, u8 *data, int len)
{
	struct iwreq iwr;
	int err;


	memset (&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, ifname, os_strlen(ifname));
	iwr.u.data.flags = OID_GET_SET_TOGGLE | oid;
	iwr.u.data.pointer = data;
	iwr.u.data.length = len;

	if ( (err = ioctl(ioctl_sock, RT_PRIV_IOCTL, &iwr)) < 0)
	{
		wpa_printf(MSG_ERROR, "ERR(%d)::  %s in 0x%04x", err, __FUNCTION__, oid);
		return -1;
	}

	return 0;
}

#ifdef CONFIG_WOW_SUPPORT
#define OID_HE_SET_WOW_IP					0x06CA
#define OID_HE_SET_UDP_PORT					0x06CB
#define OID_HE_GET_WOW_STAT					0x06CC
#define OID_HE_SET_TCP_PORT					0x06CD
#define OID_HE_SET_REPLAY_COUNTER			0x06CE
#define OID_HE_SET_PTK						0x06CF
#endif /* CONFIG_WOW_SUPPORT */
#ifdef CONFIG_WOW_SUPPORT
int linux_driver_mtk_get_oid(int ioctl_sock, const char *ifname, int oid, u8 *data, int len)
{
	struct iwreq iwr;
	int err;


	memset (&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, ifname, os_strlen(ifname));
	iwr.u.data.flags = oid;
	iwr.u.data.pointer = data;
	iwr.u.data.length = len;

	if ( (err = ioctl(ioctl_sock, RT_PRIV_IOCTL, &iwr)) < 0)
	{
		wpa_printf(MSG_ERROR, "%s: ERR(%d) (ifname=%s, oid=0x%04x)", __FUNCTION__, err, ifname, oid);
		return -1;
	}
	wpa_printf(MSG_DEBUG, "%s:SUCCESS (ifname=%s,oid=0x%04x)", __FUNCTION__, ifname, oid);
    
	return iwr.u.data.length;
}
int linux_set_ptk(int sock, const char *ifname, u8 *data, int len)
{
    return linux_driver_mtk_set_oid(sock, ifname, OID_HE_SET_PTK, (u8*)data, len);
}

int linux_set_replay_counter(int sock, const char *ifname, u8 *data, int len)
{
	return linux_driver_mtk_set_oid(sock, ifname, OID_HE_SET_REPLAY_COUNTER, (u8*)data, len);
}
int linux_get_wow_stat(int sock, const char *ifname, u8 *data)
{
	linux_driver_mtk_get_oid(sock, ifname, OID_HE_GET_WOW_STAT, (u8*)data, (int)sizeof(data));
	return 0;
}
int linux_set_tcp_port(int sock, const char *ifname, int *data, int len)
{
	return linux_driver_mtk_set_oid(sock, ifname, OID_HE_SET_TCP_PORT, (u8*)data, len * sizeof(int));
}
int linux_set_udp_port(int sock, const char *ifname, int *data, int len)
{
	return linux_driver_mtk_set_oid(sock, ifname, OID_HE_SET_UDP_PORT, (u8*)data, len * sizeof(int));
}
int linux_set_wow_ip(int sock, const char *ifname, int *data, int len)
{
	return linux_driver_mtk_set_oid(sock, ifname, OID_HE_SET_WOW_IP, (u8*)data, len);
}
#endif /* CONFIG_WOW_SUPPORT */
