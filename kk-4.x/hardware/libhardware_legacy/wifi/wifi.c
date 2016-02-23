/*
 * Copyright 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "hardware_legacy/wifi.h"
#include "libwpa_client/wpa_ctrl.h"
#include "wifi_device_id.h"

#define LOG_TAG "WifiHW"
#include "cutils/log.h"
#include "cutils/memory.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include "private/android_filesystem_config.h"
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif
//add for wifi0_check
#include <linux/if.h>
#include <sys/types.h>
#include <sys/socket.h>

/* PRIMARY refers to the connection on the primary interface
 * SECONDARY refers to an optional connection on a p2p interface
 *
 * For concurrency, we only support one active p2p connection and
 * one active STA connection at a time
 */
#define PRIMARY     0
#define SECONDARY   1
#define MAX_CONNS   2
static struct wpa_ctrl *ctrl_conn[MAX_CONNS];
static struct wpa_ctrl *monitor_conn[MAX_CONNS];

static struct wpa_ctrl *wfd_ctrl_conn;
static struct wpa_ctrl *wfd_monitor_conn;
/* socket pair used to exit from a blocking read */
static int exit_sockets[MAX_CONNS][2];
static int wfd_exit_sockets[2];

extern int do_dhcp();
extern int ifc_init();
extern void ifc_close();
extern int ifc_down(const char *name);
extern int ifc_up(const char *name);

extern char *dhcp_lasterror();
extern void get_dhcp_info();
extern int init_module(void *, unsigned long, const char *);
extern int delete_module(const char *, unsigned int);
int wifi_set_dongle_type(int type);
int wifi_get_dongle_type();
int wifi_set_dongle_mode(int mode);
int wifi_get_dongle_mode();
void wifi_close_sockets(int index);

static char primary_iface[PROPERTY_VALUE_MAX];
// TODO: use new ANDROID_SOCKET mechanism, once support for multiple
// sockets is in

#ifndef WIFI_DRIVER_MODULE_ARG
#define WIFI_DRIVER_MODULE_ARG          ""
#endif
#ifndef WIFI_FIRMWARE_LOADER
#define WIFI_FIRMWARE_LOADER		""
#endif
#define WIFI_TEST_INTERFACE		"wlan0"
#define WIFI_TEST_INTERFACE_P2P		"p2p0"
#define WIFI_TEST_INTERFACE_AP		"wlan0"

#ifndef WIFI_DRIVER_FW_PATH_STA
#define WIFI_DRIVER_FW_PATH_STA		"STA"
#endif
#ifndef WIFI_DRIVER_FW_PATH_AP
#define WIFI_DRIVER_FW_PATH_AP		"AP"
#endif
#ifndef WIFI_DRIVER_FW_PATH_P2P
#define WIFI_DRIVER_FW_PATH_P2P		"P2P"
#endif

#ifndef WIFI_DRIVER_FW_PATH_PARAM
#define WIFI_DRIVER_FW_PATH_PARAM	"/sys/module/wlan/parameters/fwpath"
#endif

#define WIFI_DRIVER_LOADER_DELAY	1000000

static const char IFACE_DIR[]           = "/data/misc/wifi/wpa_supplicant";
#define WIFI_DRIVER_MODULE_PATH 1
#ifdef WIFI_DRIVER_MODULE_PATH
static  char DRIVER_MODULE_NAME[40]  = "";
static  char DRIVER_MODULE_TAG[40]   = "";  //todo!!!
static  char DRIVER_MODULE_PATH[40]  = "";
static  char DRIVER_MODULE_ARG[40]   = "";

static const char DRIVER_MODULE_TAG_AP[]   = "rtnet5572ap";  //todo!!!

static const char DRIVER_MODULE_TAG_AP_7601[]   = "mt7601Uap";  
static const char DRIVER_MODULE_TAG_AP_7662[]   = "mt7662u_sta";
static const char DRIVER_MODULE_TAG_STA_7662[]   = "mt7662u_sta";

static const char DRIVER_MODULE_TAG_AP_7603[]   = "mt7603u_sta";
static const char DRIVER_MODULE_TAG_STA_7603[]   = "mt7603u_sta";


#endif
static const char FIRMWARE_LOADER[]     = WIFI_FIRMWARE_LOADER;
static const char DRIVER_PROP_NAME[]    = "wlan.driver.status";
static char SUPPLICANT_NAME[40]     = "";
static char P2P_SUPPLICANT_NAME[40] = "";
static char SUPP_PROP_NAME[40]      = "";
static char P2P_PROP_NAME[40]       = "";
static const char SUPP_CONFIG_TEMPLATE[]= "/system/etc/wifi/wpa_supplicant.conf";
static const char P2P_CONFIG_TEMPLATE[]= "/system/etc/wifi/p2p_supplicant.conf";
static const char SUPP_CONFIG_FILE[]    = "/data/misc/wifi/wpa_supplicant.conf";
static const char P2P_CONFIG_FILE[]     = "/data/misc/wifi/p2p_supplicant.conf";
//static const char CONTROL_IFACE_PATH[]  = "/data/misc/wifi/sockets";
static const char CONTROL_IFACE_PATH[]  = "/data/misc/wifi/wpa_supplicant";
static const char MODULE_FILE[]         = "/proc/modules";
static const char WIFI_MODULE_NAME[]    = "wlan.driver.module";
static const char WIFI_MODULE_MODE[]    = "wlan.driver.mode";

static const char SUPPLICANT_NAME_PROPERTY[]     = "wlan.s_name";
static const char SUPPLICANT_NAME_P2P_PROPERTY[] = "wlan.s_p2p_name";
static const char SUPPLICANT_PROP_NAME_PROPERTY[] = "wlan.s_prop_name";
static const char SUPPLICANT_PROP_P2P_NAME_PROPERTY[] = "wlan.s_p2p_prop_name";
static const char DRIVER_MODULE_PATH_PROPERTY[]   = "wlan.d_m_path";
static const char DRIVER_MODULE_NAME_PROPERTY[]   = "wlan.d_m_name";
static const char DRIVER_MODULE_ARG_PROPERTY[]    = "wlan.d_m_arg";
static const char DRIVER_MODULE_TAG_PROPERTY[]    = "wlan.d_m_tag";
static const char SUPP_ENTROPY_FILE[]   = WIFI_ENTROPY_FILE;
static unsigned char dummy_key[21] = { 0x02, 0x11, 0xbe, 0x33, 0x43, 0x35,
                                       0x68, 0x47, 0x84, 0x99, 0xa9, 0x2b,
                                       0x1c, 0xd3, 0xee, 0xff, 0xf1, 0xe2,
                                       0xf3, 0xf4, 0xf5 };
#define No_Dongle                              -1
#define Ralink_5570                             0
#define REALTEK_CU                              1
#define REALTEK_SU                              2
#define REALTEK_DU							                3
#define ATHEROS_K2                              4
#define ATHEROS_ATH6K							              5
#define BCM_WIFI                                6
#define MTK_7601                                7
#define MTK_7662                                8
#define MTK_7603                                9
#define RTK_8188                                10
#define WIFI_DEV_NUM                            11



#define   STA_MODE                           0
#define   AP_MODE                            1

#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
static const char AP_DAEMON_NAME[]      = "ap_daemon";
static const char AP_PROP_NAME[]        = "init.svc.ap_daemon";
#endif


static char* supp_prefix[] = {"", "rt", "rt", "rt", "k2", "6k", "", "", "","",""};
static char* module_path[]= {"rt5572sta.ko", "8192cu.ko", "8712u.ko","8192du.ko", "ath_usbdrv.ko", "ath6kl_usb.ko",  "wl.ko", "mt7601Usta.ko", "mt7662u_sta.ko","mt7603u_sta.ko","8188eu.ko"};
static char* module_name[]= {"rt5572sta", "8192cu", "8712u","8192du", "ath_usbdrv", "ath6kl_usb",  "wl" , "mt7601Usta", "mt7662u_sta","mt7603u_sta","8188eu"};
static char* module_arg[] = {"", "", "", "", "", "ath6kl_p2p=1 chip_pwd_l=0", "", "", "","ifname=wlan0 if2name=p2p0"};

/* Is either SUPPLICANT_NAME or P2P_SUPPLICANT_NAME */
static char supplicant_name[PROPERTY_VALUE_MAX];
/* Is either SUPP_PROP_NAME or P2P_PROP_NAME */
static char supplicant_prop_name[PROPERTY_VALUE_MAX];
static int is_primary_interface(const char *ifname)
{
    //Treat NULL as primary interface to allow control
    //on STA without an interface
    if (ifname == NULL || !strncmp(ifname, primary_iface, strlen(primary_iface))) {
        return 1;
    }
    return 0;
}

static struct device_node* pcurr_device = NULL;
 //list for keep device_id
static struct device_node* phead = NULL;

struct device_node
{
    struct device_node *  pnext;
    char  device_id[20];
};

//ignore some dir
static int badname(const char *name)
{
    //ALOGD("%s %s",__FUNCTION__,name);
    if (*name == '.' || isalpha(*name)) {
        return 1;
    }
    while (*name) {
        if (':'==(*name++)) {
            return 1;
        }
    }
    return 0;
}

static int find_existing_devices()
{
    char devname[80],detail[8],busdir_path[32],tmp[50];
    DIR *busdir , *devdir ;
    struct dirent *de;
    int num,fd,pid,vid;

    busdir = opendir("/sys/bus/usb/devices");
    snprintf(busdir_path, sizeof busdir_path, "%s","/sys/bus/usb/devices");
    if (busdir == 0)
        return 0;

    while ((de = readdir(busdir)) != 0) {
        //ALOGE("enter %s %d %s/%s\n",__FUNCTION__,__LINE__,busdir_path,de->d_name);
        if (badname(de->d_name))
            continue;
        snprintf(devname, sizeof devname, "%s/%s/%s", busdir_path, de->d_name,"idProduct");
        memset(detail,0,8);

        fd = open(devname, O_RDONLY);
        if (fd < 0){
            ALOGE( " enter %d devname %s \n",__LINE__, strerror(errno));
        }
        read(fd,detail,8);
        sscanf(detail,"%x",&pid);
        close(fd);

        memset(detail,0,8);
        snprintf(devname, sizeof devname, "%s/%s/%s", busdir_path, de->d_name,"idVendor");
        fd = open(devname, O_RDONLY);
        if (fd < 0){
            ALOGE( " enter %d devname %s \n",__LINE__, strerror(errno));
        }
        read(fd,detail,8);
        close(fd);

        sscanf(detail,"%x",&vid);
        
		//sprintf(tmp,"%x/%x/aaa",vid,pid);
		sprintf(tmp,"%x/%x",vid,pid);
		
		wifi_set_dongle_inside(tmp);
		
    } //end of busdir while
    closedir(busdir);


    return 0;
}

