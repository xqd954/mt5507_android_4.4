/*
 *   Copyright (C) 2006 The Android Open Source Project
 *   
 *   ©2010-2013 TCL CORPORATION All Rights Reserved.
 *  
 */

#define LOG_TAG "Wifimultisupport"
#include "utils/Log.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "jni.h"


/*****************************************/
#include <dlfcn.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <cctype>
#include "hardware_legacy/wifi.h"
#include "libwpa_client/wpa_ctrl.h"
#include "cutils/log.h"
#include "cutils/memory.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include "private/android_filesystem_config.h"
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif

#define HAL_SHARE_LIBRARY "libhardware_legacy.so"

int (*T_get_wifi_deviceNum)();
int (*T_get_wifi_switch)();
int (*T_set_wifi_switch)(int state);
void *handle = NULL;

struct wifi_vid_pid {
	unsigned short int vid;
	unsigned short int pid;
};

//atheros 1021
static struct wifi_vid_pid ath_vid_pid_tables[] = 
{
	{ 0x0cf3, 0x1021 }
};

static struct wifi_vid_pid rel_vid_pid_tables[] =
{
	//realtek
	{ 0x0bda, 0x8194 },
	{ 0x0bda, 0x8178 },//cu
	{ 0x0bda, 0x8176 },
	{ 0x0bda, 0x0179 },
	{ 0x0bda, 0x818b },//eu

	{ 0x0bda, 0x8812 },//881X
	{ 0x0bda, 0x881A },
	{ 0x0bda, 0x881B },
	{ 0x0bda, 0x881C },

	//D-link
	{ 0x2001, 0x3308 },
	{ 0x2001, 0x330a },
	//ralink
	{ 0x148f, 0x5372 },
	{ 0x0e8d, 0x7603 },
	{ 0x148f, 0x7601 }
};

static int ath_table_len = sizeof(ath_vid_pid_tables) / sizeof(struct wifi_vid_pid);
static int rel_table_len = sizeof(rel_vid_pid_tables) / sizeof(struct wifi_vid_pid);


/*****************************************/
static int loadlibrary()
{
	handle = dlopen(HAL_SHARE_LIBRARY, RTLD_LAZY);
	if(!handle) {
		ALOGE("%s\n", dlerror());
		//exit(EXIT_FAILURE);
		return -1;
	}

	dlerror(); /* Clear any existing error */

	return 0;
}

/*****************************************/

int is_wifi_device(int vid, int pid)
{
	int k;

	//ALOGD("enter %s %d dev_name vid %x pid %x \n", __FUNCTION__, __LINE__, vid, pid);

	for(k = 0; k < ath_table_len; k++) {
		if(vid == ath_vid_pid_tables[k].vid && \
				pid == ath_vid_pid_tables[k].pid) {
			return 1;
		}
	}

	for(k = 0; k < rel_table_len; k++) {
		if(vid == rel_vid_pid_tables[k].vid && \
				pid == rel_vid_pid_tables[k].pid) {
			return 1;
		}
	}
	return 0;
}

static int badname(const char *name)
{
	//ALOGD(" ######## badname ########");
	if(*name == '.' || isalpha(*name)) {
		return 1;
	}
	while(*name) {
		//ALOGD("enter %s %d dev_name %s \n", __FUNCTION__, __LINE__, name);
		if(':' == (*name++)) {
			//ALOGD("enter %s %d dev_name \n", __FUNCTION__, __LINE__);
			return 1;
		} else {
			//ALOGD("enter %s %d dev_name \n", __FUNCTION__, __LINE__);
		}
	}
	return 0;
}

int find_existing_devices()
{
	//ALOGD(" ######## find_existing_devices ######## ");
	char devname[60], detail[8], busdir_path[32];
	DIR *busdir, *devdir;
	struct dirent *de;
	int num, fd, pid, vid;
	int done = 0;
	num = 0;

	busdir = opendir("/sys/bus/usb/devices");
	snprintf(busdir_path, sizeof busdir_path, "%s", "/sys/bus/usb/devices");
	//if(busdir == 0) return 1;
	if(!busdir)	
		return -ENOTDIR;

	while((de = readdir(busdir)) != 0 && !done) {
		//ALOGD("enter %s %d %s/%s \n", __FUNCTION__, __LINE__, busdir_path, de->d_name);
		if(badname(de->d_name)) continue;
		snprintf(devname, sizeof devname, "%s/%s/%s", busdir_path, de->d_name, "idProduct");

		memset(detail, 0, 8);
		//ALOGD("enter %d devname %s \n", __LINE__, devname);
		fd = open(devname, O_RDONLY);
		if(fd < 0) {
			ALOGD("enter %d devname %s \n", __LINE__, strerror(errno));
		}
		read(fd, detail, 31);
		if(fd < 0) {
			ALOGE("enter %d devname %s \n", __LINE__, strerror(errno));
		}
		pid = atoi(detail);
		sscanf(detail, "%x", &pid);

		close(fd);

		memset(detail, 0, 8);
		snprintf(devname, sizeof devname, "%s/%s/%s", busdir_path, de->d_name, "idVendor");
		
		fd = open(devname, O_RDONLY);
		read(fd, detail, 31);

		close(fd);
		vid = atoi(detail);
		sscanf(detail, "%x", &vid);

		done = is_wifi_device(vid, pid);
		if(1 == done) {
			num++;
		}
		done = 0;
		//ALOGD("enter %s %d %s is_wifi_device %d \n", __FUNCTION__, __LINE__, detail, num);
	}
	closedir(busdir);
	return num;
}


