/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#define LOG_TAG "BluetoothMapService.cpp"

#include "android_runtime/AndroidRuntime.h"
#include "JNIHelp.h"
#include "jni.h"
#include "utils/Log.h"
#include "cutils/sockets.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#ifdef  __MTK_BT_EXTERNAL_PLATFORM__
#include "bt_ext_debug.h"
#else
#include <cutils/xlog.h>
#endif

extern "C"
{
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_map_struct.h"
#include "bt_map_api.h"
#include "bt_map_util.h"
#include "bt_map_mmi_event.h"
#include "bt_ext_utility.h"
extern void btmtk_map_handle_message(ilm_struct *message);
}




/************constant definition*************/

/****first part: method ID*****/

static jfieldID field_mNativeData;

static jmethodID method_onRequestOrIndicationReceived;
	
static jmethodID method_getMessageListCache;
static jmethodID method_getMessageRequestCache;
static jmethodID method_getFolderListCache;
static jmethodID method_getFolderCache;
static jmethodID method_getPushMessageCache;
static jmethodID method_getMessagObjectCache;
static jmethodID method_getStatusSwitchCache;


//arraylist method
static jmethodID method_ArrayList_get;
static jmethodID method_ArrayList_size;
static jmethodID method_ArrayList_add;
static jmethodID method_Integer_intValue;
static jmethodID method_Integer_int;

/*cache class*/
static jclass 	 class_MessageListRequest;
static jclass 	 class_FolderListRequest;
static jclass 	 class_FolderListObject;
static jclass 	 class_MessageRequest;
static jclass 	 class_MessageListObject;
static jclass 	 class_MessageItem;
static jclass 	 class_BMessageObject;
static jclass	 class_EventReport;
static jclass	 class_SetFolderRequest;
static jclass    class_StatusSwitchRequest;

static jclass	 class_Integer;

//EventReport field
static jfieldID	 field_EventReport_mMasId;
static jfieldID  field_EventReport_mEventType;
static jfieldID  field_EventReport_mHandle;
static jfieldID  field_EventReport_mFolder;
static jfieldID  field_EventReport_mOldFolder;
static jfieldID  field_EventReport_mMsgType;

//field
static jfieldID field_MessageRequest_mMasId;
static jfieldID field_MessageRequest_mAddr;
static jfieldID field_MessageRequest_mHandle;
static jfieldID field_MessageRequest_mAttachement;
static jfieldID field_MessageRequest_mCharset;
static jfieldID field_MessageRequest_mFractionReq;

	//MessageListRequest
static jfieldID field_MessageListRequest_mMasId	;
static jfieldID field_MessageListRequest_mAddr;
static jfieldID field_MessageListRequest_mChildFolder;
static jfieldID field_MessageListRequest_mListSize;
static jfieldID field_MessageListRequest_mListOffset;
static jfieldID field_MessageListRequest_mMaxSubjectLen;
static jfieldID field_MessageListRequest_mMask;
static jfieldID field_MessageListRequest_mType;
static jfieldID field_MessageListRequest_mStartTime;
static jfieldID field_MessageListRequest_mEndTime;
static jfieldID field_MessageListRequest_mReadStatus;
static jfieldID field_MessageListRequest_mRecipient;
static jfieldID field_MessageListRequest_mOrignator;
static jfieldID field_MessageListRequest_mPriority;
static jfieldID field_MessageListRequest_mFolder;
	
	//MessageListObject
static jfieldID field_MessageListObject_mNewMessage;
static jfieldID field_MessageListObject_mCurrentTime;
static jfieldID field_MessageListObject_mSize;
static jfieldID field_MessageListObject_mMessageItems;
	//MessageItem
static jfieldID field_MessageItem_MsgHandle;
static jfieldID field_MessageItem_Subject;
static jfieldID field_MessageItem_DateTime;
static jfieldID field_MessageItem_SenderName;
static jfieldID field_MessageItem_ReplyToAddr;
static jfieldID field_MessageItem_RecipientName;
static jfieldID field_MessageItem_RecipientAddr	;	
static jfieldID field_MessageItem_MsgType;
static jfieldID field_MessageItem_OrignalMsgSize;
static jfieldID field_MessageItem_bText;
static jfieldID field_MessageItem_ReciptionStatus;
static jfieldID field_MessageItem_AttachSize;
static jfieldID field_MessageItem_bPriority;
static jfieldID field_MessageItem_read;
static jfieldID field_MessageItem_bSent;
static jfieldID field_MessageItem_bProtected;
static jfieldID field_MessageItem_SenderAddr;

	
	//BMessage
static jfieldID field_BMessageObject_mFileName;
static jfieldID field_BMessageObject_mVersion;
static jfieldID field_BMessageObject_mReadStatus;
static jfieldID field_BMessageObject_mMsgType;
static jfieldID field_BMessageObject_mFolderPath;
static jfieldID field_BMessageObject_mOrignatorSize;
static jfieldID field_BMessageObject_mOrignator;
static jfieldID field_BMessageObject_mRecipientSize;
static jfieldID field_BMessageObject_mRecipient;
static jfieldID field_BMessageObject_mPartId;
static jfieldID field_BMessageObject_mEncoding;
static jfieldID field_BMessageObject_mCharset;
static jfieldID field_BMessageObject_mLanguage;
static jfieldID field_BMessageObject_mContentSize;
static jfieldID field_BMessageObject_mWholeSize;
static jfieldID field_BMessageObject_mTransparent;
static jfieldID field_BMessageObject_mRetry;
static jfieldID field_BMessageObject_mFracDeliver;



//folder list request
static jfieldID field_FolderListRequest_mMasId;
static jfieldID field_FolderListRequest_mAddr;
static jfieldID field_FolderListRequest_mSize;
static jfieldID field_FolderListRequest_mOffset;

//folder list response
static jfieldID field_FolderListObject_mTime;
static jfieldID field_FolderListObject_mSize;
static jfieldID field_FolderListObject_mName;

//set folder request object
static jfieldID field_SetFolderRequest_mMasId;
static jfieldID field_SetFolderRequest_mAddr;
static jfieldID field_SetFolderRequest_mFolder;

//set message status request
static jfieldID field_StatusSwitchRequest_mHandle;
static jfieldID field_StatusSwitchRequest_mIndicator;
static jfieldID field_StatusSwitchRequest_mValue;
static jfieldID field_StatusSwitchRequest_mAddress;


/****first part end*/



#define BTMTK_MAX_OBJECT_PATH_SIZE      18


/*******define service event end *****/


typedef struct {
    JavaVM 					*vm;
    int 					envVer;
    jobject 				me;  // for callbacks to java

    pthread_mutex_t 		thread_mutex;
    pthread_t 				thread;

    int 					mapsrvsock;       // for receiving IND and rsp
    int 					mapsock;           // for send REQ

	
} native_data_t;

static native_data_t *nat = NULL;  // global native data

void *(MAPMMICallback)(int type, void *message); 

//test code

//bt_addr_struct address_cache;


/****************************/
/*function body  begin ***********/
static jboolean map_startEventLoop(JNIEnv* env, jobject object);
static void map_stopEventLoop();
static void *map_eventLoopMain(void *ptr);
void btmtk_map_server_callback(U8 type, S8 *content) ;



static inline native_data_t * get_native_data(JNIEnv *env, jobject object)
{
    return (native_data_t *)(env->GetIntField(object, field_mNativeData));
}
//static void classInitNative(JNIEnv* env, jclass clazz) 
static void classInitNative(JNIEnv* env, jclass clazz) 