int wifi_add_in_list(const char* device_id)
{
    int i ;
    struct device_node *  ptmp = NULL;
    struct device_node *  pthis = NULL;
    //add 0 if the vid is begin with 0 984/5555/111 to 0987/5555/111
    char *p = strstr(device_id , "/");
    char device_tmp[20] = {0};
    if((p - device_id) == 3)
        sprintf(device_tmp , "0%s",device_id);
    else
        sprintf(device_tmp,"%s",device_id);

    if(phead == NULL)
    {
        phead = (struct device_node *)malloc(sizeof(struct device_node));
        if(phead == NULL)
            return -1;
        phead->pnext = NULL;
        strncpy(phead->device_id,device_tmp,sizeof(device_tmp));
        LOGD("wifi_add_in_list head device  %s in--wtf",phead->device_id);
        return 0;
    }
    ptmp = phead->pnext;
    pthis = phead;
    for( i = 0; i < 200 && ptmp != NULL; i++)
    {
        pthis = ptmp;
        ptmp = ptmp->pnext;
    }

    if(ptmp == NULL)
    {
        pthis->pnext = (struct device_node *)malloc(sizeof(struct device_node));
        if(pthis->pnext == NULL)
            return -1;
        ptmp = pthis->pnext;
        ptmp->pnext = NULL;
        strncpy(ptmp->device_id,device_tmp,sizeof(device_tmp));
        LOGD("wifi_add_in_list device  %s in--wtf",ptmp->device_id);
        return 0;
    }

    LOGD("wifi_add_in_list ten device already %s in--wtf",device_tmp);
    return (-1);
}

int wifi_delete_from_list(const char* device_id)
{
    LOGD("wifi_delete_from_list device_id[%s]\n", device_id);
    int i ;
    struct device_node *  ptmp = NULL;
    struct device_node *  pthis = NULL;

    //add 0 if the vid is begin with 0 984/5555/111 to 0987/5555/111
    char *p = strstr(device_id , "/");

    char device_tmp[20] = {0};
    if((p - device_id) == 3)
        sprintf(device_tmp , "0%s",device_id);
    else
        sprintf(device_tmp , "%s",device_id);

    if(phead == NULL)
    {
        LOGD("wifi_delete_from_list no device in--wtf");
        return 0;
    }
    if(strcmp(phead->device_id,device_tmp) == 0)
    {
        LOGD("wifi_delete_from_list  device head  %s in--wtf",phead->device_id);
        ptmp = phead;
        phead = phead->pnext;
        free(ptmp);
        return 0;
    }
    ptmp = phead->pnext;
    pthis = phead;
    for( i = 0; i < 200 && ptmp != NULL; i++)
    {
        if(strcmp(ptmp->device_id,device_tmp) == 0)
        {
            LOGD("wifi_delete_from_list  device  %s in--wtf",ptmp->device_id);
            pthis->pnext = ptmp->pnext;
            free(ptmp);
            return 0;
        }
        pthis = ptmp;
        ptmp = ptmp->pnext;
    }

    LOGD("wifi_delete_from_list delete nothing %sin--wtf",device_tmp);
    return -1;
}

int wifi0_check()
{
	int s = 0;
	int ret = -1;
	struct ifreq ifr;
	int i;
	memset(&ifr, 0, sizeof(struct ifreq));

	LOGD ("wifi0_check \n");

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, WIFI_TEST_INTERFACE,strlen(WIFI_TEST_INTERFACE));

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if(s < 0)
	{
		LOGE("Fail to create socket!");
		return -1;
	}
	ret = ioctl(s, SIOCGIFFLAGS, &ifr);
	close(s);

	if (ret == 0)
	{
		return wifi_get_dongle_mode();
	}
    LOGE("wifi0_check no interface!");
	return -1;
}

