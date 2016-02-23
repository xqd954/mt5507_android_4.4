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

#include <wchar.h>
#include <jni.h>
#include <cutils/jstring.h>
#include <utils/Log.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "cutils/sockets.h"
#include <sys/select.h>

#include <cutils/xlog.h>

#include <bt_simulator.h>

extern "C"
{

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_hid_struct.h"

#include "bt_hid_hdl.h"
#include "bt_hid_api.h"
}

typedef struct {
    JNIEnv* env;
    int apisock;
    int servsock;
    U32 con_id;
} native_data_t;

static JavaVM *g_jvm = NULL;

/*
 * Callback functions to BluetootHidService
 * Direction: C-> Java
 */

static jmethodID sendServiceMsg;

static jfieldID m_native_data;

static JNIEnv* g_jni_env;
static jobject* g_service_object;
static jobject g_object;

#define HID_UTLOG(fmt, ...) XLOGI("[BT][HIDUT][EXT]%s:" fmt, __FUNCTION__, ## __VA_ARGS__)


/* Inline function for getting native data address from a java instance field. */
static inline native_data_t* get_native_data(JNIEnv* env, jobject object) {
    return (native_data_t*) (env->GetIntField(object, m_native_data));
}


void bt_hid_jni_cb() {

	char		temp[17];
	jstring	device_addr=NULL;
	jint		status;
       jint		servObj = 0;
	bool 		isAttached = false;
	int curCbFunc = -1;
	UTJniLog* curLog=NULL;
	char* addr = NULL;
	U32 hid_ui_event;
	char* cbFuncs[] = {(char*)"sendServiceMsg"
						};
	int  cbFuncNum = 1;

	curCbFunc = BTUTLog_listenToCbFunc(cbFuncs,cbFuncNum);

	if (curCbFunc != 0)
		return;

	curLog = BTUTLog_getInfo(UT_PROFILE_HID);

	if (curLog == NULL)
		return;

	hid_ui_event = atoi(curLog->params[0]);
	addr = curLog->params[1];

	HID_UTLOG("get JNI environment");
	if(g_jvm->GetEnv((void **)&g_jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		HID_UTLOG("failed to get JNI environment, assuming native thread");
		if(g_jvm->AttachCurrentThread(&g_jni_env, NULL) < 0)
		{
			HID_ERR(" failed to attach current thread");
		}
		else
		{
			isAttached = true;
		}
	}

	status=hid_ui_event;

	switch(hid_ui_event)
	{
		case MBTEVT_HID_HOST_ENABLE_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_ENABLE_SUCCESS");
			break;
		case MBTEVT_HID_HOST_ENABLE_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_ENABLE_FAIL");
			break;
		case MBTEVT_HID_HOST_DISABLE_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_DISABLE_SUCCESS");
			break;
		case MBTEVT_HID_HOST_DISABLE_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_DISABLE_FAIL");
			break;
		case MBTEVT_HID_HOST_CONNECT_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_CONNECT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_CONNECT_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_CONNECT_FAIL");
			break;
		case MBTEVT_HID_HOST_DISCONNECT_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_DISCONNECT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_DISCONNECT_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_DISCONNECT_FAIL");
			break;
		case MBTEVT_HID_HOST_GET_DESC_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_GET_DESC_SUCCESS");
			break;
		case MBTEVT_HID_HOST_GET_DESC_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_GET_DESC_FAIL");
			break;
		case MBTEVT_HID_HOST_RECEIVE_UNPLUG:
			HID_UTLOG("MBTEVT_HID_HOST_RECEIVE_UNPLUG");
			break;
		case MBTEVT_HID_HOST_SEND_CONTROL_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_SEND_CONTROL_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SEND_CONTROL_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_SEND_CONTROL_FAIL");
			break;
		case MBTEVT_HID_HOST_SEND_REPORT_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_SEND_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SEND_REPORT_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_SEND_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_SET_REPORT_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_SET_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SET_REPORT_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_SET_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_GET_REPORT_SUCCESS:
			HID_UTLOG("MBTEVT_HID_HOST_GET_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_GET_REPORT_FAIL:
			HID_UTLOG("MBTEVT_HID_HOST_GET_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_RECEIVE_AUTHORIZE:
			HID_UTLOG("MBTEVT_HID_HOST_RECEIVE_AUTHORIZE");
			break;
		default:
			HID_UTLOG("Unknown HID event");
			break;			
	}

	if(addr!=0)
	{
		device_addr = g_jni_env->NewStringUTF((const char*) addr);
		HID_LOG("addr=%s",addr);
	}

	g_jni_env->CallVoidMethod(g_object, sendServiceMsg,  status,device_addr);	
       if(device_addr != NULL){
	    g_jni_env->DeleteLocalRef(device_addr);
       }
	BTUTLog_next(UT_PROFILE_HID);
	// release resource
	if(isAttached)
	{
		g_jvm->DetachCurrentThread();
	}
}

static jboolean initServiceNative(JNIEnv* env, jobject object) {

    native_data_t* nat = NULL;

    HID_LOG("initServiceNative Enter");

   // jvm
   env->GetJavaVM(&g_jvm);

    /* Find class structure */
    jclass clazz = env->FindClass("com/mediatek/bluetooth/hid/BluetoothHidService");
    if (clazz == NULL) {
	HID_ERR("FindClass failed");
	goto init_fail;
    }

    g_object = env->NewGlobalRef(object);

    g_service_object = (jobject*) calloc(1, sizeof(jobject));
    if (g_service_object == NULL) {
	HID_ERR("Allocate g_service_object: out of memory");
	goto init_fail;
    }
    //memset(g_service_object, 0, sizeof(jobject));
    memcpy(g_service_object, &object, sizeof(jobject));

    /* Retrieve the method id and field id */
    sendServiceMsg = env->GetMethodID(clazz, "sendServiceMsg", "(ILjava/lang/String;)V");


    m_native_data = env->GetFieldID(clazz, "mNativeData", "I");

    /* Allocate nat and store its address in object field */
    nat = (native_data_t*) calloc(1, sizeof(native_data_t));
    if (nat == NULL) {
	HID_ERR("Allocate nat: out of memory");
	goto init_fail;
    }

    memset(nat, 0, sizeof(native_data_t));
    env->SetIntField(object, m_native_data, (jint) nat);

	HID_UTLOG("initServiceNative Succeeded");
	BTUTLog_next(UT_PROFILE_HID);
	return JNI_TRUE;

init_fail:
    if (nat) {
	free(nat);
    }
    HID_ERR("Failed");
    return JNI_FALSE;
}

static void cleanServiceNative(JNIEnv* env, jobject object) {
	HID_LOG("cleanServiceNative");
	BTUTLog_next(UT_PROFILE_HID);
	
    native_data_t* nat = get_native_data(env, object);
    if (nat) {
	free(nat);
    }

    env->DeleteGlobalRef(g_object);

    if (g_service_object) {
	free(g_service_object);
    }
    clean_up(UT_PROFILE_HID);
}

static void forceClearServerNative(JNIEnv* env, jobject object) {
    HID_LOG("Force clear server.");
}

/*
 * Keeps listening to the socket for incoming ilm 
 */
static jboolean listentoSocketNative(JNIEnv* env, jobject object) {
    //HID_LOG("listentoSocketNative");

	g_jni_env = env;
	bt_hid_jni_cb();
	return JNI_TRUE;
}

static void wakeupListenerNative(JNIEnv* env, jobject object)
{
	HID_LOG("wakeupListenerNative");
	BTUTLog_next(UT_PROFILE_HID);
}


static void stopListentoSocketNative(JNIEnv* env, jobject object) {
	HID_LOG("stopListentoSocketNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static void serverAuthorizeReqNative(JNIEnv* env, jobject object,  jstring BT_Addr,jboolean result) {
	HID_LOG("serverAuthorizeReqNative");
}

static void serverActivateReqNative(JNIEnv* env, jobject object) {
	HID_LOG("serverActivateReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static void serverDeactivateReqNative(JNIEnv* env, jobject object) {
	HID_LOG("serverDeactivateReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}


static void serverConnectReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverConnectReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static  void serverDisconnectReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverDisconnectReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static  void serverUnplugReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverUnplugReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static  void serverSendReportReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jstring report) {
	HID_LOG("serverSendReportReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static  void serverSetReportReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jint reportType, jstring report) {
	HID_LOG("serverSetReportReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static  void serverGetReportReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jint reportType, jint reportId) {
	HID_LOG("serverGetReportReqNative");
	BTUTLog_next(UT_PROFILE_HID);
}

static  void serverSetProtocolReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jint protocolMode) {
	HID_LOG("serverSetProtocolReqNative");
	//BTUTLog_next(UT_PROFILE_HID);
}

static  void serverGetProtocolReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverGetProtocolReqNative");
	//BTUTLog_next(UT_PROFILE_HID);
}

static  void serverSetIdleReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverSetIdleReqNative");
	//BTUTLog_next(UT_PROFILE_HID);
}

static  void serverGetIdleReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverGetIdleReqNative");
	//BTUTLog_next(UT_PROFILE_HID);
}

/*
 * Native function table associated with BluetoothHidService
 * Direction: Java -> C
 */
static JNINativeMethod sMethods[] = {

    /* HID server relative native functions */
    {"initServiceNative", "()Z", (void*) initServiceNative},
    {"cleanServiceNative", "()V", (void*) cleanServiceNative},
    {"forceClearServerNative", "()V", (void*) forceClearServerNative},
    {"listentoSocketNative", "()Z", (void*) listentoSocketNative},
    {"wakeupListenerNative", "()V", (void*) wakeupListenerNative},
    {"stopListentoSocketNative", "()V", (void*) stopListentoSocketNative},
    {"serverAuthorizeReqNative", "(Ljava/lang/String;Z)V", (void*) serverAuthorizeReqNative},
    {"serverActivateReqNative", "()V", (void*) serverActivateReqNative},
    {"serverDeactivateReqNative", "()V", (void*) serverDeactivateReqNative},
    {"serverConnectReqNative", "(Ljava/lang/String;)V", (void*) serverConnectReqNative},
    {"serverDisconnectReqNative", "(Ljava/lang/String;)V", (void*) serverDisconnectReqNative},
    {"serverUnplugReqNative", "(Ljava/lang/String;)V", (void*) serverUnplugReqNative},
    {"serverSendReportReqNative", "(Ljava/lang/String;Ljava/lang/String;)V", (void*) serverSendReportReqNative},
    {"serverSetReportReqNative", "(Ljava/lang/String;ILjava/lang/String;)V", (void*) serverSetReportReqNative},
    {"serverGetReportReqNative", "(Ljava/lang/String;II)V", (void*) serverGetReportReqNative},
    {"serverSetProtocolReqNative", "(Ljava/lang/String;I)V", (void*) serverSetProtocolReqNative},
    {"serverGetProtocolReqNative", "(Ljava/lang/String;)V", (void*) serverGetProtocolReqNative},
    {"serverSetIdleReqNative", "(Ljava/lang/String;)V", (void*) serverSetIdleReqNative},
    {"serverGetIdleReqNative", "(Ljava/lang/String;)V", (void*) serverGetIdleReqNative},

};

static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* methods, int numMethods) 
{
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
	HID_ERR("Native registration unable to find class '%s'\n", className);
	return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
	HID_ERR("RegisterNatives failed for '%s'\n", className);
	return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env) {
    if (!registerNativeMethods(env, "com/mediatek/bluetooth/hid/BluetoothHidService",
	    sMethods, sizeof(sMethods) / sizeof(sMethods[0])))
	return JNI_FALSE;
    return JNI_TRUE;
}

/*
 * When library is loaded by Java by invoking "loadLibrary()".
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	HID_ERR("ERROR: GetEnv failed\n");
	goto bail;
    }
    assert(env != null);

    if (!registerNatives(env)) {
	HID_ERR("ERROR: BluetoothHidService failed to register natives\n");
	goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;;
}