{

    XLOGV(__FUNCTION__);            
	//get class name                                                   com.mediatek.bluetooth.map.cache.BMessageObject
	class_MessageListRequest 	= env->FindClass("com/mediatek/bluetooth/map/cache/MessageListRequest");

	class_FolderListRequest		= env->FindClass("com/mediatek/bluetooth/map/cache/FolderListRequest");
	class_FolderListObject		= env->FindClass("com/mediatek/bluetooth/map/cache/FolderListObject");
	class_MessageRequest		= env->FindClass("com/mediatek/bluetooth/map/cache/MessageRequest");
	class_MessageListObject		= env->FindClass("com/mediatek/bluetooth/map/cache/MessageListObject");
	class_MessageItem 			= env->FindClass("com/mediatek/bluetooth/map/cache/MessageItem");
	class_BMessageObject 		= env->FindClass("com/mediatek/bluetooth/map/cache/BMessageObject");
	class_EventReport 			= env->FindClass("com/mediatek/bluetooth/map/cache/EventReport");
	class_SetFolderRequest		= env->FindClass("com/mediatek/bluetooth/map/cache/SetFolderRequest");
	class_StatusSwitchRequest	= env->FindClass("com/mediatek/bluetooth/map/cache/StatusSwitchRequest");
	class_Integer				= env->FindClass("java/lang/Integer");
	if(class_FolderListRequest == NULL)
	{
		XLOGE("class_EventReport is null");
	}
	
	//toDO: check the class name
    method_getMessageListCache 		= env->GetMethodID(clazz, "getMessageListCache",
    									"(I)Lcom/mediatek/bluetooth/map/cache/MessageListRequest;");
 	method_getFolderListCache		= env->GetMethodID(clazz, "getFolderListCache",
    									"(I)Lcom/mediatek/bluetooth/map/cache/FolderListRequest;");
	method_getMessageRequestCache	= env->GetMethodID(clazz, "getMessageRequestCache",
    									"(I)Lcom/mediatek/bluetooth/map/cache/MessageRequest;");
	method_getFolderCache			= env->GetMethodID(clazz, "getFolderCache",
    									"(I)Lcom/mediatek/bluetooth/map/cache/SetFolderRequest;");
	method_getStatusSwitchCache		= env->GetMethodID(clazz,"getStatusSwitchCache", 
										"()Lcom/mediatek/bluetooth/map/cache/StatusSwitchRequest;");
	method_onRequestOrIndicationReceived = env->GetMethodID(clazz,"onRequestOrIndicationReceived",
												"(IIILjava/lang/Object;)V");
	
	//arraylist method
	//todo: get method ID
	jclass arraylist			= env->FindClass("java/util/ArrayList");
	method_ArrayList_get		= env->GetMethodID(arraylist, "get", "(I)Ljava/lang/Object;");
	method_ArrayList_size		= env->GetMethodID(arraylist, "size", "()I");
	method_ArrayList_add		= env->GetMethodID(arraylist, "add", "(Ljava/lang/Object;)Z");

	method_Integer_int			= env->GetMethodID(class_Integer, "<init>", "(I)V");	
	method_Integer_intValue		= env->GetMethodID(class_Integer, "intValue", "()I");	
	
	//native data
    field_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");
	if (field_mNativeData == NULL)
	{
		XLOGE("field_mNativeData is null");
	}
		
	//EventReport fields
	field_EventReport_mMasId 	 = env->GetFieldID(class_EventReport, "mMasId","I");
	field_EventReport_mEventType = env->GetFieldID(class_EventReport, "mEventType", "I");
	field_EventReport_mHandle 	 = env->GetFieldID(class_EventReport, "mHandle", "J");
	field_EventReport_mFolder	 = env->GetFieldID(class_EventReport, "mFolder", "Ljava/lang/String;");
	field_EventReport_mOldFolder = env->GetFieldID(class_EventReport, "mOldFolder", "Ljava/lang/String;");
	field_EventReport_mMsgType	 = env->GetFieldID(class_EventReport, "mMsgType", "I");
	

	//MessageRequest
	field_MessageRequest_mMasId			= env->GetFieldID(class_MessageRequest, "mMasId", "I");
	field_MessageRequest_mAddr			= env->GetFieldID(class_MessageRequest, "mAddr", "Ljava/lang/String;");
	field_MessageRequest_mHandle		= env->GetFieldID(class_MessageRequest, "mHandle", "J") ;
	field_MessageRequest_mAttachement	= env->GetFieldID(class_MessageRequest, "mAttachement", "Z");
	field_MessageRequest_mCharset		= env->GetFieldID(class_MessageRequest, "mCharset", "I");
	field_MessageRequest_mFractionReq	= env->GetFieldID(class_MessageRequest, "mFractionReq", "I");

	//MessageListRequest
	field_MessageListRequest_mMasId			= env->GetFieldID(class_MessageListRequest, "mMasId", "I");
	field_MessageListRequest_mAddr			= env->GetFieldID(class_MessageListRequest, "mAddr", "Ljava/lang/String;");
	field_MessageListRequest_mChildFolder	= env->GetFieldID(class_MessageListRequest, "mChildFolder", "Ljava/lang/String;");
	field_MessageListRequest_mListSize		= env->GetFieldID(class_MessageListRequest, "mListSize", "I");
	field_MessageListRequest_mListOffset	= env->GetFieldID(class_MessageListRequest, "mListOffset","I");
	field_MessageListRequest_mMaxSubjectLen = env->GetFieldID(class_MessageListRequest, "mMaxSubjectLen", "I");
	field_MessageListRequest_mMask			= env->GetFieldID(class_MessageListRequest, "mMask", "I");
	field_MessageListRequest_mType			= env->GetFieldID(class_MessageListRequest, "mType", "I");
	field_MessageListRequest_mStartTime		= env->GetFieldID(class_MessageListRequest, "mStartTime", "Ljava/lang/String;");
	field_MessageListRequest_mEndTime		= env->GetFieldID(class_MessageListRequest, "mEndTime", "Ljava/lang/String;");
	field_MessageListRequest_mReadStatus	= env->GetFieldID(class_MessageListRequest, "mReadStatus", "I");
	field_MessageListRequest_mRecipient		= env->GetFieldID(class_MessageListRequest, "mRecipient", "Ljava/lang/String;");
	field_MessageListRequest_mOrignator		= env->GetFieldID(class_MessageListRequest, "mOrignator", "Ljava/lang/String;");
	field_MessageListRequest_mPriority		= env->GetFieldID(class_MessageListRequest, "mPriority", "I");
	field_MessageListRequest_mFolder		= env->GetFieldID(class_MessageListRequest, "mFolder", "Ljava/lang/String;");
	
	//MessageListObject
	field_MessageListObject_mNewMessage		= env->GetFieldID(class_MessageListObject, "mNewMessage", "Z");
	field_MessageListObject_mCurrentTime	= env->GetFieldID(class_MessageListObject, "mCurrentTime","Ljava/lang/String;");
	field_MessageListObject_mSize			= env->GetFieldID(class_MessageListObject, "mSize", "I");
	field_MessageListObject_mMessageItems	= env->GetFieldID(class_MessageListObject, "mMessageItems", "Ljava/util/ArrayList;");

	//MessageItem
	field_MessageItem_MsgHandle				= env->GetFieldID(class_MessageItem, "MsgHandle", "J");
	field_MessageItem_Subject			 	= env->GetFieldID(class_MessageItem, "Subject", "Ljava/lang/String;");
	field_MessageItem_DateTime				= env->GetFieldID(class_MessageItem, "DateTime", "Ljava/lang/String;");
	field_MessageItem_SenderAddr			= env->GetFieldID(class_MessageItem, "SenderAddr", "Ljava/lang/String;");
	field_MessageItem_SenderName			= env->GetFieldID(class_MessageItem, "SenderName", "Ljava/lang/String;");
	field_MessageItem_ReplyToAddr			= env->GetFieldID(class_MessageItem, "ReplyToAddr", "Ljava/lang/String;");
	field_MessageItem_RecipientName			= env->GetFieldID(class_MessageItem, "RecipientName","Ljava/lang/String;");
	field_MessageItem_RecipientAddr			= env->GetFieldID(class_MessageItem, "RecipientAddr","Ljava/lang/String;");
	field_MessageItem_MsgType				= env->GetFieldID(class_MessageItem, "MsgType", "I");
	field_MessageItem_OrignalMsgSize		= env->GetFieldID(class_MessageItem, "OrignalMsgSize", "I");
	field_MessageItem_bText					= env->GetFieldID(class_MessageItem, "bText", "Z");
	field_MessageItem_ReciptionStatus		= env->GetFieldID(class_MessageItem, "RecipientStatus", "I");
	field_MessageItem_AttachSize			= env->GetFieldID(class_MessageItem, "AttachSize", "I");
	field_MessageItem_bPriority				= env->GetFieldID(class_MessageItem, "bPriority", "Z");
	field_MessageItem_read					= env->GetFieldID(class_MessageItem, "read", "I");
	field_MessageItem_bSent					= env->GetFieldID(class_MessageItem, "bSent", "Z");
	field_MessageItem_bProtected			= env->GetFieldID(class_MessageItem, "bProtected", "Z");

	//BMessageObject
	field_BMessageObject_mFileName			= env->GetFieldID(class_BMessageObject, "mFileName", "Ljava/lang/String;");
	field_BMessageObject_mVersion			= env->GetFieldID(class_BMessageObject, "mVersion", "I");
	field_BMessageObject_mReadStatus		= env->GetFieldID(class_BMessageObject, "mReadStatus", "I");
	field_BMessageObject_mMsgType			= env->GetFieldID(class_BMessageObject, "mMsgType", "I");
	field_BMessageObject_mFolderPath		= env->GetFieldID(class_BMessageObject, "mFolderPath", "Ljava/lang/String;");
	field_BMessageObject_mOrignatorSize		= env->GetFieldID(class_BMessageObject, "mOrignatorSize", "I");
	field_BMessageObject_mOrignator			= env->GetFieldID(class_BMessageObject, "mOrignator", "Ljava/lang/String;");
//	field_BMessageObject_mRecipientSize		= env->GetFieldID(class_BMessageObject, "mRecipientSize", "I");
//	field_BMessageObject_mRecipient			= env->GetFieldID(class_BMessageObject, "mRecipient", "Ljava/lang/String;");
	field_BMessageObject_mRecipientSize 	= env->GetFieldID(class_BMessageObject, "mRecipientSize", "Ljava/util/ArrayList;");
	field_BMessageObject_mRecipient 		= env->GetFieldID(class_BMessageObject, "mRecipient", "Ljava/util/ArrayList;");
	field_BMessageObject_mPartId			= env->GetFieldID(class_BMessageObject, "mPartId", "I");
	field_BMessageObject_mEncoding			= env->GetFieldID(class_BMessageObject, "mEncoding", "I");
	field_BMessageObject_mCharset			= env->GetFieldID(class_BMessageObject, "mCharset", "I");
	field_BMessageObject_mLanguage			= env->GetFieldID(class_BMessageObject, "mLanguage", "I");
	field_BMessageObject_mContentSize		= env->GetFieldID(class_BMessageObject, "mContentSize", "Ljava/util/ArrayList;");
	field_BMessageObject_mWholeSize			= env->GetFieldID(class_BMessageObject, "mWholeSize", "J");
	field_BMessageObject_mTransparent		= env->GetFieldID(class_BMessageObject, "mTransparent", "Z");
	field_BMessageObject_mRetry				= env->GetFieldID(class_BMessageObject, "mRetry", "Z");
	field_BMessageObject_mFracDeliver		= env->GetFieldID(class_BMessageObject, "mFracDeliver", "I");
	//folder list request
	field_FolderListRequest_mMasId			= env->GetFieldID(class_FolderListRequest, "mMasId", "I");
	field_FolderListRequest_mAddr			= env->GetFieldID(class_FolderListRequest, "mAddr", "Ljava/lang/String;");
	field_FolderListRequest_mSize			= env->GetFieldID(class_FolderListRequest, "mSize", "I");
	field_FolderListRequest_mOffset			= env->GetFieldID(class_FolderListRequest, "mOffset", "I");

	//folder list object
	field_FolderListObject_mName			= env->GetFieldID(class_FolderListObject, "mName", "Ljava/lang/String;");
	field_FolderListObject_mSize			= env->GetFieldID(class_FolderListObject, "mSize", "I");
	field_FolderListObject_mTime			= env->GetFieldID(class_FolderListObject, "mTime", "Ljava/lang/String;");
		

	field_SetFolderRequest_mMasId			= env->GetFieldID(class_SetFolderRequest, "mMasId", "I");
	field_SetFolderRequest_mAddr			= env->GetFieldID(class_SetFolderRequest, "mAddr", "Ljava/lang/String;");
	field_SetFolderRequest_mFolder			= env->GetFieldID(class_SetFolderRequest, "mFolder", "Ljava/lang/String;");

	// set message status request
	field_StatusSwitchRequest_mHandle		= env->GetFieldID(class_StatusSwitchRequest, "mHandle", "J");
	field_StatusSwitchRequest_mIndicator	= env->GetFieldID(class_StatusSwitchRequest, "mIndicator", "I");  
	field_StatusSwitchRequest_mValue		= env->GetFieldID(class_StatusSwitchRequest, "mValue", "I");
	field_StatusSwitchRequest_mAddress		= env->GetFieldID(class_StatusSwitchRequest, "mAddress", "Ljava/lang/String;");

}

