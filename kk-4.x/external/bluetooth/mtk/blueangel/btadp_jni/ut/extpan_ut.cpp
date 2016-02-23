/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * This file is only fot unit test.
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
#include <fcntl.h>
#include <cutils/xlog.h>

#include <bt_simulator.h>

#define PAN_UT_LOG(fmt, ...) XLOGI("[BT][PANUT][EXT]%s:" fmt, __FUNCTION__, ## __VA_ARGS__)

#define PAN_UT_ERR_LOG(fmt, ...) XLOGE("[BT][PAN][ERR-EXT]%s:" fmt, __FUNCTION__, ## __VA_ARGS__)

/*typedef struct {
    JNIEnv* env;
    int apisock;
    int servsock;
} native_data_t;
*/
/*
 * Callback functions to BluetootPanService
 * Direction: C-> Java
 */
static jmethodID callback_pan_handle_activate_cnf;
static jmethodID callback_pan_handle_deactivate_cnf;
static jmethodID callback_pan_handle_connection_authorize_ind;
static jmethodID callback_pan_handle_connect_ind;
static jmethodID callback_pan_handle_connect_cnf;
static jmethodID callback_pan_handle_disconnect_ind;
static jmethodID callback_pan_handle_disconnect_cnf;

//static jfieldID m_native_data;

static JavaVM *g_jvm = NULL;
static JNIEnv* g_jni_env = NULL;
static jobject g_service_object;

/* Inline function for getting native data address from a java instance field. 
static inline native_data_t* get_native_data(JNIEnv* env, jobject object) {
    return (native_data_t*) (env->GetIntField(object, m_native_data));
}
*/

