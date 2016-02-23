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

#define LOG_TAG "wifi"

#include "jni.h"
#include <ScopedUtfChars.h>
#include <utils/misc.h>
#include <android_runtime/AndroidRuntime.h>
#include <utils/Log.h>
#include <utils/String16.h>

#include "wifi.h"

//#include "c_pcl.h"
#include <sys/stat.h>
#define REPLY_BUF_SIZE 4096 // wpa_supplicant's maximum size.
#define EVENT_BUF_SIZE 2048
extern "C" {
    extern int c_pcl_set_wifi_wol_ctrl(int a);
}
namespace android {

static jint DBG = true;

static bool doCommand(JNIEnv* env, jstring jIface, jstring javaCommand,
                      char* reply, size_t reply_len) {
    ScopedUtfChars command(env, javaCommand);
    ScopedUtfChars ifname(env, jIface);
    if (command.c_str() == NULL) {
        return false; // ScopedUtfChars already threw on error.
    }

    if (DBG) {
        ALOGD("doCommand: %s", command.c_str());
    }

    --reply_len; // Ensure we have room to add NUL termination.
    if (::wifi_command(ifname.c_str(), command.c_str(), reply, &reply_len) != 0) {
        return false;
    }

    // Strip off trailing newline.
    if (reply_len > 0 && reply[reply_len-1] == '\n') {
        reply[reply_len-1] = '\0';
    } else {
        reply[reply_len] = '\0';
    }
    return true;
}

static jint doIntCommand(JNIEnv* env, jstring jIface,jstring javaCommand) {
    char reply[REPLY_BUF_SIZE];
    if (!doCommand(env, jIface, javaCommand, reply, sizeof(reply))) {
        return -1;
    }
    return static_cast<jint>(atoi(reply));
}

static jboolean doBooleanCommand(JNIEnv* env, jstring jIface, jstring javaCommand) {
    char reply[REPLY_BUF_SIZE];
    if (!doCommand(env, jIface,  javaCommand, reply, sizeof(reply))) {
        return JNI_FALSE;
    }
    return (strcmp(reply, "OK") == 0);
}

// Send a command to the supplicant, and return the reply as a String.
static jstring doStringCommand(JNIEnv* env, jstring jIface,jstring javaCommand) {
    char reply[REPLY_BUF_SIZE];
    if (!doCommand(env, jIface,javaCommand, reply, sizeof(reply))) {
        return NULL;
    }
    return env->NewStringUTF(reply);
}

static jboolean android_net_wifi_isDriverLoaded(JNIEnv* env, jobject)
{
    return (::is_wifi_driver_loaded() == 1);
}

static jboolean android_net_wifi_loadDriver(JNIEnv* env, jobject)
{
    return (::wifi_load_driver() == 0);
}

static jboolean android_net_wifi_unloadDriver(JNIEnv* env, jobject)
{
    return (::wifi_unload_driver() == 0);
}

static jboolean android_net_wifi_startSupplicant(JNIEnv* env, jobject, jboolean p2pSupported)
{
    return (::wifi_start_supplicant(p2pSupported) == 0);
}

static jboolean android_net_wifi_killSupplicant(JNIEnv* env, jobject, jboolean p2pSupported)
{
    return (::wifi_stop_supplicant(p2pSupported) == 0);
}

static jboolean android_net_wifi_connectToSupplicant(JNIEnv* env, jobject, jstring jIface)
{
	LOGD ("android_net_wifi_connectToSupplicant");
	ScopedUtfChars ifname(env, jIface);
    return (::wifi_connect_to_supplicant(ifname.c_str()) == 0);
}

static jboolean android_net_wifi_wfdconnectToSupplicant(JNIEnv* env, jobject, jstring jIface)
{
	LOGD ("android_net_wifi_wfdconnectToSupplicant");
    ScopedUtfChars ifname(env, jIface);
    return (jboolean)(::wfd_connect_to_supplicant(ifname.c_str()) == 0);
}
static void android_net_wifi_closeSupplicantConnection(JNIEnv* env, jobject, jstring jIface)
{
    
	LOGD ("android_net_wifi_wfdconnectToSupplicant");
    ScopedUtfChars ifname(env, jIface);
    ::wifi_close_supplicant_connection(ifname.c_str());
}
static void android_net_wifi_wfdcloseSupplicantConnection(JNIEnv* env, jobject)
{	
	LOGD ("android_net_wifi_closeSupplicantConnection");	
    ::wfd_close_supplicant_connection();
}

static jstring android_net_wifi_waitForEvent(JNIEnv* env, jobject, jstring jIface)
{
    char buf[EVENT_BUF_SIZE];
    ScopedUtfChars ifname(env, jIface);
    int nread = ::wifi_wait_for_event(ifname.c_str(),buf, sizeof buf);
    if (nread > 0) {
        return env->NewStringUTF(buf);
    } else {
        return NULL;
    }
}

static jboolean android_net_wifi_doBooleanCommand(JNIEnv* env, jobject, jstring jIface, jstring javaCommand) {
    return doBooleanCommand(env, jIface, javaCommand);
}

static jint android_net_wifi_doIntCommand(JNIEnv* env, jobject, jstring jIface, jstring javaCommand) {
    return doIntCommand(env, jIface, javaCommand);
}

static jstring android_net_wifi_doStringCommand(JNIEnv* env, jobject, jstring jIface,jstring javaCommand) {
    return doStringCommand(env,jIface, javaCommand);
}


static jboolean android_net_wifi_setDongleInside(JNIEnv* env, jobject clazz, jstring info)
{
	LOGD ("android_net_wifi_setDongleInside\n");

    const char *info_str = env->GetStringUTFChars(info, NULL);
    return  (jboolean)(::wifi_set_dongle_inside(info_str) == 0);
}

static jint android_net_p2p_modify_psk(JNIEnv *env, jobject, jstring file_name, jstring psk)
{
	char mfile_name[128] = { 0 };
	ScopedUtfChars mfilename(env, file_name);
	ScopedUtfChars mpass(env, psk);
	char temp_file_name[32] = { 0 };

	strcpy(temp_file_name, "/data/misc/wifi/temp");
	sprintf(mfile_name, "%s", mfilename.c_str());

	FILE *fp = NULL;
	FILE *fp_temp = NULL;
	char line[100] = { 0 };
	char replace_line[128] = { 0 };
	int found = 0;

	ALOGD("Open file : %s", mfile_name);
	if((fp_temp = fopen(temp_file_name, "w")) == NULL) {
		ALOGE("Open temp file fail");
		return -1;
	}
	if((fp = fopen(mfile_name, "rw")) == NULL) {
		fclose(fp_temp);
		ALOGE("--Open file fail");
		return -1;
	}
	while((fgets(line, sizeof(line), fp)) != NULL) {
		ALOGD("get: %s", line);
		if(strstr(line, "psk=") != NULL) {
			ALOGD("Found psk!");
			found = 1;

			sprintf(replace_line, "\tpsk=%s\n", mpass.c_str());
			ALOGD("replace_line : %s", replace_line);

			fputs(replace_line, fp_temp);
		} else {
			fputs(line, fp_temp);
		}
	}
	fflush(fp);
	fflush(fp_temp);
	fclose(fp); fp = NULL;
	fclose(fp_temp); fp_temp = NULL;
	remove(mfile_name);
	rename(temp_file_name, mfile_name);

	//add by gaoyunpei,chmod .conf
        /*
	if(system("chmod 664 /data/misc/wifi/p2p_supplicant.conf"))
	{
	    ALOGE("gaoyunpei chmod failed.Error:%s\n",strerror(errno));
	}
	else
	{
	    ALOGE("gaoyunpei chmod succ.");
	}
	//chown .conf
	if(system("chown 1000:1010 /data/misc/wifi/p2p_supplicant.conf"))
	{
	    ALOGE("gaoyunpei chown failed.Error:%s\n",strerror(errno));
	}
	else
	{
	    ALOGE("gaoyunpei chown succ.");
	}
        */
        ALOGE("MyPid: %d, MyUid: %d, mfile_name: %s\n", getpid(), getuid(), mfile_name);

        int mode = 0660;
        int uid = getuid();
        int gid = 1010; //WIFI
        ALOGE("setuid: %d, setGid: %d\n", uid, gid);
        int res = chown(mfile_name, uid, gid);
        if (res != 0) {
            ALOGE("chown(%s) error: %s\n", mfile_name, strerror(errno));
        }
        res = chmod(mfile_name, mode);
        if (res != 0) {
            ALOGE("chmod(%s) error: %s\n", mfile_name, strerror(errno));
        }

        //add by gaoyunpei end

	return found;
}


static jboolean android_net_wifi_setDongleRemove(JNIEnv* env, jobject clazz, jstring info)
{
	LOGD ("android_net_wifi_setDongleRemove\n");
    const char *info_str = env->GetStringUTFChars(info, NULL);

    return  (jboolean)(::wifi_set_dongle_remove(info_str) == 0);
}

static jint android_net_wifi_check(JNIEnv *env, jobject clazz)
{
	LOGD ("android_net_wifi_check\n");
	return (::wifi0_check());
}

static jint android_net_wifi_is_dongle_in(JNIEnv *env, jobject clazz)
{
	LOGD ("android_net_wifi_is_dongle_in\n");
	return (::wifi_check_is_dongle_in());
}

static jint android_net_wifi_is_dongle_out(JNIEnv *env, jobject clazz)
{
	LOGD ("android_net_wifi_is_dongle_out\n");
	return (::wifi_check_is_dongle_out());
}

static jboolean android_net_wifi_set_wowl(JNIEnv *env, jobject clazz,  jstring ifaceName, jboolean enable)
{
    
    LOGD ("android_net_wifi_set_wowl\n");
    ScopedUtfChars ifname(env, ifaceName); 
    if(enable)
    {
        c_pcl_set_wifi_wol_ctrl(1);
    }else
    {
        c_pcl_set_wifi_wol_ctrl(0);        
    }
    return (jboolean)(::wifi_enalbe_wowl(ifname.c_str(), enable) == 0);
}

// ----------------------------------------------------------------------------

/*
 * JNI registration.
 */
static JNINativeMethod gWifiMethods[] = {
    /* name, signature, funcPtr */

    { "loadDriver", "()Z",  (void *)android_net_wifi_loadDriver },
    { "isDriverLoaded", "()Z",  (void *)android_net_wifi_isDriverLoaded },
	{ "modify_psk", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)android_net_p2p_modify_psk},
    { "unloadDriver", "()Z",  (void *)android_net_wifi_unloadDriver },
    { "startSupplicant", "(Z)Z",  (void *)android_net_wifi_startSupplicant },
    { "killSupplicant", "(Z)Z",  (void *)android_net_wifi_killSupplicant },
    