static jboolean initNative(JNIEnv *env, jobject obj)
{
    XLOGV(__FUNCTION__);
	XLOGI("[BT][MAP][JNI]initNative");
	sockaddr_un mapname;
    socklen_t   mapnamelen;
    struct sockaddr_un btname;
    socklen_t   btnamelen;   
	nat = (native_data_t *)calloc(1, sizeof(native_data_t));
	if (NULL == nat) {
		  XLOGE("%s: out of memory!", __FUNCTION__);
		  goto exit;
	}
	memset(nat, 0, sizeof(native_data_t));
	env->SetIntField(obj, field_mNativeData, (jint)nat);
	
	//Init socket handler value
	nat->mapsrvsock = -1;
	nat->mapsock = -1;
	env->GetJavaVM( &(nat->vm) );
	nat->envVer = env->GetVersion();
	nat->me = env->NewGlobalRef(obj);
	pthread_mutex_init(&(nat->thread_mutex), NULL);

	//server socket
	btname.sun_family = AF_UNIX;
    strcpy (btname.sun_path, /*BT_SERV_SOCK_ADDR*/BT_SOCK_NAME_INT_ADP);
    btnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (btname.sun_path) + 1);   
    nat->mapsrvsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_MAP, 
                                          ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
                                          SOCK_DGRAM);
    if(nat->mapsrvsock < 0)
    {
        XLOGE("[BT][MAP][ERR] create map server socket failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }
    else
   	{
        mapnamelen = sizeof(mapname.sun_path);
        mapname.sun_path[0] = '\0';
		XLOGI("[BT][MAP]create map server socket success");
        if (getsockname(nat->mapsrvsock, (sockaddr*)&mapname, &mapnamelen) < 0)
        {
            XLOGI("[BT][MAP]getsockname failed : %s, errno=%d", strerror(errno), errno);
			goto exit;
		}
        else
        {
            XLOGI("[BT][MAP]Auto bind MAP server : len=%d, addr=%s", mapnamelen, &mapname.sun_path[1]);
        }
   	}

	//api socket
	mapnamelen = sizeof(short);
    nat->mapsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (nat->mapsock < 0)
    {
        XLOGE("[BT][MAP][ERR] create MAP api socket failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }
    if (bind (nat->mapsock, (struct sockaddr *) &mapname, mapnamelen) < 0)
    {
        XLOGE("[BT][MAP][ERR] bind MAP api socket failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }
    else
    {
        mapnamelen = sizeof(mapname.sun_path);
        mapname.sun_path[0] = '\0';
        if (getsockname(nat->mapsock, (sockaddr*)&mapname, &mapnamelen) < 0)
        {
            XLOGE("[BT][MAP][ERR] getsockname failed : %s, errno=%d", strerror(errno), errno);
			goto exit;
        }
        else
        {
            XLOGI("[BT][MAP]Auto bind MAP server : len=%d, addr=%s", mapnamelen, &mapname.sun_path[1]);
        }
    }
    if ( connect(nat->mapsock, (const struct sockaddr*)&btname, btnamelen) < 0)
    {
        XLOGE("[BT][MAP][ERR] connect to /data/btserv failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }
	if (JNI_TRUE != map_startEventLoop(env,obj)) 
	{
		XLOGE("[BT][MAP][ERR] fail to start event loop");
		goto exit;
	}

	btmtk_map_server_init(btmtk_map_server_callback);

	btmtk_map_set_socket(nat->mapsock, nat->mapsrvsock);

		return JNI_TRUE;

	exit: 
		if(nat->mapsrvsock >= 0) 
		{
			close(nat->mapsrvsock);
			nat->mapsrvsock = -1;
		}
		if(nat->mapsock >= 0) 
		{
			close(nat->mapsock);
			nat->mapsock = -1;
		}
		if (nat){
			free(nat);
			nat = NULL;
		}
		return JNI_FALSE;

	//jniThrowIOException(env, ENOSYS);
}

static void deinitNative(JNIEnv *env, jobject obj)
{
    XLOGV(__FUNCTION__);
	XLOGI("[BT][MAP][JNI] deinitNative +++");
	int ret;
	if (NULL == nat) 
	{
		XLOGE("[MAP][JNI] %s: native data is null!", __FUNCTION__);
		return ;
	}

	map_stopEventLoop();	
	if(nat->mapsock >= 0) 
	{
		close(nat->mapsock);
		nat->mapsock = -1;
	}
	pthread_mutex_destroy(&(nat->thread_mutex));
	if (nat->me) env->DeleteGlobalRef(nat->me);
	nat->me = NULL;
	if (nat)
	{
		free(nat);
		nat = NULL;
	}
	XLOGI("[MAP][JNI] deinitNative ---");

}


static jint enableNative(JNIEnv *env, jobject obj) 
{
	XLOGI("[BT][MAP][JNI] enableNative ");
	BT_BOOL result = FALSE;	
	result = btmtk_map_server_enable();
	return result ? 0 : -1;
}

static jint disableNative(JNIEnv *env, jobject obj)
{
	BT_BOOL result;
	XLOGI("[BT][MAP][JNI]disableNative");
	result = btmtk_map_server_disable();
	return result ? 0 : -1;
}
static jint registerInstanceNative(JNIEnv *env, jobject obj, jint masInstaneId, jstring severName, jint messageType, jstring rootPath) 
{	
	XLOGI("[BT][MAP][JNI]registerInstanceNative");
	BT_BOOL result = FALSE;

	MBT_MAP_MAS_INFO masInfo;
	masInfo.MasInstId = masInstaneId;	
	const char *c_root = NULL;
	c_root = env->GetStringUTFChars(rootPath, NULL);	
	const char *c_svname = NULL;
	c_svname = env->GetStringUTFChars(severName, NULL);
	if (c_root != NULL) 
	{
		strncpy((char *)masInfo.RootPath, c_root, MBT_MAX_FILE_NAME_LEN);
	}
	XLOGI("[BT][MAP][JNI]RootPath is %s",masInfo.RootPath);
	if (c_svname != NULL) 
	{
		strncpy((char *)masInfo.SvcName, c_svname, MBT_MAP_MAX_SVC_NAME_LEN);
	}
	masInfo.SupMsgType = messageType;
	
	result = btmtk_map_server_register(&masInfo);

	env->ReleaseStringUTFChars(rootPath, c_root);
	env->ReleaseStringUTFChars(severName,c_svname);
	
	return result ? 0 : -1;
}

static jint deregisterInstanceNative(JNIEnv *env, jobject obj, jint masInstaneId) 
{
	XLOGI("[BT][MAP][JNI]deregisterInstanceNative");
	BT_BOOL result;
	result = btmtk_map_server_deregister((MBT_BYTE)masInstaneId);
	return result ? 0 : -1;
}

static jint disconnectMasNative(JNIEnv * env,jobject obj,jint masId)
{
	XLOGI("[BT][MAP][JNI]disconnectMasNative");	
	BT_BOOL result;
	result = btmtk_maps_disconnect((S8) masId);
	return result ? 0 :-1;	
}
static jint disconnectServerNative(JNIEnv * env,jobject obj)
{
	XLOGI("[BT][MAP][JNI]disconnectServerNative");	
	BT_BOOL result;
	result = btmtk_maps_disconnect_server();
	return result ? 0 :-1;
}

static jint disconnectMnsSessionNative(JNIEnv * env,jobject obj,jstring address)
{
	bt_addr_struct addr;
	XLOGI("[BT][MAP][JNI]disconnectMnsSessionNative");
	const char *c_address = env->GetStringUTFChars(address, NULL);	
	btmtk_util_convert_string2bdaddr(c_address, &addr);	
	env->ReleaseStringUTFChars(address, c_address);
	return (btmtk_maps_mns_disconnect(&addr) == TRUE) ? 0 : -1;
}

static jint setNotifRegResponseNative
	(JNIEnv *env, jobject obj,jstring path, jint masInstanceId, jint result) 
{
	const char *c_path = env->GetStringUTFChars(path, NULL);
	bt_addr_struct addr; 
	MBT_MAP_PUSH_MSG_RSP *mas_rsp;
	
	BT_BOOL ret;
	XLOGI("[BT][MAP][JNI]setNotifRegResponseNative");
	btmtk_util_convert_string2bdaddr(c_path, &addr);
	env->ReleaseStringUTFChars(path, c_path);
	
//	U8 allow = (result == 0)? MBT_ALLOW_RES : MBT_ERROR_RES;
	
	ret = btmtk_maps_set_notif_reg(masInstanceId, result);	
	return ret ? 0 :-1;
}

static jint sendMessageResponseNative
	(JNIEnv *env, jobject obj, jstring path, jint masInstanceId, jint result, jlong handle) 
{
	BT_BOOL pushRet = FALSE; 
	MBT_MAP_PUSH_MSG_RSP* rsp = NULL;
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masInstanceId);
	if (!mas) 
	{
		XLOGE("no the specified MAS ");
		return -1;
	}
	rsp = &mas->Oper.PushMsg.Rsp;
	rsp->MsgHandle = (MBT_UINT64)handle;
	pushRet = btmtk_maps_push_message((U8)masInstanceId, result);
	return pushRet;
}

//TODO
static jint parseMessageNative
	(JNIEnv *env, jobject obj, jstring path, jint masInstanceId, jobject message) 
{	
	const char *c_path = NULL;
	const char *c_file = NULL; 
	bt_addr_struct addr; 	
	BT_BOOL parseRet = FALSE; 
	MBT_MAP_MSG_OBJECT *msg = NULL;
	unsigned int index = 0;
	MBT_MAP_PUSH_MSG_REQ *req = NULL;
	MBT_MAP_MSG_OBJECT *object = NULL;
	jstring stringobject;
	XLOGI("[BT][MAP][JNI]parseMessageNative");

	c_path = env->GetStringUTFChars(path, NULL);
	btmtk_util_convert_string2bdaddr(c_path, &addr);
	env->ReleaseStringUTFChars(path, c_path);
	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masInstanceId);
	if (!mas) 
	{
		XLOGE("no the specified MAS ");
		return -1;
	}
	req = &mas->Oper.PushMsg.Req;
	object = &req->Data.File.Msg;
	memset(object, 0x0, sizeof(MBT_MAP_MSG_OBJECT));
	object->Charset = 0xFF;

	req->StorageType = MBT_MAP_STORAGE_TYPE_FILE;
	//set file name
	jstring file = (jstring)env->GetObjectField(message, field_BMessageObject_mFileName);
	c_file = env->GetStringUTFChars(file, NULL);
	strcpy(req->Data.File.FileName, c_file);
	env->ReleaseStringUTFChars(file, c_file);
	
	parseRet = btmtk_maps_parse_message((U8)masInstanceId);
	if (parseRet == TRUE)
	{
		env->SetBooleanField(message, field_BMessageObject_mTransparent, 
							(req->Transp == MBT_MAP_RETRY_TYPE_OFF)? JNI_FALSE : JNI_TRUE);
		env->SetBooleanField(message, field_BMessageObject_mRetry, 
							(req->Retry == MBT_MAP_RETRY_TYPE_OFF)? JNI_FALSE : JNI_TRUE);

		env->SetIntField(message, field_BMessageObject_mReadStatus, object->ReadStatus);
		env->SetIntField(message, field_BMessageObject_mMsgType, object->MsgType);
		
		stringobject = env->NewStringUTF(req->FolderPath);
		env->SetObjectField(message, field_BMessageObject_mFolderPath, stringobject);
		env->DeleteLocalRef(stringobject);
		
		env->SetIntField(message, field_BMessageObject_mPartId, object->PartId);
		if(object->Charset == 0xFF)
		{
			object->Charset = req->Charset;
		}
		env->SetIntField(message, field_BMessageObject_mCharset, object->Charset);
		env->SetIntField(message, field_BMessageObject_mEncoding, object->Encoding);
		env->SetIntField(message, field_BMessageObject_mLanguage, object->Lang);
		if (object->OriginatorSize != 0 && object->Originator != NULL)
		{
			stringobject = env->NewStringUTF(object->Originator);
			env->SetObjectField(message, field_BMessageObject_mOrignator, stringobject);
			env->SetIntField(message, field_BMessageObject_mOrignatorSize, object->OriginatorSize);
			env->DeleteLocalRef(stringobject);
		}
		env->SetIntField(message, field_BMessageObject_mPartId, object->PartId);
		jobject recipient = env->GetObjectField(message, field_BMessageObject_mRecipient);
		for(int i = 0; i < MBT_MAP_MAX_MSG_ENVELOPE_NUM; i++)
		{
			if (object->Envelope[i].Recipient != NULL)
			{
				stringobject = env->NewStringUTF(object->Envelope[i].Recipient);
				env->CallVoidMethod(recipient, method_ArrayList_add, stringobject);
				env->DeleteLocalRef(stringobject);
			}
		}
		env->SetIntField(message, field_BMessageObject_mWholeSize, object->ContentSize);
		jobject content_size = env->GetObjectField(message, field_BMessageObject_mContentSize);
		jclass intClass =  env->FindClass("java/lang/Integer");
		for(index = 0; index < object->FragmentNum; index ++)
		{
			
			jobject integer = env->NewObject(intClass, method_Integer_int, object->FragmentSize[index]);
			env->CallVoidMethod(content_size, method_ArrayList_add, integer);
			env->DeleteLocalRef(integer);
		}
	}

	// clear memory
	if(req){
        	if (req->StorageType == MBT_MAP_STORAGE_TYPE_BUFFER)
        	{
        		msg = &req->Data.File.Msg;
        	} 
        	else if (req->StorageType == MBT_MAP_STORAGE_TYPE_FILE) 
        	{
        		msg = &req->Data.Buffer.Msg;
        	}
       }
	if ( msg != NULL && msg->Originator != NULL)
	{
		free(msg->Originator);
		msg->Originator = NULL;
	}
	for(index = 0 ;index < MBT_MAP_MAX_MSG_ENVELOPE_NUM; index++)
	{
		if (msg != NULL && msg->Envelope[index].Recipient != NULL) 
		{
			free(msg->Envelope[index].Recipient);
			msg->Envelope[index].Recipient = NULL;
		}
	}
	return parseRet ? 0 :-1;
	
}



static jint getMessageListResponseNative
	(JNIEnv *env, jobject obj,jstring path, jint masInstanceId, jint result, jobject messageList)
{
		const char *c_path = env->GetStringUTFChars(path, NULL);
		bt_addr_struct addr;
		jstring stringCache;
		const char* charCache; 
		int msgNum;
		jobject messageItem;
		MBT_MAP_GET_MSG_LIST_RSP *mas_rsp = NULL;
		MBT_MAP_MSG_LIST_ENTRY *list = NULL;		
		XLOGI("[BT][MAP][JNI]getMessageListResponseNative");
		btmtk_util_convert_string2bdaddr(c_path, &addr);
		env->ReleaseStringUTFChars(path, c_path);
	
		if (result == MBT_ALLOW_RES)
		{
			if (messageList == NULL)
			{
				XLOGI("the input object is null");
				result = MBT_NOT_FOUND_RES;
				goto END;
			}
			MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masInstanceId);
			if (!mas) 
			{
				XLOGE("no the specified MAS ");
				return -1;
			}
			mas_rsp = &mas->Oper.GetMsgList.Rsp;
			
			memset(mas_rsp, 0x0, sizeof(MBT_MAP_GET_MSG_LIST_RSP));
			stringCache = (jstring)env->GetObjectField(messageList, field_MessageListObject_mCurrentTime);
			const char *timechar = env->GetStringUTFChars(stringCache, NULL);
			if (timechar != NULL)
			{
				strncpy(mas_rsp->MSETime, timechar, sizeof(MBT_MAP_MAX_TIME_LEN));
			}
			env->ReleaseStringUTFChars(stringCache, timechar);

			msgNum = env->GetIntField(messageList, field_MessageListObject_mSize);	
			
			XLOGI("msgNum %d", msgNum);
			if (msgNum < 0){
				XLOGI("the msg num < 0");
				result = MBT_NOT_FOUND_RES;
				goto END;
			}
			mas_rsp->NewMsg = (MBT_BYTE)env->GetIntField(messageList, field_MessageListObject_mNewMessage);
			mas_rsp->TotalMsgNum = msgNum;
			mas_rsp->TodoMsgNum = 0;
			mas_rsp->StorageType = MBT_MAP_STORAGE_TYPE_BUFFER;
			
			jobject arraylist = env->GetObjectField(messageList, field_MessageListObject_mMessageItems);
			int size = env->CallIntMethod(arraylist, method_ArrayList_size);	
			XLOGI("size %d", size);
			if (size <= 0 ) {
				XLOGI("the actual size <= 0");
				mas_rsp->Data.Buffer.MsgNum  = 0;
				goto END;
			}
			mas_rsp->Data.Buffer.MsgNum = size;

			list = (MBT_MAP_MSG_LIST_ENTRY *)malloc(sizeof(MBT_MAP_MSG_LIST_ENTRY)*size);
			if (list == NULL)
			{
				XLOGE("fail to malloc memory");
				result = MBT_ERROR_RES;
				goto END;
			}
			//neccessary to set cache as 0
			memset(list, 0x0, sizeof(MBT_MAP_MSG_LIST_ENTRY)*size);
			mas_rsp->Data.Buffer.MsgList = list;			
						
			for (int i = 0; i < size; i++) 
			{	
				//no item in list
				if (size == 0) {
					break;
				}
				messageItem = env->CallObjectMethod(arraylist, method_ArrayList_get, i);	
				if (messageItem == NULL){
					XLOGV("messageItem is null");
					break;
				}
				list->AttachSize = env->GetIntField(messageItem, field_MessageItem_AttachSize);
				list->MsgHandle	 = env->GetLongField(messageItem, field_MessageItem_MsgHandle); 
				list->bPriority  = env->GetBooleanField(messageItem, field_MessageItem_bPriority) == JNI_TRUE ?
									TRUE : FALSE;
				list->bProtected = env->GetBooleanField(messageItem, field_MessageItem_bProtected) == JNI_TRUE ?
									TRUE : FALSE;
				int read = env->GetIntField(messageItem, field_MessageItem_read);
				if (MBT_MAP_MSG_STATUS_UNREAD == read)
				{
					list->bRead = FALSE;
				} else {
					list->bRead = TRUE;
				}
				list->bSent		 = env->GetBooleanField(messageItem, field_MessageItem_bSent) == JNI_TRUE ?
									TRUE : FALSE;
				list->bText		 = env->GetBooleanField(messageItem, field_MessageItem_bText) == JNI_TRUE ?
									TRUE : FALSE;
				stringCache = (jstring)env->GetObjectField(messageItem, field_MessageItem_DateTime);
				const char* datechar = env->GetStringUTFChars(stringCache, NULL);				
				if (stringCache != NULL && datechar != NULL) 
				{
					strncpy(list->DateTime, datechar, MBT_MAP_MAX_DATETIME_LEN);
				}
				env->ReleaseStringUTFChars(stringCache, datechar);

				list->MsgType	= env->GetIntField(messageItem, field_MessageItem_MsgType);
				list->OriginMsgSize = env->GetIntField(messageItem, field_MessageItem_OrignalMsgSize);
				list->ReceptionStatus = (MBT_BYTE)env->GetIntField(messageItem, field_MessageItem_ReciptionStatus);

				
				stringCache = (jstring)env->GetObjectField(messageItem, field_MessageItem_RecipientAddr);
				const char *RDchar = env->GetStringUTFChars(stringCache, NULL);
				if (stringCache != NULL && RDchar != NULL) 
				{
					strncpy(list->RecipientAddr, RDchar, MBT_MAP_MAX_MSG_LIST_ATTR_LEN);
				}
				env->ReleaseStringUTFChars(stringCache, RDchar);

				stringCache = (jstring)env->GetObjectField(messageItem, field_MessageItem_RecipientName);
				const char *RNchar = env->GetStringUTFChars(stringCache, NULL);
				if (stringCache != NULL && RNchar != NULL) 
				{
					strncpy(list->RecipientName, RNchar, MBT_MAP_MAX_MSG_LIST_ATTR_LEN);
				}
				env->ReleaseStringUTFChars(stringCache, RNchar);

				
				stringCache = (jstring)env->GetObjectField(messageItem, field_MessageItem_ReplyToAddr);
				const char *RTchar = env->GetStringUTFChars(stringCache, NULL);
				if (stringCache != NULL && RTchar != NULL) 
				{
					strncpy(list->ReplyToAddr, RTchar, MBT_MAP_MAX_MSG_LIST_ATTR_LEN);
				}
				env->ReleaseStringUTFChars(stringCache, RTchar);
				
				stringCache = (jstring)env->GetObjectField(messageItem, field_MessageItem_SenderAddr);
				const char *SDchar = env->GetStringUTFChars(stringCache, NULL);
				XLOGI("sender address %s", SDchar);
				if (stringCache != NULL && SDchar != NULL) 
				{
					strncpy(list->SenderAddr , SDchar, MBT_MAP_MAX_MSG_LIST_ATTR_LEN);
				}
				XLOGI("SenderAddr %s", list->SenderAddr);
				env->ReleaseStringUTFChars(stringCache, SDchar);

				stringCache = (jstring)env->GetObjectField(messageItem, field_MessageItem_SenderName);
				const char *SNchar = env->GetStringUTFChars(stringCache, NULL);
				if (stringCache != NULL && SNchar != NULL) 
				{
					strncpy(list->SenderName, SNchar, MBT_MAP_MAX_MSG_LIST_ATTR_LEN);
				}
				env->ReleaseStringUTFChars(stringCache, SNchar);

				stringCache = (jstring)env->GetObjectField(messageItem, field_MessageItem_Subject);
				const char *SUchar = env->GetStringUTFChars(stringCache, NULL);
				if (stringCache != NULL && SUchar != NULL) 
				{
					strncpy(list->Subject, SUchar, MBT_MAP_MAX_MSG_LIST_ATTR_LEN);
				}
				env->ReleaseStringUTFChars(stringCache, SUchar);
				
			//	list += sizeof(MBT_MAP_GET_MSG_LIST_RSP);
				list += 1;
			}
		}
	END:	
		if(btmtk_maps_get_message_list(masInstanceId, result) == FALSE)
		{
			XLOGE("error to send message list");
		}
	 	if (mas_rsp != NULL && mas_rsp->Data.Buffer.MsgList != NULL) {
			free(mas_rsp->Data.Buffer.MsgList);
			mas_rsp->Data.Buffer.MsgList = NULL;
			list = NULL;
		}
		return 0;	
}

//TODO
static int getFolderListResponseNative
	(JNIEnv *env, jobject obj,jstring path, jint masInstanceId, jint result, jobjectArray folderList) 
{
	const char *c_path = env->GetStringUTFChars(path, NULL);
	bt_addr_struct addr;
	int index;
	const char* temp_char = NULL;
	jobject folderItem;
	jsize length = 0;
	MBT_MAP_FOLDER_LIST_ENTRY *list = NULL;
	MBT_MAP_GET_FOLDER_LIST_RSP *mas_rsp = NULL; 
	XLOGI("[BT][MAP][JNI]getFolderListResponseNative");

	btmtk_util_convert_string2bdaddr(c_path,&addr);
	env->ReleaseStringUTFChars(path, c_path);

	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masInstanceId);
	if (!mas) {
		XLOGE("[BT][MAP][JNI]can not find the MBT_MAP_MAS_STATUS related to the masid");
		return -1;
	}

	if (MBT_ALLOW_RES != result) 
	{
		goto END;
	}

  
	mas_rsp = &mas->Oper.GetFolderList.Rsp;
	memset(mas_rsp, 0x0, sizeof(MBT_MAP_GET_FOLDER_LIST_RSP));
	mas_rsp->StorageType     = MBT_MAP_STORAGE_TYPE_BUFFER; 
	
	if (folderList != NULL) {			
		length = env->GetArrayLength(folderList);			
	}
	mas_rsp->TotalFolderNum = length;
	mas_rsp->Data.Buffer.FolderNum = length;
	
	if (length == 0) 
	{
		XLOGI("folder num is zero");
		goto END;
	}
	
	list = (MBT_MAP_FOLDER_LIST_ENTRY *)malloc(sizeof(MBT_MAP_FOLDER_LIST_ENTRY)*length) ;
	if (!list) {
		XLOGE("[BT][MAP][JNI]fail to malloc memory");
		return -1;
	}
	mas_rsp->Data.Buffer.FolderList = list;
	XLOGI("length is %d", length);
	for (index = 0; index < length; index++) {
		folderItem = env->GetObjectArrayElement(folderList, index);
		//list->Mode
		//folder name
		jstring name = (jstring)env->GetObjectField(folderItem, field_FolderListObject_mName);
		temp_char = env->GetStringUTFChars(name, NULL);
		if (temp_char != NULL)
		{
			strncpy(list->Name,temp_char, MBT_MAX_FILE_NAME_LEN);
			list->Name[MBT_MAX_FILE_NAME_LEN] = '\0';
		}
		XLOGI("name is %s", temp_char);
		env->ReleaseStringUTFChars(name, temp_char);

		XLOGI("for end");

		jstring time = (jstring)env->GetObjectField(folderItem, field_FolderListObject_mTime);
		const char* date_char = env->GetStringUTFChars(time, NULL);
		if (date_char != NULL)
		{
			strncpy(list->Time,date_char, MBT_MAP_MAX_DATETIME_LEN);
			list->Time[MBT_MAP_MAX_DATETIME_LEN] = '\0';
		}
		XLOGI("date is %s", date_char);
		env->ReleaseStringUTFChars(time, date_char);

		list->Size = env->GetIntField(folderItem, field_FolderListObject_mSize);
		list++;
	}
	XLOGI("for end");

END:
	btmtk_maps_get_folder_list(masInstanceId, result);
	if ((mas_rsp != NULL) && (mas_rsp->Data.Buffer.FolderList != NULL)) 
	{
		free(mas_rsp->Data.Buffer.FolderList);
		mas_rsp->Data.Buffer.FolderList = NULL;
		list = NULL;
	}
	return 0;
}

static int getMessageResponseNative
	(JNIEnv *env, jobject obj,jstring path, jint masInstanceId, jint result, jobject message)
{	
	const char *c_path = env->GetStringUTFChars(path, NULL);
	bt_addr_struct addr;
	MBT_MAP_MSG_OBJECT * object = NULL;
	jstring stringcache;
	jobject fragment_num;
	const char *folderchar = NULL;
	const char *Ochar = NULL;
	const char* filechar = NULL;
	int orignator_length;
	jobject recipient;
	int child_recipient_length = 0;
	int recipientNum = 0;
	XLOGI("[BT][MAP][JNI]getMessageResponseNative");

	btmtk_util_convert_string2bdaddr(c_path, &addr);
	env->ReleaseStringUTFChars(path, c_path);

	MBT_MAP_MAS_STATUS *mas = btmtk_map_util_search_mas_instance(masInstanceId);
	if (!mas) {
		XLOGE("[BT][MAP][JNI]can not find the MBT_MAP_MAS_STATUS related to the masid");
		return -1;
	}

	MBT_MAP_GET_MSG_RSP *mas_rsp = &mas->Oper.GetMsg.Rsp;
	memset(mas_rsp, 0x0, sizeof(MBT_MAP_GET_MSG_RSP));

	if (message == NULL)
	{
		XLOGE("the input is null");
		goto END;
	}

	if (MBT_ALLOW_RES != result) {
		goto END;
	}
			
//	mas_rsp->StorageType = MBT_MAP_STORAGE_TYPE_BUFFER; 
	mas_rsp->StorageType = MBT_MAP_STORAGE_TYPE_FILE; 

	
	//TODO: set fraction deliver based on MMI
	mas_rsp->FracDeliver = env->GetIntField(message, field_BMessageObject_mFracDeliver);
	
	mas_rsp->Data.File.Size = (MBT_UINT)env->GetLongField(message, field_BMessageObject_mWholeSize);
	stringcache = (jstring)env->GetObjectField(message, field_BMessageObject_mFileName);
	filechar = env->GetStringUTFChars(stringcache, NULL);
	if(filechar != NULL)
	{
		strncpy(mas_rsp->Data.File.FileName, filechar,  MBT_MAX_FILE_NAME_LEN);
		mas_rsp->Data.File.FileName[MBT_MAX_FILE_NAME_LEN] = '\0';
	}
	env->ReleaseStringUTFChars(stringcache, filechar);

	
	object = &(mas_rsp->Data.File.Msg);
	if (object == NULL) {
		XLOGE("[BT][MAP][JNI]object is null");
		result = MBT_ERROR_RES;
		goto END;
	}
	object->Charset = env->GetIntField(message,field_BMessageObject_mCharset);
	object->Content = NULL;

	fragment_num = env->GetObjectField(message, field_BMessageObject_mContentSize);
	object->FragmentNum = env->CallIntMethod(fragment_num, method_ArrayList_size);

	for (unsigned int i=0; i < object->FragmentNum; i++)
	{
		jobject fragsize = env->CallObjectMethod(fragment_num, method_ArrayList_get, i);
		object->FragmentSize[i] =  env->CallIntMethod(fragsize, method_Integer_intValue);
		XLOGI("object->FragmentSize[%d]:%d", i, object->FragmentSize[i]);
	}
			
	object->ContentSize = env->GetIntField(message, field_BMessageObject_mWholeSize);
	object->Encoding = env->GetIntField(message,field_BMessageObject_mEncoding);

	//TODO:
	recipient = env->GetObjectField(message, field_BMessageObject_mRecipient);
//	jstring child_recipient = NULL;
	recipientNum = env->CallIntMethod(recipient, method_ArrayList_size);
	if (recipientNum > 0) 
	{
		for (int i = 0 ; i < MBT_MAP_MAX_MSG_ENVELOPE_NUM && i < recipientNum; i++)
		{
			stringcache = (jstring)env->CallObjectMethod(recipient, method_ArrayList_get, i);
			child_recipient_length = env->GetStringLength(stringcache);
			if (child_recipient_length == 0) 
			{
				continue;
			}
			object->Envelope[i].Recipient = (MBT_CHAR *)malloc (child_recipient_length+1); 
			if (object->Envelope[i].Recipient == NULL) 
			{
				XLOGE("fail to malloc memory for Envelope[%d].Recipient", i);
				goto END;
			}				
			object->Envelope[i].RecipientSize = child_recipient_length;
			const char * recipientchar = env->GetStringUTFChars(stringcache, NULL);	
			if(recipientchar != NULL)
			{
				strncpy(object->Envelope[i].Recipient, recipientchar, child_recipient_length);
			}
			object->Envelope[i].Recipient[child_recipient_length] = '\0';
			env->ReleaseStringUTFChars(stringcache, recipientchar);
		}
	}
	

	stringcache = (jstring)env->GetObjectField(message, field_BMessageObject_mFolderPath);
	folderchar = env->GetStringUTFChars(stringcache, NULL);
	if (object->FolderPath != NULL && folderchar != NULL)
	{
		strncpy(object->FolderPath, folderchar,  MBT_MAP_MAX_FOLDER_PATH_LEN);
		object->FolderPath[MBT_MAP_MAX_FOLDER_PATH_LEN] = '\0';
	} else {
		XLOGE("[BT][MAP][JNI] object->FolderPath is null");
	}
	env->ReleaseStringUTFChars(stringcache, folderchar);

	object->Lang = env->GetIntField(message, field_BMessageObject_mLanguage);
	object->MsgType = env->GetIntField(message, field_BMessageObject_mMsgType);

	//orignator size and orignator
	stringcache = (jstring)env->GetObjectField(message, field_BMessageObject_mOrignator);
	orignator_length = env->GetStringLength(stringcache);
	object->OriginatorSize = orignator_length;
	object->Originator = (MBT_CHAR *)malloc (orignator_length+1); 
	if (object->Originator != NULL && orignator_length > 0) 
	{
		Ochar = env->GetStringUTFChars(stringcache, NULL);
		if (Ochar != NULL)
		{
			strncpy(object->Originator, Ochar,  orignator_length);
		}
		env->ReleaseStringUTFChars(stringcache, Ochar);
	}

	object->PartId = env->GetIntField(message, field_BMessageObject_mPartId);
	object->ReadStatus = env->GetIntField(message, field_BMessageObject_mReadStatus);
		
	/*end*/
END:	
	btmtk_maps_get_message(masInstanceId, result);
	if (object != NULL) 
	{
		for (int i = 0 ; i < MBT_MAP_MAX_MSG_ENVELOPE_NUM && i < recipientNum; i++)
		{
			if ((object != NULL) && (object->Envelope[i].Recipient != NULL)) 
			{
				free(object->Envelope[i].Recipient);
				object->Envelope[i].Recipient = NULL;
			}
		}
		if (object != NULL && object->Originator != NULL) 
		{
			free(object->Originator);
			object->Originator = NULL;
		}
	}
	return 0;

}
static jint setMessageStatusResponseNative
	(JNIEnv *env, jobject obj,jstring path, jint masInstanceId, jint result)
{
	XLOGI("[BT][MAP][JNI]setMessageStatusResponseNative");
	const char *c_path = env->GetStringUTFChars(path, NULL);
	bt_addr_struct addr;
	
	btmtk_util_convert_string2bdaddr(c_path, &addr);
	env->ReleaseStringUTFChars(path, c_path);

	btmtk_maps_set_message_status(masInstanceId, result);
	return 0;
}
static jint updateInboxResponseNative
	(JNIEnv *env, jobject obj,jstring path, jint masInstanceId, jint result)
{
	const char *c_path = env->GetStringUTFChars(path, NULL);
	bt_addr_struct addr;
	XLOGI("[BT][MAP][JNI]updateInboxResponseNative");

	btmtk_util_convert_string2bdaddr(c_path, &addr);

	env->ReleaseStringUTFChars(path, c_path);

	btmtk_maps_update_inbox(masInstanceId, result);
	return 0;
}
static jint setFolderResponseNative	
	(JNIEnv *env, jobject obj,jstring path, jint masInstanceId, jint result)
{	
	const char *c_path = env->GetStringUTFChars(path, NULL);
	bt_addr_struct addr;

	XLOGV("[BT][MAP][JNI]setFolderResponseNative(): masID is %d,result is %d",masInstanceId, result);
	btmtk_util_convert_string2bdaddr(c_path, &addr);
	env->ReleaseStringUTFChars(path, c_path);

	btmtk_maps_set_folder(masInstanceId, result);
	return 0;

}



static jint sendReportNative
	(JNIEnv *env, jobject obj, jstring path, jobject report)
{
	const char *c_path = env->GetStringUTFChars(path, NULL);
	const char *c_folder = NULL;
	const char *c_old_folder = NULL;
	jstring folder;
	jstring old_folder;
	bt_addr_struct addr;
	XLOGI("[BT][MAP][JNI]sendReportNative()()");

	btmtk_util_convert_string2bdaddr(c_path, &addr);	

	
	XLOGI("c_path is %s ", c_path);
	XLOGI("convert to addr.lap=0x%X, addr.uap=0x%X,addr.nap=0x%X", addr.lap, addr.uap, addr.nap);
	
	env->ReleaseStringUTFChars(path, c_path);	
	
	//it maybe better to set a fixed memoty for the report 
	MBT_MAP_MNS_STATUS * mns = btmtk_map_util_search_mns(&addr); 
	if (mns == NULL){
		XLOGE("fail to find the mns for the device:");		
		return -1;
	}
	if (mns->State != MBT_MAP_STATE_CONNECTED) {
		XLOGE("the mns session is busy");
		return -1;
	}
	
	MBT_MAP_SEND_EVENT_REQ* SendEvt = &mns->SendEvt; 
	
	memset(SendEvt, 0x0, sizeof(MBT_MAP_SEND_EVENT_REQ));
	
	folder = (jstring)env->GetObjectField(report, field_EventReport_mFolder);
	old_folder = (jstring)env->GetObjectField(report, field_EventReport_mOldFolder);
	
	c_folder = env->GetStringUTFChars(folder, NULL);
	c_old_folder = env->GetStringUTFChars(old_folder, NULL);

	//set SendEvt
	SendEvt->MasInstId = env->GetIntField(report, field_EventReport_mMasId);
	SendEvt->EvtRptObj.NotifType = env->GetIntField(report, field_EventReport_mEventType);
	SendEvt->EvtRptObj.MsgHandle = env->GetLongField(report, field_EventReport_mHandle);
	if (c_folder != NULL)
	{
		strncpy(SendEvt->EvtRptObj.FolderPath, c_folder,MBT_MAP_MAX_FOLDER_PATH_LEN) ;
	}
	if (c_old_folder != NULL)
	{
		strncpy(SendEvt->EvtRptObj.OldFolderPath, c_old_folder, MBT_MAP_MAX_FOLDER_PATH_LEN);
	}
	SendEvt->EvtRptObj.MsgType = env->GetIntField(report, field_EventReport_mMsgType);

	

	btmtk_maps_send_event(&addr);
	env->ReleaseStringUTFChars(folder, c_folder);	
	env->ReleaseStringUTFChars(old_folder, c_old_folder);
	return 0;

}


static jint authorizeResponseNative(JNIEnv *env, jobject obj, jstring path, jint authMode) 
{
	const char *c_path = env->GetStringUTFChars(path, NULL);
	bt_addr_struct addr;
	XLOGI("[BT][MAP][JNI]authorizeResponseNative");

	btmtk_util_convert_string2bdaddr(c_path, &addr);	
	btmtk_maps_authorize_res(&addr, authMode);
	env->ReleaseStringUTFChars(path, c_path);	
	return 0;
}


static void  btmtk_map_handle_enable_confirm(BT_BOOL result) 
{
	JNIEnv *env;
	nat->vm->GetEnv((void**)&env, nat->envVer);
	jint ret;

	ret = (result == TRUE)? 0: -1; 	

	env->CallVoidMethod(nat->me, method_onRequestOrIndicationReceived, 
						MAP_SERVER_ENABLE_CNF,
						ret,
						-1,
						NULL);
	
}

static void  btmtk_map_handle_disable_confirm(BT_BOOL result) 
{
	JNIEnv *env;
	nat->vm->GetEnv((void**)&env, nat->envVer);
	
	jint ret;

	ret = (result == TRUE)? 0: -1;	
		

	env->CallVoidMethod(nat->me, method_onRequestOrIndicationReceived, 
						MAP_SERVER_DISABLE_CNF,
						ret,
						-1,
						NULL);
}
static void  btmtk_map_handle_register_confirm(U8 *masId, BT_BOOL result) 
{
	JNIEnv *env;
	nat->vm->GetEnv((void**)&env, nat->envVer);
	
	jint ret;
	ret = (result == TRUE)? 0: -1;	
		
	
	env->CallVoidMethod(nat->me, method_onRequestOrIndicationReceived, 
						MAP_SERVER_REGISTER_CNF,
						*masId,
						ret,
						NULL);
}
static void  btmtk_map_handle_deregister_confirm(U8 *masId, BT_BOOL result) 
{
	JNIEnv *env;
	nat->vm->GetEnv((void**)&env, nat->envVer);
	jint ret;
	ret = (result == TRUE)? 0: -1;	 	
	
	env->CallVoidMethod(nat->me, method_onRequestOrIndicationReceived, 
						MAP_SERVER_DEREGISTER_CNF,
						*masId,
						ret,
						NULL);
}
static void btmtk_map_handle_authorize_indication(bt_map_addr_struct *address)
{
	JNIEnv *env;
	jstring pathstring;
	nat->vm->GetEnv((void**)&env, nat->envVer);
	char path[18];
	memset(path, 0x0, 18);
	btmtk_util_convert_bdaddr2string(path, address);

	pathstring = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me, 
						method_onRequestOrIndicationReceived, 
						MAP_SERVER_AUTHORIZE_IND,
						-1,
						-1,
						pathstring);
	env->DeleteLocalRef(pathstring);

}


static void btmtk_map_handle_connect_indication(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	jstring path_string;
	char path[18];
	memset(path, 0x0, 18);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me, 
							method_onRequestOrIndicationReceived, 
							MAP_SERVER_CONNECT_IND,
							status->masInfo.MasInstId,
							-1,
							path_string);
	env->DeleteLocalRef(path_string);
}
static void btmtk_map_handle_disconnect(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	jstring path_string;
	char path[18];
	memset(path, 0x0, 18);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me, 
							method_onRequestOrIndicationReceived, 
							MAP_SERVER_DISCONNECT_CNF,
							status->masInfo.MasInstId,
							-1,
							path_string);
	env->DeleteLocalRef(path_string);
}
static void btmtk_map_handle_set_folder_indication(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	char path[18];
	jstring path_string;
	memset(path, 0x0, 18);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);

	jobject folderache = env->CallObjectMethod(nat->me, method_getFolderCache, status->masInfo.MasInstId);
	env->SetIntField(folderache, field_SetFolderRequest_mMasId, status->masInfo.MasInstId);	

	path_string = env->NewStringUTF(path);
	env->SetObjectField(folderache, field_SetFolderRequest_mAddr, path_string);
	env->DeleteLocalRef(path_string);

	path_string = env->NewStringUTF(status->Oper.SetFolderReq.FolderPath);
	env->SetObjectField(folderache, field_SetFolderRequest_mFolder, path_string);
	env->DeleteLocalRef(path_string);
	
	env->CallVoidMethod(nat->me, 
							method_onRequestOrIndicationReceived, 
							MAP_SERVER_SET_FOLDER,
							status->masInfo.MasInstId,
							-1,
							folderache);
	env->DeleteLocalRef(folderache);
}
static void btmtk_map_handle_get_folder_list_indication(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	char path[18];
	jstring path_string;
	memset(path, 0x0, 18);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);

	jobject folderListCache = env->CallObjectMethod(nat->me, method_getFolderListCache, status->masInfo.MasInstId);
	env->SetIntField(folderListCache, field_FolderListRequest_mMasId, status->masInfo.MasInstId);	

	path_string = env->NewStringUTF(path);
	env->SetObjectField(folderListCache, field_FolderListRequest_mAddr, path_string);
	env->DeleteLocalRef(path_string);

	env->SetIntField(folderListCache, field_FolderListRequest_mSize, status->Oper.GetFolderList.Req.MaxListCount);
	env->SetIntField(folderListCache, field_FolderListRequest_mOffset, status->Oper.GetFolderList.Req.ListStartOffset);
	
	env->CallVoidMethod(nat->me, 
							method_onRequestOrIndicationReceived, 
							MAP_SERVER_GET_FOLDER_LIST,
							status->masInfo.MasInstId,
							-1,
							folderListCache);
	env->DeleteLocalRef(folderListCache);
}