/*
*   jni callback.
*   when finish one callback,the jni method should goto next method.
*/
void bt_pan_jni_cb() 
{	
	int curCbFunc = -1;
	UTJniLog* curLog=NULL;
	// callback function list.
	char* cbFuncs[] = {(char*)"callback_pan_handle_activate_cnf",
						(char*)"callback_pan_handle_deactivate_cnf",
						(char*)"callback_pan_handle_connection_authorize_ind",
						(char*)"callback_pan_handle_connect_ind",
						(char*)"callback_pan_handle_connect_cnf",
						(char*)"callback_pan_handle_disconnect_ind",
						(char*)"callback_pan_handle_disconnect_cnf"
						};
	int  cbFuncNum = 7;
	bool	isAttached = false;
	jstring	device_addr=NULL;

	if(g_jvm->GetEnv((void **)&g_jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		PAN_UT_LOG("failed to get JNI environment, assuming native thread");
		if(g_jvm->AttachCurrentThread(&g_jni_env, NULL) < 0)
		{
			PAN_UT_LOG(" failed to attach current thread");
		}
		else
		{
			isAttached = true;
		}
	}

	/*if (g_service_object == NULL)
	{
		PAN_UT_LOG("g_service_object is null");
		return ;
	}*/
	//get current callback function num.then goto the correct case.
	curCbFunc = BTUTLog_listenToCbFunc(cbFuncs,cbFuncNum);

	switch(curCbFunc)
	{
		case 0:
			{
				PAN_UT_LOG("callback_pan_handle_activate_cnf ++++");				
				curLog = BTUTLog_getInfo(UT_PROFILE_PAN);
                if (curLog == NULL) return;
				jboolean res = JNI_FALSE;

				res = (strcmp(curLog->params[0],"1") == 0) ? JNI_TRUE : JNI_FALSE;
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_activate_cnf,res);
				
				PAN_UT_LOG("callback_pan_handle_activate_cnf -----");
				char* str = (char*)"callback_pan_handle_activate_cnf";
				BTUTLog_notifyStateChange(UT_PROFILE_PAN, 1, str);
			}
			break;
		case 1:
			{
				PAN_UT_LOG("callback_pan_handle_deactivate_cnf ++++");
                curLog = BTUTLog_getInfo(UT_PROFILE_PAN);
                if (curLog == NULL) return;
				jboolean res = JNI_FALSE;

				res = (strcmp(curLog->params[0],"1") == 0) ? JNI_TRUE : JNI_FALSE;
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_deactivate_cnf, res);
				PAN_UT_LOG("callback_pan_handle_deactivate_cnf -----");
				char* str = (char*)"callback_pan_handle_deactivate_cnf";
				BTUTLog_notifyStateChange(UT_PROFILE_PAN, 2, str);
			}
			break;
		case 2:
			{
				PAN_UT_LOG("callback_pan_handle_connection_authorize_ind ++++");
				curLog = BTUTLog_getInfo(UT_PROFILE_PAN);
                if (curLog == NULL) return;
				jint service = 0;
				char *devAddress = NULL;

				service = atoi(curLog->params[0]);
				devAddress = curLog->params[1];
				
				PAN_UT_LOG("BTUTLog_getInfo param(%d) %s %s",
					 curLog->paramLen,curLog->params[0],curLog->params[1]);
				if (devAddress == NULL)
				{
					PAN_UT_LOG("callback_pan_handle_connection_authorize_ind params[1] error!");
					break;
				}
				device_addr = g_jni_env->NewStringUTF((const char*) devAddress);
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_connection_authorize_ind
					, service,device_addr);
				g_jni_env->DeleteLocalRef(device_addr);

				PAN_UT_LOG("callback_pan_handle_connection_authorize_ind -----");
				char* str = (char*)"callback_pan_handle_connection_authorize_ind";
				BTUTLog_notifyStateChange(UT_PROFILE_PAN, 3, str);
			}
			break;
		case 3:
			{
				PAN_UT_LOG("callback_pan_handle_connect_ind ++++");
				char* devAddress = NULL;
				curLog = BTUTLog_getInfo(UT_PROFILE_PAN);
                if (curLog == NULL) return;
				//get the parameters from test case
				jint service = atoi(curLog->params[0]);
				devAddress = curLog->params[1];
				jint unit = atoi(curLog->params[2]);
				
				PAN_UT_LOG("BTUTLog_getInfo param(%d) %d %s %d",
					 curLog->paramLen,curLog->params[0],curLog->params[1],curLog->params[2]);
				device_addr = g_jni_env->NewStringUTF((const char*) devAddress);
				//call back method:callback_pan_handle_connect_ind
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_connect_ind, service, device_addr, unit);				
				g_jni_env->DeleteLocalRef(device_addr);
				PAN_UT_LOG("callback_pan_handle_connect_ind -----");
				char* str = (char*)"callback_pan_handle_connect_ind";
				BTUTLog_notifyStateChange(UT_PROFILE_PAN, 4, str);

			}
			break;
		case 4:
			{
				PAN_UT_LOG("callback_pan_handle_connect_cnf ++++");
				char* devAddress = NULL;
				curLog = BTUTLog_getInfo(UT_PROFILE_PAN);
                if (curLog == NULL) return;

				jboolean res = JNI_FALSE;
				res = (strcmp(curLog->params[0],"1") == 0) ? JNI_TRUE : JNI_FALSE;
				
				devAddress = curLog->params[1];
				jint unit = atoi(curLog->params[2]);
				
				PAN_UT_LOG("BTUTLog_getInfo paramLen(%d) %d %s %d",
					 curLog->paramLen,curLog->params[0],curLog->params[1],curLog->params[2]);
				device_addr = g_jni_env->NewStringUTF((const char*) devAddress);
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_connect_cnf, res, device_addr, unit);
				g_jni_env->DeleteLocalRef(device_addr);
				PAN_UT_LOG("callback_pan_handle_connect_cnf -----");
				char* str = (char*)"callback_pan_handle_connect_cnf";
				BTUTLog_notifyStateChange(UT_PROFILE_PAN, 5, str);

			}
			break;
		case 5:
			{
				PAN_UT_LOG("callback_pan_handle_disconnect_ind ++++");				
				char* devAddress = NULL;
				curLog = BTUTLog_getInfo(UT_PROFILE_PAN);
                if (curLog == NULL) return;
				//get the parameters from test case
				devAddress = curLog->params[0];
				
				PAN_UT_LOG("BTUTLog_getInfo param length(%d) and param is (%s)",
					 curLog->paramLen,curLog->params[0]);
				device_addr = g_jni_env->NewStringUTF((const char*) devAddress);
				//call back method:callback_pan_handle_disconnect_ind
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_disconnect_ind, device_addr);				
				g_jni_env->DeleteLocalRef(device_addr);
				PAN_UT_LOG("callback_pan_handle_disconnect_ind -----");
				char* str = (char*)"callback_pan_handle_disconnect_ind";
				BTUTLog_notifyStateChange(UT_PROFILE_PAN, 6, str);
			}
			break;
		case 6:
			{
				PAN_UT_LOG("callback_pan_handle_disconnect_cnf ++++");				
				char* devAddress = NULL;
				jboolean result = JNI_FALSE;
				curLog = BTUTLog_getInfo(UT_PROFILE_PAN);
                if (curLog == NULL) return;
				result = (strcmp(curLog->params[0],"1") == 0) ? JNI_TRUE : JNI_FALSE;
				devAddress = curLog->params[1];
				
				PAN_UT_LOG("BTUTLog_getInfo param length(%d) and param is %d, %s",
					 curLog->paramLen,curLog->params[0],curLog->params[1]);
				device_addr = g_jni_env->NewStringUTF((const char*) devAddress);
				//call back method:callback_pan_handle_disconnect_ind
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_disconnect_cnf ,result, device_addr);				
				g_jni_env->DeleteLocalRef(device_addr);
				
				PAN_UT_LOG("callback_pan_handle_disconnect_cnf -----");
				char* str = (char*)"callback_pan_handle_disconnect_cnf";
				BTUTLog_notifyStateChange(UT_PROFILE_PAN, 7, str);
			}
			break;
		default:
			break;
	
	}

	if(isAttached)
	{
		g_jvm->DetachCurrentThread();
	}
}