int wifi_get_deviceNum()
{
	ALOGE(" ########### wifi_get_deviceNum #########");
	int num = 0;
	num = find_existing_devices();
	ALOGE("find_existing_devices exit !! %d\n", num);

	return num;
}
int set_wifi_switch(int state)
{
	return 1; //add by cuix@tcl.com 2014.1.7
	int ret = 0;

	ret = loadlibrary();
	if(ret < 0) {
		ALOGE("loadlibrary failed");
		//exit(EXIT_FAILURE);
		return -1;
	}

	T_set_wifi_switch = (int (*)(int))dlsym(handle, "set_wifi_switch");
	if(NULL == T_set_wifi_switch) {
		ALOGE("there is no symbol named set_wifi_switch, function: %s line: %d",
				__FUNCTION__, __LINE__);
		//exit(EXIT_FAILURE);
		return -1;
	}
	
	int status = T_set_wifi_switch(state);

	dlclose(handle);

	return status;
}

int get_wifi_switch()
{
	return 1; //add by cuix@tcl.com 2014.1.7
	int ret = 0;

	ret = loadlibrary();
	if(ret < 0) {
		ALOGE("loadlibrary failed");
		//exit(EXIT_FAILURE);
		return 0;
	}

	T_get_wifi_switch = (int (*)())dlsym(handle, "get_wifi_switch");
	if(NULL == T_get_wifi_switch) {
		ALOGE("there is no symbol named get_wifi_switch, function: %s line: %d",
				__FUNCTION__, __LINE__);
		//exit(EXIT_FAILURE);
		return 0;
	}

	int status = T_get_wifi_switch();

	dlclose(handle);

	return status;
}

//add by Xi'An bsp cx,cuix@tcl.com 2013-04-02
static jint android_net_wifi_getDeviceNum(JNIEnv *env, jobject)
{
	int num = 0;
	num = wifi_get_deviceNum();
	return (jint)num;
}

static jint android_net_wifi_getWifiSwitch(JNIEnv *env, jobject)
{
	int state = 0;
	//ALOGD(" ******** functionname: %s  line: %d ********", __FUNCTION__, __LINE__);
	state = get_wifi_switch();
	return (jint)state;
}

static jint android_net_wifi_setWifiSwitch(JNIEnv *env, jobject, int state)
{
	jint status = 0;
	//ALOGD(" ******** functionname: %s  line: %d  state %d ********", __FUNCTION__,  __LINE__, state);
	status = set_wifi_switch((jint)state);
	return (jint)status;
}
//end by Xi'An bsp cx,cuix@tcl.com 2013-04-02

// ----------------------------------------------------------------------------

/*
 * Array of methods.
 *
 * Each entry has three fields: the name of the method, the method
 * signature, and a pointer to the native implementation.
 */
static const JNINativeMethod gMethods[] = {
	//add by Xi'An bsp cx,cuix@tcl.com 2013-04-02
	{ "getDeviceNum","()I", (void *)android_net_wifi_getDeviceNum },
	{ "getWifiSwitch", "()I", (void *)android_net_wifi_getWifiSwitch },
	{ "setWifiSwitch", "(Z)I", (void *)android_net_wifi_setWifiSwitch },
	//add by Xi'An bsp cx,cuix@tcl.com 2013-04-02

};
/*
 * Explicitly register all methods for our class.
 *
 * While we're at it, cache some class references and method/field IDs.
 *
 * Returns 0 on success.
 */
static int registerMethods(JNIEnv* env) {
    static const char* const kClassName =
        "com/tcl/devicemanager/WifiMultiSupport";
    jclass clazz;

    /* look up the class */
    clazz = env->FindClass(kClassName);
    if (clazz == NULL) {
        ALOGE("Can't find class %s\n", kClassName);
        return -1;
    }

    /* register all the methods */
    if (env->RegisterNatives(clazz, gMethods,
            sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK)
    {
        ALOGE("Failed registering methods for %s\n", kClassName);
        return -1;
    }

    /* fill out the rest of the ID cache */
    return 0;
}

/*
 * This is called by the VM when the shared library is first loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (registerMethods(env) != 0) {
        ALOGE("ERROR: Wifimultisupport native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}