static void btmtk_map_handle_get_message_list_request(void *content)
{	
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	jstring objectmember;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	memset(path, 0x0, 18);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);
	jobject messageListObject = env->CallObjectMethod(nat->me, method_getMessageListCache, status->masInfo.MasInstId);

	if(messageListObject == NULL)
	{
		XLOGV("messageList object can not be found due to no available resource or invalid instance id");
		return;
	}

	
	/*set each field for the messagelist*/
	/*addr*/
	objectmember = env->NewStringUTF(path);
	env->SetObjectField(messageListObject, field_MessageListRequest_mAddr, objectmember);
	env->DeleteLocalRef(objectmember);
	
	env->SetIntField(messageListObject, field_MessageListRequest_mMasId, 
						status->masInfo.MasInstId);

	objectmember = env->NewStringUTF(status->Oper.GetMsgList.Req.FolderPath);
	env->SetObjectField(messageListObject, field_MessageListRequest_mFolder, objectmember);
	env->DeleteLocalRef(objectmember);
	
	objectmember = env->NewStringUTF(status->Oper.GetMsgList.Req.FilterPeriodBegin);
	env->SetObjectField(messageListObject, field_MessageListRequest_mStartTime, objectmember);
	env->DeleteLocalRef(objectmember);

	objectmember = env->NewStringUTF(status->Oper.GetMsgList.Req.FilterPeriodEnd);
	env->SetObjectField(messageListObject, field_MessageListRequest_mEndTime, objectmember);
	env->DeleteLocalRef(objectmember);
	
	env->SetIntField(messageListObject, field_MessageListRequest_mListSize, 
						status->Oper.GetMsgList.Req.MaxListCount);
	env->SetIntField(messageListObject, field_MessageListRequest_mListOffset, 
						status->Oper.GetMsgList.Req.ListStartOffset);
	env->SetIntField(messageListObject, field_MessageListRequest_mMask, 
						status->Oper.GetMsgList.Req.ParamMask);
	env->SetIntField(messageListObject, field_MessageListRequest_mMaxSubjectLen, 
						status->Oper.GetMsgList.Req.SubjectLen);

	objectmember = env->NewStringUTF(status->Oper.GetMsgList.Req.FilterOriginator);
	env->SetObjectField(messageListObject, field_MessageListRequest_mOrignator, objectmember);
	env->DeleteLocalRef(objectmember);

	objectmember = env->NewStringUTF(status->Oper.GetMsgList.Req.FilterRecipient);
	env->SetObjectField(messageListObject, field_MessageListRequest_mRecipient, objectmember);
	env->DeleteLocalRef(objectmember);
	
	env->SetIntField(messageListObject, field_MessageListRequest_mPriority, 
						status->Oper.GetMsgList.Req.FilterPriority);
	env->SetIntField(messageListObject, field_MessageListRequest_mReadStatus, 
						status->Oper.GetMsgList.Req.FilterReadStatus);
	env->SetIntField(messageListObject, field_MessageListRequest_mType, 
						status->Oper.GetMsgList.Req.FilterMsgType);

	/*call APP API*/
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_GET_MESSAGE_LIST,
						status->masInfo.MasInstId,
						-1,
						messageListObject);
	env->DeleteLocalRef(messageListObject);
}

