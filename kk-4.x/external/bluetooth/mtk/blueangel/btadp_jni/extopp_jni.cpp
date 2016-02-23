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

/**
 * 1. management sockets ( apisock + servsock )
 */

#define LOG_TAG "Bluetooth.OPP"

#include <jni.h>
#include <utils/Log.h>
#include <utils/misc.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <cutils/sockets.h>

extern "C"
{
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "bt_opp_porting.h"
#include "bt_opp_comm.h"
#include "bt_ext_utility.h"
}

typedef struct {
	JNIEnv *env;
	int apisock;	// used for: REQ (sned) + CNF (recv) + RSP (send)
	int servsock;	// used for: IND (recv)
} native_data_t;

static JavaVM *g_jvm = NULL;

static jfieldID field_mNativeData;

static jobject mCallbackObject;
static jmethodID mCallbackMethod;
static jclass mStringClass;

static int g_bOppInit = -1;   // for ALPS00267499

// ********************************************************************************************************************
// jni env init
// ********************************************************************************************************************

/**
 * init java callback function reference ( for Indication process )
 */
static void classInitNative(JNIEnv* env, jclass clazz)
{
	ALOGD("[JNI][classInitNative][+]");

	// jvm
	env->GetJavaVM(&g_jvm);

	// nat
	field_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");

	// callback method
	mCallbackMethod = env->GetMethodID(clazz, "jniCallback", "(I[Ljava/lang/String;)V");

	// class: java.lang.String
	mStringClass = env->FindClass("java/lang/String");
	if( mStringClass != NULL )
	{
		mStringClass = (jclass)(env->NewGlobalRef(mStringClass));
	}

	ALOGD("[JNI][classInitNative][-]");
}

/**
 * keep native_data at java ( used at subsequence call )
 */
static void objectInitNative(JNIEnv* env, jobject object)
{
	native_data_t *nat = (native_data_t *)calloc(1, sizeof(native_data_t));

	ALOGD("[JNI][objectInitNative][+]");

	mCallbackObject = env->NewGlobalRef(object);
	if (NULL == nat) {
		ALOGE("[JNI][objectInitNative]: %s out of memory!", __FUNCTION__);
		return;
	}
	memset(nat, 0, sizeof(native_data_t));
	env->SetIntField(object, field_mNativeData, (jint)nat);

	ALOGD("[JNI][objectInitNative][-]");
}

/**
 * clean native_data
 */
static void objectDeinitNative(JNIEnv* env, jobject object)
{
	native_data_t *nat = (native_data_t *)env->GetIntField(object, field_mNativeData);

	ALOGD("[JNI][objectDeinitNative][+]");
	
	env->DeleteGlobalRef(mCallbackObject);
	if (nat)
	{
		free(nat);
	}
	ALOGD("[JNI][objectDeinitNative][-]");
}

/**
 * get the native_data from java
 */
static inline native_data_t * get_native_data(JNIEnv *env, jobject object) {

	native_data_t *nat = (native_data_t *)(env->GetIntField(object, field_mNativeData));
	if(!nat)
	{
		ALOGE("[JNI][get_native_data]: nat is null");
	}
	return nat;
}


// ********************************************************************************************************************
// private native functions
// ********************************************************************************************************************
jarray newStringArray(JNIEnv *env, char **cpp, int count)
{
	int i;
	jobjectArray ary;

	// new java string array
	ary = env->NewObjectArray(count, mStringClass, 0);
	if( ary == 0 )	return NULL;

	// fill string array element
	for (i = 0; i < count; i++)
	{
		jstring str = env->NewStringUTF(*cpp++);
		if( str == 0 )	return NULL;
		env->SetObjectArrayElement(ary, i, str);
		env->DeleteLocalRef(str);
	}

	return ary;
}


// ********************************************************************************************************************
// java native functions - service
// ********************************************************************************************************************

