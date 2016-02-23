/*
 * Linux ioctl helper functions for driver wrappers
 * Copyright (c) 2002-2010, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef LINUX_IOCTL_H
#define LINUX_IOCTL_H

int linux_set_iface_flags(int sock, const char *ifname, int dev_up);
int linux_iface_up(int sock, const char *ifname);
int linux_get_ifhwaddr(int sock, const char *ifname, u8 *addr);
int linux_set_ifhwaddr(int sock, const char *ifname, const u8 *addr);
int linux_br_add(int sock, const char *brname);
int linux_br_del(int sock, const char *brname);
int linux_br_add_if(int sock, const char *brname, const char *ifname);
int linux_br_del_if(int sock, const char *brname, const char *ifname);
int linux_br_get(char *brname, const char *ifname);

#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
int linux_set_p2p_go_softap(int sock, const char *ifname, int flag);
#endif /* CONFIG_P2P_AUTO_GO_AS_SOFTAP */
#ifdef CONFIG_WOW_SUPPORT
int linux_set_ptk(int sock, const char *ifname, u8 *data, int len);
#endif /* CONFIG_WOW_SUPPORT */
#endif /* LINUX_IOCTL_H */
