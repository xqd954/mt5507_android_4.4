/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/wireless.h>

#include <openssl/evp.h>
#include <openssl/sha.h>

#define LOG_TAG "SoftapController"
#include <cutils/log.h>
#include <netutils/ifc.h>
#include <private/android_filesystem_config.h>
#include "wifi.h"
#include "ResponseCode.h"

#include "SoftapController.h"

#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
#include <arpa/inet.h>
#endif

#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
#define WLAN_INTERFACE "wlan0"
#define SOFTAP_INTERFACE "p2p0"
#define SOFTAP_REAL_INTERFACE "p2p-ap0-0"
#endif

#define MTK_7601                                7
#define MTK_7662                                8
#define MTK_7603                                9

#define HAVE_HOSTAPD 1
static const char HOSTAPD_CONF_FILE[]    = "/data/misc/wifi/hostapd.conf";
static const char HOSTAPD_BIN_FILE[]    = "/system/bin/hostapd";
//wait to enhance
static const char RALINK_AP_DAT[] = "/data/misc/wifi/RT2870AP.dat";

SoftapController::SoftapController()
    : mPid(0) {}

SoftapController::~SoftapController() {
}

int SoftapController::startSoftap() {
    pid_t pid = 1;
	int ret = 0;
	int dongle_type = 0;
	dongle_type	= wifi_get_dongle_type();
	
    if (mPid) {
        ALOGE("SoftAP is already running");
        return ResponseCode::SoftapStatusResult;
    }

	if(dongle_type == MTK_7662 || dongle_type == MTK_7603 || dongle_type == MTK_7601)//7662 run new flow,others run old folw
	{
	    if ((pid = fork()) < 0) {
	        ALOGE("fork failed (%s)", strerror(errno));
	        return ResponseCode::ServiceStartFailed;
	    }
	}
    if (!pid) {
		if(dongle_type == MTK_7662 || dongle_type == MTK_7603 || dongle_type == MTK_7601)//7662 run new flow,others run pld folw
		{
	        ensure_entropy_file_exists();
	        if (execl(HOSTAPD_BIN_FILE, HOSTAPD_BIN_FILE,
	                  "-e", WIFI_ENTROPY_FILE,
	                  HOSTAPD_CONF_FILE, (char *) NULL)) {
	            ALOGE("execl failed (%s)", strerror(errno));
	        }
		
	        ALOGE("SoftAP failed to start");
	        return ResponseCode::ServiceStartFailed;
		}	
    } 
	else {
        //*mBuf = 0;
		//ret = execl("/system/bin/netcfg", "/system/bin/netcfg", "wlan0", "up",(char *) NULL);
		system("netcfg wlan0 up");
		ret = 0;
        //ret = setCommand(mIface, "AP_BSS_START");
        if (ret) {
            ALOGE("Softap startap - failed: %d", ret);
        }
        else {
			if(dongle_type == MTK_7662 || dongle_type == MTK_7603 || dongle_type == MTK_7601)
			{
        		mPid = pid;
			}
        	ALOGD("Softap startap - Ok");
        	usleep(AP_BSS_START_DELAY);
    	}
    }
    return ResponseCode::SoftapStatusResult;
}

int SoftapController::stopSoftap() {
    int ret;
	int dongle_type = 0;
	dongle_type	= wifi_get_dongle_type();
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
    ALOGD("stopSoftap: %s", mIface);
    if (mPid == 0) {
        ALOGE("Softap already stopped");
        return 0;
    }

    if (mSock < 0) {
        ALOGE("Softap stopap - failed to open socket");
        return -1;
    }
    *mBuf = 0;
    mPid = 0;
    ALOGV("Softap service stopped: %d", ret);
 

    ret = setConfig("P2P_GROUP_REMOVE p2p-ap0-0", NULL);
    if (ret != 0)
        ALOGV("stopSoftap P2P_GROUP_REMOVE p2p-ap0-0 fail, ret = %d", ret);
    
    stopDriver(mIface);
    usleep(AP_BSS_STOP_DELAY);
#else

    if (mPid == 0) {
        ALOGE("SoftAP is not running");
        return ResponseCode::SoftapStatusResult;
    }

#ifdef HAVE_HOSTAPD
	if(dongle_type == MTK_7662 || dongle_type == MTK_7603 || dongle_type == MTK_7601)
	{
	    ALOGD("Stopping Softap service");
	    kill(mPid, SIGTERM);
	    waitpid(mPid, NULL, 0);
	}
#endif



    //*mBuf = 0;
	//ret = execl("/system/bin/netcfg", "/system/bin/netcfg", "wlan0", "down",(char *) NULL);
    //ret = setCommand(mIface, "AP_BSS_STOP");
	//ret = execl("/system/bin/netcfg", "/system/bin/netcfg", "wlan0", "up",(char *) NULL);
    system("netcfg wlan0 down");
    ret = 0;
    mPid = 0;
    ALOGD("Softap service stopped: %d", ret);
    usleep(AP_BSS_STOP_DELAY);
#endif    
    return ResponseCode::SoftapStatusResult;
}