void opp_jni_cb(U8 event, S8** parameters, U8 count)
{
	bool isAttached = false;
	JNIEnv *env;
	jarray args;

//	ALOGD("[JNI][opp_jni_cb][+]");

	// prepare env
	if(g_jvm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		ALOGW("[JNI][opp_jni_cb]: failed to get JNI environment, assuming native thread");
		if(g_jvm->AttachCurrentThread(&env, NULL) < 0)
		{
			ALOGE("[JNI][opp_jni_cb]: failed to attach current thread");
		}
		else
		{
			isAttached = true;
		}
	}

	assert(env != NULL);

	// prepare parameter for java method
	if( parameters != NULL )
	{
		args = newStringArray(env, parameters, count);
		if( args == 0 )
		{
			ALOGE("[JNI][opp_jni_cb]: can't new StringArray for callback parameters(size:%d)", count);
		}
	}
	else
	{
		args = NULL;
	}

	// call java method via env
	env->CallVoidMethod(mCallbackObject, mCallbackMethod, event, args);

	// delete local reference and JVM will gc the object when not necessary
	if( args != NULL )
	{
		env->DeleteLocalRef(args);
	}

	// release resource
	if(isAttached)
	{
		g_jvm->DetachCurrentThread();
	}

//	ALOGD("[JNI][opp_jni_cb][-]");
}

/**
 * prepare resource for service: socket for IPC with BT-Task
 */
static jboolean enableServiceNative(JNIEnv* env, jobject object)
{
	native_data_t *nat;
	sockaddr_un name;
	socklen_t namelen;

	ALOGD("[JNI][enableServiceNative][+]");

	// reset socket
	GOPP(apisock) = -1;
	GOPP(servsock) = -1;

	// get native data
	nat = get_native_data(env, object);
	if(!nat)
	{
		ALOGE("[JNI][enableServiceNative]: null nat");
		return JNI_FALSE;
	}

	// setup apisock (client) - call BT-Task
	ALOGI("[JNI][enableServiceNative]: connecting to bttask...");
	nat->apisock = socket(PF_LOCAL, SOCK_DGRAM, 0);
	ALOGD("[JNI][enableServiceNative]: bttask client socket(nat->apisock): %d", nat->apisock);
	if (nat->apisock < 0)
	{
		ALOGE("[JNI][enableServiceNative]: create opp bttask client socket(nat->apisock) failed - %s(errno=%d)", strerror(errno), errno);
		goto error_exit;
	}
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, BT_SOCK_NAME_INT_ADP);
	namelen = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1);
	if ( connect(nat->apisock, (const struct sockaddr*)&name, namelen) < 0 )
	{
		ALOGE("[JNI][enableServiceNative]: connect to %s (bttask) failed - %s(errno=%d)", name.sun_path, strerror(errno), errno);
		goto error_exit;
	}
	// register for msg usage
	GOPP(apisock) = nat->apisock;

	if (g_bOppInit == 0)
	{
		// setup servsock (server) - recv IND
		ALOGI("[JNI][enableServiceNative]: creating serv socket for Cnf & Ind(from bttask)...");
		nat->servsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_OPP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
		ALOGD("[JNI][enableServiceNative]: opp server socket(nat->servsock): %d", nat->servsock);
		if (nat->servsock < 0)
		{
			ALOGE("[JNI][enableServiceNative]: create opp server socket(nat->servsock) failed - %s(errno=%d)", strerror(errno), errno);
			goto error_exit;
		}
		
		// register for msg usage
		GOPP(servsock) = nat->servsock;

		g_bOppInit = -1;
	}

	// register call back function
	ALOGI("[JNI][enableServiceNative]: register jni callback (to external-adaptation layer)");
	if( btmtk_opp_set_jni_callback(opp_jni_cb) != TRUE )
	{
		ALOGE("[JNI][enableServiceNative]: set jni callback fail");
		goto error_exit;
	}

	ALOGD("[JNI][enableServiceNative][-]");
	return JNI_TRUE;

error_exit:
	if(nat)
	{
		if(nat->apisock >= 0)
		{
			close(nat->apisock);
			nat->apisock = -1;
		}
		if(nat->servsock >= 0)
		{
			close(nat->servsock);
			nat->servsock = -1;
		}
	}
	return JNI_FALSE;
}

/**
 * call by listen Thread (infinite loop)
 */
static void startListenNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][startListenNative][+]");
	startListenMessage();
	ALOGD("[JNI][startListenNative][-]");
}

static void stopListenNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][stopListenNative][+]");
	stopListenMessage();
	ALOGD("[JNI][stopListenNative][-]");

}

static void disableServiceNative(JNIEnv* env, jobject object)
{
	native_data_t *nat;

	ALOGD("[JNI][disableServiceNative][+]");

	nat = get_native_data(env, object);
	if(nat)
	{
		//btmtk_pbap_send_deactive_req(nat->apisock);
		if(nat->apisock >= 0)
		{
			ALOGD("[JNI][disableServiceNative]: closing bttask client socket(nat->apisock)...");
			close(nat->apisock);
			nat->apisock = -1;
		}
	}
	else
	{
		ALOGE("[JNI][disableServiceNative]: null nat");
	}

	ALOGD("[JNI][disableServiceNative][-]");
}

