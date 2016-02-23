/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#define LOG_TAG "JNI_AVRCP"

#include <wchar.h>
#include <jni.h>
#include <cutils/jstring.h>
#include <cutils/sockets.h>
#include <utils/Log.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>


/* Ninput */
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <linux/input.h>
/* end of Ninput */

#include <bt_simulator.h>

extern "C"
{
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_avrcp_struct.h"
#include "bt_avrcp_api.h"
#include "bt_ext_debug.h"

#include "bluetooth_struct.h"

}

#define AVRCP_UT_LOG(fmt, ...) XLOGI("[BT][AVRCP-UT][EXT]%s:" fmt, __FUNCTION__, ## __VA_ARGS__)

#define AVRCP_UT_ERR_LOG(fmt, ...) XLOGI("[BT][AVRCP-UT][ERR-EXT]%s:" fmt, __FUNCTION__, ## __VA_ARGS__)

/* Local function */
static int registerNatives(JNIEnv* env);

/* AVRCP common */
static jmethodID method_onActivateCnf;
static jmethodID method_onDeactivateCnf;
static jmethodID method_onConnectInd;
static jmethodID method_onDisconnectInd;
/* AVRCP 1.0 */
static jmethodID method_passThroughKeyInd;
static jmethodID method_sendAvrcpKeyEventInd;
static jmethodID method_playerAppCapabilitiesInd;

static U8 g_avrcp_version = 14;
static U8 g_avrcp_sdpfeature = 0x01;

static void classInitNative(JNIEnv* env, jclass clazz) 
{
    AVRCP_UT_LOG("classInitNative ++++++");
	
    /* AVRCP 1.0 common */
    method_onActivateCnf   = env->GetMethodID(clazz, "activateCnf", "(BI)V");
    method_onDeactivateCnf   = env->GetMethodID(clazz, "deactivateCnf", "(BI)V");
    method_onConnectInd    = env->GetMethodID(clazz, "connectInd", "([BLjava/lang/String;I)V");
    method_onDisconnectInd = env->GetMethodID(clazz, "disconnectInd", "()V");

    /* AVRCP 1.0 TG Ind */
    method_passThroughKeyInd = env->GetMethodID(clazz, "passThroughKeyInd", "(IB)V"); 
    method_sendAvrcpKeyEventInd = env->GetMethodID(clazz, "sendAvrcpKeyEventInd", "(IB)V");
    /* AVRCP 1.3 TG Ind */ 
    method_playerAppCapabilitiesInd = env->GetMethodID(clazz, "playerAppCapabilitiesInd", "(I)V"); 
    
    AVRCP_UT_LOG("classInitNative ---------");
}

static void initializeNativeObjectNative(JNIEnv* env, jobject object) 
{
    AVRCP_UT_LOG("initializeNativeObjectNative ++++++++++");
}

static void deinitializeNativeObjectNative(JNIEnv* env, jobject object) 
{
	AVRCP_UT_LOG("deinitializeNativeDataNative ++++++++++++");
}

static void cleanupNativeObjectNative(JNIEnv* env, jobject object) 
{
    AVRCP_UT_LOG("cleanupNativeDataNative +++++++++++");
    clean_up(UT_PROFILE_AVRCP);
}

static jboolean enableNative(JNIEnv* env, jobject object)
{
    AVRCP_UT_LOG("enableNative +++++++++++++++++");
    // get native data
    UTJniLog* curLog=NULL;
    curLog = BTUTLog_getInfo(UT_PROFILE_AVRCP);
    if (curLog == NULL)
    {
        AVRCP_UT_ERR_LOG("Get current log failed");
	   return JNI_FALSE;
    }
    int ret = atoi(curLog->retv);
    AVRCP_UT_LOG("enableNative the set returen value is %d ", ret);
    if (ret > 0)
    {
	   AVRCP_UT_LOG("enableNative return true");
	   char* str = (char*)"enableNative";
	   BTUTLog_notifyStateChange(UT_PROFILE_AVRCP, 0, str);
	   //BTUTLog_next(UT_PROFILE_AVRCP);
	   return JNI_TRUE;
    }
    else
    {
	   AVRCP_UT_LOG("enableNative return false");
	   char* str = (char*)"enableNative";
	   BTUTLog_notifyStateChange(UT_PROFILE_AVRCP, 0, str);
	   //BTUTLog_next(UT_PROFILE_AVRCP);
	   return JNI_FALSE;
    }
}

void wakeupListenerNative(JNIEnv* env, jobject object)
{
    AVRCP_UT_LOG("wakeupListenerNative +++++++++++");
    //setAvrcpEvent(env, object, AVRCP_TERMINATE_THREAD_EVENT);
}

static void disableNative(JNIEnv* env, jobject object)
{
    AVRCP_UT_LOG("disableNative ++++++++++++");    
}