static void btmtk_map_handle_get_message_request(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	jboolean isAttachment;
	jstring path_string;
	memset(&path, 0x0, 18);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);

	jobject messageRequest = env->CallObjectMethod(nat->me, method_getMessageRequestCache, status->masInfo.MasInstId);
	if(messageRequest == NULL)
	{
		XLOGV("messageRequest object can not be found due to no available resource or invalid instance id");
		return;
	}
	env->SetIntField(messageRequest, field_MessageRequest_mMasId, status->masInfo.MasInstId);

	path_string = env->NewStringUTF(path);
	env->SetObjectField(messageRequest, field_MessageRequest_mAddr, path_string);
	env->DeleteLocalRef(path_string);
	
	isAttachment = (status->Oper.GetMsg.Req.Attach == MBT_MAP_ATTACH_TYPE_ON)? JNI_TRUE: JNI_FALSE; 
	env->SetBooleanField(messageRequest, field_MessageRequest_mAttachement,isAttachment); 
	env->SetIntField(messageRequest, field_MessageRequest_mCharset, 
							status->Oper.GetMsg.Req.Charset);
	env->SetIntField(messageRequest, field_MessageRequest_mFractionReq, 
							status->Oper.GetMsg.Req.FracReq);
	env->SetLongField(messageRequest, field_MessageRequest_mHandle, 
							status->Oper.GetMsg.Req.MsgHandle);	
	/*call APP API*/
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_GET_MESSAGE,
						status->masInfo.MasInstId,
						-1,
						messageRequest);
	env->DeleteLocalRef(messageRequest);
}