// ********************************************************************************************************************
// java native functions - OPP Client API
// ********************************************************************************************************************

/**
 * btmtk_opp_client_enable
 */
static jboolean oppcEnableNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][oppcEnableNative][+]");
	btmtk_opp_client_enable();
	ALOGD("[JNI][oppcEnableNative][-]");
	return JNI_TRUE;
}

/**
 * btmtk_opp_client_disable
 */
static jboolean oppcDisableNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][oppcDisableNative][+]");
	btmtk_opp_client_disable();
	ALOGD("[JNI][oppcDisableNative][-]");
	return JNI_TRUE;
}

/**
 * btmtk_opp_client_connect
 */
static jboolean oppcConnectNative(JNIEnv* env, jobject object, jstring destAddr)
{
    ALOGD("[JNI][oppcConnectNative][+]");

    // jni
    native_data_t *nat;
    jboolean result = JNI_FALSE;
    const char *c_bdaddr;

    // bt
    bt_addr_struct btaddr;

    // get native data
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[JNI][oppcConnectNative]: null nat");
        return JNI_FALSE;
    }

    // check parameter
    if ( destAddr == NULL )
    {
        ALOGE("[JNI][oppcConnectNative]: required parameter null - destAddr");
        return JNI_FALSE;
    }

    // convert jni parameters
    ALOGD("[JNI][oppcConnectNative]: converting jni parameters...");
    c_bdaddr = env->GetStringUTFChars(destAddr, NULL);
    if (c_bdaddr == NULL )
    {
        ALOGE("[JNI][oppcConnectNative]: OutOfMemoryError while converting jni parameters");
        result = JNI_FALSE;
        goto connect_release_exit;
    }

    // translate bdaddr
    ALOGD("[JNI][oppcConnectNative]: converting bdaddr(to struct bt_addr_struct)");
    btmtk_util_convert_string2bdaddr(c_bdaddr, &btaddr);

    // call api
    ALOGI("[JNI][oppcConnectNative]: call EXT API - btmtk_opp_client_connect()...");
    btmtk_opp_client_connect(&btaddr);

    // success
    result = JNI_TRUE;

    ALOGD("[JNI][oppcConnectNative][-]: result[%d]", result);
    goto connect_release_exit;
connect_release_exit:
    if(nat)
    {
        if (c_bdaddr != NULL)
        {
            env->ReleaseStringUTFChars(destAddr, c_bdaddr);
        }
    }
    return result;
}

/**
 * btmtk_opp_client_pushobject
 */
static jboolean oppcPushNative(JNIEnv* env, jobject object, jstring mimeType, jstring objectName, jstring filename)
{
    // jni
    native_data_t *nat;
    jboolean result = JNI_FALSE;
    const char *c_mimetype, *c_objectname, *c_filename;

    ALOGD("[JNI][oppcPushNative][+]");

    // get native data
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[JNI][oppcPushNative]: null nat");
        return JNI_FALSE;
    }

    // convert jni parameters
    ALOGD("[JNI][oppcPushNative]: converting jni parameters...");
    c_mimetype = env->GetStringUTFChars(mimeType, NULL);
    c_objectname = env->GetStringUTFChars(objectName, NULL);
    c_filename = env->GetStringUTFChars(filename, NULL);
    if (c_mimetype == NULL || c_objectname == NULL || c_filename == NULL)
    {
        ALOGE("[JNI][oppcPushNative]: OutOfMemoryError while converting jni parameters");
        result = JNI_FALSE;
        goto push_object_release_exit;
    }

    // call api
    ALOGI("[JNI][oppcPushNative]: call EXT API - btmtk_opp_client_push()...");
    btmtk_opp_client_push(c_mimetype, c_objectname, c_filename);

    // success
    result = JNI_TRUE;

    ALOGD("[JNI][oppcPushNative][-]: result[%d]", result);
    goto push_object_release_exit;

push_object_release_exit:
    if(nat)
    {
        if (c_mimetype != NULL)
        {
            env->ReleaseStringUTFChars(mimeType, c_mimetype);
        }
        if (c_objectname != NULL)
        {
            env->ReleaseStringUTFChars(objectName, c_objectname);
        }
        if (c_filename != NULL)
        {
            env->ReleaseStringUTFChars(filename, c_filename);
        }
    }
    return result;
}