bool SoftapController::isSoftapStarted() {
    return (mPid != 0);
}
#if 0
int SoftapController::addParam(int pos, const char *cmd, const char *arg)
{
    if (pos < 0)
        return pos;
	pos = 0;
    if ((unsigned)(pos + strlen(cmd) + strlen(arg) + 1) >= sizeof(mBuf)) {
        ALOGE("Command line is too big");
        return -1;
    }
    pos += sprintf(&mBuf[pos], "%s=%s", cmd, arg);
    return pos;
}
#endif

#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
int SoftapController::startDriver(char *iface) {
    int ret = 0;

    ALOGD("startDriver: %s", iface);
    
    if (mSock < 0) {
        ALOGE("Softap driver start - failed to open socket");
        return -1;
    }
    if (!iface || (iface[0] == '\0')) {
        ALOGD("Softap driver start - wrong interface");
        iface = mIface;
    }
    if (mDaemonState == 1) {
        ALOGD("Softap startap - daemon is already running");
        return -1;
    }        

    //Init wlan0 firstly.

    ifc_init();
    ret = ifc_up(WLAN_INTERFACE);
    
    //Init p2p0
    ret = ifc_up(iface);
    ifc_close();    

    ALOGD("chmod 777 to p2p0");
    system("chmod 777 /data/misc/wifi/wpa_supplicant/p2p0");
    
    *mBuf = 0;
    ret = wifi_ap_start_supplicant();
    if (ret < 0) {
        ALOGE("Softap daemon start: %d", ret);
        return ret;
    }

    mDaemonState = 1;
    
    usleep(AP_DRIVER_START_DELAY);
    ALOGV("Softap daemon start: %d", ret);
    return ret;
}

int SoftapController::stopDriver(char *iface) {
    int ret;

    ALOGD("stopDriver: %s", iface);
    
    if (mSock < 0) {
        ALOGE("Softap driver stop - failed to open socket");
        return -1;
    }
    if (!iface || (iface[0] == '\0')) {
        ALOGD("Softap driver stop - wrong interface");
        iface = mIface;
    }
    *mBuf = 0;

    ifc_init();

    ret = ifc_down(iface);
    if (ret != 0)
        ALOGD("Softap driver stop - turn down %s fail\n", iface);
    //Set wlan0 down

    ret = ifc_down(SOFTAP_INTERFACE);
    if (ret != 0)
        ALOGD("Softap driver stop - turn down p2p0 fail\n");
    
    ret = ifc_down(WLAN_INTERFACE);
    if (ret != 0)
        ALOGD("Softap driver stop - turn down p2p-ap0-0 fail\n");
    
    ifc_close();
    if (ret < 0) {
        ALOGE("Softap %s down: %d", iface, ret);
    }

    ret = wifi_ap_stop_supplicant();
    
    mDaemonState = 0;
    
    ALOGV("Softap daemon stop: %d", ret);
    return ret;
}