static void btmtk_map_handle_push_message_request(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	jstring path_string;
	memset(path, 0x0, BTMTK_MAX_OBJECT_PATH_SIZE);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);

	
	/*call APP API*/
	//notes: we just send the event, and do not parse message until MMI accept 
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_PUSH_MESSAGE,
						status->masInfo.MasInstId,
						-1,
						path_string);
	env->DeleteLocalRef(path_string);
	
}


static void btmtk_map_handle_set_notification_registration(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	jstring path_string;
	memset(path, 0x0, BTMTK_MAX_OBJECT_PATH_SIZE);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);

	
	/*call APP API*/
	//notes: we just send the event, and do not parse message until MMI accept 
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_SET_NOTIFICATION,
						status->masInfo.MasInstId,
						status->Oper.SetNotifReg.NotifStatus,
						path_string);
	env->DeleteLocalRef(path_string);
}

static void btmtk_map_handle_set_message_status(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	jint type;
	jobject msg_status_req;
	jstring path_string;
	memset(path, 0x0, BTMTK_MAX_OBJECT_PATH_SIZE);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);

	msg_status_req = env->CallObjectMethod(nat->me, method_getStatusSwitchCache);
	if(msg_status_req == NULL)
	{
		XLOGV("msg_status_req object can not be found due to no available resource");
		return;
	}
	