/**
 * btmtk_opp_client_pushobject
 */
static jboolean oppcPushObjectNative(JNIEnv* env, jobject object, jstring destAddr, jstring mimeType, jstring objectName, jstring filename)
{
	// jni
	native_data_t *nat;
	jboolean result = JNI_FALSE;
	const char *c_bdaddr, *c_mimetype, *c_objectname, *c_filename;

	// bt
	bt_addr_struct btaddr;

	ALOGD("[JNI][oppcPushObjectNative][+]");

	// get native data
	nat = get_native_data(env, object);
	if(!nat)
	{
		ALOGE("[JNI][oppcPushObjectNative]: null nat");
		return JNI_FALSE;
	}

	// check parameter
	if ( destAddr == NULL )
	{
		ALOGE("[JNI][oppcPushObjectNative]: required parameter null - destAddr");
		return JNI_FALSE;
	}

	// convert jni parameters
	ALOGD("[JNI][oppcPushObjectNative]: converting jni parameters...");
	c_bdaddr = env->GetStringUTFChars(destAddr, NULL);
	c_mimetype = env->GetStringUTFChars(mimeType, NULL);
	c_objectname = env->GetStringUTFChars(objectName, NULL);
    c_filename = env->GetStringUTFChars(filename, NULL);
	if (c_bdaddr == NULL || c_mimetype == NULL || c_objectname == NULL || c_filename == NULL)
	{
		ALOGE("[JNI][oppcPushObjectNative]: OutOfMemoryError while converting jni parameters");
		result = JNI_FALSE;
		goto push_object_release_exit;
	}

	// translate bdaddr
	ALOGD("[JNI][oppcPushObjectNative]: converting bdaddr(to struct bt_addr_struct)");
	btmtk_util_convert_string2bdaddr(c_bdaddr, &btaddr);

	// call api
	ALOGI("[JNI][oppcPushObjectNative]: call EXT API - btmtk_opp_client_pushobject()...");
	btmtk_opp_client_pushobject(&btaddr, c_mimetype, c_objectname, c_filename);

	// success
	result = JNI_TRUE;

	ALOGD("[JNI][oppcPushObjectNative][-]: result[%d]", result);
	goto push_object_release_exit;

push_object_release_exit:
	if(nat)
	{
		if (c_bdaddr != NULL)
		{
			env->ReleaseStringUTFChars(destAddr, c_bdaddr);
		}
		if (c_mimetype != NULL)
		{
			env->ReleaseStringUTFChars(mimeType, c_mimetype);
		}
		if (c_objectname != NULL)
		{
			env->ReleaseStringUTFChars(objectName, c_objectname);
		}
		if (c_filename != NULL)
		{
			env->ReleaseStringUTFChars(filename, c_filename);
		}
	}
	return result;
}


/**
 * btmtk_opp_client_pullobject
 */
static jboolean oppcPullObjectNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][oppcPullObjectNative][+]");
	ALOGD("[JNI][oppcPullObjectNative][-]");
	return JNI_FALSE;
}

/**
 * btmtk_opp_client_exchobject
 */
static jboolean oppcExchangeObjectNative(JNIEnv* env, jobject object, jstring destAddr, jstring mimeType, jstring objectName, jstring filename)
{
	ALOGD("[JNI][oppcExchangeObjectNative][+]");
	ALOGD("[JNI][oppcExchangeObjectNative][-]");
	return JNI_FALSE;
}

/**
 * btmtk_opp_client_disconnect
 */
static jboolean oppcAbortNative(JNIEnv* env, jobject object)
{
    ALOGD("[JNI][oppcAbortNative][+]");
    btmtk_opp_client_abort();
    ALOGD("[JNI][oppcAbortNative][-]");
    return JNI_FALSE;
}

/**
 * btmtk_opp_client_disconnect
 */
static jboolean oppcDisconnectNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][oppcDisconnectNative][+]");
	btmtk_opp_client_disconnect();
	ALOGD("[JNI][oppcDisconnectNative][-]");
	return JNI_FALSE;
}

// ********************************************************************************************************************
// java native functions - OPP Server API
// ********************************************************************************************************************

/**
 * btmtk_opp_server_enable
 */