static jboolean initServiceNative(JNIEnv* env, jobject object)
{

	PAN_UT_LOG("initServiceNative +++++");
	//native_data_t* nat = NULL;

	char* str = "initServiceNative";

   	// jvm
   	env->GetJavaVM(&g_jvm);

	g_service_object = env->NewGlobalRef(object);
	
	jclass clazz = env->FindClass("com/mediatek/bluetooth/pan/BluetoothPanService");
	if (clazz == NULL) {
		PAN_UT_LOG("FindClass com.mediatek.bluetooth.pan.BluetoothPanService failed");
		goto init_fail;
	}
 
	/* Retrieve the method id and field id */
	callback_pan_handle_activate_cnf = env->GetMethodID(clazz, "callback_pan_handle_activate_cnf", "(Z)V");
	callback_pan_handle_deactivate_cnf = env->GetMethodID(clazz, "callback_pan_handle_deactivate_cnf", "(Z)V");
	callback_pan_handle_connection_authorize_ind = env->GetMethodID(clazz, "callback_pan_handle_connection_authorize_ind", "(ILjava/lang/String;)V");
	callback_pan_handle_connect_ind = env->GetMethodID(clazz, "callback_pan_handle_connect_ind", "(ILjava/lang/String;I)V");
	callback_pan_handle_connect_cnf = env->GetMethodID(clazz, "callback_pan_handle_connect_cnf", "(ZLjava/lang/String;I)V");
	callback_pan_handle_disconnect_ind = env->GetMethodID(clazz, "callback_pan_handle_disconnect_ind", "(Ljava/lang/String;)V");
	callback_pan_handle_disconnect_cnf = env->GetMethodID(clazz, "callback_pan_handle_disconnect_cnf", "(ZLjava/lang/String;)V");

	//m_native_data = env->GetFieldID(clazz, "mNativeData", "I");

	/* Allocate nat and store its address in object field 
	nat = (native_data_t*) calloc(1, sizeof(native_data_t));
	if (nat == NULL) 
	{
		PAN_UT_ERR_LOG("Allocate nat: out of memory");
		goto init_fail;
	}
	
	memset(nat, 0, sizeof(native_data_t));
	env->SetIntField(object, m_native_data, (jint) nat);
	*/

	PAN_UT_LOG("initServiceNative -----");
	
	BTUTLog_notifyStateChange(UT_PROFILE_PAN, 0, str);
	
	return JNI_TRUE;

init_fail:
	/*if (nat) {
		free(nat);
	}*/
	PAN_UT_ERR_LOG("Failed");
	return JNI_FALSE;
}