//	jboolean is = env->IsInstanceOf(msg_status_req, class_StatusSwitchRequest);

	
	env->SetLongField(msg_status_req, field_StatusSwitchRequest_mHandle, 
						status->Oper.SetMsgStatusReq.MsgHandle);
	
	env->SetIntField(msg_status_req, field_StatusSwitchRequest_mIndicator, 
						status->Oper.SetMsgStatusReq.StatusInicator);
	env->SetIntField(msg_status_req, field_StatusSwitchRequest_mValue, 
						status->Oper.SetMsgStatusReq.StatusVal);

	path_string = env->NewStringUTF(path);
	env->SetObjectField(msg_status_req, field_StatusSwitchRequest_mAddress,path_string);
	env->DeleteLocalRef(path_string);
	
	/*call APP API*/
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_SET_MESSAGE_STATUS,
						status->masInfo.MasInstId,
						-1,
						msg_status_req);
	env->DeleteLocalRef(msg_status_req);
}
static void btmtk_map_handle_update_inbox(void *content)
{
	MBT_MAP_MAS_STATUS *status = NULL;
	JNIEnv *env;
	jstring path_string;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	memset(path, 0x0, BTMTK_MAX_OBJECT_PATH_SIZE);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MAS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);

	
	/*call APP API*/
	//notes: we just send the event, and do not parse message until MMI accept 
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_UPDATE_INBOX,
						status->masInfo.MasInstId,
						-1,
						path_string);
	env->DeleteLocalRef(path_string);
}
static void btmtk_map_handle_cancel(void *content)
{
	//do nothing
}
static void btmtk_map_handle_MNS_connect(void *content, BT_BOOL result)
{
	bt_addr_struct *addr = NULL;
	JNIEnv *env;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	jint ret;
	jstring path_string;
	ret = (result == TRUE)? 1: 0;	
	memset(path, 0x0, BTMTK_MAX_OBJECT_PATH_SIZE);
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	addr = (bt_addr_struct *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, addr);

	ret = (result == TRUE)? 1 : 0;

	//test code
//	memcpy(&address_cache, addr, sizeof(bt_addr_struct));
	/*call APP API*/
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_MNS_CONNECT_CNF,
						ret,
						-1,
						path_string);
	env->DeleteLocalRef(path_string);
}
static void btmtk_map_handle_send_report_confirm(void *content, BT_BOOL result)
{
	MBT_MAP_MNS_STATUS *status = NULL;
	JNIEnv *env;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	jint ret;
	jstring path_string;
	memset(path, 0x0, BTMTK_MAX_OBJECT_PATH_SIZE);
	
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	ret = (result == TRUE) ? 1: 0; 
	status = (MBT_MAP_MNS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_SEND_REPORT_CNF,
						ret,
						-1,
						path_string);
	env->DeleteLocalRef(path_string);
}

static void btmtk_map_handle_MNS_disconnect(void *content)
{
	MBT_MAP_MNS_STATUS *status = NULL;
	JNIEnv *env;
	char path[BTMTK_MAX_OBJECT_PATH_SIZE];
	jstring path_string;
	memset(path, 0x0, BTMTK_MAX_OBJECT_PATH_SIZE);
	
	if(content == NULL) 
	{
		XLOGE("the input argument is null");
		return;
	}
	status = (MBT_MAP_MNS_STATUS *)content;
	nat->vm->GetEnv((void**)&env, nat->envVer);

	btmtk_util_convert_bdaddr2string(path, &status->BdAddr);
	path_string = env->NewStringUTF(path);
	env->CallVoidMethod(nat->me,
						method_onRequestOrIndicationReceived,
						MAP_SERVER_MNS_DISCONNCET_CNF,
						-1,
						-1,
						path_string);
	env->DeleteLocalRef(path_string);
}