int SoftapController::setCommand(char *iface, const char *cmd, unsigned buflen) {
    int connectTries = 0;
    unsigned replybuflen;
    int ret = 0;
    
    ALOGD("Enter setCommand");
    if (mDaemonState != 1) {
        ALOGD("setCommand - daemon is not running");
        startDriver(mIface);
    }        
    
    if (buflen == 0) {
        replybuflen = SOFTAP_MAX_BUFFER_SIZE;
    } 
    else {
        replybuflen = buflen;
    }
    
    // <1> connect to the daemon
    while (true) {
        ALOGD("try to connect to daemon");
        if (wifi_connect_to_supplicant(mIface) == 0) {
            ALOGD("connect to daemon");
            break;
        }
        //maximum delay 12s
        if (connectTries++ < 40) {
            sched_yield();
            //ALOGD("softap sleep %d us\n", AP_CONNECT_TO_DAEMON_DELAY);
            usleep(AP_CONNECT_TO_DAEMON_DELAY);
        } else {
            ALOGE("connect to daemon failed!");
            return -1;
        }
    }
    /*when enable softap,  set wifi DRIVER P2P_GO_SOFTAP 1 first*/
    if(strncmp(cmd, "P2P_GROUP_ADD", strlen("P2P_GROUP_ADD")) == 0)
    {
        wifi_command(mIface, "DRIVER P2P_GO_SOFTAP 1", mBuf, &buflen);  
    }
    
    if (wifi_command(mIface, cmd, mBuf, &buflen) != 0) {
        ALOGE("Command failed: \"%s\"", cmd);
        ret = -1;
    }
    else {
        ALOGD("Command OK: \"%s\"", cmd);
        mBuf[buflen] = '\0';
    }
    
    wifi_close_supplicant_connection(mIface);
    
    return ret;
}
int SoftapController::setConfig(const char *cmd, const char *arg)
{
    ALOGD("Enter set config ");

    char cmd_str[SOFTAP_MAX_BUFFER_SIZE];
    
    snprintf(cmd_str, SOFTAP_MAX_BUFFER_SIZE, "%s", cmd);
    ALOGD("cmd_str = %s", cmd_str);
    return setCommand(mIface, cmd_str, 0);
}

#endif


/*/mBuf
 * Arguments:
 *  argv[2] - wlan interface
 *  argv[3] - SSID
 *  argv[4] - Broadcast/Hidden
 *  argv[5] - Channel
 *  argv[6] - Security
 *  argv[7] - Key
 */