static void cleanServiceNative(JNIEnv* env, jobject object) 
{
	PAN_UT_LOG("cleanServiceNative +++++++++++");
/*
	native_data_t* nat = get_native_data(env, object);
	if (nat) {
		free(nat);
	}*/
	env->DeleteGlobalRef(g_service_object);
	if (g_jni_env != NULL)
	{
		g_jni_env = NULL;
	}
	char* str = (char*)"cleanServiceNative";
	BTUTLog_notifyStateChange(UT_PROFILE_PAN, 10, str);
	clean_up(UT_PROFILE_PAN);
}

static void forceClearServerNative(JNIEnv* env, jobject object) 
{
	PAN_UT_LOG("forceClearServerNative +++++++++");
	char* str = (char*)"forceClearServerNative";
	BTUTLog_notifyStateChange(UT_PROFILE_PAN, 9, str);
}

/*
 * Keeps listening to the socket for incoming ilm 
 */
static jboolean listentoSocketNative(JNIEnv* env, jobject object) 
{
	g_jni_env = env;

	bt_pan_jni_cb();

 	return JNI_TRUE;
}

static void wakeupListenerNative(JNIEnv* env, jobject object)
{
	PAN_UT_LOG("wakeupListenerNative enter");
}

static void stopListentoSocketNative(JNIEnv* env, jobject object) 
{
	PAN_UT_LOG("stopListentoSocketNative enter");
}

static void serverAuthorizeRspNative(JNIEnv* env, jobject object, jstring BT_Addr,jboolean result) 
{
	PAN_UT_LOG("serverAuthorizeRspNative enter");
}

static void serverActivateReqNative(JNIEnv* env, jobject object) {
	PAN_UT_LOG("serverActivateReqNative enter");
	char* str = (char*)"serverActivateReqNative";
	BTUTLog_notifyStateChange(UT_PROFILE_PAN, 0, str);
}

static void serverDeactivateReqNative(JNIEnv* env, jobject object) {
	PAN_UT_LOG("serverDeactivateReqNative enter");
	char* str = (char*)"serverActivateReqNative";
	BTUTLog_notifyStateChange(UT_PROFILE_PAN, 0, str);
}


static void serverConnectReqNative(JNIEnv* env, jobject object,jint service, jstring BT_Addr) {
	PAN_UT_LOG("serverConnectReqNative enter");
}

static  void serverDisconnectReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	PAN_UT_LOG("serverDisconnectReqNative enter");
}

/*
 * Native function table associated with BluetoothPanService
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
    {"serverAuthorizeRspNative", "(Ljava/lang/String;Z)V", (void*) serverAuthorizeRspNative},
    {"serverActivateReqNative", "()V", (void*) serverActivateReqNative},
    {"serverDeactivateReqNative", "()V", (void*) serverDeactivateReqNative},
    {"serverConnectReqNative", "(ILjava/lang/String;)V", (void*) serverConnectReqNative},
    {"serverDisconnectReqNative", "(Ljava/lang/String;)V", (void*) serverDisconnectReqNative},
};

static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* methods, int numMethods) 
{
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
		PAN_UT_ERR_LOG("Native registration unable to find class '%s'\n", className);
		return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
		PAN_UT_ERR_LOG("RegisterNatives failed for '%s'\n", className);
		return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env) {
    if (!registerNativeMethods(env, "com/mediatek/bluetooth/pan/BluetoothPanService",
	    sMethods, sizeof(sMethods) / sizeof(sMethods[0]))) {
	    return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
 * When library is loaded by Java by invoking "loadLibrary()".
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

	PAN_UT_LOG("JNI_OnLoad in extpan_ut +++++++");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		PAN_UT_ERR_LOG("ERROR: GetEnv failed\n");
		goto bail;
    }
    assert(env != null);

    if (!registerNatives(env)) {
		PAN_UT_ERR_LOG("ERROR: BluetoothPanService failed to register natives\n");
		goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;;
}

