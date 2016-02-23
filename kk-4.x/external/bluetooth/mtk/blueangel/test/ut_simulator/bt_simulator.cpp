/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * This file is only for unit test.
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
#include <stdlib.h>

#include <bt_simulator.h>
#include "BTSimulatorSender.h"
#include "BTSimulatorReceiver.h"

extern "C"
{

#include <pthread.h>

}

using namespace android;

#define BT_UTLOG(fmt, ...) XLOGI("[BT][UT][COMMOM]%s:" fmt, __FUNCTION__, ## __VA_ARGS__)

#define BT_FREESTR(x)  if ((x)!=NULL) delete ((x));

#define BT_JNILOG_REQ   "Req"
#define BT_JNILOG_CB	"Cb"

#define BT_UT_NEED_CB	"true"
#define BT_UT_NO_NEED_CB	"false"

static jmethodID notifyStateChange;
static jmethodID cleanUp;

static JavaVM *g_jvm = NULL;

static JNIEnv* g_jni_env = NULL;
static jobject g_service_object;

typedef enum 
{
	UT_LOG_ERROR = 0,
	UT_LOG_REQ ,
	UT_LOG_CB 
}UT_LOG_TYPE;

typedef struct __UT_LOG_NODE__
{
	UTJniLog     jnilog;
	char*        need_cb;
	UT_LOG_TYPE  type;
	int		   delayTime;  //ms

	struct __UT_LOG_NODE__ *next;
}UTLogNode;

typedef struct __UT_LOGS__
{
	int    logNum;
    int    currProfile;
	struct __UT_LOG_NODE__ *curr;
	struct __UT_LOG_NODE__ *next;    

	BTSimulatorSender simulatorSender;
}UTLog;

static UTLog gUTLogs;

static short gLockedForListen = FALSE;

//receiver cb func :
static BTSim_onDumpEmmaReport   gDumpEmmaReport = NULL;

static BTSim_onCallbackPrivate  gCbPrivate = NULL;