int SoftapController::setSoftap(int argc, char *argv[]) {
    char psk_str[2*SHA256_DIGEST_LENGTH+1];
    int ret = ResponseCode::SoftapStatusResult;
    int i = 0;
    int fd;
    int hidden = 0;
    char *ssid;
    int channel = AP_CHANNEL_DEFAULT;
    char *wbuf = NULL;
    char *fbuf = NULL;   
    int dongle_type = 0;
	dongle_type	= wifi_get_dongle_type();
#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
    char *iface;
#endif

    if (argc < 5) {
        ALOGE("Softap set is missing arguments. Please use:");
        ALOGE("softap <wlan iface> <SSID> <hidden/broadcast> <channel> <wpa2?-psk|open> <passphrase>");
        return ResponseCode::CommandSyntaxError;
    }

    if (!strcasecmp(argv[4], "hidden"))
        hidden = 1;

#ifdef CONFIG_P2P_AUTO_GO_AS_SOFTAP
    
    ALOGD("SetSoftap : define CONFIG_P2P_AUTO_GO_AS_SOFTAP");
    if (mSock < 0) {
        ALOGE("Softap set - failed to open socket");
        return -1;
    }
    if (argc < 4) {
        ALOGE("Softap set - missing arguments");
        return -1;
    }
    
    strncpy(mIface, SOFTAP_INTERFACE, sizeof(mIface));
    iface = argv[2];
    
    ALOGD("SetSoftap : mIface = %s, argc = %d", mIface, argc);
    ALOGD("argv[0] = %s", argv[0]);
    ALOGD("argv[1] = %s", argv[1]);
    ALOGD("argv[2] = %s", argv[2]);
    ALOGD("argv[3] = %s", argv[3]);
    ALOGD("argv[4] = %s", argv[4]);
    ALOGD("argv[5] = %s", argv[5]);
    
    /* Create command line */
    if (argc > 3) {
        ssid = argv[3];
    } else {
        ssid = (char *)"AndroidAP";
    }
    
    if (argc > 7) {
        asprintf(&wbuf, "ssid=%s wpapsk=%s key_mgmt=WPA-PSK pairwise=CCMP proto=WPA2",ssid, argv[7]);
        if (!strcmp(argv[6], "wpa-psk")) {
            generatePsk(argv[3], argv[7], psk_str);
            asprintf(&wbuf, "ssid=%s wpapsk=%s key_mgmt=WPA-PSK pairwise=TKIP proto=WPA",ssid, argv[7]);
        } else if (!strcmp(argv[6], "wpa2-psk")) {
            generatePsk(argv[3], argv[7], psk_str);
            asprintf(&wbuf, "ssid=%s wpapsk=%s key_mgmt=WPA-PSK pairwise=CCMP proto=WPA2",ssid, argv[7]);
        } else if (!strcmp(argv[6], "open")) {
            asprintf(&wbuf, "ssid=%s wpapsk= key_mgmt=NONE pairwise=NONE proto=WPA2",ssid);
        }
    } else if (argc > 6) {
        if (!strcmp(argv[6], "open")) {
            asprintf(&wbuf, "ssid=%s wpapsk= key_mgmt=NONE pairwise=NONE proto=WPA2",ssid);
        }
    } else {
        asprintf(&wbuf, "ssid=%s wpapsk= key_mgmt=NONE pairwise=NONE proto=WPA2",ssid);
    }
    
    asprintf(&fbuf, "P2P_GROUP_ADD softap %s", wbuf);

    ALOGD("fbuf = %s", fbuf);
    ret = setConfig(fbuf, NULL);

    if (wbuf != NULL)
       free(wbuf);

    if (fbuf != NULL)
       free(fbuf);

    return ret;
#endif       

    

if(dongle_type == MTK_7662 || dongle_type == MTK_7603 || dongle_type == MTK_7601)
{
    if (argc >= 5) {
        channel = atoi(argv[5]);
        if (channel <= 0)
            channel = AP_CHANNEL_DEFAULT;
    }


    asprintf(&wbuf, "interface=%s\ndriver=nl80211\nctrl_interface="
            "/data/misc/wifi/hostapd\nssid=%s\nchannel=%d\nieee80211n=1\n"
            "hw_mode=g\nignore_broadcast_ssid=%d\n",
            argv[2], argv[3], channel, hidden);

    if (argc > 7) {
        if (!strcmp(argv[6], "wpa-psk")) {
            generatePsk(argv[3], argv[7], psk_str);
            asprintf(&fbuf, "%swpa=1\nwpa_pairwise=TKIP CCMP\nwpa_psk=%s\n", wbuf, psk_str);
        } else if (!strcmp(argv[6], "wpa2-psk")) {
            generatePsk(argv[3], argv[7], psk_str);
            asprintf(&fbuf, "%swpa=2\nrsn_pairwise=CCMP\nwpa_psk=%s\n", wbuf, psk_str);
        } else if (!strcmp(argv[6], "open")) {
            asprintf(&fbuf, "%s", wbuf);
        }
    } else if (argc > 6) {
        if (!strcmp(argv[6], "open")) {
            asprintf(&fbuf, "%s", wbuf);
        }
    } else {
        asprintf(&fbuf, "%s", wbuf);
    }

    fd = open(HOSTAPD_CONF_FILE, O_CREAT | O_TRUNC | O_WRONLY | O_NOFOLLOW, 0660);
    if (fd < 0) {
        ALOGE("Cannot update \"%s\": %s", HOSTAPD_CONF_FILE, strerror(errno));
        free(wbuf);
        free(fbuf);
        return ResponseCode::OperationFailed;
    }
    if (write(fd, fbuf, strlen(fbuf)) < 0) {
        ALOGE("Cannot write to \"%s\": %s", HOSTAPD_CONF_FILE, strerror(errno));
        ret = ResponseCode::OperationFailed;
    }
    close(fd);
    free(wbuf);
    free(fbuf);

    /* Note: apparently open can fail to set permissions correctly at times */
    if (chmod(HOSTAPD_CONF_FILE, 0660) < 0) {
        ALOGE("Error changing permissions of %s to 0660: %s",
                HOSTAPD_CONF_FILE, strerror(errno));
        unlink(HOSTAPD_CONF_FILE);
        return ResponseCode::OperationFailed;
    }

    if (chown(HOSTAPD_CONF_FILE, AID_SYSTEM, AID_WIFI) < 0) {
        ALOGE("Error changing group ownership of %s to %d: %s",
                HOSTAPD_CONF_FILE, AID_WIFI, strerror(errno));
        unlink(HOSTAPD_CONF_FILE);
        return ResponseCode::OperationFailed;
    }
}
else
{
	if (argc > 3) {
        ssid = argv[3];
    } else {
        ssid = (char *)"AndroidAP";
    }

    asprintf(&wbuf, "Default\nBssidNum=1\nWirelessMode=9\nCountryRegion=5\n"
            "Channel=6\nSSID=%s\nBeaconPeriod=100\nTxPower=100\nRTSThreshold=2347\nFragThreshold=2346\n"
			"TxBurst=1\nPktAggregate=0\nWmmCapable=1\n"
			"HT_RDG=1\nHT_EXTCHA=0\nHT_OpMode=0\nHT_MpduDensity=4\nHT_BW=0\nHT_BADecline=0\nHT_AutoBA=1\n"
			"HT_AMSDU=0\nHT_BAWinSize=64\nHT_GI=1\nHT_MCS=33\nHT_MIMOPSMode=3\nHT_DisallowTKIP=1\nHT_STBC=0\n",
            ssid);

    if (argc > 4) {
        if (!strcmp(argv[6], "wpa-psk")) {
            asprintf(&fbuf, "%sAuthMode=WPAPSK\nEncrypType=TKIPAES\nWPAPSK=%s\nDefaultKeyID=2\n", wbuf, argv[7]);
        } else if (!strcmp(argv[6], "wpa2-psk")) {
            asprintf(&fbuf, "%sAuthMode=WPA2PSK\nEncrypType=AES\nWPAPSK=%s\nDefaultKeyID=2\n", wbuf, argv[7]);
        } else if (!strcmp(argv[6], "open")) {
            asprintf(&fbuf, "%sAuthMode=OPEN\nEncrypType=NONE\nDefaultKeyID=1\n", wbuf);
        }
    } else {
        asprintf(&fbuf, "%sAuthMode=OPEN\nEncrypType=NONE\nDefaultKeyID=1\n", wbuf);
    }

    fd = open(RALINK_AP_DAT, O_CREAT | O_TRUNC | O_WRONLY, 0660);
    if (fd < 0) {
        ALOGE("Cannot update \"%s\": %s", RALINK_AP_DAT, strerror(errno));
        free(wbuf);
        free(fbuf);
        return -1;
    }
    if (write(fd, fbuf, strlen(fbuf)) < 0) {
        ALOGE("Cannot write to \"%s\": %s", RALINK_AP_DAT, strerror(errno));
        ret = -1;
    }
    close(fd);
    free(wbuf);
    free(fbuf);

    /* Note: apparently open can fail to set permissions correctly at times */
    if (chmod(RALINK_AP_DAT, 0660) < 0) {
        ALOGE("Error changing permissions of %s to 0660: %s",
                RALINK_AP_DAT, strerror(errno));
        unlink(RALINK_AP_DAT);
        return -1;
    }

    if (chown(RALINK_AP_DAT, AID_SYSTEM, AID_WIFI) < 0) {
        ALOGE("Error changing group ownership of %s to %d: %s",
                RALINK_AP_DAT, AID_WIFI, strerror(errno));
        unlink(RALINK_AP_DAT);
        return -1;
    }

	usleep(AP_SET_CFG_DELAY);

#if 0

 /* Create command line */
	*mBuf = 0;

	char *key_buf;
	int key_len = 0;
    if (argc > 4) {
        ssid = argv[4];
    } else {
        ssid = (char *)"AndroidAP";
    }
    i = addParam(i, "SSID", ssid);

	if (argc > 5){
		 if (!strcmp(argv[5], "wpa-psk")) {
			i = addParam(i, "AuthMode", "WPAPSK");
			setCommand(iface, "set");
			*mBuf = 0;
			i = addParam(i, "EncrypType", "TKIPAES");
			setCommand(iface, "set");
            *mBuf = 0;
			i = addParam(i, "DefaultKeyID", "2");
            setCommand(iface, "set");
            *mBuf = 0;
			key_len = strlen(argv[6]);
			key_buf = (char *)malloc(key_len);
			memset(key_buf, 0, key_len);
			memcpy(key_buf, argv[6], key_len);
			key_buf[key_len] = '\0';
			ALOGE("key_buf = %s argv[6] = %s key_len = %d",
                key_buf, argv[6], key_len);
			i = addParam(i, "WPAPSK", key_buf);
			ALOGE("mBuf = %s", mBuf);
            setCommand(iface, "set");
			free(key_buf);
            *mBuf = 0;
			usleep(AP_SET_CFG_DELAY);
        } else if (!strcmp(argv[5], "wpa2-psk")) {
			i = addParam(i, "AuthMode", "WPA2PSK");
            setCommand(iface, "set");
            *mBuf = 0;
            i = addParam(i, "EncrypType", "AES");
            setCommand(iface, "set");
            *mBuf = 0;
			i = addParam(i, "DefaultKeyID", "2");
            setCommand(iface, "set");
            *mBuf = 0;
			key_len = strlen(argv[6]);
			key_buf = (char *)malloc(key_len);
			memset(key_buf, 0, key_len);
            memcpy(key_buf, argv[6], key_len);
			key_buf[key_len] = '\0';
            i = addParam(i, "WPAPSK", key_buf);
			ALOGE("key_buf = %s argv[6] = %s key_len = %d",
                key_buf, argv[6], key_len);
			ALOGE("mBuf = %s", mBuf);
            setCommand(iface, "set");
			free(key_buf);
            *mBuf = 0;
			usleep(AP_SET_CFG_DELAY);
        } else if (!strcmp(argv[5], "open")) {
            i = addParam(i, "AuthMode", "OPEN");
            setCommand(iface, "set");
            *mBuf = 0;
            i = addParam(i, "EncrypType", "NONE");
            setCommand(iface, "set");
            *mBuf = 0;
        }
	} else {
		i = addParam(i, "AuthMode", "OPEN");
        setCommand(iface, "set");
        *mBuf = 0;
        i = addParam(i, "EncrypType", "NONE");
        setCommand(iface, "set");
        *mBuf = 0;
	}

    if (argc > 7) {
		i = addParam(i, "Channel", argv[7]);
        setCommand(iface, "set");
        *mBuf = 0;
    } else {
        i = addParam(i, "Channel", "6");
		setCommand(iface, "set");
        *mBuf = 0;
    }
    if ((i < 0) || ((unsigned)(i + 4) >= sizeof(mBuf))) {
        ALOGE("Softap set - command is too big");
        return i;
    }

	*mBuf = 0;
	i = addParam(i, "SSID", ssid);
	ret = setCommand(iface, "set");
    *mBuf = 0;
    /* system("iwpriv eth0 WL_AP_CFG ASCII_CMD=AP_CFG,SSID=\"AndroidAP\",SEC=\"open\",KEY=12345,CHANNEL=1,PREAMBLE=0,MAX_SCB=8,END"); */
    //ret = setCommand(iface, "AP_SET_CFG");
    if (ret) {
        ALOGE("Softap set - failed: %d", ret);
    }
    else {
        ALOGD("Softap set - Ok");
        usleep(AP_SET_CFG_DELAY);
    }
#endif
}
return ret;
}