void btmtk_map_server_callback(U8 type, S8 *content) 
{
	XLOGV("btmtk_map_server_callback, type is %d", type);
	switch(type)
	{		
		case MBTEVT_MAP_SERVER_ENABLE_SUCCESS:
			btmtk_map_handle_enable_confirm(TRUE);
			break;
		case MBTEVT_MAP_SERVER_ENABLE_FAIL:			
			btmtk_map_handle_enable_confirm(FALSE);
			break;
		case MBTEVT_MAP_SERVER_DISABLE_SUCCESS:			
			btmtk_map_handle_disable_confirm(TRUE);
			break;
		case MBTEVT_MAP_SERVER_DISABLE_FAIL:	
			btmtk_map_handle_disable_confirm(FALSE);
			break;
		case MBTEVT_MAP_SERVER_REGISTER_SUCCESS:
			btmtk_map_handle_register_confirm((U8 *)content, TRUE);
			break;
		case MBTEVT_MAP_SERVER_REGISTER_FAIL:
			btmtk_map_handle_register_confirm((U8 *)content, FALSE);
			break;
		case MBTEVT_MAP_SERVER_DEREGISTER_SUCCESS:
			btmtk_map_handle_deregister_confirm((U8 *)content, TRUE);
			break;
		case MBTEVT_MAP_SERVER_DEREGISTER_FAIL:
			btmtk_map_handle_deregister_confirm((U8 *)content, FALSE);
			break;
		case MBTEVT_MAP_SERVER_AUTHREQ:
			btmtk_map_handle_authorize_indication((bt_map_addr_struct *)content);
			break;
		case MBTEVT_MAP_SERVER_CONNECT_IND:
			btmtk_map_handle_connect_indication(content);
			break;
		case MBTEVT_MAP_SERVER_CONNECT_FAIL:
			XLOGV("connect fail");
			break;
		case MBTEVT_MAP_SERVER_DISCONNECT_SUCCESS:
			btmtk_map_handle_disconnect(content);
			break;
		//	case MBTEVT_MAP_SERVER_DISCONNECT_IND:
		case MBTEVT_MAP_SERVER_SET_FOLDER_IND:
			btmtk_map_handle_set_folder_indication(content);
			break;
		case MBTEVT_MAP_SERVER_GET_FOLDER_LIST_IND:
			btmtk_map_handle_get_folder_list_indication(content);
			break;
		case MBTEVT_MAP_SERVER_GET_MESSAGE_LIST_IND:
			btmtk_map_handle_get_message_list_request(content);
			break;
		case MBTEVT_MAP_SERVER_GET_MESSAGE_IND:
			btmtk_map_handle_get_message_request(content);
			break;
		case MBTEVT_MAP_SERVER_PUSH_MESSAGE_IND:
			btmtk_map_handle_push_message_request(content);
			break;
		case MBTEVT_MAP_SERVER_SET_NOTIFICATION_IND:
			btmtk_map_handle_set_notification_registration(content);
			break;
		case MBTEVT_MAP_SERVER_SET_MESSAGE_STATUS_IND:			
			btmtk_map_handle_set_message_status(content);
			break;
		case MBTEVT_MAP_SERVER_UPDATE_INBOX:
			btmtk_map_handle_update_inbox(content);
			break;
		case MBTEVT_MAP_SERVER_CANCEL:
			btmtk_map_handle_cancel(content);
			break;
		case MBTEVT_MAP_SERVER_MNS_CONNECT_SUCCESS:
			btmtk_map_handle_MNS_connect(content, TRUE);
			break;
		case MBTEVT_MAP_SERVER_MNS_CONNECT_FAIL:
			btmtk_map_handle_MNS_connect(content, FALSE);
			break;
		case MBTEVT_MAP_SERVER_SEND_EVENT_SUCCESS:
			btmtk_map_handle_send_report_confirm(content, TRUE);
			break;
		case MBTEVT_MAP_SERVER_SEND_EVENT_FAIL:
			btmtk_map_handle_send_report_confirm(content, FALSE);
			break;
		case MBTEVT_MAP_SERVER_MNS_DISCONNECT_SUCCESS:
			btmtk_map_handle_MNS_disconnect(content);
			break;
		default:
			XLOGV("unrecognized event is received");
	}
}

static jboolean map_startEventLoop(JNIEnv* env, jobject object)
{
    XLOGI("[MAP][JNI] map_startEventLoop");

 //   pthread_mutex_lock(&(nat->thread_mutex));
    
    env->GetJavaVM( &(nat->vm) );
    nat->envVer = env->GetVersion();
//    nat->me = env->NewGlobalRef(object);
    
    pthread_create(&(nat->thread), NULL, map_eventLoopMain, nat);
    
  //  pthread_mutex_unlock(&(nat->thread_mutex));
    return JNI_TRUE;
}
static void map_stopEventLoop(void)
{
 	struct sockaddr_un addr;
    socklen_t addrlen = sizeof(struct sockaddr_un);
    int ret;

	XLOGI("[MAP][JNI] map_stopEventLoop");

    addr.sun_path[0] = '\0';
    if (getsockname(nat->mapsrvsock, (struct sockaddr *)&addr, &addrlen) < 0)
    {
        XLOGE("[MAP][ERR] get map server socket address failed: %s(%d)", strerror(errno), errno);
        return;
    }
    ret = sendto(nat->mapsrvsock, 0, 0, 0, (struct sockaddr *)&addr, addrlen);
	pthread_join(nat->thread, NULL);
}


static void *map_eventLoopMain(void *ptr)
{
    JNIEnv *env;
    int res = 0;
    fd_set readfs;
    int sockfd;
    ilm_struct ilm;

    JavaVMAttachArgs args;
    char name[] = "BT MAP EventLoop";
    JavaVM	*vm = nat->vm;

    args.version = nat->envVer;
    args.name = name;
    args.group = NULL;
    
	XLOGI("map_eventLoopMain()");
    
    vm->AttachCurrentThread(&env, &args);
    sockfd = nat->mapsrvsock;       
	   
    while (1)
    {
        
        FD_ZERO(&readfs);
        if(0 <= sockfd)
        {
            FD_SET(sockfd, &readfs);
        }
        else
        {
            XLOGE("[Map][JNI][EventLoop][ERR] nat->mapsrvsock == 0. exit");
			break;
        }

        res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, NULL));
        if(0 < res)
        {
            res = recvfrom(sockfd, (void*)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
            XLOGI("[Map][JNI][EventLoop] Recv MAP CNF/IND : %d", ilm.msg_id);
            if(0 >= res)
            {
                XLOGE("[Map][JNI][EventLoop][ERR] recvfrom failed : %s, %d", strerror(errno), errno);
				break;
			}
            else
            {
                btmtk_map_handle_message(&ilm);

                nat->vm->GetEnv((void**)&env, nat->envVer);
            }
        }
        else if(0 == res)
        {
            XLOGW("[MAP][JNI][EventLoop][ERR] timeout waiting indication");
            break;
        }
        else
        {
            XLOGE("[MAP][JNI][EventLoop][ERR] select failed : %s, %d", strerror(errno), errno);
            break;
        }
    }

	if(sockfd >= 0) 
	{
		close(sockfd);
		sockfd = -1;
	}
	vm->DetachCurrentThread();
    return NULL;
}



//TODO: set JNI methods
static JNINativeMethod sMethods[] = {
		 /* name, signature, funcPtr */		 
		{"classInitNative", "()V", (void *)classInitNative},
		{"initNative", "()Z", (void *)initNative},
		{"deinitNative", "()V", (void *)deinitNative},
		{"enableNative", "()I", (void *)enableNative},
		{"disableNative", "()I", (void *)disableNative},
		{"registerInstanceNative", "(ILjava/lang/String;ILjava/lang/String;)I", (void *)registerInstanceNative},
		{"deregisterInstanceNative", "(I)I", (void *)deregisterInstanceNative},
		{"disconnectMasNative", "(I)I", (void *)disconnectMasNative},
		{"disconnectServerNative", "()I", (void *)disconnectServerNative},
		{"disconnectMnsSessionNative", "(Ljava/lang/String;)I", (void *)disconnectMnsSessionNative},
		{"setNotifRegResponseNative", "(Ljava/lang/String;II)I", (void *)setNotifRegResponseNative},
		{"sendMessageResponseNative", "(Ljava/lang/String;IIJ)I", 
										(void *)sendMessageResponseNative},
		{"parseMessageNative", "(Ljava/lang/String;ILcom/mediatek/bluetooth/map/cache/BMessageObject;)I", 
										(void *)parseMessageNative},
		{"getMessageListResponseNative", "(Ljava/lang/String;IILcom/mediatek/bluetooth/map/cache/MessageListObject;)I", 
										(void *)getMessageListResponseNative},
	
		{"getFolderListResponseNative", "(Ljava/lang/String;II[Lcom/mediatek/bluetooth/map/cache/FolderListObject;)I", 
										(void *)getFolderListResponseNative},
				 
		{"getMessageResponseNative", "(Ljava/lang/String;IILcom/mediatek/bluetooth/map/cache/BMessageObject;)I", 
										(void *)getMessageResponseNative},
		{"setMessageStatusResponseNative", "(Ljava/lang/String;II)I", 
										(void *)setMessageStatusResponseNative},
		
		{"updateInboxResponseNative", "(Ljava/lang/String;II)I", (void *)updateInboxResponseNative},
				 
		{"setFolderResponseNative", "(Ljava/lang/String;II)I", (void*) setFolderResponseNative},
		{"sendReportNative", "(Ljava/lang/String;Lcom/mediatek/bluetooth/map/cache/EventReport;)I", 
										(void *)sendReportNative},
		{"authorizeResponseNative", "(Ljava/lang/String;I)I", 
										(void *)authorizeResponseNative}
		
};
	
	
static int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* methods, int numMethods) 
{
	jclass clazz = env->FindClass(className);
	if (clazz == NULL) 
	{
		XLOGE("Native registration unable to find class '%s'\n", className);
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, methods, numMethods) < 0) 
	{
		XLOGE("RegisterNatives failed for '%s'\n", className);
		return JNI_FALSE;
	}
	return JNI_TRUE;
}
	
static int registerNatives(JNIEnv* env) 
{
	if (!registerNativeMethods(env, 
		"com/mediatek/bluetooth/map/BluetoothMapServerService",
			sMethods, sizeof(sMethods) / sizeof(sMethods[0])))
	{
		XLOGE("[SIMAP_JNI] registerNativeMethods failed");
		return JNI_FALSE;
	}
	return JNI_TRUE;
}


	
/*
 * When library is loaded by Java by invoking "loadLibrary()".
*/
jint JNI_OnLoad(JavaVM* vm, void* reserved) 
{
		JNIEnv* env = NULL;
		jint result = -1;
		
		XLOGI("[MAP_JNI]JNI_OnLoad [+]");
		
		if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) 
		{
			XLOGE("ERROR: GetEnv failed\n");
			goto end;
		}
		assert(env != null);
	
		if (!registerNatives(env)) 
		{
			XLOGE("ERROR: BluetoothMapService failed to register natives\n");
			goto end;
		}
	
		result = JNI_VERSION_1_4;
		//TODO: check whether VM is usefull, and diable it temp
		//g_jvm = vm;
	
	end:
	
		XLOGI("[MAP_JNI]JNI_OnLoad [-]");
		return result;;
}