    { "connectToSupplicantNative", "(Ljava/lang/String;)Z", (void *)android_net_wifi_connectToSupplicant },  	
    { "closeSupplicantConnectionNative", "(Ljava/lang/String;)V",
            (void *)android_net_wifi_closeSupplicantConnection },
            
    { "connectNativeWfdToSupplicant", "(Ljava/lang/String;)Z",
    	    		(void *)android_net_wifi_wfdconnectToSupplicant },    	    		
    { "closeNativeWfdSupplicantConnection", "()V",
					(void *)android_net_wifi_wfdcloseSupplicantConnection },    
					
    { "waitForEventNative", "(Ljava/lang/String;)Ljava/lang/String;", (void*)android_net_wifi_waitForEvent },
    { "doBooleanCommandNative", "(Ljava/lang/String;Ljava/lang/String;)Z", (void*)android_net_wifi_doBooleanCommand },
    { "doIntCommandNative", "(Ljava/lang/String;Ljava/lang/String;)I", (void*)android_net_wifi_doIntCommand },
    { "doStringCommandNative", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
            (void*) android_net_wifi_doStringCommand },
    { "setDongleInside", "(Ljava/lang/String;)Z", (void*) android_net_wifi_setDongleInside},
  { "setDongleRemove", "(Ljava/lang/String;)Z", (void*) android_net_wifi_setDongleRemove},
	{ "wifi0Check", "()I", (void*) android_net_wifi_check},
    { "wifiIsDongleIn", "()I", (void*) android_net_wifi_is_dongle_in},
    { "wifiIsDongleOut", "()I", (void*) android_net_wifi_is_dongle_out},
    { "wifiEnableWoWL", "(Ljava/lang/String;Z)Z", (void*) android_net_wifi_set_wowl},
};

int register_android_net_wifi_WifiNative(JNIEnv* env) {
    return AndroidRuntime::registerNativeMethods(env,
            "android/net/wifi/WifiNative", gWifiMethods, NELEM(gWifiMethods));
}

}; // namespace android