/*
 * Arguments:
 *	argv[2] - interface name
 *	argv[3] - AP or P2P or STA
 */
int SoftapController::fwReloadSoftap(int argc, char *argv[])
{
    int i = 0;
    char *fwpath = NULL;

    if (argc < 4) {
        ALOGE("SoftAP fwreload is missing arguments. Please use: softap <wlan iface> <AP|P2P|STA>");
        return ResponseCode::CommandSyntaxError;
    }

    ALOGD("Softap fwrealod - INTERFACE IS %s",argv[3]);
    if (strcmp(argv[3], "AP") == 0) {
        fwpath = (char *)wifi_get_fw_path(WIFI_GET_FW_PATH_AP);
    } else if (strcmp(argv[3], "P2P") == 0) {
        fwpath = (char *)wifi_get_fw_path(WIFI_GET_FW_PATH_P2P);
    } else if (strcmp(argv[3], "STA") == 0) {
        fwpath = (char *)wifi_get_fw_path(WIFI_GET_FW_PATH_STA);
    }
    if (!fwpath)
        return ResponseCode::CommandParameterError;
    if (wifi_change_fw_path((const char *)fwpath)) {
        ALOGE("Softap fwReload failed");
        return ResponseCode::OperationFailed;
    }
    else {
        ALOGD("Softap fwReload - Ok");
    }
    return ResponseCode::SoftapStatusResult;
}

void SoftapController::generatePsk(char *ssid, char *passphrase, char *psk_str) {
    unsigned char psk[SHA256_DIGEST_LENGTH];
    int j;
    // Use the PKCS#5 PBKDF2 with 4096 iterations
    PKCS5_PBKDF2_HMAC_SHA1(passphrase, strlen(passphrase),
            reinterpret_cast<const unsigned char *>(ssid), strlen(ssid),
            4096, SHA256_DIGEST_LENGTH, psk);
    for (j=0; j < SHA256_DIGEST_LENGTH; j++) {
        sprintf(&psk_str[j*2], "%02x", psk[j]);
    }
}