int wifi_trans_pid_to_module(const char *info)
{
    LOGD("wifi_trans_pid_to_module %s",info);
    int i;
    char * pid_temp = NULL;

    for( i = 0; strcmp(mt7662usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(mt7662usb_device_table[i],info,strlen(mt7662usb_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module mt7662usb_device_table");
            return MTK_7662;
        }
    }
    for( i = 0; strcmp(mt7601usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(mt7601usb_device_table[i],info,strlen(mt7601usb_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module mt7601usb_device_table");
            return MTK_7601;
        }
    }
	for( i = 0; strcmp(rt8188usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(rt8188usb_device_table[i],info,strlen(rt8188usb_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module rt8188usb_device_table");
            return RTK_8188;
        }
    }
	for( i = 0; strcmp(mt7603usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(mt7603usb_device_table[i],info,strlen(mt7603usb_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module mt7603usb_device_table");
            return MTK_7603;
        }
    }
    for( i = 0; strcmp(rt2500usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(rt2500usb_device_table[i], info, strlen(rt2500usb_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module rt2500usb_device_table");
            return Ralink_5570;
        }
    }

    for( i = 0; strcmp(rt2800usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(rt2800usb_device_table[i],info,strlen(rt2800usb_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module rt2800usb_device_table");
            return Ralink_5570;
        }
    }

    for( i = 0; strcmp(rt73usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(rt73usb_device_table[i],info,strlen(rt73usb_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module rt73usb_device_table");
            return Ralink_5570;
        }
    }
    for( i = 0; strcmp(realtekcu_devive_table[i], "0") != 0; i++)
    {
        if(strncmp(realtekcu_devive_table[i],info,strlen(realtekcu_devive_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module REALTEK_CU");
            return REALTEK_CU;
        }
    }

    for( i = 0; strcmp(realteksu_device_table[i], "0") != 0; i++)
    {
        if(strncmp(realteksu_device_table[i],info,strlen(realteksu_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module REALTEK_SU");
            return REALTEK_SU;
        }
    }

    for( i = 0; strcmp(realtekdu_device_table[i], "0") != 0; i++)
    {
        if(strncmp(realtekdu_device_table[i],info,strlen(realtekdu_device_table[i])) == 0)
        {
            LOGD("wifi_trans_pid_to_module REALTEK_DU");
            return REALTEK_DU;
        }
    }

    for (i = 0;strcmp(ath6k_usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(ath6k_usb_device_table[i],info,strlen(ath6k_usb_device_table[i])) == 0)
        {
            LOGD ("ATHEROS_ATH6K device \n");
            return ATHEROS_ATH6K;
        }
    }

    for (i = 0;strcmp(athk2_usb_device_table[i],"0") != 0; i++)
    {
        if(strncmp(athk2_usb_device_table[i],info,strlen(athk2_usb_device_table[i])) == 0)
        {
            LOGD ("ATHEROS_K2 device \n");
            return ATHEROS_K2;
        }
    }

    LOGD ("No Dongle");
    return No_Dongle;
}

void wifi_set_envir_vali(int dongle_type)
{
    memset(SUPPLICANT_NAME, 0, sizeof(SUPPLICANT_NAME));
    strcpy(SUPPLICANT_NAME, "wpa_supplicant");
    strcat(SUPPLICANT_NAME, supp_prefix[dongle_type]);

    memset(P2P_SUPPLICANT_NAME, 0, sizeof(P2P_SUPPLICANT_NAME));
    strcpy(P2P_SUPPLICANT_NAME, "p2p_supplicant");
    strcat(P2P_SUPPLICANT_NAME, supp_prefix[dongle_type]);

    memset(SUPP_PROP_NAME, 0, sizeof(SUPP_PROP_NAME));
    strcpy(SUPP_PROP_NAME, "init.svc.wpa_supplicant");
    strcat(SUPP_PROP_NAME, supp_prefix[dongle_type]);

    memset(P2P_PROP_NAME, 0, sizeof(SUPP_PROP_NAME));
    strcpy(P2P_PROP_NAME, "init.svc.p2p_supplicant");
    strcat(P2P_PROP_NAME, supp_prefix[dongle_type]);


    memset(DRIVER_MODULE_PATH, 0, sizeof(DRIVER_MODULE_PATH));
    strcpy(DRIVER_MODULE_PATH, module_path[dongle_type]);

    memset(DRIVER_MODULE_NAME, 0, sizeof(DRIVER_MODULE_NAME));
    strcpy(DRIVER_MODULE_NAME, module_name[dongle_type]);

    memset(DRIVER_MODULE_ARG, 0, sizeof(DRIVER_MODULE_ARG));
    strcpy(DRIVER_MODULE_ARG, module_arg[dongle_type]);


    memset(DRIVER_MODULE_TAG, 0, sizeof(DRIVER_MODULE_TAG));
    strcpy(DRIVER_MODULE_TAG, module_name[dongle_type]);

	// set this value to the property.
	LOGD("wifi_set_envir_vali: set property values!!!");
	property_set(SUPPLICANT_NAME_PROPERTY, SUPPLICANT_NAME);
	property_set(SUPPLICANT_NAME_P2P_PROPERTY, P2P_SUPPLICANT_NAME);
	property_set(SUPPLICANT_PROP_NAME_PROPERTY, SUPP_PROP_NAME);	
	property_set(SUPPLICANT_PROP_P2P_NAME_PROPERTY, P2P_PROP_NAME);		
	property_set(DRIVER_MODULE_PATH_PROPERTY, DRIVER_MODULE_PATH);
	property_set(DRIVER_MODULE_NAME_PROPERTY, DRIVER_MODULE_NAME);
	property_set(DRIVER_MODULE_ARG_PROPERTY, DRIVER_MODULE_ARG);
	property_set(DRIVER_MODULE_TAG_PROPERTY, DRIVER_MODULE_TAG);
    LOGD("wifi_set_envir_vali:\n");
    LOGD("SUPPLICANT_NAME[%s]P2P_SUPPLICANT_NAME[%s]SUPP_PROP_NAME[%s]DRIVER_MODULE_PATH \
        [%s]DRIVER_MODULE_NAME[%s]DRIVER_MODULE_ARG[%s]DRIVER_MODULE_TAG[%s]", SUPPLICANT_NAME, P2P_SUPPLICANT_NAME\
        , SUPP_PROP_NAME, DRIVER_MODULE_PATH ,DRIVER_MODULE_NAME, DRIVER_MODULE_ARG,DRIVER_MODULE_TAG);
    return;
}

int wifi_set_dongle_inside(const char *info)
{
    LOGD("wifi_set_dongle_inside set %s",info);

    wifi_add_in_list(info);

    return 0;
}

int wifi_check_is_dongle_in()
{
    struct device_node *  ptmp = NULL;
    int module = -1;
	int i = find_existing_devices();

    ptmp = phead;

    while(ptmp != NULL)
    {
        module = wifi_trans_pid_to_module(ptmp->device_id);
        if (module >= 0)
        {
            wifi_set_dongle_type(module);
            wifi_set_envir_vali(module);
            wifi_set_dongle_mode(0);
            pcurr_device = ptmp;
            return 0;//wifi_load_driver();
        }
        ptmp = ptmp->pnext;
    }

    return -1;
}

int wifi_check_is_dongle_out()
{
    struct device_node *  ptmp = NULL;
    int module = -1;

    if(!is_wifi_driver_loaded())
    {
        return -1;
    }
    if(pcurr_device == NULL)
    {
        int ret =  wifi_unload_driver();
        module = No_Dongle;
        wifi_set_dongle_type(module);
        return ret;
    }

    return -1;
}

int wifi_set_dongle_remove(const char *info)
{

    char *p = strstr(info , "/");
    char device_tmp[20] = {0};
    if((p - info) == 3)
        sprintf(device_tmp , "0%s",info);
    else
        sprintf(device_tmp,"%s",info);
   LOGD("wifi_set_dongle_remove  info[%s] ",info );
    if((pcurr_device!=NULL)&&(pcurr_device->device_id !=NULL)) //wanqian debug for camera make system hang 20120920
    {
        if(is_wifi_driver_loaded() && (strcmp(pcurr_device->device_id, device_tmp) == 0))
        {
            pcurr_device = NULL;  // if NULL for unload driver
        }
	}
    wifi_delete_from_list(info);
    return -1;
}

static int insmod(const char *filename, const char *args)
{
    void *module;
    unsigned int size;
    int ret;

    module = load_file(filename, &size);
    if (!module)
        return -1;
    LOGD ("insmod filename[%s]", filename);
    ret = init_module(module, size, args);

    free(module);

    return ret;
}

static int rmmod(const char *modname)
{
    int ret = -1;
    int maxtry = 10;

    while (maxtry-- > 0) {
        ret = delete_module(modname, O_NONBLOCK | O_EXCL);
        if (ret < 0 && errno == EAGAIN)
            usleep(500000);
        else
            break;
    }

    if (ret != 0)
        ALOGD("Unable to unload driver module \"%s\": %s\n",
             modname, strerror(errno));
    return ret;
}

static int insmod_driver(const char *driver_ko, const char *args)
{
	char file_path[256] = {0};
	snprintf (file_path, sizeof (file_path), "/data/misc/wifi/%s", driver_ko);
	if (insmod(file_path,args) != 0)
	{
		LOGD ("insmod %s failed", file_path);
		{
			memset (file_path, 0, sizeof (file_path));
			snprintf (file_path, sizeof (file_path), "/system/lib/modules/%s", driver_ko);
			if (insmod (file_path, args) != 0 )
			{
				 LOGD ("insmod %s failed", file_path);
			}
  		}
	}
	return 0;
}

static int wifi_insmod (int dongle_type)
{
    LOGD ("wifi_insmod dongle_type = %d \n", dongle_type);
    if (dongle_type == Ralink_5570)
    {
        int mode = wifi_get_dongle_mode();
        if(mode == STA_MODE)
        {
//            insmod_driver ("rfkill.ko", "");
            insmod_driver ("cfg80211.ko", "");
            insmod_driver ("mac80211.ko", "");
            insmod_driver ("rt5572sta.ko", "");        }
        else if(mode == AP_MODE)
        {
            insmod_driver ("rtutil5572ap.ko", "");
            insmod_driver ("rt5572ap.ko", "");
            insmod_driver ("rtnet5572ap.ko", "");
        }
        else
            return -1;

 //       LOGD ("execute the wpa_supplicantat_pre shell \n" );
 //       property_set("ctl.start", "wifi_set:ra_pre");//  it should be in the wifi_set.sh
    }
    else if (dongle_type == MTK_7601)
    {
    	  int mode =wifi_get_dongle_mode();
        if( mode == STA_MODE)
        {
            insmod_driver ("rfkill.ko", ""); 
            insmod_driver ("cfg80211.ko", ""); 
            insmod_driver ("mac80211.ko", "");
            insmod_driver ("mt7601Usta.ko", "");        }
        else if(mode == AP_MODE)
        {
            insmod_driver ("mt7601Uap.ko", ""); 
        }
    }
	else if (dongle_type == RTK_8188)
    {
    	  int mode =wifi_get_dongle_mode();
        
          insmod_driver ("8188eu.ko", "ifname=wlan0 if2name=p2p0"); 
        
    }
	else if (dongle_type == MTK_7662)
    {
    	  int mode =wifi_get_dongle_mode();
        if( mode == STA_MODE)
        {
            insmod_driver ("rfkill.ko", ""); 
            insmod_driver ("cfg80211.ko", ""); 
            insmod_driver ("mac80211.ko", "");
            insmod_driver ("mt7662u_sta.ko", "");        }
        else if(mode == AP_MODE)
        {
            insmod_driver ("mt7662u_ap.ko", ""); 
        }
    }
	else if (dongle_type == MTK_7603)
    {
    	  int mode =wifi_get_dongle_mode();
        if( mode == STA_MODE)
        {
            insmod_driver ("rfkill.ko", ""); 
            insmod_driver ("cfg80211.ko", ""); 
            insmod_driver ("mac80211.ko", "");
            insmod_driver ("mt7603u_sta.ko", "");        }
        else if(mode == AP_MODE)
        {
            insmod_driver ("mt7603u_sta.ko", ""); 
        }
    }
    else if (dongle_type == REALTEK_CU)
    {
        insmod_driver ("rfkill.ko", "");
        insmod_driver ("cfg80211.ko", "");
        insmod_driver ("8192cu.ko", "");
    }
	else if  (REALTEK_SU == dongle_type)
	{
        insmod_driver ("rfkill.ko", "");
        insmod_driver ("cfg80211.ko", "");
		insmod_driver("8712u.ko","");
	}
	else if  (REALTEK_DU == dongle_type)
	{
	    insmod_driver ("rfkill.ko", "");
        insmod_driver ("cfg80211.ko", "");
		insmod_driver("8192du.ko","");
	}
    else if (dongle_type == ATHEROS_ATH6K)
    {
        if (insmod_driver ("rfkill.ko", "") != 0)
        {
            LOGD("insmod rfkill.ko failed\n");
            //return -1; //rfkill will become build-in, skip return -1, if can't insmod this ko
        }
        if (insmod_driver ("compat.ko", "") !=  0)
        {
            LOGD("insmod compat.ko failed\n");
            return -1;
        }
        insmod_driver ("cfg80211_ath6kl.ko", "");
        insmod_driver ("ath6kl_usb.ko", "ath6kl_p2p=1 chip_pwd_l=0");
    }
	else if  (dongle_type == ATHEROS_K2)
	{
		insmod_driver ("adf.ko", "");
		insmod_driver ("asf.ko", "");
		insmod_driver ("ath_hif_usb.ko", "");
		insmod_driver ("ath_htc_hst.ko", "");
		insmod_driver ("ath_hal.ko", "");
		insmod_driver ("ath_dfs.ko", "");
		insmod_driver ("ath_rate_atheros.ko", "");
		insmod_driver ("ath_dev.ko", "");
		insmod_driver ("umac.ko", "");
		insmod_driver ("ath_usbdrv.ko", "");
        LOGD ("execute the wifi_set:k2_pre  \n" );
        property_set("ctl.start", "wifi_set:k2_pre");// todo!!! it should be in the wifi_set.sh
	}
    else
    {
        LOGD ("unknown wifi driver type");
		return -1;
    }

    return 0;
}

static int wifi_rmmod (int dongle_type)
{
    LOGD ("wifi_rmmod dongle_type = %d\n", dongle_type);
    if (dongle_type == Ralink_5570)
    {
        int mode =wifi_get_dongle_mode();
        if(mode == STA_MODE)
        {
            rmmod ("rt5572sta");
            rmmod ("mac80211");
            rmmod ("cfg80211");
//            rmmod ("rfkill");
    }
        else if(mode == AP_MODE)
        {
    		rmmod("rtnet5572ap");
    		rmmod("rt5572ap");
    		rmmod("rtutil5572ap");
        }
        else
            return -1;
    }
    else if (dongle_type == MTK_7601)
    { 
		int mode =wifi_get_dongle_mode();
        if(mode == STA_MODE)
        {
            rmmod ("mt7601Usta");
            rmmod ("mac80211");
            rmmod ("cfg80211"); 
            rmmod ("rfkill"); 
        }
        else if(mode == AP_MODE)
        {
    		rmmod("mt7601Uap"); 
        }
        else
            return -1;
    }
	else if (dongle_type == RTK_8188)
    { 
			rmmod("8188eu"); 
      
    }
    else if (dongle_type == MTK_7662)
    { 
		int mode =wifi_get_dongle_mode();
        if(mode == STA_MODE)
        {
            rmmod ("mt7662u_sta");
            rmmod ("mac80211");
            rmmod ("cfg80211"); 
            rmmod ("rfkill"); 
        }
        else if(mode == AP_MODE)
        {
    		rmmod("mt7662u_sta"); 
        }
        else
            return -1;
    }
	else if (dongle_type == MTK_7603)
    { 
		int mode =wifi_get_dongle_mode();
        if(mode == STA_MODE)
        {
            rmmod ("mt7603u_sta");
            rmmod ("mac80211");
            rmmod ("cfg80211"); 
            rmmod ("rfkill"); 
        }
        else if(mode == AP_MODE)
        {
    		rmmod("mt7603u_sta"); 
        }
        else
            return -1;
    }
    else if (dongle_type == REALTEK_CU)
    {
        rmmod ("8192cu");
        rmmod ("cfg80211");
        rmmod ("rfkill");
    }
	else if  (REALTEK_SU == dongle_type)
	{
		rmmod ("8712u");
        rmmod ("cfg80211");
        rmmod ("rfkill");
	}
	else if  (REALTEK_DU == dongle_type)
	{
		rmmod("8192du");
        rmmod ("cfg80211");
        rmmod ("rfkill");
	}

    else if (dongle_type == ATHEROS_ATH6K)
    {
        rmmod ("ath6kl_usb");
        rmmod ("cfg80211");
        rmmod ("compat");
        rmmod ("rfkill");
    }
	else if  (dongle_type == ATHEROS_K2)
	{
		rmmod ("ath_usbdrv");
		rmmod ("umac");
		rmmod ("ath_dev");
		rmmod ("ath_rate_atheros");
		rmmod ("ath_dfs");
		rmmod ("ath_hal");
		rmmod ("ath_htc_hst");
		rmmod ("ath_hif_usb");
		rmmod ("asf");
		rmmod ("adf");

	}
	else
	{
		LOGE ("Unknown dongle type");
	}
    return 0;
}
int do_dhcp_request(int *ipaddr, int *gateway, int *mask,
                    int *dns1, int *dns2, int *server, int *lease) {
    /* For test driver, always report success */
    if (strcmp(primary_iface, WIFI_TEST_INTERFACE) == 0)
        return 0;

    if (ifc_init() < 0)
        return -1;

    if (do_dhcp(primary_iface) < 0) {
        ifc_close();
        return -1;
    }
    ifc_close();
    get_dhcp_info(ipaddr, gateway, mask, dns1, dns2, server, lease);
    return 0;
}

const char *get_dhcp_error_string() {
    return dhcp_lasterror();
}

int is_wifi_driver_loaded() {

	//wifi_set_dongle_inside();

	if(wifi_check_is_dongle_in() == 0) {
		LOGD ("is_wifi_driver_loaded:wifi_check_is_dongle_in ok");
	} else {
		LOGD ("is_wifi_driver_loaded:wifi_check_is_dongle_in none");
	}

    char driver_status[PROPERTY_VALUE_MAX];
    int dongle_type = wifi_get_dongle_type();
    if(dongle_type >= 0)
    {
        memset(DRIVER_MODULE_TAG, 0, sizeof(DRIVER_MODULE_TAG));
        strcpy(DRIVER_MODULE_TAG, module_name[dongle_type]);
    }
    else
    {
        LOGD ("no dongle in  [%d]\n", dongle_type);
        return 0;
    }
#ifdef WIFI_DRIVER_MODULE_PATH
    FILE *proc;
    char line[sizeof(DRIVER_MODULE_TAG)+10];
#endif

    if (!property_get(DRIVER_PROP_NAME, driver_status, NULL)
            || strcmp(driver_status, "ok") != 0) {
        LOGD ("driver not loaded driver_status [%s]\n", driver_status);
        return 0;  /* driver not loaded */
    }
#ifdef WIFI_DRIVER_MODULE_PATH
    /*
     * If the property says the driver is loaded, check to
     * make sure that the property setting isn't just left
     * over from a previous manual shutdown or a runtime
     * crash.
     */
    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        ALOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
        property_set(DRIVER_PROP_NAME, "unloaded");
        return 0;
    }
    while ((fgets(line, sizeof(line), proc)) != NULL) {
        LOGD ("is_wifi_driver_loaded line is [%s]\n", line);
        if((wifi_get_dongle_type() == Ralink_5570) && (wifi_get_dongle_mode() == AP_MODE))
        {
            if (strncmp(line, DRIVER_MODULE_TAG_AP, strlen(DRIVER_MODULE_TAG_AP)) == 0) {
                fclose(proc);
                return 1;
                }
        }else if((wifi_get_dongle_type() == MTK_7601) && (wifi_get_dongle_mode() == AP_MODE))
    	{
    		 if (strncmp(line, DRIVER_MODULE_TAG_AP_7601, strlen(DRIVER_MODULE_TAG_AP_7601)) == 0) {
                fclose(proc);
                return 1;
                }
    	}else if((wifi_get_dongle_type() == RTK_8188) && (wifi_get_dongle_mode() == AP_MODE))
    	{
    		 if (strncmp(line, "8188eu", strlen("8188eu")) == 0) {
                fclose(proc);
                return 1;
                }
    	}
		else if((wifi_get_dongle_type() == MTK_7662) && (wifi_get_dongle_mode() == AP_MODE))
    	{
    		 if (strncmp(line, DRIVER_MODULE_TAG_AP_7662, strlen(DRIVER_MODULE_TAG_AP_7662)) == 0) {
                fclose(proc);
                return 1;
                }
    	}
		else if((wifi_get_dongle_type() == MTK_7662) && (wifi_get_dongle_mode() == STA_MODE))
    	{
    		 if (strncmp(line, DRIVER_MODULE_TAG_STA_7662, strlen(DRIVER_MODULE_TAG_STA_7662)) == 0) {
                fclose(proc);
                return 1;
                }
    	}
		else if((wifi_get_dongle_type() == MTK_7603) && (wifi_get_dongle_mode() == AP_MODE))
    	{
    		 if (strncmp(line, DRIVER_MODULE_TAG_AP_7603, strlen(DRIVER_MODULE_TAG_AP_7603)) == 0) {
                fclose(proc);
                return 1;
                }
    	}
		else if((wifi_get_dongle_type() == MTK_7603) && (wifi_get_dongle_mode() == STA_MODE))
    	{
    		 if (strncmp(line, DRIVER_MODULE_TAG_STA_7603, strlen(DRIVER_MODULE_TAG_STA_7603)) == 0) {
                fclose(proc);
                return 1;
                }
    	}
        else
        {
            if (strncmp(line, DRIVER_MODULE_TAG, strlen(DRIVER_MODULE_TAG)) == 0) {
                fclose(proc);
                return 1;
                }
        }
    }
    fclose(proc);
    property_set(DRIVER_PROP_NAME, "unloaded");

    if (property_get(DRIVER_PROP_NAME, driver_status, NULL)) {
        LOGD ("is_wifi_driver_unloaded driver_status success [%s]\n", driver_status);
    }
    else
    {
        LOGD ("is_wifi_driver_unloaded driver_status fail [%s]\n", driver_status);
    }
    return 0;
#else
    return 1;
#endif
}

int wifi_load_driver()
{

	if(wifi_check_is_dongle_in() == 0) {
		LOGD ("wifi_load_driver:wifi_check_is_dongle_in ok");
	} else {
		LOGD ("wifi_load_driver:wifi_check_is_dongle_in none");
	}

#ifdef WIFI_DRIVER_MODULE_PATH
    FILE *proc;
    char line[sizeof(DRIVER_MODULE_TAG)+10];

    LOGD ("wifi_load_driver");
    char driver_status[PROPERTY_VALUE_MAX];
    int count = 100; /* wait at most 20 seconds for completion */

    if (is_wifi_driver_loaded()) {
        LOGD ("wifi driver have load\n");
        return 0;
    }

	int module = wifi_get_dongle_type();
   if (wifi_insmod(module) < 0)
  // if (insmod(DRIVER_MODULE_PATH, DRIVER_MODULE_ARG) < 0)
	{
		LOGD ("driver insmod failed");
        return -1;
	}
    LOGD ("wifi insmod succ");
    if (strcmp(FIRMWARE_LOADER,"") == 0) {
        /* usleep(WIFI_DRIVER_LOADER_DELAY); */
        property_set(DRIVER_PROP_NAME, "ok");
    }
    else {
        property_set("ctl.start", FIRMWARE_LOADER);
    }
    sched_yield();
    while (count-- > 0) {
        if (property_get(DRIVER_PROP_NAME, driver_status, NULL)) {
            if (strcmp(driver_status, "ok") == 0)
            {
                if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
                    ALOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
                    property_set(DRIVER_PROP_NAME, "unloaded");
                    return -1;
                }
                while ((fgets(line, sizeof(line), proc)) != NULL) {
                    if((wifi_get_dongle_type() == Ralink_5570) && (wifi_get_dongle_mode() == AP_MODE))
                    {
                        if (strncmp(line, DRIVER_MODULE_TAG_AP, strlen(DRIVER_MODULE_TAG_AP)) == 0) {
                            fclose(proc);
                            return 0;
                            }
                    }else if((wifi_get_dongle_type() == MTK_7601) && (wifi_get_dongle_mode() == AP_MODE))
                    {
                        if (strncmp(line, DRIVER_MODULE_TAG_AP_7601, strlen(DRIVER_MODULE_TAG_AP_7601)) == 0) {
                            fclose(proc);
                            return 0;
                            }
                    }else if((wifi_get_dongle_type() == RTK_8188))
                    {
                        if (strncmp(line, "8188eu", strlen("8188eu")) == 0) {
                            fclose(proc);
                            return 0;
                            }
                    }else if((wifi_get_dongle_type() == MTK_7662) && (wifi_get_dongle_mode() == AP_MODE))
                    {
                        if (strncmp(line, DRIVER_MODULE_TAG_AP_7662, strlen(DRIVER_MODULE_TAG_AP_7662)) == 0) {
                            fclose(proc);
                            return 0;
                            }
                    }
					else if((wifi_get_dongle_type() == MTK_7603) && (wifi_get_dongle_mode() == AP_MODE))
                    {
                        if (strncmp(line, DRIVER_MODULE_TAG_AP_7603, strlen(DRIVER_MODULE_TAG_AP_7603)) == 0) {
                            fclose(proc);
                            return 0;
                            }
                    }
                    else
                    {
                        if (strncmp(line, DRIVER_MODULE_TAG, strlen(DRIVER_MODULE_TAG)) == 0) {
                            fclose(proc);
                            return 0;
                            }
                    }
                }
                fclose(proc);
				LOGD ("driver load succ");
            }
            else if (strcmp(DRIVER_PROP_NAME, "failed") == 0) {
                wifi_unload_driver();
                return -1;
            }
        }
        usleep(200000);
    }
    property_set(DRIVER_PROP_NAME, "timeout");
    wifi_unload_driver();
    return -1;
#else
    property_set(DRIVER_PROP_NAME, "ok");
    return 0;
#endif
}

int wifi_unload_driver()
{
    LOGD ("wifi_unload_driver");
    usleep(200000); /* allow to finish interface down */
#ifdef WIFI_DRIVER_MODULE_PATH
    int module = wifi_get_dongle_type();
    if (wifi_rmmod(module) == 0)
    {
        int count = 20; /* wait at most 10 seconds for completion */
        while (count-- > 0)
        {
            if (!is_wifi_driver_loaded())
				break;
            usleep(500000);
        }
        usleep(500000); /* allow card removal */
        if (count) {
			LOGD("WIFI Unload success");
            return 0;
        }
        return -1;
    }
    else
        return -1;
#else
    property_set(DRIVER_PROP_NAME, "unloaded");
    return 0;
#endif
}

int ensure_entropy_file_exists()
{
    int ret;
    int destfd;

    ret = access(SUPP_ENTROPY_FILE, R_OK|W_OK);
    if ((ret == 0) || (errno == EACCES)) {
        if ((ret != 0) &&
            (chmod(SUPP_ENTROPY_FILE, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) != 0)) {
            ALOGE("Cannot set RW to \"%s\": %s", SUPP_ENTROPY_FILE, strerror(errno));
            return -1;
        }
        return 0;
    }
    destfd = TEMP_FAILURE_RETRY(open(SUPP_ENTROPY_FILE, O_CREAT|O_RDWR, 0660));
    if (destfd < 0) {
        ALOGE("Cannot create \"%s\": %s", SUPP_ENTROPY_FILE, strerror(errno));
        return -1;
    }

    if (TEMP_FAILURE_RETRY(write(destfd, dummy_key, sizeof(dummy_key))) != sizeof(dummy_key)) {
        ALOGE("Error writing \"%s\": %s", SUPP_ENTROPY_FILE, strerror(errno));
        close(destfd);
        return -1;
    }
    close(destfd);

    /* chmod is needed because open() didn't set permisions properly */
    if (chmod(SUPP_ENTROPY_FILE, 0777) < 0) {
        ALOGE("Error changing permissions of %s to 0777: %s",
             SUPP_ENTROPY_FILE, strerror(errno));
        unlink(SUPP_ENTROPY_FILE);
        return -1;
    }

    if (chown(SUPP_ENTROPY_FILE, AID_SYSTEM, AID_WIFI) < 0) {
        ALOGE("Error changing group ownership of %s to %d: %s",
             SUPP_ENTROPY_FILE, AID_WIFI, strerror(errno));
        unlink(SUPP_ENTROPY_FILE);
        return -1;
    }
    return 0;
}

int update_ctrl_interface(const char *config_file) {

    int srcfd, destfd;
    int nread;
    char ifc[PROPERTY_VALUE_MAX];
    char *pbuf;
    char *sptr;
    struct stat sb;
    int ret;

    if (stat(config_file, &sb) != 0)
        return -1;

    pbuf = malloc(sb.st_size + PROPERTY_VALUE_MAX);
    if (!pbuf)
        return 0;
    srcfd = TEMP_FAILURE_RETRY(open(config_file, O_RDONLY));
    if (srcfd < 0) {
        ALOGE("Cannot open \"%s\": %s", config_file, strerror(errno));
        free(pbuf);
        return 0;
    }
    nread = TEMP_FAILURE_RETRY(read(srcfd, pbuf, sb.st_size));
    close(srcfd);
    if (nread < 0) {
        ALOGE("Cannot read \"%s\": %s", config_file, strerror(errno));
        free(pbuf);
        return 0;
    }

    if (!strcmp(config_file, SUPP_CONFIG_FILE)) {
        property_get("wifi.interface", ifc, WIFI_TEST_INTERFACE);
    } else {
        strcpy(ifc, CONTROL_IFACE_PATH);
    }
    /* Assume file is invalid to begin with */
    ret = -1;
    /*
     * if there is a "ctrl_interface=<value>" entry, re-write it ONLY if it is
     * NOT a directory.  The non-directory value option is an Android add-on
     * that allows the control interface to be exchanged through an environment
     * variable (initialized by the "init" program when it starts a service
     * with a "socket" option).
     *
     * The <value> is deemed to be a directory if the "DIR=" form is used or
     * the value begins with "/".
     */
    if ((sptr = strstr(pbuf, "ctrl_interface="))) {
        ret = 0;
        if ((!strstr(pbuf, "ctrl_interface=DIR=")) &&
                (!strstr(pbuf, "ctrl_interface=/"))) {
            char *iptr = sptr + strlen("ctrl_interface=");
            int ilen = 0;
            int mlen = strlen(ifc);
            int nwrite;
            if (strncmp(ifc, iptr, mlen) != 0) {
                ALOGE("ctrl_interface != %s", ifc);
                while (((ilen + (iptr - pbuf)) < nread) && (iptr[ilen] != '\n'))
                    ilen++;
                mlen = ((ilen >= mlen) ? ilen : mlen) + 1;
                memmove(iptr + mlen, iptr + ilen + 1, nread - (iptr + ilen + 1 - pbuf));
                memset(iptr, '\n', mlen);
                memcpy(iptr, ifc, strlen(ifc));
                destfd = TEMP_FAILURE_RETRY(open(config_file, O_RDWR, 0660));
                if (destfd < 0) {
                    ALOGE("Cannot update \"%s\": %s", config_file, strerror(errno));
                    free(pbuf);
                    return -1;
                }
                TEMP_FAILURE_RETRY(write(destfd, pbuf, nread + mlen - ilen -1));
                close(destfd);
            }
        }
    }
    free(pbuf);
    return ret;
}

int is_config_p2p(const char *config_file)
{
    const char *p2p = "p2p";
    if (strstr(config_file, p2p) != NULL) {
         return 1;
    }
         return 0;
}

int ensure_config_file_exists(const char *config_file)
{
    
	int 	ret = 0;
    if (is_config_p2p(config_file)) {
        ret = ensure_config_file(config_file, P2P_CONFIG_TEMPLATE);

    } else {
        ret = ensure_config_file(config_file, SUPP_CONFIG_TEMPLATE);
    }

    //zengyongying: always make sure config_file has access right.	begin
    	/* chmod is needed because open() didn't set permisions properly */
	if (chmod(config_file, 0777) < 0) {
	        ALOGE("Error changing permissions of %s to 0777: %s",
	             config_file, strerror(errno));
	        //unlink(config_file);
	        //return -1;
	    }

	    if (chown(config_file, AID_SYSTEM, AID_WIFI) < 0) {
	        ALOGE("Error changing group ownership of %s to %d: %s",
	             config_file, AID_WIFI, strerror(errno));
	        //unlink(config_file);
	        //return -1;
	}
		return ret;
    //zengyongying: always make sure config_file has access right. end
}


int ensure_config_file(const char *config_file, const char *config_template)

{
    char buf[2048];
    int srcfd, destfd;
    struct stat sb;
    int nread;
    int ret;

    ret = access(config_file, R_OK|W_OK);
    if ((ret == 0) || (errno == EACCES)) {
        if ((ret != 0) &&
            (chmod(config_file, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) != 0)) {
            ALOGE("Cannot set RW to \"%s\": %s", config_file, strerror(errno));
            return -1;
        }
        /* return if filesize is at least 10 bytes */
        if (stat(config_file, &sb) == 0 && sb.st_size > 10) {
            return update_ctrl_interface(config_file);
        }
    } else if (errno != ENOENT) {
        ALOGE("Cannot access \"%s\": %s", config_file, strerror(errno));
        return -1;
    }

    srcfd = TEMP_FAILURE_RETRY(open(config_template, O_RDONLY));
    if (srcfd < 0) {
        ALOGE("Cannot open \"%s\": %s", config_template, strerror(errno));
        return -1;
    }

    destfd = TEMP_FAILURE_RETRY(open(config_file, O_CREAT|O_RDWR, 0660));
    if (destfd < 0) {
        close(srcfd);
        ALOGE("Cannot create \"%s\": %s", config_file, strerror(errno));
        return -1;
    }

    while ((nread = TEMP_FAILURE_RETRY(read(srcfd, buf, sizeof(buf)))) != 0) {
        if (nread < 0) {
            ALOGE("Error reading \"%s\": %s", config_template, strerror(errno));
            close(srcfd);
            close(destfd);
            unlink(config_file);
            return -1;
        }
        TEMP_FAILURE_RETRY(write(destfd, buf, nread));
    }

    close(destfd);
    close(srcfd);

    /* chmod is needed because open() didn't set permisions properly */
    /*if (chmod(config_file, 0660) < 0) {
        ALOGE("Error changing permissions of %s to 0660: %s",
             config_file, strerror(errno));
        //unlink(config_file);
        return -1;
    }

    if (chown(config_file, AID_SYSTEM, AID_WIFI) < 0) {
        ALOGE("Error changing group ownership of %s to %d: %s",
             config_file, AID_WIFI, strerror(errno));
        //unlink(config_file);
        return -1;
    }*/
    return update_ctrl_interface(config_file);
}

/**
 * wifi_wpa_ctrl_cleanup() - Delete any local UNIX domain socket files that
 * may be left over from clients that were previously connected to
 * wpa_supplicant. This keeps these files from being orphaned in the
 * event of crashes that prevented them from being removed as part
 * of the normal orderly shutdown.
 */
void wifi_wpa_ctrl_cleanup(void)
{
    DIR *dir;
    struct dirent entry;
    struct dirent *result;
    size_t dirnamelen;
    size_t maxcopy;
    char pathname[PATH_MAX];
    char *namep;
    char *local_socket_dir = CONFIG_CTRL_IFACE_CLIENT_DIR;
    char *local_socket_prefix = CONFIG_CTRL_IFACE_CLIENT_PREFIX;

    if ((dir = opendir(local_socket_dir)) == NULL)
        return;

    dirnamelen = (size_t)snprintf(pathname, sizeof(pathname), "%s/", local_socket_dir);
    if (dirnamelen >= sizeof(pathname)) {
        closedir(dir);
        return;
    }
    namep = pathname + dirnamelen;
    maxcopy = PATH_MAX - dirnamelen;
    while (readdir_r(dir, &entry, &result) == 0 && result != NULL) {
        if (strncmp(entry.d_name, local_socket_prefix, strlen(local_socket_prefix)) == 0) {
            if (strlcpy(namep, entry.d_name, maxcopy) < maxcopy) {
                unlink(pathname);
            }
        }
    }
    closedir(dir);
}
int wifi_start_supplicant(int p2p_supported)
{
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};
    int count = 200; /* wait at most 20 seconds for completion */
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
    const prop_info *pi;
    unsigned serial = 0, i;
#endif

    if (p2p_supported) {
        //strcpy(supplicant_name, P2P_SUPPLICANT_NAME);
        //strcpy(supplicant_prop_name, P2P_PROP_NAME);
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP	
        if(p2p_supported == 1)
        {
        	property_get(SUPPLICANT_NAME_P2P_PROPERTY, supplicant_name, NULL);
        	property_get(SUPPLICANT_PROP_P2P_NAME_PROPERTY, supplicant_prop_name, NULL);
        }else
        {
            /* start soft ap daemon */
            strcpy(supplicant_name, AP_DAEMON_NAME);
            strcpy(supplicant_prop_name, AP_PROP_NAME);                  
        }
#else                    
		property_get(SUPPLICANT_NAME_P2P_PROPERTY, supplicant_name, NULL);
		property_get(SUPPLICANT_PROP_P2P_NAME_PROPERTY, supplicant_prop_name, NULL);
#endif
        /* Ensure p2p config file is created */
        if (ensure_config_file_exists(P2P_CONFIG_FILE) < 0) {
            ALOGE("Failed to create a p2p config file");
            return -1;
        }

    } else {
    	property_get(SUPPLICANT_NAME_PROPERTY, supplicant_name, NULL);
		property_get(SUPPLICANT_PROP_NAME_PROPERTY, supplicant_prop_name, NULL);
		//strcpy(supplicant_name, SUPPLICANT_NAME);
        //strcpy(supplicant_prop_name, SUPP_PROP_NAME);
    }
	LOGD("wifi_start_supplicant:------>[%s]" , supplicant_name);
	LOGD("wifi_start_supplicant:------>[%s]" , supplicant_prop_name);

    /* Check whether already running */
    if (property_get(supplicant_name, supp_status, NULL)
            && strcmp(supp_status, "running") == 0) {
        return 0;
    }
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP	
    //if (p2p_supported != 2/*AP*/)	// zengyongying: always make sure wpa_supplicant.conf exists
    {
#endif

    /* Before starting the daemon, make sure its config file exists */
        if (ensure_config_file_exists(SUPP_CONFIG_FILE) < 0) {
            ALOGE("Wi-Fi will not be enabled");
            return -1;
        }
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP	
    }
#endif

    if (ensure_entropy_file_exists() < 0) {
        ALOGE("Wi-Fi entropy file was not created");
    }

    /* Clear out any stale socket files that might be left over. */
    wifi_wpa_ctrl_cleanup();

    /* Reset sockets used for exiting from hung state */
    for (i=0; i<MAX_CONNS; i++) {
        exit_sockets[i][0] = exit_sockets[i][1] = -1;
    }

#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
    /*
     * Get a reference to the status property, so we can distinguish
     * the case where it goes stopped => running => stopped (i.e.,
     * it start up, but fails right away) from the case in which
     * it starts in the stopped state and never manages to start
     * running at all.
     */
    pi = __system_property_find(supplicant_prop_name);
    if (pi != NULL) {
        serial = __system_property_serial(pi);
    }
#endif
    property_get("wifi.interface", primary_iface, WIFI_TEST_INTERFACE);

	LOGD (">>>> start wpa_supplicant daemon_cmd[%s]\n", supplicant_name);
    property_set("ctl.start", supplicant_name);
    sched_yield();

    while (count-- > 0) {
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
        if (pi == NULL) {
            pi = __system_property_find(supplicant_prop_name);
        }
        if (pi != NULL) {
            __system_property_read(pi, NULL, supp_status);
            if (strcmp(supp_status, "running") == 0) {
                return 0;
            } else if (__system_property_serial(pi) != serial &&
                    strcmp(supp_status, "stopped") == 0) {
                return -1;
            }
        }
#else
        if (property_get(supplicant_prop_name, supp_status, NULL)) {
            if (strcmp(supp_status, "running") == 0)
                return 0;
        }
#endif
        usleep(100000);
    }
    return -1;
}

int wifi_stop_supplicant(int p2p_supported)
{
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};
    int count = 50; /* wait at most 5 seconds for completion */
	int i = 0;

    if (p2p_supported) {
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP	
        if (p2p_supported == 1)
        {
            property_get(SUPPLICANT_NAME_P2P_PROPERTY, supplicant_name, NULL);
            property_get(SUPPLICANT_PROP_P2P_NAME_PROPERTY,supplicant_prop_name , NULL);
        }
        else if (p2p_supported == 2)
        {
        	strcpy(supplicant_name, AP_DAEMON_NAME);
        	strcpy(supplicant_prop_name, AP_PROP_NAME);
        }        
#else                        
        //strcpy(supplicant_name, P2P_SUPPLICANT_NAME);
        //strcpy(supplicant_prop_name, P2P_PROP_NAME);        
		property_get(SUPPLICANT_NAME_P2P_PROPERTY, supplicant_name, NULL);
		property_get(SUPPLICANT_PROP_P2P_NAME_PROPERTY,supplicant_prop_name , NULL);
#endif        
    } else {
        //strcpy(supplicant_name, SUPPLICANT_NAME);
        //strcpy(supplicant_prop_name, SUPP_PROP_NAME);        
    	property_get(SUPPLICANT_NAME_PROPERTY, supplicant_name, NULL);
		property_get(SUPPLICANT_PROP_NAME_PROPERTY, supplicant_prop_name, NULL);
    }

    /* Check whether supplicant already stopped */
    if (property_get(supplicant_prop_name, supp_status, NULL)
        && strcmp(supp_status, "stopped") == 0) {
        return 0;
    }

    property_set("ctl.stop", supplicant_name);
    sched_yield();

	for (i = 0; i < MAX_CONNS; i++) {
		wifi_close_sockets(i);
	}

    while (count-- > 0) {
        if (property_get(supplicant_prop_name, supp_status, NULL)) {
            if (strcmp(supp_status, "stopped") == 0)
                return 0;
        }
        usleep(100000);
    }
    ALOGE("Failed to stop supplicant");
    return -1;
}

int wifi_connect_on_socket_path(int index, const char *path)
{
	LOGD ("wifi_connect_on_socket_path index[%d] path[%s]", index, path);
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};

    /* Make sure supplicant is running */
    if (!property_get(supplicant_prop_name, supp_status, NULL)
            || strcmp(supp_status, "running") != 0) {
        ALOGE("Supplicant not running, cannot connect");
        return -1;
    }

    ctrl_conn[index] = wpa_ctrl_open(path);
    if (ctrl_conn[index] == NULL) {
        ALOGE("Unable to open connection to supplicant on \"%s\": %s",
             path, strerror(errno));
        return -1;
    }
    monitor_conn[index] = wpa_ctrl_open(path);
    if (monitor_conn[index] == NULL) {
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = NULL;
        return -1;
    }
    if (wpa_ctrl_attach(monitor_conn[index]) != 0) {
        wpa_ctrl_close(monitor_conn[index]);
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = monitor_conn[index] = NULL;
        return -1;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, exit_sockets[index]) == -1) {
        wpa_ctrl_close(monitor_conn[index]);
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = monitor_conn[index] = NULL;
        return -1;
    }

    return 0;
}

/* Establishes the control and monitor socket connections on the interface */
int wifi_connect_to_supplicant(const char *ifname)
{
	LOGD ("wifi_connect_to_supplicant ifname[%s]", ifname);
    char path[256];

    if (is_primary_interface(ifname)) {
        if (access(IFACE_DIR, F_OK) == 0) {
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP	
        if (strcmp(supplicant_name, AP_DAEMON_NAME) == 0)
        {           
            snprintf(path, sizeof(path), "%s/%s", IFACE_DIR, "p2p0");
            ALOGV("wifi_connect_to_supplicant: Change iface dir.\n");
        }
        else
            snprintf(path, sizeof(path), "%s/%s", IFACE_DIR, primary_iface);
#else            
        snprintf(path, sizeof(path), "%s/%s", IFACE_DIR, primary_iface);
#endif
        } else {
            strlcpy(path, primary_iface, sizeof(path));
        }
        return wifi_connect_on_socket_path(PRIMARY, path);
    } else {
        ALOGD("supplicant_name = %s\n", supplicant_name);
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP	
        if (strcmp(supplicant_name, AP_DAEMON_NAME) == 0) //for AP
        {
            snprintf(path, sizeof(path), "%s/%s", CONTROL_IFACE_PATH, "p2p0");
            ALOGV("wifi_connect_to_supplicant: Change iface dir.\n");
        }
        else
            sprintf(path, "%s/%s", CONTROL_IFACE_PATH, ifname);
#else                
        sprintf(path, "%s/%s", CONTROL_IFACE_PATH, ifname);
#endif
        return wifi_connect_on_socket_path(SECONDARY, path);
    }
}
int wfd_connect_on_socket_path(const char *path)
{
	LOGD ("wfd_connect_on_socket_path path[%s]", (char *)path);
	char supp_status[PROPERTY_VALUE_MAX] = {'\0'};

	/* Make sure supplicant is running */
	if (!property_get(supplicant_prop_name, supp_status, NULL)
			|| strcmp(supp_status, "running") != 0) {
		ALOGE("Supplicant not running, cannot connect");
		return -1;
	}

	wfd_ctrl_conn = wpa_ctrl_open(path);
	if (wfd_ctrl_conn == NULL) {
		ALOGE("Unable to open connection to supplicant on \"%s\": %s",
		   path, strerror(errno));
		return -1;
	}
	wfd_monitor_conn = wpa_ctrl_open(path);
	if (wfd_monitor_conn == NULL) {
		wpa_ctrl_close(wfd_ctrl_conn);
		wfd_ctrl_conn = NULL;
		return -1;
	}
	if (wpa_ctrl_attach(wfd_monitor_conn) != 0) {
		wpa_ctrl_close(wfd_monitor_conn);
		wpa_ctrl_close(wfd_ctrl_conn);
		wfd_ctrl_conn = wfd_monitor_conn = NULL;
		return -1;
	}

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, wfd_exit_sockets) == -1) {
		wpa_ctrl_close(wfd_monitor_conn);
		wpa_ctrl_close(wfd_ctrl_conn);
		wfd_ctrl_conn = wfd_monitor_conn = NULL;
		return -1;
	}
    return 0;

}

/*
*wfd_connect_to_supplicant
*/
int wfd_connect_to_supplicant(const char *ifname)
{
    char path[256] = {0};
	char interfacename[PROPERTY_VALUE_MAX] = {0};
	memcpy(interfacename, ifname, strlen(ifname));
	if (access(IFACE_DIR, F_OK) == 0) {
		snprintf(path, sizeof(path), "%s/%s", IFACE_DIR, interfacename);
	} else {
		strlcpy(path, interfacename, sizeof(path));
	}
	return wfd_connect_on_socket_path(path);
}

void wfd_close_supplicant_connection(void)
{
	 if (wfd_ctrl_conn != NULL) {
        wpa_ctrl_close(wfd_ctrl_conn);
        wfd_ctrl_conn = NULL;
    }

    if (wfd_monitor_conn != NULL) {
        wpa_ctrl_close(wfd_monitor_conn);
        wfd_monitor_conn = NULL;
    }

    if (wfd_exit_sockets[0] >= 0) {
        close(wfd_exit_sockets[0]);
        wfd_exit_sockets[0] = -1;
    }

    if (wfd_exit_sockets[1] >= 0) {
        close(wfd_exit_sockets[1]);
        wfd_exit_sockets[1] = -1;
    }
}

int wfd_send_command(const char *cmd, char *reply, size_t *reply_len)
{
	ALOGD("'%s' wfd_send_command command :.\n", cmd);

    int ret;

    if (wfd_ctrl_conn == NULL) {
        ALOGV("Not connected to wpa_supplicant - \"%s\" command dropped.\n", cmd);
        return -1;
    }
    ret = wpa_ctrl_request(wfd_ctrl_conn, cmd, strlen(cmd), reply, reply_len, NULL);
    if (ret == -2) {
        ALOGD("'%s' command timed out.\n", cmd);
        /* unblocks the monitor receive socket for termination */
        TEMP_FAILURE_RETRY(write(wfd_exit_sockets[0], "T", 1));
        return -2;
    } else if (ret < 0 || strncmp(reply, "FAIL", 4) == 0) {
        return -1;
    }
    if (strncmp(cmd, "PING", 4) == 0) {
        reply[*reply_len] = '\0';
    }
    return 0;
}



int wifi_send_command(int index, const char *cmd, char *reply, size_t *reply_len)
{
    int ret;

    if (ctrl_conn[index] == NULL) {
        ALOGV("Not connected to wpa_supplicant - \"%s\" command dropped.\n", cmd);
        return -1;
    }
    ret = wpa_ctrl_request(ctrl_conn[index], cmd, strlen(cmd), reply, reply_len, NULL);
    if (ret == -2) {
        ALOGD("'%s' command timed out.\n", cmd);
        /* unblocks the monitor receive socket for termination */
        TEMP_FAILURE_RETRY(write(exit_sockets[index][0], "T", 1));
        return -2;
    } else if (ret < 0 || strncmp(reply, "FAIL", 4) == 0) {
        return -1;
    }
    if (strncmp(cmd, "PING", 4) == 0) {
        reply[*reply_len] = '\0';
    }
    return 0;
}
int wifi_ctrl_recv(int index, char *reply, size_t *reply_len)
{
    int res;
    int ctrlfd = wpa_ctrl_get_fd(monitor_conn[index]);
    struct pollfd rfds[2];

    memset(rfds, 0, 2 * sizeof(struct pollfd));
    rfds[0].fd = ctrlfd;
    rfds[0].events |= POLLIN;
    rfds[1].fd = exit_sockets[index][1];
    rfds[1].events |= POLLIN;
    res = TEMP_FAILURE_RETRY(poll(rfds, 2, -1));
    if (res < 0) {
        ALOGE("Error poll = %d", res);
        return res;
    }
    if (rfds[0].revents & POLLIN) {
        return wpa_ctrl_recv(monitor_conn[index], reply, reply_len);
    } else if (rfds[1].revents & POLLIN) {
        /* Close only the p2p sockets on receive side
         * see wifi_close_supplicant_connection()
         */
        if (index == SECONDARY) {
            ALOGD("close sockets %d", index);
            wifi_close_sockets(index);
        }
    }
    return -2;
}

int wifi_wait_on_socket(int index, char *buf, size_t buflen)
{
    size_t nread = buflen - 1;
    int fd;
    fd_set rfds;
    int result;
    struct timeval tval;
    struct timeval *tptr;

    if (monitor_conn[index] == NULL) {
        ALOGD("Connection closed\n");
        strncpy(buf, WPA_EVENT_TERMINATING " - connection closed", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }

    result = wifi_ctrl_recv(index, buf, &nread);

    /* Terminate reception on exit socket */
    if (result == -2) {
        strncpy(buf, WPA_EVENT_TERMINATING " - connection closed", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }

    if (result < 0) {
        ALOGD("wifi_ctrl_recv failed: %s\n", strerror(errno));
        strncpy(buf, WPA_EVENT_TERMINATING " - recv error", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }
    buf[nread] = '\0';
    /* Check for EOF on the socket */
    if (result == 0 && nread == 0) {
        /* Fabricate an event to pass up */
        ALOGD("Received EOF on supplicant socket\n");
        strncpy(buf, WPA_EVENT_TERMINATING " - signal 0 received", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }
    /*
     * Events strings are in the format
     *
     *     <N>CTRL-EVENT-XXX
     *
     * where N is the message level in numerical form (0=VERBOSE, 1=DEBUG,
     * etc.) and XXX is the event name. The level information is not useful
     * to us, so strip it off.
     */
    if (buf[0] == '<') {
        char *match = strchr(buf, '>');
        if (match != NULL) {
            nread -= (match+1-buf);
            memmove(buf, match+1, nread+1);
        }
    }

    return nread;
}

int wifi_wait_for_event(const char *ifname, char *buf, size_t buflen)
{
    if (is_primary_interface(ifname)) {
        return wifi_wait_on_socket(PRIMARY, buf, buflen);
    } else {
        return wifi_wait_on_socket(SECONDARY, buf, buflen);
    }
}

void wifi_close_sockets(int index)
{
    if (ctrl_conn[index] != NULL) {
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = NULL;
    }

    if (monitor_conn[index] != NULL) {
        wpa_ctrl_close(monitor_conn[index]);
        monitor_conn[index] = NULL;
    }

    if (exit_sockets[index][0] >= 0) {
        close(exit_sockets[index][0]);
        exit_sockets[index][0] = -1;
    }

    if (exit_sockets[index][1] >= 0) {
        close(exit_sockets[index][1]);
        exit_sockets[index][1] = -1;
    }
}

void wifi_close_supplicant_connection(const char *ifname)
{
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};
    int count = 50; /* wait at most 5 seconds to ensure init has stopped stupplicant */

    if (is_primary_interface(ifname)) {
        wifi_close_sockets(PRIMARY);
    } else {
        /* p2p socket termination needs unblocking the monitor socket
         * STA connection does not need it since supplicant gets shutdown
         */
        TEMP_FAILURE_RETRY(write(exit_sockets[SECONDARY][0], "T", 1));
        /* p2p sockets are closed after the monitor thread
         * receives the terminate on the exit socket
         */
        wifi_close_sockets(SECONDARY);
        return;
    }

    while (count-- > 0) {
        if (property_get(supplicant_prop_name, supp_status, NULL)) {
            if (strcmp(supp_status, "stopped") == 0)
                return;
        }
        usleep(100000);
    }
}

int wifi_command(const char *ifname, const char *command, char *reply, size_t *reply_len)
{
	if(strncmp(ifname, WIFI_TEST_INTERFACE, strlen(WIFI_TEST_INTERFACE)) && 
		strncmp(ifname, WIFI_TEST_INTERFACE_P2P, strlen(WIFI_TEST_INTERFACE_P2P)))
	{
		LOGD("wifi_command interface name is %s\n", ifname);
		return wfd_send_command(command, reply, reply_len);
	}
	
    if (is_primary_interface(ifname)) {
        return wifi_send_command(PRIMARY, command, reply, reply_len);
    } else {
        return wifi_send_command(SECONDARY, command, reply, reply_len);
    }
}

const char *wifi_get_fw_path(int fw_type)
{
    switch (fw_type) {
    case WIFI_GET_FW_PATH_STA:
        return WIFI_DRIVER_FW_PATH_STA;
    case WIFI_GET_FW_PATH_AP:
        return WIFI_DRIVER_FW_PATH_AP;
    case WIFI_GET_FW_PATH_P2P:
        return WIFI_DRIVER_FW_PATH_P2P;
    }
    return NULL;
}

int wifi_turn_str2int(char * str)
{
    int module = -1;
    sscanf(str,"%d",&module);
    return module;
}

int wifi_set_dongle_type(int type)
{
    char dongle_tmp[3] = {0};
    sprintf(dongle_tmp , "%d",type);
    property_set(WIFI_MODULE_NAME, dongle_tmp);
    char dongle_type[PROPERTY_VALUE_MAX];
    if (property_get(WIFI_MODULE_NAME, dongle_type, NULL))
    {
        ALOGD("current module is %s", dongle_type);
    }
    else
    {
        ALOGD("current module get fail");
    }

    return 0;
}
int wifi_get_dongle_type()
{
    char dongle_type[PROPERTY_VALUE_MAX];
    if (property_get(WIFI_MODULE_NAME, dongle_type, NULL)) {
        ALOGD("current module is %s", dongle_type);
    }
    return wifi_turn_str2int(dongle_type);
}

int wifi_set_dongle_mode(int mode)
{
    char dongle_tmp[2] = {0};
    sprintf(dongle_tmp , "%d",mode);
    property_set(WIFI_MODULE_MODE, dongle_tmp);
    char dongle_type[PROPERTY_VALUE_MAX];
    if (property_get(WIFI_MODULE_MODE, dongle_type, NULL))
    {
        ALOGD("current mode is %s", dongle_type);
    }
    else
    {
        ALOGD("current mode get fail");
    }
    return 0;
}
int wifi_get_dongle_mode()
{
    char dongle_mode[PROPERTY_VALUE_MAX];
    if (property_get(WIFI_MODULE_MODE, dongle_mode, NULL)) {
        ALOGD("current mode is %s", dongle_mode);
    }
    return wifi_turn_str2int(dongle_mode);
}

int wifi_change_fw_path(const char *fwpath)
{
    int dongle_type = wifi_get_dongle_type();
    ALOGD("current dongle_type is %d", dongle_type);
	if(dongle_type == MTK_7662 || dongle_type == MTK_7603 || dongle_type == MTK_7601 || dongle_type == RTK_8188)
	{
	    /*AP and Sta use the same ko, so don't to reload driver anymore, only to change the module.*/
	    if (strcmp(fwpath, "AP") == 0) {
	        wifi_set_dongle_mode(AP_MODE);
	        return 0;//wifi_change_wifi_mode(AP_MODE);
	    }
	    wifi_set_dongle_mode(STA_MODE);
	    return 0;//wifi_change_wifi_mode(STA_MODE);
	}
	else		
	{
	    char dongle_type[PROPERTY_VALUE_MAX];
	    if (property_get(WIFI_MODULE_NAME, dongle_type, NULL)) {
	        ALOGD("current module is %s", dongle_type);
	    }
		//add for ralink 5570
	    ALOGD("%s worked %d", __FUNCTION__, __LINE__);

	    int module = wifi_get_dongle_type();
	    ALOGD("current module is %d", module);
	    if(module == Ralink_5570)
	    {
	        if (strcmp(fwpath, "AP") == 0) {
	            return wifi_change_wifi_mode(AP_MODE);
	        }
	        return wifi_change_wifi_mode(STA_MODE);
	    }else if(module == MTK_7601)
		{
			 if (strcmp(fwpath, "AP") == 0) {
	            return wifi_change_wifi_mode(AP_MODE);
	        }
	        return wifi_change_wifi_mode(STA_MODE);
		}else if(module == MTK_7662)
		{
			 if (strcmp(fwpath, "AP") == 0) {
	            return wifi_change_wifi_mode(AP_MODE);
	        }
	        return wifi_change_wifi_mode(STA_MODE);
	    }else if(module == MTK_7603)
		{
			 if (strcmp(fwpath, "AP") == 0) {
	            return wifi_change_wifi_mode(AP_MODE);
	        }
	        return wifi_change_wifi_mode(STA_MODE);
	    }else if(module == MTK_7601)
		{
			 if (strcmp(fwpath, "AP") == 0) {
	            return wifi_change_wifi_mode(AP_MODE);
	        }
	        return wifi_change_wifi_mode(STA_MODE);
	    }
		
	    int len;
	    int fd;
	    int ret = 0;

	    if (!fwpath)
	        return ret;
	    fd = TEMP_FAILURE_RETRY(open(WIFI_DRIVER_FW_PATH_PARAM, O_WRONLY));
	    if (fd < 0) {
	        ALOGE("Failed to open wlan fw path param (%s)", strerror(errno));
	        return -1;
	    }
	    len = strlen(fwpath) + 1;
	    if (TEMP_FAILURE_RETRY(write(fd, fwpath, len)) != len) {
	        ALOGE("Failed to write wlan fw path param (%s)", strerror(errno));
	        ret = -1;
	    }
	    close(fd);
	    return ret;
	}
   
}

//for ralink 5570 only
int is_wifi_driver_unloaded() {

   int type = wifi_get_dongle_type();
   
   ALOGD("%s dongle_type =  %d", __FUNCTION__, type);

   if(type != Ralink_5570 && type != MTK_7601 && type != MTK_7662 && type != MTK_7603 && type != RTK_8188)
   {
        return -1;
   }

    char driver_status[PROPERTY_VALUE_MAX];
#ifdef WIFI_DRIVER_MODULE_PATH
    FILE *proc;
    char line[sizeof(DRIVER_MODULE_TAG)+10];
#endif

    if (!property_get(DRIVER_PROP_NAME, driver_status, NULL)
            || strcmp(driver_status, "unloaded") != 0) {
        LOGD ("driver not loaded driver_status [%s]\n", driver_status);
        return 0;  /* driver not unloaded */
    }
#ifdef WIFI_DRIVER_MODULE_PATH
    /*
     * If the property says the driver is loaded, check to
     * make sure that the property setting isn't just left
     * over from a previous manual shutdown or a runtime
     * crash.
     */
    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        ALOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
   //     property_set(DRIVER_PROP_NAME, "failed");
        return 0;
    }
    while ((fgets(line, sizeof(line), proc)) != NULL) {
        if(wifi_get_dongle_mode() == AP_MODE)
        {
        	if(type == Ralink_5570)
    		{
		        if (strncmp(line, "rtutil5572ap", strlen("rtutil5572ap")) == 0) {
		            fclose(proc);
		            return 0;
		            }
    		}else if(type == MTK_7601)
			{
				if (strncmp(line, "mt7601Uap", strlen("mt7601Uap")) == 0) {
		            fclose(proc);
		            return 0;
		            }
			}else if(type == MTK_7662)
			{
				if (strncmp(line, "mt7662u_ap", strlen("mt7662u_ap")) == 0) {
		            fclose(proc);
		            return 0;
		            }
			}else if(type == MTK_7603)
			{
				if (strncmp(line, "mt7603u_ap", strlen("mt7603u_ap")) == 0) {
		            fclose(proc);
		            return 0;
		            }
			}
        }
        else
        {
            if (strncmp(line, "cfg80211", strlen("cfg80211")) == 0) {
                fclose(proc);
                return 0;
                }
        }
    }
    fclose(proc);
    return 1;
#else
    return 1;
#endif
}

int wifi_check_unload_is_done(int mode)
{
    FILE *proc = NULL;
    char line[sizeof(DRIVER_MODULE_TAG)+10];
    int count = 0;
    bool doAgain = true;
    do{
        if(count >= 30)
        {
            LOGD ("wifi_check_unload_is_done 10s time out driver still in \n");
            return -1;
        }
        usleep(100000);
        count++;

        doAgain = false;
        if(is_wifi_driver_unloaded() <= 0)
        {
             doAgain = true;
        }
    }while(doAgain);

    LOGD ("wifi_check_unload_is_done driver not exist \n");
    return 0;
}
int wifi_change_wifi_mode(int mode)
{
    int ret = 0;
    LOGD("%s: change to %d\n", __FUNCTION__,mode);
    int cur_mode = wifi_get_dongle_mode();
    if(cur_mode == mode)
    {
           if(cur_mode == AP_MODE)
            {

                ifc_init();
                ret = ifc_down(WIFI_TEST_INTERFACE_AP);
				usleep(400000);
                ret = ifc_up(WIFI_TEST_INTERFACE_AP);
                LOGD ("ifc_up %d",ret);
                ifc_close();
  			}
        return 0;
    }
    LOGD ("wifi_change_wifi_mode unload driver");
    do
    {
    // down interface to unload driver
        if(cur_mode == STA_MODE)
        {
            ifc_init();
            ret = ifc_down(WIFI_TEST_INTERFACE);
            LOGD ("ifc_down WIFI_TEST_INTERFACE %d",ret);

            ret = ifc_down(WIFI_TEST_INTERFACE_P2P);
            LOGD ("ifc_downWIFI_TEST_INTERFACE_P2P %d",ret);
            ifc_close();
        }
        else
        {
            ifc_init();
            ret = ifc_down(WIFI_TEST_INTERFACE_AP);
            LOGD ("ifc_down WIFI_TEST_INTERFACE_AP %d",ret);
             ifc_close();
        }
		
		LOGD("%s: line= %d\n", __FUNCTION__, __LINE__);
        ret = wifi_unload_driver();
        if(ret != 0)
        {
            LOGD ("wifi_unload_driver fail in changemode %d",ret);
            break;
        }
            char driver_status[PROPERTY_VALUE_MAX];

            if (property_get(DRIVER_PROP_NAME, driver_status, NULL)) {
                LOGD ("d===== driver_status [%s]\n", driver_status);
            }

        if(wifi_check_unload_is_done(cur_mode) == -1)
        {
            LOGD ("wifi_check_unload_is_done fail in changemode %d",ret);
            return -1;
        }
        wifi_set_dongle_mode(mode);
        char dongle_mode_read[PROPERTY_VALUE_MAX];
        ret = wifi_load_driver();
        if(ret != 0)
        {
            LOGD ("wifi_load_driver fail in changemode %d",ret);
            break;
        }
        LOGD ("wifi_change_wifi_mode change successfully");
        return 0;
    }while(0);

    LOGD ("wifi_change_wifi_mode fail driver");
    return -1;
}
/*
* This function used to enable or disable the WoWL function. 
*WoWL: Wake over WLAN
*@param ifname, interface name wlan0..
*@param : 0, disalbe , 1 ,enable
*@return : 0, operation success, other : fail.
*/

int wifi_enalbe_wowl(const char * ifname, int enable)
{
#if 0
    char s_cmd[100] = {0};

    if(NULL == ifname )
    {
        LOGD("wifi_anable_wowl : ifname is NULL");        
        return -1;
    }
    
    sprintf(s_cmd, "/sbin/sh iwpriv %s set wow_enable=%d", ifname, enable);    
    //sprintf(s_cmd, "/sbin/sh ls");    
    LOGD("wifi_anable_wowl :%s", s_cmd);
    system(s_cmd);
#endif
    LOGD("wifi_enable_wowl : %d \n", enable);
    if(enable)
    {
        property_set("ctl.start", "wifi_set:wowl_on");
    }else
    {        
        property_set("ctl.start", "wifi_set:wowl_off");
    }

    return 0;
}

/*For soft ap*/

#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
int wifi_ap_start_supplicant()
{
    int ret = 0;
    
    ALOGD("wifi_ap_start_supplicant");

    //Start ap daemon
    if (wifi_start_supplicant(2) < 0) {
        ret = -1;
        ALOGD("wifi_ap_start_supplicant fail\n");
    }
    
    return ret;
}

int wifi_ap_stop_supplicant()
{
    int ret = 0;
    char buffer[32];
    size_t len = 32;
    char supp_status[PROPERTY_VALUE_MAX] = {'\0'};
    int count = 50; /* wait at most 5 seconds for completion */    
    
    ALOGD("wifi_ap_stop_supplicant");
    
    if(wifi_stop_supplicant(2) < 0) {
        ret = -1;
    }    
    
    return ret;
}


#endif