/*
* used to notify state changed, callback to check state is under control
* in this callback, add assert to check state
*/
void BTUTLog_notifyStateChange(int profileID, int log_id, const char* ret_string)
{
	BT_UTLOG("BTUTLog_notifyStateChange enter");
	bool	isAttached = false;
	jstring ret = NULL;

    if (g_jvm == NULL || g_jni_env == NULL)
    {
        BT_UTLOG(" the JVM is NULL ");
        return;
    }

    if (g_jni_env == NULL)
    {
        BT_UTLOG(" jni env is null ");
        return;
    }

    if (profileID != gUTLogs.currProfile)
    {
        BT_UTLOG("other profile notifyChange, return directly");
        return;
    }

	if(g_jvm->GetEnv((void **)&g_jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		BT_UTLOG("failed to get JNI environment, assuming native thread");
		if(g_jvm->AttachCurrentThread(&g_jni_env, NULL) < 0)
		{
			BT_UTLOG(" failed to attach current thread");
		}
		else
		{
			isAttached = true;
		}
	}

	if (ret_string == NULL)
	{
		BT_UTLOG("return string is null");
		return;
	}

	if (g_jni_env != NULL)
	{
	    //check does need callback to check state
		if (strcmp((gUTLogs.curr)->need_cb, BT_UT_NEED_CB) == 0)
		{
			BT_UTLOG("call back enter, and log id is : %d", log_id);
			ret = g_jni_env->NewStringUTF((const char*) ret_string);
			g_jni_env->CallVoidMethod(g_service_object, notifyStateChange, log_id, ret);
			g_jni_env->DeleteLocalRef(ret);
		}
		//goto next log
		BTUTLog_next(profileID);
	}
	if(isAttached)
	{
		g_jvm->DetachCurrentThread();
	}
}

/*
* clear one jni log
*/
static void clearJniLog(UTJniLog * node)
{
	if (node != NULL)
	{
		BT_FREESTR(node->name);
		BT_FREESTR(node->retv);

		if (node->params != NULL)
		{
			for (int i = 0;i<node->paramLen ; i++)
			{
				BT_FREESTR(node->params[i]);
			}
			BT_FREESTR(node->params);
		}
	}
}

/*
* clear all jni logs
*/
static void clearLogs()
{
	BT_UTLOG("clear logs enter");
	if (gUTLogs.logNum == 0)
	{
		BT_UTLOG("no log to clear");
		return ;
	}

	for (UTLogNode *cur = gUTLogs.next; cur!=NULL; )
	{
		UTLogNode *pdel = cur;
		cur = cur->next;
		
		clearJniLog(&(pdel->jnilog));

		delete pdel;
	}

	memset(&gUTLogs,0,sizeof(UTLog));
}

static char* getString(const char* str, char sep)
{
	int strlen = 0;
	const char * p = str;
	char * newStr = NULL;

	if (str == NULL || sep == 0)
		return NULL;
	
	//BT_UTLOG("getString %s",str);
	
	while ((*p)!=0 && (*p)!=sep) p++;

	strlen = p-str;
	if (strlen > 0)
	{
		newStr = new char[strlen+1];
		if (newStr != NULL)
		{
			memset(newStr,0,(strlen+1)*sizeof(char));
			memcpy(newStr,str,strlen*sizeof(char));
		}
	}

	return newStr;
}

static void addLogNode(const char * log,char sep)
{
	UTLogNode * newNode = NULL;
	const char * logStr = log;

	if (log == NULL)
		return;

	BT_UTLOG("addLogNode %s",log);

	newNode = new UTLogNode;
	if (newNode!=NULL)
	{
		char *tempStr = NULL;
		memset(newNode,0,sizeof(UTLogNode));

		tempStr = getString(logStr,sep);
		logStr += strlen(tempStr)+1;
		if (strcmp(tempStr,BT_JNILOG_REQ) == 0)
		{
			newNode->type = UT_LOG_REQ;
		}else if (strcmp(tempStr,BT_JNILOG_CB) == 0) 
		{
			newNode->type = UT_LOG_CB;
		}else
		{
			BT_UTLOG("Fatal error for Log type : %s",tempStr);
			goto ErrProcess;
		}
		BT_FREESTR(tempStr);
		tempStr = NULL;

		tempStr = getString(logStr,sep);
		if (tempStr==NULL)
		{
			BT_UTLOG("Fatal error , getString return NULL: logStr %s",logStr);
			goto ErrProcess;
		}
		logStr += strlen(tempStr)+1;
		newNode->delayTime = atoi(tempStr);
		BT_FREESTR(tempStr);
		tempStr = NULL;
		
		//this is add for callback
		tempStr = getString(logStr,sep);
		if (tempStr == NULL)
		{
			BT_UTLOG("Fatal error , getString return NULL: logStr %s",logStr);
			goto ErrProcess;
		}
		
		BT_UTLOG("need callback flag is : %s", tempStr);
		if (strcmp(tempStr, BT_UT_NEED_CB) == 0)
		{
			newNode->need_cb = (char*)BT_UT_NEED_CB;
		}
		else if (strcmp(tempStr, BT_UT_NO_NEED_CB) == 0)
		{
			newNode->need_cb = (char*)BT_UT_NO_NEED_CB;
		}
		else
		{
			BT_UTLOG("Fatal error, the need call back param should be true or false");
			goto ErrProcess;
		}
		logStr += strlen(tempStr)+1;
		BT_FREESTR(tempStr);
		tempStr = NULL;
		
		newNode->jnilog.name = getString(logStr,sep);
		logStr += strlen(newNode->jnilog.name)+1;

		newNode->jnilog.retv = getString(logStr,sep);
		logStr += strlen(newNode->jnilog.retv)+1;

		tempStr = getString(logStr,sep);
		logStr += strlen(tempStr)+1;
		newNode->jnilog.paramLen = atoi(tempStr);
		BT_FREESTR(tempStr);
		tempStr = NULL;

		if (newNode->jnilog.paramLen > 240)
		{
			BT_UTLOG("Fatal error, JNI logs is too long.");
			goto ErrProcess;
		}
		
		if (newNode->jnilog.paramLen > 0)
		{
			newNode->jnilog.params = new char*[newNode->jnilog.paramLen];
			if (newNode->jnilog.params == NULL)
			{
				goto ErrProcess;
			}
			memset(newNode->jnilog.params,0,newNode->jnilog.paramLen*sizeof(char*));
			for (int i = 0;i < newNode->jnilog.paramLen; i++ )
			{
				newNode->jnilog.params[i] = getString(logStr,sep);
				if (strlen(newNode->jnilog.params[i]) == 0)
				{
					BT_UTLOG("addLogNode: failed to get param[%d]",i);
					goto ErrProcess;
				}
				logStr += strlen(newNode->jnilog.params[i])+1;
			}
		}

		//BT_UTLOG("addLogNode : jni func name %s",newNode->jnilog.name);
		if (gUTLogs.next == NULL)
		{
			gUTLogs.next = newNode;
			gUTLogs.curr = newNode;
			gUTLogs.logNum ++;
			return;
		}

		UTLogNode * last = NULL;
		for (last = gUTLogs.next; last->next!=NULL ;last = last->next);
		last->next = newNode;
		gUTLogs.logNum ++;
		
		return ;
	}
ErrProcess:
	if (newNode!=NULL)
	{
		clearJniLog(&newNode->jnilog);
		delete newNode;
	}
	
	return;	
}

void BTSim_onDataArrival(const android::Parcel & data)
{
	// NOTE: not support the multithread protection.
	// BTSim_onDataArrival can be called only in the case that gUTLogs was not visited
	char sep = (char)data.readInt32();
	int  numOfLogs = data.readInt32();

	BT_UTLOG("sep %c numOfLogs %d",sep,numOfLogs);

	gLockedForListen = TRUE;

	clearLogs();

	for (int i = 0; i< numOfLogs ;i++)
	{
		const char* jnilog = NULL;
		jnilog = data.readCString();
		BT_UTLOG("logs %s ",jnilog);

		addLogNode(jnilog,sep);
	}

	gLockedForListen = FALSE;
}

/*
* get the current callback function number to choose the different callback(eg. extpan_ut.cpp).
*/
int BTUTLog_listenToCbFunc(char ** funList,int funcNum)
{
	int ret = -1;
	if (funList == NULL || funcNum<1 || gLockedForListen == TRUE)
		return ret;

	if (gUTLogs.logNum == 0)
	{
		//BT_UTLOG("gUTLogs is empty!!!");
		return ret;
	}

	if (gUTLogs.curr == NULL)
	{
		return ret;
	}

    //compare current call back and log
	for (int i = 0;i < funcNum; i++)
	{
		if (strcmp(funList[i],gUTLogs.curr->jnilog.name)== 0)
		{
			ret = i;
		}
	}

    //check need to sleep
	if (ret > -1)
	{
		int delayTime = gUTLogs.curr->delayTime;
		gUTLogs.curr->delayTime = 0;
		if (delayTime > 0)
		{
			sleep(delayTime);
		}
	}

	return ret;
}

/*
* clean native field, this method shoule be called after the last native in service native
*/
void clean_up(int profileID)
{
	BT_UTLOG("clean_up enter");
	bool isAttached = false;

    if (profileID != gUTLogs.currProfile)
    {
        BT_UTLOG("other profile clean up command, return directly");
        return;
    }
    
	clearLogs();

    if (g_jvm != NULL)
    {
        if(g_jvm->GetEnv((void **)&g_jni_env, JNI_VERSION_1_4) != JNI_OK)
	    {
		    BT_UTLOG("failed to get JNI environment, assuming native thread");
		    if(g_jvm->AttachCurrentThread(&g_jni_env, NULL) < 0)
		    {
			    BT_UTLOG(" failed to attach current thread");
		    }
		    else
		    {
			    isAttached = true;
		    }
	    }
	}
	//callback to clean java
	if (g_jni_env != NULL) 
	{
		g_jni_env->CallVoidMethod(g_service_object, cleanUp);
	}
	if(isAttached)
	{
		g_jvm->DetachCurrentThread();
	}

	if (g_service_object != NULL)
	{
		g_jni_env->DeleteGlobalRef(g_service_object);
		g_service_object = NULL;
	}
	if (g_jni_env != NULL)
	{
		g_jni_env = NULL;
	}
}

//goto next log
void BTUTLog_next(int profileID)
{
    if (profileID != gUTLogs.currProfile)
    {
        return;
    }
	if (gUTLogs.logNum == 0)
	{
		//BT_UTLOG("gUTLogs is empty!!!");
		return ;
	}
	if (gUTLogs.curr != NULL)
	{
		BT_UTLOG("cur log is for %s",gUTLogs.curr->jnilog.name);
		gUTLogs.curr = gUTLogs.curr->next;
	} else {
		BT_UTLOG("gUTLogs is exhausted !!!");
	}
	
}

//get current log infor, including return value, parameters,
UTJniLog* BTUTLog_getInfo(int profileID)
{
	UTJniLog * retLog = NULL;
	if (gUTLogs.logNum == 0)
	{
		//BT_UTLOG("gUTLogs is empty!!!");
		return NULL;
	}

    if (profileID != gUTLogs.currProfile)
    {
        return NULL;
    }

	if (gUTLogs.curr == NULL)
	{
		BT_UTLOG("gUTLogs is exhausted !!!");
	}else
	{
		retLog = &(gUTLogs.curr->jnilog);
	}

	return retLog;
}

/*
* this method is called in android_server_BluetoothA2dpService_ut.cpp startNative method
*/
void BTUTLog_initReceiver(char * recvName)
{
	BTSimulatorOnFuncs funcs;
	funcs.dataArrival = ::BTSim_onDataArrival;
	funcs.dump = gDumpEmmaReport;
	funcs.cbPrivate = gCbPrivate;
	BTSimulatorReceiver::launchReceiver(recvName, &funcs, FALSE);
}

/*
* this method is called in BTSimulatorReceiver.cpp
* used to generate coverage
*/
void BTUTLog_setOnDumpEmmaReport(BTSim_onDumpEmmaReport dumpEmmaReport)
{
	if (dumpEmmaReport != NULL)
	{
		gDumpEmmaReport = dumpEmmaReport;
	}
}

/*
* this method is called in BTSimulatorReceiver.cpp
* used to call private in BluetoothA2dpService
*/
void BTUTLog_setCallbackPrivate(BTSim_onCallbackPrivate cb)
{
	if (cb != NULL)
	{
		gCbPrivate = cb;
	}
}

/*
* this method is called in setCaselog
*/
static void setCaseLogs2RemoteNative(JNIEnv* env, jobject object,jobjectArray parameters,
										jstring separator,jstring remote)
{
	BT_UTLOG("setCaseLogs2RemoteNative +++++++++++");
	jint idx=0, size=0;	

	if ( parameters != NULL && remote != NULL && separator != NULL)
	{
		char sep = 0;
		Parcel data;
		jstring cParameter;

		const char *sepStr = env->GetStringUTFChars(separator, NULL);
		sep = *sepStr;
		env->ReleaseStringUTFChars(separator, sepStr);
		
		size = env->GetArrayLength(parameters);
		if (size <= 0)
		{
			return;
		}

		data.writeInt32((int)sep);
		data.writeInt32(size);

		BT_UTLOG("setCaseLogs2RemoteNative : separator %c",sep);

		const char *remoteStr = env->GetStringUTFChars(remote, NULL);
		if (remoteStr == NULL)
		{
			return;
		}

		for (idx = 0; idx<size; idx++)
		{
			const char* param = NULL;
			cParameter = (jstring)env->GetObjectArrayElement(parameters, idx);
			param = env->GetStringUTFChars(cParameter, NULL);
			if (param == NULL)
			{
				BT_UTLOG("parameters[%d] str val NULL",idx);
				continue;
			}
			data.writeCString(param);

			// can't release here or the adpParam will be invalid
			env->ReleaseStringUTFChars(cParameter, param);			
		}

		gUTLogs.simulatorSender.sendLogs(data,remoteStr);

		env->ReleaseStringUTFChars(remote, remoteStr);
	}

}

static void setCaseLogsNative(JNIEnv* env, jobject object,jobjectArray parameters,jstring separator)
{
	BT_UTLOG("setCaseLogsNative +++++++++++");

	jint idx=0, size=0;
	jstring cParameter;
	
	// check parameter
	if ( parameters != NULL && separator!= NULL)
	{
		char sep = 0;

		clearLogs();

		const char *sepStr = env->GetStringUTFChars(separator, NULL);
		sep = *sepStr;
		env->ReleaseStringUTFChars(separator, sepStr);
		
		size = env->GetArrayLength(parameters);
		if (size <= 0)
		{
			return;
		}

		BT_UTLOG("setCaseLogsNative : separator %c",sep);

		for (idx = 0; idx<size; idx++)
		{
			const char* param = NULL;
			cParameter = (jstring)env->GetObjectArrayElement(parameters, idx);
			param = env->GetStringUTFChars(cParameter, NULL);
			if (param == NULL)
			{
				continue;
			}
			addLogNode(param,sep);
			// can't release here or the adpParam will be invalid
			env->ReleaseStringUTFChars(cParameter, param);			
		}
	}

}

static void dumpEmmaReportNative(JNIEnv* env, jobject object,jstring remote)
{
	BT_UTLOG("dumpEmmaReportNative enter+++++ ");

	if (remote != NULL)
	{
		const char *remoteStr = env->GetStringUTFChars(remote, NULL);
		if (remoteStr == NULL)
		{
			return;
		}

		gUTLogs.simulatorSender.dumpEmmaReport(remoteStr);

		env->ReleaseStringUTFChars(remote, remoteStr);
	}
}

static void callPrivateMethodNative(JNIEnv* env, jobject object, int classId, int id, jstring remote)
{
	BT_UTLOG("callPrivateMethodNative enter+++++ ");
	if (remote != NULL)
	{
		const char *remoteStr = env->GetStringUTFChars(remote, NULL);
		if (remoteStr == NULL)
		{
			BT_UTLOG("[callPrivateMethodNative] Service Name is null");
			return ;
		}
		gUTLogs.simulatorSender.callPrivateMethod(classId, id, remoteStr);
		env->ReleaseStringUTFChars(remote, remoteStr);
	}
}

static void setLogProfileNative(JNIEnv* env, jobject object, jint profileID)
{
	BT_UTLOG("setLogProfileNative ++++");
	gUTLogs.currProfile = profileID;
}

static void initUnitTestNative(JNIEnv* env, jobject object)
{
	BT_UTLOG("initUnitTestNative ++++");
	jclass clazz = env->FindClass("com/mediatek/bluetooth/BluetoothUnitTestJni");
	if (clazz == NULL) {
		BT_UTLOG("FindClass com.mediatek.bluetooth.BluetoothUnitTestJni failed");
		return;
	}

	notifyStateChange = env->GetMethodID(clazz, "notifyStateChange", "(ILjava/lang/String;)V");
	cleanUp = env->GetMethodID(clazz, "cleanUp", "()V");

	env->GetJavaVM(&g_jvm);
	g_jni_env = env;
	g_service_object = env->NewGlobalRef(object);
	gUTLogs.logNum  = 0;
	gUTLogs.next = NULL;
}

/*
 * Native function table associated with BluetoothUnitTestJni
 * Direction: Java -> C
 */
static JNINativeMethod utMethod[] = {
	{"initUnitTestNative", "()V", (void*) initUnitTestNative},
    {"setLogProfileNative", "(I)V", (void*) setLogProfileNative},
	//{"cleanUnitTestNative", "()V", (void*) cleanUnitTestNative},
	{"setCaseLogsNative", "([Ljava/lang/String;Ljava/lang/String;)V", (void*)setCaseLogsNative },
	{"dumpEmmaReportNative", "(Ljava/lang/String;)V", (void*) dumpEmmaReportNative},
	{"callPrivateMethodNative","(IILjava/lang/String;)V", (void*) callPrivateMethodNative},
	{"setCaseLogs2RemoteNative", "([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void*)      setCaseLogs2RemoteNative },
};

static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* methods, int numMethods) 
{
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
		BT_UTLOG("Native registration unable to find class '%s'\n", className);
		return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
		BT_UTLOG("RegisterNatives failed for '%s'\n", className);
		return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env) {
	if (!registerNativeMethods(env, "com/mediatek/bluetooth/BluetoothUnitTestJni",
	    utMethod, sizeof(utMethod) / sizeof(utMethod[0])))    
	{
		BT_UTLOG("registerNativeMethods failed.\n");
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

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		BT_UTLOG("ERROR: GetEnv failed\n");
		goto bail;
    }
    assert(env != null);

    if (!registerNatives(env)) {
		BT_UTLOG("ERROR: BluetoothPanService failed to register natives\n");
		goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;;
}