static jboolean listenIndicationNative(JNIEnv* env, jobject object, jboolean notWait)
{
    char* cbFuncs[] = {(char*)"method_onActivateCnf",
			        (char*)"method_onDeactivateCnf",
			        (char*)"method_onConnectInd",
			   	   (char*)"method_onDisconnectInd",
			        (char*)"method_passThroughKeyInd"
                    };
    int  cbFuncNum = 5;

    int curCallbackNum = BTUTLog_listenToCbFunc(cbFuncs,cbFuncNum);

    UTJniLog* curLog = NULL;

    switch(curCallbackNum)
    {
        case 0:
        //activate cnf
        {
            AVRCP_UT_LOG("callback activateCnf +++++++++++++");
            curLog = BTUTLog_getInfo(UT_PROFILE_AVRCP);
            if (curLog == NULL) 
            {
                return JNI_TRUE;
            }
            int int_index = atoi(curLog->params[0]);
            if (int_index > 255 || int_index < 0) {
                AVRCP_UT_ERR_LOG("[callback: activateCnf] index param out of bound");
                break;
            }
            U8 index = (U8) int_index;
            jint result = atoi(curLog->params[1]);
            AVRCP_UT_LOG("[callback: activateCnf] the transfer para is : %d, %d", index, result);
            env->CallVoidMethod(object, method_onActivateCnf, index, result);
            AVRCP_UT_LOG("callback activateCnf -------------");
            char* str = (char*)"activateCnf";
            BTUTLog_notifyStateChange(UT_PROFILE_AVRCP, 1, str);
            //BTUTLog_next(UT_PROFILE_AVRCP);
        }
        break;
        case 1:
        //deactivate cnf
        {
            AVRCP_UT_LOG("callback deactivateCnf +++++++++++++");
            curLog = BTUTLog_getInfo(UT_PROFILE_AVRCP);
            if (curLog == NULL) 
            {
                return JNI_TRUE;
            }
            int int_index = atoi(curLog->params[0]);
            if (int_index > 255 || int_index < 0) {
                AVRCP_UT_ERR_LOG("[callback: deactivateCnf] index param out of bound");
                break;
            }
            U8 index = (U8) int_index;
            jint result = atoi(curLog->params[1]);
            AVRCP_UT_LOG("[callback: deactivateCnf] the transfer para is : %d, %d", index, result);
            env->CallVoidMethod(object, method_onDeactivateCnf, index, result);
            AVRCP_UT_LOG("callback deactivateCnf -------------");
            char* str = (char*)"deactivateCnf";
            BTUTLog_notifyStateChange(UT_PROFILE_AVRCP, 2, str);
		    //BTUTLog_next(UT_PROFILE_AVRCP);
        }
        break;
        case 2:
        //connect ind
        {
            AVRCP_UT_LOG("callback connect ind +++++++++++++");
            curLog = BTUTLog_getInfo(UT_PROFILE_AVRCP);
            if (curLog == NULL) 
            {
                return JNI_TRUE;
            }
            jbyte add[] = {0x00,
            		 0x0f,
            		 0x53,
            		 0x34,
            		 0x53,
            		 0x11
                        };
            char *device_name = NULL;
            jstring deviceName = NULL;
            device_name = curLog->params[1];
            deviceName = env->NewStringUTF((const char*) device_name);
             int value = atoi(curLog->params[2]);
            //AVRCP_UT_LOG("[callback connectInd] the transfer para is : %s, %d", deviceName, value);
            env->CallVoidMethod(object, method_onConnectInd, add[0], deviceName, value);
            env->DeleteLocalRef(deviceName);
            AVRCP_UT_LOG("callback connect ind -------------");
            char* str = (char*)"connectInd";
            BTUTLog_notifyStateChange(UT_PROFILE_AVRCP, 3, str);
            //BTUTLog_next(UT_PROFILE_AVRCP);
        }
        break;
        case 3:
        //disconnect ind
        {
            AVRCP_UT_LOG("callback disconnect ind +++++++++++++");
            env->CallVoidMethod(object, method_onDisconnectInd);
            AVRCP_UT_LOG("callback disconnect ind -------------");
            char* str = (char*)"disconnectInd";
            BTUTLog_notifyStateChange(UT_PROFILE_AVRCP, 4, str);
            //BTUTLog_next(UT_PROFILE_AVRCP);
        }
        break;

        case 4:
        //pass through key ind
        {
            AVRCP_UT_LOG("callback pass through key ind +++++++++++++");
            curLog = BTUTLog_getInfo(UT_PROFILE_AVRCP);
            if (curLog == NULL) 
                return JNI_TRUE;
            int key_value = atoi(curLog->params[0]);
            int is_press = atoi(curLog->params[1]);
            U8 b_is_press = (U8) is_press;
            AVRCP_UT_LOG("callback pass through key %d, %d", key_value, b_is_press);
            env->CallVoidMethod(object, method_passThroughKeyInd, key_value, b_is_press);
            AVRCP_UT_LOG("callback pass through key ind -------------");
            char* str = (char*)"passThroughKeyInd";
            BTUTLog_notifyStateChange(UT_PROFILE_AVRCP, 5, str);
            //BTUTLog_next(UT_PROFILE_AVRCP);            
        }
        break; 	   
        default:
            //ALOGE("AVRCP Unexpected message : %lu", ilm.msg_id);
            break;
        }
    return JNI_TRUE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    activateConfigNative
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_activateConfig_3req
	(JNIEnv *env, jobject object, jbyte version, jbyte sdpfeature, jbyte reserved ){
	// setup the default activate version 	
	AVRCP_UT_LOG("activate_config %d, %d, %d", version, sdpfeature, reserved);

	switch(version)
	{
		case 10:
		case 11:
		case 0x10:
		case 0x11:
			version = 10; /* version 1.0 */
			break;
		case 3:
		case 13:
		case 0x13:
			version = 13; /* version 1.3 */
		break;
		case 4:
		case 14:
		case 0x14:
			version = 14; /* version 1.4 */
		break;
	}
	g_avrcp_version = version;
	g_avrcp_sdpfeature = sdpfeature;
	AVRCP_UT_LOG("activate_config ---------");
	return JNI_TRUE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    activateReqNative
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_activate_1req
  (JNIEnv *env, jobject object, jbyte index){
    //send out a active request
    AVRCP_UT_LOG("activate_1req index:%d, version:%d, sdpfeature:%d", index, g_avrcp_version, g_avrcp_sdpfeature);
    return JNI_TRUE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    deactivateReqNative
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_deactivate_1req
  (JNIEnv *env, jobject object, jbyte index){
    jboolean ret = JNI_FALSE;

    AVRCP_UT_LOG("[BT][AVRCP]+deactivate_1req index:%d", index);
    ret = JNI_TRUE;

    AVRCP_UT_LOG("deactivate_1req return value :%d", ret);
    return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    connectReqNative
 * Not invoked in avrcp service
 * Signature: (Ljava/lang/String;)Z
 */

JNIEXPORT jboolean JNICALL BluetoothAvrcpService_connectReqNative
  (JNIEnv *env, jobject object, jstring addrString){
  	AVRCP_UT_LOG("connectReqNative ++++++++++++");
  	return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL BluetoothAvrcpService_disconnectReqNative
  (JNIEnv *env, jobject object){
      jboolean ret = JNI_FALSE;
            
      AVRCP_UT_LOG("disconnectReqNative return value : %d", ret);
      return ret;
}

JNIEXPORT jboolean JNICALL BluetoothAvrcpService_connectNative (JNIEnv *env, jobject object, jstring addrString)
{
	jboolean ret = JNI_FALSE;
	AVRCP_UT_LOG("return value is : %d", ret);
	return ret;
}

JNIEXPORT jboolean JNICALL BluetoothAvrcpService_disconnectNative (JNIEnv *env, jobject object)
{
      jboolean ret = JNI_FALSE;
      AVRCP_UT_LOG("return value is : %d", ret);
      return ret;
}

/**
 * Table of methods associated with BluetoothAvrcpService.
 * This is used to increase the speed from java to access c code
 */
static JNINativeMethod sMethods[] = {
    { "classInitNative", "()Z",              (void*) classInitNative },
    { "initializeNativeObjectNative", "()V", (void*)initializeNativeObjectNative },
    { "cleanupNativeObjectNative", "()V",    (void*)cleanupNativeObjectNative },
    { "enableNative", "()Z",                 (void*)enableNative },
    { "disableNative", "()V",                (void*)disableNative },
    { "activateConfigNative", "(BBB)Z",         (void*)BluetoothAvrcpService_activateConfig_3req },    
    { "activateReqNative", "(B)Z",           (void*)BluetoothAvrcpService_activate_1req },
    { "deactivateReqNative", "(B)Z",         (void*)BluetoothAvrcpService_deactivate_1req },
    { "listenerNativeEventLoop", "(Z)Z",     (void*)listenIndicationNative },
    { "connectReqNative", "(Ljava/lang/String;)Z",  (void*)BluetoothAvrcpService_connectReqNative },
    { "disconnectReqNative", "()Z",          (void*)BluetoothAvrcpService_disconnectReqNative},
    { "connectNative", "(Ljava/lang/String;)Z",  (void*)BluetoothAvrcpService_connectNative},
    { "disconnectNative", "()Z",             (void*)BluetoothAvrcpService_disconnectNative},
    { "wakeupListenerNative", "()V", (void*) wakeupListenerNative}
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* methods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        AVRCP_UT_ERR_LOG("Native registration unable to find class '%s'\n", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
        AVRCP_UT_ERR_LOG("RegisterNatives failed for '%s'\n", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Register native methods.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, "com/mediatek/bluetooth/avrcp/BluetoothAvrcpService",
            sMethods, sizeof(sMethods) / sizeof(sMethods[0])))
    {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
 * Set some test stuff up.
 *
 * Returns the JNI version on success, -1 on failure.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    AVRCP_UT_LOG("JNI_OnLoad +++++++");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        AVRCP_UT_ERR_LOG("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (!registerNatives(env)) {
        AVRCP_UT_ERR_LOG("ERROR: BluetoothAvrcpService native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;
bail:
    AVRCP_UT_ERR_LOG("[BT][AVRCP]-JNI_OnLoad: %d", result);
    return result;
}