static jboolean oppsEnableNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][oppsEnableNative][+]");
	btmtk_opp_server_enable();
	ALOGD("[JNI][oppsEnableNative][-]");
	return JNI_TRUE;
}

/**
 * btmtk_opp_server_disable
 */
static jboolean oppsDisableNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][oppsDisableNative][+]");
	btmtk_opp_server_disable();
	ALOGD("[JNI][oppsDisableNative][-]");
	return JNI_TRUE;
}

/**
 * btmtk_opp_client_pushobject
 */
static jboolean oppsAccessResponseNative(JNIEnv* env, jobject object, jint goepStatus, jobjectArray parameters)
{
	// jni
	native_data_t *nat;
	jboolean result = JNI_FALSE;
	jint idx=0, size=0;
	jstring c_parameter;
	const S8** adpParam;

	ALOGD("[JNI][oppsAccessResponseNative][+]");

	// get native data
	nat = get_native_data(env, object);
	if(!nat)
	{
		ALOGE("[JNI][oppsAccessResponseNative]: null nat");
		return JNI_FALSE;
	}

	// check parameter
	if ( parameters != NULL )
	{
		size = env->GetArrayLength(parameters);
		adpParam = new const S8*[size];
		for (idx=0; idx<size; idx++)
		{
			c_parameter = (jstring)env->GetObjectArrayElement(parameters, idx);
			adpParam[idx] = NULL;
			adpParam[idx] = env->GetStringUTFChars(c_parameter, NULL);
			if ( adpParam[idx] == NULL )
			{
				ALOGE("[JNI][oppsAccessResponseNative]: get parameter element(%d) from object-array(size:%d) fail", idx, size);
				result = JNI_FALSE;
				goto access_response_release_exit;
			}
			// can't release here or the adpParam will be invalid
			//env->ReleaseStringUTFChars(c_parameter, adpParam[idx]);
			ALOGD("[JNI][oppsAccessResponseNative]: parameter(%d)=[%s]", idx, adpParam[idx] );
		}
	}
	else
	{
		adpParam = NULL;
	}

	// call adp_ext(bt_opp_int.c)
	ALOGI("[JNI][oppsAccessResponseNative]: calling EXT API - btmtk_opp_server_access_response()...");
	btmtk_opp_server_access_response(goepStatus, adpParam);
	result = JNI_TRUE;

	ALOGD("[JNI][oppsAccessResponseNative][-]: result[%d]", result);

access_response_release_exit:

	if(adpParam)
	{
		// release each element first
		for (idx=0; idx<size; idx++)
		{
			if( adpParam[idx] != NULL ){
				c_parameter = (jstring)env->GetObjectArrayElement(parameters, idx);
				env->ReleaseStringUTFChars(c_parameter, adpParam[idx]);
			}
		}
		// release pointer array
		delete [] adpParam;
	}
	return result;
}


/**
 * btmtk_opp_server_disconnect
 */
static jboolean oppsDisconnectNative(JNIEnv* env, jobject object)
{
	ALOGD("[JNI][oppsDisconnectNative][+]");
	btmtk_opp_server_disconnect();
	ALOGD("[JNI][oppsDisconnectNative][-]");
	return JNI_FALSE;
}

// ********************************************************************************************************************
// jni env init
// ********************************************************************************************************************

/*
static jboolean testJNI(JNIEnv* env, jobject object, jint goepStatus, jobjectArray parameters)
{
	// jni
	native_data_t *nat;
	jboolean result = JNI_FALSE;
	jint idx=0, size=0;
	jstring c_parameter;
	const S8** adpParam;

	ALOGD("[JNI][testJNI][+]");

	// get native data
	nat = get_native_data(env, object);
	if(!nat)
	{
		ALOGE("[JNI][testJNI]: null nat");
		return JNI_FALSE;
	}

	// check parameter
	if ( parameters != NULL )
	{
		size = env->GetArrayLength(parameters);
		ALOGD("[JNI][testJNI]: parameter count:[%d]", size);

		adpParam = new const S8*[size];
		for (idx=0; idx<size; idx++)
		{
			adpParam[idx] = NULL;
			ALOGI("[JNI][testJNI] array int(%s)", adpParam[idx] );
			c_parameter = (jstring)env->GetObjectArrayElement(parameters, idx);
			adpParam[idx] = env->GetStringUTFChars(c_parameter, NULL);
			if ( adpParam[idx] == NULL )
			{
				ALOGE("[JNI][testJNI]: get parameter element(%d) from object-array(size:%d) fail", idx, size);
				result = JNI_FALSE;
				goto test_jni_exit;
			}
			ALOGI("[JNI][testJNI]1: parameter(%d)=[%s] addr=(%p)", idx, adpParam[idx], adpParam[idx] );
			//env->ReleaseStringUTFChars(c_parameter, adpParam[idx]);
			//ALOGI("[JNI][testJNI]2: parameter(%d)=[%s] addr=(%d)", idx, adpParam[idx], adpParam[idx] );
		}
	}
	else
	{
		adpParam = NULL;
	}
	ALOGD("[JNI][testJNI][-]: end of test");
test_jni_exit:
	ALOGD("[JNI][testJNI][goto]: test_jni_exit");
	if(adpParam)
	{
		for (idx=0; idx<size; idx++)
		{
			if( adpParam[idx] != NULL ){
				ALOGD("[JNI][testJNI][goto]: release.....");
				c_parameter = (jstring)env->GetObjectArrayElement(parameters, idx);
				env->ReleaseStringUTFChars(c_parameter, adpParam[idx]);
			}
		}
		delete [] adpParam;
	}
	return result;
}
*/


/**
 * function table ( for best performance when java call native function )
 */
static JNINativeMethod sMethods[] = {

// Test
//{"testJNI", "(I[Ljava/lang/String;)Z", (void*)testJNI },

	// format: { name, signature, funcPtr }
	// JNI
	{"classInitNative", "()V", (void*)classInitNative},
	{"objectInitNative", "()V", (void*)objectInitNative},
	{"objectDeinitNative", "()V", (void*)objectDeinitNative},
	// Service
	{"enableServiceNative", "()Z", (void*)enableServiceNative },
	{"startListenNative", "()V", (void*)startListenNative },
	{"stopListenNative", "()V", (void*)stopListenNative },
	{"disableServiceNative", "()V", (void*)disableServiceNative },
	// API - Client
	{"oppcEnableNative", "()Z", (void*)oppcEnableNative },
	{"oppcDisableNative", "()Z", (void*)oppcDisableNative },
	{"oppcConnectNative", "(Ljava/lang/String;)Z", (void*)oppcConnectNative },
	{"oppcPushNative", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void*)oppcPushNative },
	{"oppcPushObjectNative", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void*)oppcPushObjectNative },
	{"oppcPullObjectNative", "()Z", (void*)oppcPullObjectNative },
	{"oppcExchangeObjectNative", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void*)oppcExchangeObjectNative },
	{"oppcAbortNative", "()Z", (void*)oppcAbortNative },
	{"oppcDisconnectNative", "()Z", (void*)oppcDisconnectNative },
	// API - Server
	{"oppsEnableNative", "()Z", (void*)oppsEnableNative },
	{"oppsDisableNative", "()Z", (void*)oppsDisableNative },
	{"oppsAccessResponseNative", "(I[Ljava/lang/String;)Z", (void*)oppsAccessResponseNative },
	{"oppsDisconnectNative", "()Z", (void*)oppsDisconnectNative }
};

/*
 * register native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* methods, int numMethods)
{
	jclass clazz;

	clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		ALOGE("[JNI][registerNativeMethods]: native registration unable to find class[%s]", className);
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, methods, numMethods) < 0)
	{
		ALOGE("[JNI][registerNativeMethods]: env->RegisterNatives() failed for class[%s]", className);
		return JNI_FALSE;
	}
	return JNI_TRUE;
}

/*
 * register native methods.
 */
static int registerNatives(JNIEnv* env)
{
	if (!registerNativeMethods(env, "com/mediatek/bluetooth/opp/adp/OppServiceNative", sMethods, sizeof(sMethods) / sizeof(sMethods[0])))
		return JNI_FALSE;
	return JNI_TRUE;
}

/*
 * returns the JNI version on success, -1 on failure.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	ALOGD("[JNI][JNI_OnLoad][+]");

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		ALOGE("[JNI][JNI_OnLoad]: vm->GetEnv failed, JNI_OnLoad: %d", result);
		return result;
	}
	assert(env != NULL);

	if (!registerNatives(env))
	{
		ALOGE("[JNI][JNI_OnLoad]: native registration failed, JNI_OnLoad: %d", result);
		return result;
	}

	g_bOppInit = 0;

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;
	ALOGD("[JNI][JNI_OnLoad][-]");
	return result;
}
