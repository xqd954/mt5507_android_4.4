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

/* Local global variable */
#include "extavrcpkey_jni.h"

/* Local function */
static int registerNatives(JNIEnv* env);

// classifier for exported API   EXPort AVrcp API
#define EXP_AVAPI 

#define IsValidRspCode(code) (code==0)

typedef struct {
    JNIEnv *env;
    U32 con_id;
    AvrcpChannelContext avrcp;  //
    //int apisock;
    //int servsock;
} native_data_t;

#define AVRCP_TERMINATE_THREAD_EVENT 0x1A

typedef struct _jni_avrcp_dev_addr_struct
{
	U32 lap; 
	U8  uap; 
	U16 nap; // non-significant 32..47
} jni_avrcp_dev_addr_struct;

/* global static */
static bt_avrcp_get_playerapp_attributetext_rsp_struct g_avrcp_jni_attrtext;
static void testAvrcpUtil();


/* AVRCP common */
static jfieldID field_mNativeData;
static jmethodID method_onActivateCnf;
static jmethodID method_onDeactivateCnf;
static jmethodID method_onConnectInd;
static jmethodID method_onDisconnectInd;
/* AVRCP 1.0 */
static jmethodID method_passThroughKeyInd;
static jmethodID method_sendAvrcpKeyEventInd;
static jmethodID method_playerAppCapabilitiesInd;
/* AVRCP 1.3 */
static jmethodID method_listPlayerAppAttributeInd;
static jmethodID method_listPlayerAppValueInd;
static jmethodID method_getCurPlayerAppValueInd;
static jmethodID method_setPlayerAppValueInd;
static jmethodID method_getPlayerAppAttrTextInd;
static jmethodID method_getPlayerAppValueTextInd;

static jmethodID method_informDisplayCharsetInd;
static jmethodID method_informBatteryStatusofctInd;
static jmethodID method_getElementAttributesInd;
static jmethodID method_getPlayerstatusInd;
static jmethodID method_setAbsoluteVolumeInd;
static jmethodID method_registerNotificationInd;
static jmethodID method_abortContinueInd;
/* AVRCP 1.4 */
static jmethodID method_setAddressedplayerInd;
static jmethodID method_getMediaPlayerList;
static jmethodID method_getFileSystemitemsList;
static jmethodID method_getSearchResultitemsList;
static jmethodID method_getNowPlayingitemsList;
static jmethodID method_setBrowsedplayerInd;
static jmethodID method_changePathInd;
static jmethodID method_getItemAttributesInd;
static jmethodID method_playItemsInd;
static jmethodID method_searchInd;
static jmethodID method_addTonowplayingInd;

static U8 avrcp_activate_flag = 0;
static U8 g_avrcp_norelease_handle = FALSE;
static U8 g_avrcp_pressed_keycode = 0; // if remote disconnects before sending release key, use this to send twice
static U8 g_avrcp_browsing_connected = FALSE;
static U8 g_avrcp_connected_pts = FALSE;

static U8 g_avrcp_version = 14;
static U8 g_avrcp_sdpfeature = 0x01;
static U8 g_avrcp_stopmusic = FALSE;


/* Internal functions */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getFileSystemItemEndRspNative
	(JNIEnv *env, jobject object, jbyte error, jbyte status, jshort uid_counter);
    
/* Internal Functions */    
static inline native_data_t * get_native_data(JNIEnv *env, jobject object) {
    return (native_data_t *)(env->GetIntField(object, field_mNativeData));
}


static void setAvrcpEvent(JNIEnv* env, jobject object, int event)
{
	switch(event)
	{
	case AVRCP_TERMINATE_THREAD_EVENT:
	{
		// send event
		native_data_t *nat = get_native_data(env, object);

		ALOGI("[API][AVRCP] setEventNative(%d)", event);
		if(nat)
		{
		    sockaddr_un name;
		    socklen_t   namelen;
		    int ret;
		    
		    namelen = sizeof(sockaddr_un);
		    ret = getsockname(nat->avrcp.servsock, (struct sockaddr*)&name, &namelen);
		    ALOGI("[BT][AVRCP]nat->hfgservsock : name=%s, size=%d", &name.sun_path[1], namelen);
		    if(ret == 0 && namelen > 0)
		    {
		        ret = sendto(nat->avrcp.apisock, &event, sizeof(int), 0, (struct sockaddr*)&name, namelen);
		        if(ret < 0)
		        {
		            ALOGE("[ERR][AVRCP] sendto servsock itself failed : %s, errno=%d", strerror(errno), errno);
		        }
		    }
		    else
		    {
		        ALOGE("[ERR][AVRCP] getsockname failed : %s, errno=%d", strerror(errno), errno);
		    }
		}
		else
		{
		    ALOGE("[ERR][AVRCP] nat is null");
		}
	}
		break;
	default:
		ALOGE("[ERR][AVRCP] unsupported event");
		break;
	}
}

static void CONVERT_BDADDRSRC2STRING(char *dest, btbm_bd_addr_t *source)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
        (U8) (source->lap & 0x000000FF),
        (U8) ((source->lap & 0x0000FF00) >> 8),
        (U8) ((source->lap & 0x00FF0000) >> 16),
        source->uap,
        (U8) (source->nap & 0x00FF),
        (U8) ((source->nap & 0xFF00) >> 8));
}

static void classInitNative(JNIEnv* env, jclass clazz) 
{
    ALOGI("[BT][AVRCP]+classInitNative");
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
    
    method_listPlayerAppAttributeInd = env->GetMethodID(clazz, "listPlayerAppAttributeInd", "()V");
    method_listPlayerAppValueInd = env->GetMethodID(clazz, "listPlayerAppValueInd", "(B)V");
    method_getCurPlayerAppValueInd = env->GetMethodID(clazz, "getCurPlayerAppValueInd", "(B[B)V");
    method_setPlayerAppValueInd = env->GetMethodID(clazz, "setPlayerAppValueInd", "(B[B[B)V");
    method_getPlayerAppAttrTextInd = env->GetMethodID(clazz, "getPlayerAppAttrTextInd", "(B[B)V");
    method_getPlayerAppValueTextInd = env->GetMethodID(clazz, "getPlayerAppValueTextInd", "(BB[B)V");
    
    
    method_informDisplayCharsetInd = env->GetMethodID(clazz, "informDisplayCharsetInd", "(B[S)V"); 
    method_informBatteryStatusofctInd = env->GetMethodID(clazz, "informBatteryStatusofctInd", "(B)V"); 
    method_getElementAttributesInd = env->GetMethodID(clazz, "getElementAttributesInd", "(JB[I)V"); 
    method_getPlayerstatusInd = env->GetMethodID(clazz, "getPlayerstatusInd", "()V"); 
    method_registerNotificationInd = env->GetMethodID(clazz, "registerNotificationInd", "(BI)V"); 
    method_abortContinueInd = env->GetMethodID(clazz, "abortContinueInd", "()V"); 

    /* AVRCP 1.4 TG Ind */
    method_setAbsoluteVolumeInd = env->GetMethodID(clazz, "setAbsoluteVolumeInd", "(BB)V");
    method_setAddressedplayerInd = env->GetMethodID(clazz, "setAddressedplayerInd", "(S)V");
    method_getMediaPlayerList = env->GetMethodID(clazz, "getMediaPlayerList", "(II)V");
    method_getFileSystemitemsList = env->GetMethodID(clazz, "getFileSystemitemsList", "(IIB[I)V");
    method_getSearchResultitemsList = env->GetMethodID(clazz, "getSearchResultitemsList", "(IIB[I)V");
    method_getNowPlayingitemsList = env->GetMethodID(clazz, "getNowPlayingitemsList", "(IIB[I)V");

    method_setBrowsedplayerInd = env->GetMethodID(clazz, "setBrowsedplayerInd", "(S)V");
    method_changePathInd = env->GetMethodID(clazz, "changePathInd", "(IBJ)V");
    method_getItemAttributesInd = env->GetMethodID(clazz, "getItemAttributesInd", "(BJSB[I)V");
    method_playItemsInd = env->GetMethodID(clazz, "playItemsInd", "(BJS)V");
    method_searchInd = env->GetMethodID(clazz, "searchInd", "(SLjava/lang/String;)V");
    method_addTonowplayingInd = env->GetMethodID(clazz, "addTonowplayingInd", "(BJS)V");

    field_mNativeData = env->GetFieldID(clazz, "mNativeObject", "I");
    
    /* Local init */
    if( avrcp_input_init( (const char *) "AVRCP") > 0 ){ // will use /system/usr/keylayout/AVRCP.kl key map
    	ALOGI("[BT][AVRCP] ok to create ninput dev ");
    }else{
    	ALOGI("[BT][AVRCP] fail to create ninput dev ");
    }
    
    // testAvrcpUtil();
    
    ALOGI("[BT][AVRCP]-classInitNative");
}

static jlong avrcp_util_array2long(U8 ptr[8]){
    jlong a, b;
    a = (jlong) (((jlong) *ptr << 56) | ((jlong) *(ptr + 1) << 48) | ((jlong) *(ptr + 2) << 40) | ((jlong) *(ptr + 3)) << 32);
    b = (jlong) (((long) *(ptr + 4) << 24) | ((long) *(ptr + 5) << 16) | ((long) *(ptr + 6) << 8) | ((long) *(ptr + 7)));
    a = a + b;
    return a;
}

static void avrcp_util_long2array(jlong id, U8 *ptr){
	// network order. bigendian
	ptr[0] = (U8) (id >> 56);
	ptr[1] = (U8) (id >> 48);
	ptr[2] = (U8) (id >> 40);
	ptr[3] = (U8) (id >> 32);
	ptr[4] = (U8) (id >> 24);
	ptr[5] = (U8) (id >> 16);
	ptr[6] = (U8) (id >> 8 );
	ptr[7] = (U8) (id & 0xff);
}

/* Save each AVRCP msg ind & req id's seq number */
U8 g_avrcp_seq_id_database[MSG_ID_BT_AVRCP_GROUP_END - MSG_ID_BT_AVRCP_GROUP_START + 1]; // save msg ids
U8 g_avrcp_req_event_database[0x0f]; // save reg event ids
U8 g_avrcp_auto_browse_connect = FALSE;

void initLocalSaveSeqId(){
	memset( g_avrcp_seq_id_database, 0, sizeof(g_avrcp_seq_id_database) );
	memset( g_avrcp_req_event_database, 0, sizeof(g_avrcp_req_event_database) );
}

void saveSeqId(int msg_id, U8 seq_id){
	if( (unsigned int) msg_id > MSG_ID_BT_AVRCP_GROUP_END || (unsigned int) msg_id < MSG_ID_BT_AVRCP_GROUP_START ){
		ALOGE("[BT][AVRCP][JNI] fail to save seq_id for msg_id:%d seq_id:%d", msg_id, seq_id);
		return;
	}
	
	msg_id = msg_id - MSG_ID_BT_AVRCP_GROUP_START;
	if( (unsigned int)msg_id >= sizeof( g_avrcp_seq_id_database ) ){
		ALOGE("[BT][AVRCP][JNI] seq_id_databse too small msg_id:%d seq_id:%d", msg_id, seq_id);
		return;
	}
	
	g_avrcp_seq_id_database[msg_id] = seq_id;
}

U8 getSavedSeqId(int msg_id){
	if( (unsigned int)msg_id > MSG_ID_BT_AVRCP_GROUP_END || (unsigned int)msg_id < MSG_ID_BT_AVRCP_GROUP_START ){
		ALOGE("[BT][AVRCP][JNI] fail to get seq_id for msg_id:%d ", msg_id);
		return 0;
	}
	
	msg_id = msg_id - MSG_ID_BT_AVRCP_GROUP_START;
	if( (unsigned int)msg_id >= sizeof( g_avrcp_seq_id_database ) ){
		ALOGE("[BT][AVRCP][JNI] seq_id_databse doesn't have msg_id:%d ", msg_id);
		return 0;
	}
	
	return g_avrcp_seq_id_database[msg_id];
}

#define AVRCP_EVENT_PLAYBACK_STATUS_CHANGED                0x01
#define AVRCP_EVENT_TRACK_CHANGED                          0x02
#define AVRCP_EVENT_TRACK_REACHED_END                      0x03
#define AVRCP_EVENT_TRACK_REACHED_START                    0x04
#define AVRCP_EVENT_PLAYBACK_POS_CHANGED                   0x05
#define AVRCP_EVENT_BATT_STATUS_CHANGED                    0x06
#define AVRCP_EVENT_SYSTEM_STATUS_CHANGED                  0x07
#define AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED     0x08
#define AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED            0x09
#define AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED              0x0a
#define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED               0x0b
#define AVRCP_EVENT_UIDS_CHANGED                           0x0c
#define AVRCP_EVENT_VOLUME_CHANGED                         0x0d

void saveRegEventSeqId(U8 event_id, U8 seq_id ){
	if( event_id >= sizeof(g_avrcp_req_event_database) ){
		ALOGE("[BT][AVRCP][JNI] seq_id_databse doesn't have event_id:%d ", event_id);
		return;
	}
	g_avrcp_req_event_database[event_id] = seq_id;
}

U8 getSavedRegEventSeqId(U8 event_id){
	if( event_id > sizeof(g_avrcp_req_event_database) ){
		return 0;
	}
	return g_avrcp_req_event_database[event_id];
}

/***************************************************************************************************************/


/* EXPORT FUNCTION */
static void testAvrcpUtil(){
	jlong l1 = ((jlong)0x12345678);
	jlong l2 = ((jlong)0x00);
	U8 data[8];
	avrcp_util_long2array(l1, data);
	ALOGI("AVRCP testAvrcpUtil l1=%ld %ld sizeof:%d ", (long)(l1>>32), (long)l1, (int) sizeof(l1) );
	ALOGI("AVRCP testAvrcpUtil data[0]:0x%x data[1]:0x%x data[2]:0x%x data[3]:0x%x", data[0], data[1], data[2], data[3]);
	ALOGI("AVRCP testAvrcpUtil data[4]:0x%x data[5]:0x%x data[6]:0x%x data[7]:0x%x", data[4], data[5], data[6], data[7]);
	l2 = avrcp_util_array2long(data);
	ALOGI("AVRCP testAvrcpUtil l2=%lx %lx sizeof:%d ", (long)(l2>>32), (long)l2, sizeof(l2));

	l1 = 0x12345678;
	l2 = 0x00;
	avrcp_util_long2array(l1, data);
	ALOGI("AVRCP testAvrcpUtil l1=%ld ", (long)l1);
	ALOGI("AVRCP testAvrcpUtil data[0]:0x%x data[1]:0x%x data[2]:0x%x data[3]:0x%x", data[0], data[1], data[2], data[3]);
	ALOGI("AVRCP testAvrcpUtil data[4]:0x%x data[5]:0x%x data[6]:0x%x data[7]:0x%x", data[4], data[5], data[6], data[7]);
	l2 = avrcp_util_array2long(data);
	ALOGI("AVRCP testAvrcpUtil l2=%ld ", (long)l2);
	
	ALOGI("AVRCP testAvrcpUtil sizeof(char):%d sizeof(int):%d sizeof(long):%d ", sizeof(char), sizeof(int), sizeof(long));
	ALOGI("AVRCP testAvrcpUtil sizeof(jchar):%d sizeof(jint):%d sizeof(jlong):%d ", sizeof(jchar), sizeof(jint), sizeof(jlong));
}

static void initializeNativeObjectNative(JNIEnv* env, jobject object) 
{
    native_data_t *nat = (native_data_t *)calloc(1, sizeof(native_data_t));

    ALOGI("[BT][AVRCP][API] initializeNativeObjectNative");
    if (NULL == nat) {
        ALOGE("%s: out of memory!", __FUNCTION__);
        return;
    }
    memset(nat, 0, sizeof(native_data_t));
    env->SetIntField(object, field_mNativeData, (jint)nat);
    ALOGI("[BT][AVRCP]-initializeNativeDataNative (3 norelease)");
}

static void deinitializeNativeObjectNative(JNIEnv* env, jobject object) 
{
	ALOGI("[BT][AVRCP]+deinitializeNativeDataNative");
    native_data_t *nat = (native_data_t *)(env->GetIntField(object, field_mNativeData));

	if( nat != 0){
		free(nat);
    	env->SetIntField(object, field_mNativeData, (jint)0);
    }
}

static void cleanupNativeObjectNative(JNIEnv* env, jobject object) 
{
    native_data_t *nat = (native_data_t *)env->GetIntField(object, field_mNativeData);
	int i = 0;

    ALOGI("[BT][AVRCP][API] cleanupNativeDataNative");
    if (nat) {
        free(nat);
        env->SetIntField(object, field_mNativeData, (jint)0);
    }
	//i = avrcp_input_deinit();  //CR: ALPS00065517 property test
	//ALOGI("[BT][AVRCP] avrcp_input_deinit %d", i);	
}

static jboolean enableNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;
    sockaddr_un name;
    socklen_t   namelen;
    struct sockaddr_un btname;
    socklen_t   btnamelen;

    ALOGI("[BT][AVRCP][API]+enableNative");
    // get native data
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[BT][AVRCP][ERR] nat is null");
        return JNI_FALSE;
    }
    // Create apisock and servsock
    
    nat->avrcp.servsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_AVRCP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    if( nat->avrcp.servsock < 0 ){
        ALOGE("[BT][AVRCP][ERR] fail to create serversock");
        goto error_exit;
    }else{
        ALOGI("[BT][AVRCP] creaet serversock ok");
    }

    // connect to set bt task socket
    name.sun_family = AF_LOCAL;      
    namelen = sizeof(short);
    nat->avrcp.apisock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    ALOGI("nat->avrcp.apisock:=%d", nat->avrcp.apisock);
    if (nat->avrcp.apisock < 0)
    {
        ALOGE("[ERR] create avrcp api socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    // Bind to a local socket number
    if (bind (nat->avrcp.apisock, (struct sockaddr *) &name, namelen) < 0)
    {
        ALOGE("[ERR] bind avrcp api socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    // Connect to bt task socket. Use send as a shotcut
    name.sun_family = AF_UNIX;
    strcpy (name.sun_path, /*BT_SERV_SOCK_ADDR*/BT_SOCK_NAME_INT_ADP);
    namelen = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1);     
    if ( connect(nat->avrcp.apisock, (const struct sockaddr*)&name, namelen) < 0)
    {
        ALOGE("[ERR] connect to %s failed : %s, errno=%d", name.sun_path, strerror(errno), errno);
        goto error_exit;
    }
    
    initLocalSaveSeqId();
    
    // success
    ALOGI("[BT][AVRCP][API]-enableNative");
    return JNI_TRUE;
    
error_exit:
	ALOGI("[BT][AVRCP][API]-enableNative fail");
    return JNI_FALSE;
}

void wakeupListenerNative(JNIEnv* env, jobject object)
{
	setAvrcpEvent(env, object, AVRCP_TERMINATE_THREAD_EVENT);
}

static void disableNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;

    ALOGI("[BT][AVRCP][API] +disableNative");
    nat = get_native_data(env, object);
    if(nat)
    {
        if( nat->avrcp.servsock > 0 ){
        	ALOGI("[BT][AVRCP][API] disableNative close servsock");
        	close( nat->avrcp.servsock );
        	nat->avrcp.servsock = 0;
        }
        if( nat->avrcp.apisock > 0 ){
        	ALOGI("[BT][AVRCP][API] disableNative close apisock");
        	close( nat->avrcp.apisock );
        	nat->avrcp.apisock = 0;
        }
        ALOGI("[BT][AVRCP][API] disableNative");
    }
    else
    {
        ALOGE("[BT][AVRCP][ERR] disableNative: nat is null");
    }    
}

/**
 * @brief Check the name and bd address to setup the IOT flag variable
 */
static jboolean check_norelease_handle(const char* device_name, bt_avrcp_dev_addr_struct *dev_addr){
	if( 0 == strcmp( (const char *) device_name, "Nokia CK-20W" ) ) {
		// has the same 
		g_avrcp_norelease_handle = TRUE;
	}
	else if( 0 == strcmp( (const char *) device_name, "Jabra BT3030" ) ){
		g_avrcp_norelease_handle = TRUE; // ALPS00837175 jabra-bt3030 seldom no releease for play keycode
	}
	else if( 0 == strcmp( (const char *) device_name, "i.Tech Clip Radio" ) ){
		g_avrcp_norelease_handle = FALSE; // for testing
	}
    else if( 0 == strcmp( (const char *) device_name, "MW600" ) ){
		//g_avrcp_norelease_handle = TRUE; // MW600 wait for release for more 
	}
	else{
		g_avrcp_norelease_handle = FALSE;
	}
	
	if( 0 == strncmp( (const char *) device_name, "PTS", 3 ) ) {
		g_avrcp_connected_pts = TRUE;
		ALOGI("[BT][AVRCP] pts connected");
	}else{
		g_avrcp_connected_pts = FALSE;
	}
	
	ALOGI("[BT][AVRCP] check_norelease normal mode");	
	return TRUE;
}

/* -1 : failed */
static jboolean listenIndicationNative(JNIEnv* env, jobject object, jboolean notWait)
{
    int res = 1;
    fd_set readfs;
    int sockfd;
    ilm_struct ilm;

    ALOGV("[BT][AVRCP][API] listenIndicationNative(%d)", notWait?1:0);
    native_data_t *nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[BT][AVRCP][ERR] check is NULL");
        return JNI_FALSE;
    }

    sockfd = nat->avrcp.servsock;
    if(!notWait)
    {
        FD_ZERO(&readfs);
        if(sockfd)
        {
            FD_SET(sockfd, &readfs);
        }
        else
        {
            ALOGE("[BT][AVRCP][ERR] nat->avrcp.servsock == 0. exit");
        }
        ALOGV("[BT][AVRCP]Start select : sockfd=%d", sockfd);
        res = TEMP_FAILURE_RETRY( select(sockfd+1, &readfs, NULL, NULL, NULL) );
        ALOGV("[BT][AVRCP]Return from select : soresckfd=%d", res);
    }
    if( res == sizeof(int) ){
    	// Internal events
		int evt = *(int*)&ilm;
		switch(evt)
		{
		case AVRCP_TERMINATE_THREAD_EVENT:
			// do nothing
			if( avrcp_activate_flag != 0 && method_onDeactivateCnf && env ){
				ALOGI("[BT][AVRCP] Send a Deactivate before Terminate");
				env->CallVoidMethod(object, method_onDeactivateCnf, 0, 0x1000 );
				avrcp_activate_flag = 0;
			}
			break;
		default:
			ALOGE("[ERR][AVRCP] unsupported event evt:0x%x", evt);
			break;
		}
    }else if(res > 0)
    {
        res = recvfrom(sockfd, (void*)&ilm, sizeof(ilm_struct), notWait ? MSG_DONTWAIT : 0, NULL, NULL);
        ALOGI("[BT][AVRCP] Recv AVRCP indication : %lu", ilm.msg_id);
        if(res < 0)
        {
            if(errno == EAGAIN)
            {
                res = 0;
            }
            else
            {
                ALOGE("[BT][AVRCP][ERR] recvfrom failed : %s, %d", strerror(errno), errno);
            }
        }
        else
        {
            switch(ilm.msg_id)
            {
                /* common message */
            case MSG_ID_BT_AVRCP_ACTIVATE_CNF:
	                {
	                    bt_avrcp_activate_cnf_struct *cnf;
	                    cnf = (bt_avrcp_activate_cnf_struct *)ilm.ilm_data;
	                    
	                    env->CallVoidMethod(object, method_onActivateCnf, cnf->chnl_num, cnf->result );
	                    if( 0x1000 == cnf->result ){
	                    	avrcp_activate_flag = 1;
	                    }
	                    // ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_ACTIVATE_CNF");
	                }
                break;
            case MSG_ID_BT_AVRCP_DEACTIVATE_CNF:
	                {
	                    bt_avrcp_deactivate_cnf_struct *cnf;
	                    cnf = (bt_avrcp_deactivate_cnf_struct *)ilm.ilm_data;
	                    
	                    env->CallVoidMethod(object, method_onDeactivateCnf, cnf->chnl_num, cnf->result );
	                    if( 0x1000 == cnf->result ){
	                    	avrcp_activate_flag = 0;
	                    }
	                }
                break;
            case MSG_ID_BT_AVRCP_CONNECT_IND:
                	{
	                    jbyte array[1];
	                    jstring sUnknow;
	                    bt_avrcp_connect_ind_struct *ind;
	                    ind = (bt_avrcp_connect_ind_struct *)ilm.ilm_data;
	                    
	                    /* for IOT NOKIA doesn't send release key */
	                    check_norelease_handle( (const char *)ind->device_name, &ind->device_addr);
	                    
	                    if( g_avrcp_norelease_handle ){
	                    	ALOGI("[BT][AVRCP][IOT] norelease_handle:%d name:('%s')", g_avrcp_norelease_handle, ind->device_name);
	                    }
	                    if( 0 == ind->device_name[0] ){
	                    	ind->device_name[0] = ' ';
	                    	ind->device_name[1] = 0;
	                    }	                    
                    	sUnknow = env->NewStringUTF((const char *)ind->device_name);
	                    env->CallVoidMethod(object, method_onConnectInd, 
	                    array[0],
	                    sUnknow,
	                    0);
	                    
	                    // Accept the incoming connection
	                    //btmtk_avrcp_authorize_res(&nat->avrcp, TRUE);
	                    btmtk_avrcp_send_connect_ind_rsp(&nat->avrcp, ind->chnl_num, 1);
	                    if( sUnknow != 0){
	                    env->DeleteLocalRef( sUnknow );
	                    }
                        if( TRUE == g_avrcp_auto_browse_connect ){
                            g_avrcp_auto_browse_connect= FALSE;
                            btmtk_avrcp_send_browse_connect_req(&nat->avrcp, 0, 0);
                            ALOGI("[BT][AVRCP] Auto Connect Browse channel");
                        }
                            
                            // init flags
                            g_avrcp_stopmusic = FALSE;
                	}
                break;
            	case MSG_ID_BT_AVRCP_CONNECT_CNF:    
            		{
	                    bt_avrcp_connect_cnf_struct *cnf;
	                    cnf = (bt_avrcp_connect_cnf_struct *)ilm.ilm_data;
	                    
            			ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_CONNECT_CNF conn_id:%d  result:%d	", cnf->connect_id, cnf->result);
            			ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_CONNECT_CNF bws:%d tg_feature:%d ct_featuer:%d", cnf->browse_channel, cnf->remote_tg_support_feature, cnf->remote_ct_support_feature);

                        if( TRUE == g_avrcp_auto_browse_connect ){
                            g_avrcp_auto_browse_connect= FALSE;
                            btmtk_avrcp_send_browse_connect_req(&nat->avrcp, 0, 0);
                            ALOGI("[BT][AVRCP] Auto Connect Browse channel");
                        }                        
            		}
            		break;
                case MSG_ID_BT_AVRCP_DISCONNECT_CNF:
                    {
                     ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_DISCONNECT_CNF");
                    }
               case MSG_ID_BT_AVRCP_DISCONNECT_IND:
                   {
                       bt_avrcp_disconnect_ind_struct* ind;
                       ind = (bt_avrcp_disconnect_ind_struct *)ilm.ilm_data;

                       // CR ALPS00049181 
                       if( 0 != g_avrcp_pressed_keycode ){
                           ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_DISC pressed_keycode:0x%x", g_avrcp_pressed_keycode);
                           avrcp_input_sendkey(g_avrcp_pressed_keycode, 0);
                           g_avrcp_pressed_keycode = 0; // avoid connect/disconect and send again.
                       }
				   
                       env->CallVoidMethod(object, method_onDisconnectInd);
                       g_avrcp_browsing_connected = FALSE;
                       g_avrcp_auto_browse_connect= FALSE;
                   }
                   break;
                   /* AVRCP 1.0 message */
               case MSG_ID_BT_AVRCP_BROWSE_CONNECT_IND:
                   {
                       g_avrcp_browsing_connected = TRUE;
                       ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_BROWSE_CONNECT_IND flag:%d", g_avrcp_browsing_connected);
                   }
                   break;
               case MSG_ID_BT_AVRCP_BROWSE_CONNECT_CNF:
                   {
                       ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_BROWSE_CONNECT_CNF flag:%d", g_avrcp_browsing_connected);
                   }
                   break;                   
               case MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_IND:
                   {
                       ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_IND");
                       g_avrcp_browsing_connected = FALSE;
                   }
                   break;
               case MSG_ID_BT_AVRCP_CMD_FRAME_CNF:
                   {
                       bt_avrcp_cmd_frame_cnf_struct* cnf;
                       cnf = (bt_avrcp_cmd_frame_cnf_struct *)ilm.ilm_data;
                                              
                       ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_CMD_FRAME_CNF size:%d", cnf->data_len);
                   }
                   break;
               case MSG_ID_BT_AVRCP_CMD_FRAME_IND:
                   {
                       jint key = 0;
                       jint press = 0;
                       bt_avrcp_cmd_frame_ind_struct* ind;
                       ind = (bt_avrcp_cmd_frame_ind_struct *)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_CMD_FRAME_IND, ind->seq_id);
                       if( ind->data_len > 1 ){
                       	    key = ind->frame_data[1];
                       }
                       ALOGI("[BT][AVRCP] MSG_ID_BT_AVRCP_CMD_FRAME_IND size:%d rawkey:%d data_len:%d", ind->data_len, key, ind->data_len);
                       
                       // response ok Immediately
                       if( 3 == ind->data_len )
                       {
                          
                        
                            press = ((ind->frame_data[1] & 0x80) == 0x80 )?0:1;
                            key = ind->frame_data[1] & 0x7f; // remove the press

                            // Pass to AP or send it to nInput
                            if( 1 == avrcp_is_ready() ){
                            	if ( g_avrcp_norelease_handle ){
                            		if( press == 1 ){
                            			avrcp_input_sendkey(key, 1);
                            			// this is no release. so send the release key when sending press key
                            			avrcp_input_sendkey(key, 0);
                            		}
                            	}else{
	                            	// only send in pressed state
	                            	if( press == 0 ){
	                            		avrcp_input_sendkey(key, 0);
	                            		// CR ALPS00049181 
	                            		g_avrcp_pressed_keycode = 0;
	                            	}else{
	                            		// press
	                            		avrcp_input_sendkey(key, 1);
	                            		// CR ALPS00049181 "disconnect before release key" causes key driver repeats the key
	                            		g_avrcp_pressed_keycode = key;										
	                            	}
	                            }
	                            
	                            switch(key){
	                            	case 0x45: // AVRCP_POP_STOP
	                            	  g_avrcp_stopmusic = TRUE;
	                            	  break;
	                            	case 0x44: //AVRCP_POP_PLAY
	                            	case 0x46: //AVRCP_POP_PAUSE
	                            	case 0x4B: //AVRCP_POP_FORWARD
	                            	case 0x4C: //AVRCP_POP_BACKWARD
	                            	case 0x48: //AVRCP_POP_REWIND
	                            	case 0x49: //AVRCP_POP_FAST_FORWARD
	                            	  g_avrcp_stopmusic = FALSE;
	                            	  break;
	                            }
                            	//env->CallVoidMethod(object, method_passThroughKeyInd, key, (jbyte)press ); /* only show msg */
                            }else{
	                            // pass to nInput
                                //env->CallVoidMethod(object, method_sendAvrcpKeyEventInd, key, (jbyte)press );
	                        }

							// send response as success
                            btmtk_avrcp_send_pass_through_rsp(
			                        &nat->avrcp,
			                        ind->chnl_num,
			                        ind->seq_id,
			                        ind->profile_id,
			                        BT_AVRCP_CR_ACCEPT,
			                        BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
			                        BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
			                        ind->data_len,
			                        ind->frame_data
			                        );
							 
							env->CallVoidMethod(object, method_passThroughKeyInd, key, (jbyte)press ); /* only show msg */
                        }else if( 8 == ind->data_len ){
                        	// group key not support yet
                        	btmtk_avrcp_send_pass_through_rsp(
                               &nat->avrcp,
                            ind->chnl_num,
                            ind->seq_id,
                            ind->profile_id,
                            BT_AVRCP_CR_NOT_IMPLEMENT,
                            BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
                            BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
                            ind->data_len,
                            ind->frame_data
                            );
                            
                            press = ((ind->frame_data[1] & 0x80) == 0x80 )?1:0; // 254: press 126: release
                            key = ind->frame_data[7]; // 0 next_group 1 prev_group
                            if( ind->frame_data[3] != 0 || ind->frame_data[4] != 0x19 || ind->frame_data[5] != 0x58){
	                            for( jbyte i = 0; i< 16 && i < ind->data_len ; i++){
	                            	ALOGI("[BT][AVRCP] dump i:%d  vaule:0x%x", i, ind->frame_data[i]);
	                            }
                            }
                            env->CallVoidMethod(object, method_passThroughKeyInd, key, (jbyte)press );
                        	
                       	}else{
                           btmtk_avrcp_send_pass_through_rsp(
                               &nat->avrcp,
                            ind->chnl_num,
                            ind->seq_id,
                            ind->profile_id,
                            BT_AVRCP_CR_NOT_IMPLEMENT,
                            BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
                            BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
                            ind->data_len,
                            ind->frame_data
                            );
                            ALOGE("[BT][AVRCP] unknow indication - dump first 16 hex value");
                            for( jbyte i = 0; i< 16 && i < ind->data_len ; i++){
                            	ALOGI("[BT][AVRCP] dump i:%d  vaule:0x%x", i, ind->frame_data[i]);
                            }
                        }
                   }
                   break;
                   
                   /* AVRCP 1.3 message */
               case MSG_ID_BT_AVRCP_GET_CAPABILITIES_IND:
                   {
                       bt_avrcp_get_capabilities_ind_struct* ind;
                       ind = (bt_avrcp_get_capabilities_ind_struct *)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_GET_CAPABILITIES_IND, ind->seq_id);
                       env->CallVoidMethod(object, method_playerAppCapabilitiesInd, ind->capability_id  );
                   }
                   break;
               case MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_IND:
                   {
						bt_avrcp_list_playerapp_attribute_ind_struct* ind;
						ind = (bt_avrcp_list_playerapp_attribute_ind_struct *)ilm.ilm_data;
						
						// no parameters
						saveSeqId( MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_IND, ind->seq_id);
						env->CallVoidMethod(object, method_listPlayerAppAttributeInd );
                   }
                   break;
               case MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_IND:
                   {
						// one byte 
						bt_avrcp_list_playerapp_value_ind_struct* ind;
						ind = (bt_avrcp_list_playerapp_value_ind_struct *)ilm.ilm_data;
						
						saveSeqId( MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_IND, ind->seq_id);
						env->CallVoidMethod(object, method_listPlayerAppValueInd, ind->attribute_id );
                   }
                   break;
               case MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_IND:
                   {
						// one byte, one byte array
						jbyte attribute_ids[BT_AVRCP_MAX_ATTRIBUTE_NUM];
						jbyteArray jbaAttr;
						bt_avrcp_get_curplayerapp_value_ind_struct* ind;
						ind = (bt_avrcp_get_curplayerapp_value_ind_struct *)ilm.ilm_data;
						
						saveSeqId( MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_IND, ind->seq_id);
						if( ind->count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
							ALOGV("AVRCP too many item %d", ind->count);
							ind->count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
						}
						
						jbaAttr = env->NewByteArray(ind->count);
						if( jbaAttr ==  NULL ){
							ALOGE("error no resource %s %d", __FILE__, __LINE__);
							break;
						}
						
						for( jbyte i = 0; i< ind->count; i++ ){
							attribute_ids[i] = ind->attribute_id[i];
							ALOGV("i:%d/%d attr_id:%d", i , ind->count, attribute_ids[i]);
						}
						env->SetByteArrayRegion(jbaAttr, 0, ind->count, attribute_ids);

						env->CallVoidMethod(object, method_getCurPlayerAppValueInd, ind->count, jbaAttr );

                   }
                   break;
               case MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_IND:
                   {
						// one byte,  two byte arrays
						jbyte attr_ids[BT_AVRCP_MAX_ATTRIBUTE_NUM];
						jbyte values[BT_AVRCP_MAX_ATTRIBUTE_NUM];
						jbyteArray jbaAttr;
						jbyteArray jbaValue;
						
						bt_avrcp_set_playerapp_value_ind_struct* ind;
						ind = (bt_avrcp_set_playerapp_value_ind_struct *)ilm.ilm_data;
						
						saveSeqId( MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_IND, ind->seq_id);
						if( ind->count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
							ALOGE("AVRCP too many item %d", ind->count);
							ind->count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
						}
						
						if( 0 != ind->count ){
							jbaAttr = env->NewByteArray(ind->count);
							jbaValue = env->NewByteArray(ind->count);
							if( jbaAttr == NULL || jbaValue == NULL ){
							ALOGE("error no resource %s %d", __FILE__, __LINE__);
							
							}
							for( jbyte i = 0; i< ind->count; i++ ){
								attr_ids[i] = ind->attribute_id[i];
								values[i] = ind->values[i];
								ALOGV("i:%d/%d attr_id:%d value:%d", i , ind->count, attr_ids[i], values[i]);
							}
							env->SetByteArrayRegion(jbaAttr, 0, ind->count, attr_ids);
							env->SetByteArrayRegion(jbaValue, 0, ind->count, values);
						}else{
							jbaAttr = env->NewByteArray(1);
							jbaValue = env->NewByteArray(1);
							attr_ids[0] = 1;
							values[0] = 1;
							env->SetByteArrayRegion(jbaAttr, 0, 1, attr_ids);
							env->SetByteArrayRegion(jbaValue, 0, 1, values);
						}
						
						env->CallVoidMethod(object, method_setPlayerAppValueInd, ind->count, jbaAttr, jbaValue );
						
                   }
                   break;
               case MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_IND:
                   {
						jbyte attr_ids[BT_AVRCP_MAX_ATTRIBUTE_NUM];
						jbyteArray jbaAttr;
						// one byte, byte array
						bt_avrcp_get_playerapp_attributetext_ind_struct* ind;
						ind = (bt_avrcp_get_playerapp_attributetext_ind_struct *)ilm.ilm_data;
						
						saveSeqId( MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_IND, ind->seq_id);
						if( ind->count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
							ALOGE("AVRCP too many item %d", ind->count);
							ind->count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
						}
						
						if( 0 != ind->count ){
							jbaAttr = env->NewByteArray(ind->count);
							if( jbaAttr == NULL ){
								ALOGE("error no resource %s %d", __FILE__, __LINE__);
							}
							
							for( jbyte i = 0; i< ind->count; i++ ){
								attr_ids[i] = ind->attribute_id[i];
								ALOGV("i:%d/%d attr_id:%d", i , ind->count, attr_ids[i]);
							}
							env->SetByteArrayRegion(jbaAttr, 0, ind->count, attr_ids);
						}else{
							jbaAttr = env->NewByteArray(1);
							attr_ids[0] = 1;
							env->SetByteArrayRegion(jbaAttr, 0, 1, attr_ids);
						}
						                  
						env->CallVoidMethod(object, method_getPlayerAppAttrTextInd, ind->count, jbaAttr );
                   }
                   break;
               case MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_IND:
                   {
						// two byte, one bye array
						jbyte value_ids[BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM];
						jbyteArray jbaValue;
						bt_avrcp_get_playerapp_valuetext_ind_struct* ind;
						ind = (bt_avrcp_get_playerapp_valuetext_ind_struct *)ilm.ilm_data;
						
						saveSeqId( MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_IND, ind->seq_id);
						if( ind->count > BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM ){
							ALOGE("AVRCP too many item %d", ind->count);
							ind->count = BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM;
						}
						if( 0 != ind->count ){
							jbaValue = env->NewByteArray(ind->count);
							if( jbaValue == NULL ){
								ALOGE("error no resource %s %d", __FILE__, __LINE__);
							}
							
							for( jbyte i = 0; i< ind->count; i++ ){
								value_ids[i] = ind->value_ids[i];
								ALOGV("i:%d/%d attr_id:%d", i , ind->count, value_ids[i]);
							}
							env->SetByteArrayRegion( jbaValue, 0, ind->count, value_ids);
						}else{
							jbaValue = env->NewByteArray(1);
							value_ids[0] = 0x01;
							env->SetByteArrayRegion( jbaValue, 0, 1, value_ids);
						}
							
						
						env->CallVoidMethod(object, method_getPlayerAppValueTextInd, ind->attribute_id, ind->count, jbaValue );
                   }
                   break;
               case MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_IND:
                   {
						// one byte, one short array
						jshort charsets[BT_AVRCP_MAX_ATTRIBUTE_NUM], num;
						jshortArray jsaValue;
						
						bt_avrcp_inform_display_charset_ind_struct* ind;
						ind = (bt_avrcp_inform_display_charset_ind_struct *)ilm.ilm_data;
						
						saveSeqId( MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_IND, ind->seq_id);
						if( ind->count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
							ALOGE("AVRCP too many item %d", ind->count);
							ind->count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
						}
						
						if( 0 != ind->count ){
							jsaValue = env->NewShortArray(ind->count);
							if( jsaValue == NULL ){
								ALOGE("error no resource %s %d", __FILE__, __LINE__);
							}
							
							for(jint i = 0; i< ind->count; i++){
								charsets[i] = ind->charset_ids[i];
								ALOGV("i:%d/%d attr_id:%d", i , ind->count, charsets[i]);
							}
							env->SetShortArrayRegion( jsaValue, 0, ind->count, charsets);
						}else{
							jsaValue = env->NewShortArray(1);
							charsets[0] =  0x6a;
							env->SetShortArrayRegion( jsaValue, 0, 1, charsets);	
						}
						
						env->CallVoidMethod(object, method_informDisplayCharsetInd, ind->count, jsaValue  );
                   }
                   break;
               case MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_IND:
                   {
                        // one byte
                       bt_avrcp_inform_battery_statusofct_ind_struct* ind;
                       ind = (bt_avrcp_inform_battery_statusofct_ind_struct *)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_IND, ind->seq_id);
                       env->CallVoidMethod(object, method_informBatteryStatusofctInd, ind->battery_status  );
                   }
                   break;
               case MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND:
                   {
						// long, one byte, one int array
						jlong identifier = 0;
						jint num;
						jint attr_ids[BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM];
						jintArray jiaAttrs;
						
						bt_avrcp_get_element_attributes_ind_struct* ind;
						
						ind = (bt_avrcp_get_element_attributes_ind_struct *)ilm.ilm_data;
						saveSeqId( MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND, ind->seq_id);
						
						if( ind->count > BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM ){
							ALOGE("[BT][AVRCP][JNI] too many item %d", ind->count);
							ind->count = BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM;
						}
						if( ind->count != 0 ){
							jiaAttrs = env->NewIntArray(ind->count);
							if( jiaAttrs == NULL ){
								ALOGE("[BT][AVRCP][JNI] error no resource %s %d", __FILE__, __LINE__);
							}
							
							for(jbyte i = 0; i< ind->count; i++){
								attr_ids[i] = ind->attributes[i];
								ALOGV("i:%d/%d attr_id:%d", i , ind->count, attr_ids[i]);
							}
							ALOGV("[BT][AVRCP][JNI] get_element_attr count:%d", ind->count);
							env->SetIntArrayRegion( jiaAttrs, 0, ind->count, attr_ids);
						}else{
							// send a sample but count is 0
							ALOGV("[BT][AVRCP][JNI] ind->count is 0 ");
							jiaAttrs = env->NewIntArray(1);
							attr_ids[0] = 1;
							env->SetIntArrayRegion( jiaAttrs, 0, 1, attr_ids);
						}
						
						env->CallVoidMethod(object, method_getElementAttributesInd, identifier, (jbyte)ind->count, jiaAttrs);
                   }
                   break;
               case MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_IND:
                   {
                        // no parameter
                       bt_avrcp_get_playerstatus_ind_struct *ind;
                       ind = (bt_avrcp_get_playerstatus_ind_struct*)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_IND, ind->seq_id);
                       env->CallVoidMethod(object, method_getPlayerstatusInd);
                   }
                   break;
               case MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_IND:
                   {
                       // event id, int interval
                       bt_avrcp_register_notification_ind_struct *ind;
                       ind = (bt_avrcp_register_notification_ind_struct*)ilm.ilm_data;
                       
                       saveRegEventSeqId(ind->event_id, ind->seq_id);
                       env->CallVoidMethod(object, method_registerNotificationInd, ind->event_id, ind->playback_interval);
                   }
                   break;
               case MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_IND:
                   {
                       /// only indication. no response
                       bt_avrcp_abort_continueresponse_ind_struct *ind;
                       ind = (bt_avrcp_abort_continueresponse_ind_struct *)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_IND, ind->seq_id);                       
                       env->CallVoidMethod(object, method_abortContinueInd);
                   }
                   break;

                   /* AVRCP 1.4 message */
               case MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_IND:
                   {
                   // one byte volume
                   	   jbyte num, volume;
                       bt_avrcp_set_absolute_volume_ind_struct *ind;
                       ind = (bt_avrcp_set_absolute_volume_ind_struct *)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_IND, ind->seq_id);
                       num = ind->chnl_num;
                       volume = ind->volume;
                       if( (volume & 0x80) != 0 ){
                       	ALOGW("[BT][AVRCP] change volume ind to & 0x80 volume:%d", volume);	
                       	//volume = ind->volume & 0x7f;
                       }
                       env->CallVoidMethod(object, method_setAbsoluteVolumeInd, num, volume);
                   }
                   break;
               case MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_IND:
                   {
                   // one short player id
                       bt_avrcp_set_addressedplayer_ind_struct *ind;
                       ind = (bt_avrcp_set_addressedplayer_ind_struct *)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_IND, ind->seq_id);
                       env->CallVoidMethod(object, method_setAddressedplayerInd, ind->player_id);
                   }
                   break;
               case MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND:
                   {
						// byte (scope), int (start), int (end), byte (count), int array (attribute_ids)
						bt_avrcp_get_folderitems_ind_struct *ind;
						ind = (bt_avrcp_get_folderitems_ind_struct *)ilm.ilm_data;
						
						jint attr_ids[BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM];
						jintArray jiaAttrs = NULL;
						
						saveSeqId( MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND, ind->seq_id);
						if( ind->scope >= 1 && ind->scope <=3 ){
							if( ind->count > BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM ){
								ALOGE("AVRCP too many item %d", ind->count);
								ind->count = BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM;
							}
							
							jiaAttrs = env->NewIntArray(ind->count);
							if( jiaAttrs == NULL ){
								ALOGE("error no resource %s %d", __FILE__, __LINE__);
							}
							
							for(jbyte i = 0; i< ind->count; i++){
								attr_ids[i] = ind->attribute_ids[i];
								ALOGV("i:%d/%d attr_id:%d", i , ind->count, attr_ids[i]);
							}
							ALOGV("[BT][AVRCP] get_element_attr count:%d", ind->count);
							env->SetIntArrayRegion( jiaAttrs, 0, ind->count, attr_ids);
						}
						
						switch( ind->scope ){
						case AVRCP_SCOPE_PLAYER_LIST:
						   env->CallVoidMethod(object, method_getMediaPlayerList, ind->start_item, ind->end_item);
						   break;
						case AVRCP_SCOPE_FILE_SYSTEM:
						   env->CallVoidMethod(object, method_getFileSystemitemsList, ind->start_item, ind->end_item, ind->count, jiaAttrs);
						   break;
						case AVRCP_SCOPE_SEARCH:
						   env->CallVoidMethod(object, method_getSearchResultitemsList, ind->start_item, ind->end_item, ind->count, jiaAttrs);
						   break;
						case AVRCP_SCOPE_NOW_PLAYING:
						   env->CallVoidMethod(object, method_getNowPlayingitemsList, ind->start_item, ind->end_item, ind->count, jiaAttrs);
						   break;
						default:
						   // reject with a error cause !
						   ALOGE("[BT][AVRCP] MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND unknow scope:%d", ind->scope);
						   BluetoothAvrcpService_getFileSystemItemEndRspNative(env, object, (jbyte)0x0a, (jbyte)0x0a, (jshort)0); // invalid scope
						   break;
						}
                   }
                   break;
               case MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_IND:
                   {
                        // short (player id)
                       bt_avrcp_set_browsedplayer_ind_struct* ind;
                       ind = (bt_avrcp_set_browsedplayer_ind_struct *)ilm.ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_IND, ind->seq_id);
                       env->CallVoidMethod(object, method_setBrowsedplayerInd, ind->player_id);
                   }
                   break;
               case MSG_ID_BT_AVRCP_CHANGE_PATH_IND:
                   {
                   // short (uid_counter), byte (direction), long (uid)
                       jlong id = 0;
                       bt_avrcp_change_path_ind_struct *ind;
                       ind = (bt_avrcp_change_path_ind_struct *)ilm.ilm_data;
                       id = avrcp_util_array2long(ind->folder_uid);
                       
                       saveSeqId( MSG_ID_BT_AVRCP_CHANGE_PATH_IND, ind->seq_id);
                       env->CallVoidMethod(object, method_changePathInd, ind->uid_counter , ind->direction, id);
                   }
                   break;
               case MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_IND:
                   {
						// byte (scope), long (uid), short (uid_counter), byte (count) , byte array (attributes)
						jlong id = 0;
						jint attr_ids[BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID];
						jintArray jiaAttr;
						
						bt_avrcp_get_itemattributes_ind_struct *ind;
						ind = (bt_avrcp_get_itemattributes_ind_struct *)ilm.ilm_data;
						id = avrcp_util_array2long(ind->uid);
						
						saveSeqId( MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_IND, ind->seq_id);
						if( ind->count > BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID ){
							ALOGE("AVRCP too many item %d", ind->count);
							ind->count = BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID;
						}
						
						jiaAttr = env->NewIntArray(ind->count);
						if( jiaAttr == NULL ){
							ALOGE("error no resource %s %d", __FILE__, __LINE__);
						}
						
						for(jint i = 0; i< ind->count; i++){
							attr_ids[i] = ind->attribute_ids[i];
							ALOGV("i:%d/%d attr_id:%d", i , ind->count, attr_ids[i]);
						}
						env->SetIntArrayRegion( jiaAttr, 0, ind->count, attr_ids);
                       
                        env->CallVoidMethod(object, method_getItemAttributesInd, ind->scope, id, ind->uid_counter, ind->count, jiaAttr);
                   }
                   break;
               case MSG_ID_BT_AVRCP_PLAY_ITEMS_IND:
                   {
                   // byte (scope), long (uid), short (uid_counter)
                       jlong id = 0;
                       bt_avrcp_play_items_ind_struct *ind;
                       ind = (bt_avrcp_play_items_ind_struct *)ilm.ilm_data;
                       id = avrcp_util_array2long(ind->uid);
                       
                       saveSeqId( MSG_ID_BT_AVRCP_PLAY_ITEMS_IND, ind->seq_id);
                       env->CallVoidMethod(object, method_playItemsInd, ind->scope, id, ind->uid_counter);
                   }
                   break;
               case MSG_ID_BT_AVRCP_SEARCH_IND:
                   {
						// short (charset), short (length), byte array (search string)
						bt_avrcp_search_ind_struct *ind;
						ind = (bt_avrcp_search_ind_struct *)ilm.ilm_data;
						char buf[128];
						jstring jstrBuf;
						
						saveSeqId( MSG_ID_BT_AVRCP_SEARCH_IND, ind->seq_id);
						ALOGV("[BT][AVRCP] search charset:0x%x text:'%s' len:%d", ind->charset, ind->search_string, ind->length);
						if( ind->charset != 0x6a ){
							/// 
							ALOGE("[BT][AVRCP] search with charset:%x not UTF8 !", ind->charset);
						}
						if( ind->length > (sizeof(buf)-1) ){
							ind->length = (sizeof(buf)-1);
						}
						memset( buf, 0, sizeof(buf));
						memcpy( buf, ind->search_string, ind->length);
						jstrBuf = env->NewStringUTF((char *) buf);
						
						env->CallVoidMethod(object, method_searchInd, ind->charset, jstrBuf);
						env->DeleteLocalRef( jstrBuf ); 
						
                   }
                   break;
               case MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_IND:
                   {
						// byte (scope), long (id), short (uid_counter)
						jlong id = 0;
						bt_avrcp_add_tonowplaying_ind_struct *ind;
						ind = (bt_avrcp_add_tonowplaying_ind_struct *)ilm.ilm_data;
						
						saveSeqId( MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_IND, ind->seq_id);
						// convert ind->uid to long
						id = avrcp_util_array2long(ind->uid);
						ALOGV("AVRCP_ADD_TONOWPLAYING_IND scope:%d id:%d uid_counter:%d", ind->scope, id, ind->uid_counter);
						
						env->CallVoidMethod(object, method_addTonowplayingInd, ind->scope, id, ind->uid_counter);
                   }
                   break;
            default:
                ALOGE("AVRCP Unexpected message : %lu", ilm.msg_id);
                break;
            }
        }
    }
    else if(res == 0)
    {
        ALOGE("[BT][AVRCP][ERR] timeout waiting indication");
    }
    else
    {
        ALOGE("[BT][AVRCP][ERR] select failed : %s, %d", strerror(errno), errno);
    }
    return (res>0) ? JNI_TRUE : JNI_FALSE;
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

    ALOGI("[BT][AVRCP]+JNI_OnLoad");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (!registerNatives(env)) {
        ALOGE("ERROR: BluetoothAvrcpService native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;
bail:
    ALOGI("[BT][AVRCP]-JNI_OnLoad: %d", result);
    return result;
}


static void btmtk_util_convert_bdaddr2string(char *dest, bt_addr_struct *source)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
            (U8) ((source->nap & 0xFF00) >> 8),
            (U8) (source->nap & 0x00FF),
            source->uap,
            (U8) ((source->lap & 0x00FF0000) >> 16),
            (U8) ((source->lap & 0x0000FF00) >> 8),
            (U8) (source->lap & 0x000000FF));
}

static void btmtk_util_convert_string2bdaddr(const char *source, bt_addr_struct *dest)
{
    unsigned char addr[6];
    int i;
    char *ptr = (char*)source;

    //assert(dest != NULL && source != NULL);
    bt_ext_log("convert from %s", ptr);
    for (i = 5; i >= 0; i--)
    {
        addr[i] = strtoul(ptr, &ptr, 16);
        ptr++;
        bt_ext_log("convert to addr[%d]=0x%X", i, addr[i]);
    }
    dest->lap = addr[0] + (addr[1] << 8) + (addr[2] << 16);
    dest->uap = addr[3];
    dest->nap = addr[4] + (addr[5] << 8);
    bt_ext_log("[API] btmtk_util_convert_string2bdaddr: %s => (0x%06X, 0x%02X, 0x%04X)", source, (unsigned int)dest->lap, dest->uap, dest->nap);
}

/*
sample:
jni: jstring destAddr
const char *c_bdaddr
bt_addr_struct btaddr;
c_bdaddr = env->GetStringUTFChars(destAddr, NULL);
btmtk_util_convert_string2bdaddr(c_bdaddr, &btaddr);
*/
/***********************************************************************************/

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    classInitNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_classInitNative
  (JNIEnv *env, jclass){
      jboolean ret = JNI_FALSE;
      ALOGI("[BT][AVRCP]+classInitNative");
      
      ALOGI("[BT][AVRCP]-classInitNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    getPlayerId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL BluetoothAvrcpService_getPlayerId
  (JNIEnv *env, jobject){
      jint i = 0;
      return i;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    getMaxPlayerNum
 * Signature: ()I
 */
JNIEXPORT jint JNICALL BluetoothAvrcpService_getMaxPlayerNum
  (JNIEnv *env, jclass){
      jint i = 0;
      i = BT_AVRCP_MAX_PLAYERS_NUM;
      return i;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    testparmnum
 * Signature: (BBBBBBBBBBBB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_testparmnum
  (JNIEnv *env, jobject object, jbyte, jbyte, jbyte, jbyte, jbyte, jbyte, jbyte, jbyte, jbyte, jbyte, jbyte, jbyte){
      jboolean ret = JNI_FALSE;
      ALOGI("[BT][AVRCP]+testparmnum");
      
      ALOGI("[BT][AVRCP]-testparmnum ret:%d", ret);
      return ret;

}

JNIEXPORT jboolean JNICALL BluetoothAvrcpService_activateConfig_3req
	(JNIEnv *env, jobject object, jbyte version, jbyte sdpfeature, jbyte reserved ){
	// setup the default activate version 	
	ALOGI("[BT][AVRCP] activate_config %d(13 for 1.3) %d %d", version, sdpfeature, reserved);

	switch(version){
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
	return JNI_TRUE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    activate_req
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_activate_1req
  (JNIEnv *env, jobject object, jbyte index){
    native_data_t *nat;
      
    //send out a active request
    ALOGI("[BT][AVRCP]+_activate_1req index:%d version:%d sdpfeature:%d", index, g_avrcp_version, g_avrcp_sdpfeature);
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }

    btmtk_avrcp_send_activate_req( &nat->avrcp, 0, 0, g_avrcp_version, g_avrcp_sdpfeature);
    /* Local init */
    if( avrcp_input_init( (const char *) "AVRCP") > 0 ){ // will use /system/usr/keylayout/AVRCP.kl key map
    	ALOGI("[BT][AVRCP] ok to create ninput dev ");
    }else{
    	ALOGI("[BT][AVRCP] fail to create ninput dev ");
    }
      
    ALOGI("[BT][AVRCP]-_activate_1req");
    return JNI_TRUE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    deactivate_req
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_deactivate_1req
  (JNIEnv *env, jobject object, jbyte index){
    jboolean ret = JNI_FALSE;
    native_data_t *nat;
          
    ALOGI("[BT][AVRCP]+deactivate_1req index:%d", index);
    nat = get_native_data(env, object);
      
    btmtk_avrcp_send_deactivate_req( &nat->avrcp, index);
    ret = JNI_TRUE;
      
    ALOGI("[BT][AVRCP]-deactivate_1req ret:%d", ret);
    return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    connect_req
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_connectReqNative
  (JNIEnv *env, jobject object, jstring addrString){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      
      const char *c_bdaddr = NULL;
      bt_addr_struct btaddr;

      nat = get_native_data(env, object);    

      ALOGI("[BT][AVRCP]+connectReqNative");
      
      memset( &btaddr, 0, sizeof(btaddr));
      c_bdaddr = env->GetStringUTFChars(addrString, NULL);
      btmtk_util_convert_string2bdaddr(c_bdaddr, &btaddr);
      env->ReleaseStringUTFChars( addrString, c_bdaddr ); 
      
      btmtk_avrcp_send_connect_req(&nat->avrcp, 0, (U8 *)&btaddr, 0);

      /* auto connect to browse channel when control channel is connected */
      g_avrcp_auto_browse_connect = TRUE;
    
      ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-connectReqNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    disconnect_req
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_disconnectReqNative
  (JNIEnv *env, jobject object){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);      
      ALOGI("[BT][AVRCP]+disconnectReqNative");
      
      btmtk_avrcp_send_disconnect_req(&nat->avrcp, 0);
      
      ALOGI("[BT][AVRCP]-disconnectReqNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    connectNative
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_connectNative
  (JNIEnv *env, jobject object, jstring addrString){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	
	const char *c_bdaddr = NULL;
	bt_addr_struct btaddr;

	nat = get_native_data(env, object);      
		  
	ALOGI("[BT][AVRCP]+connectNative");
	
	memset( &btaddr, 0, sizeof(btaddr));
	c_bdaddr = env->GetStringUTFChars(addrString, NULL);
	btmtk_util_convert_string2bdaddr(c_bdaddr, &btaddr);
	env->ReleaseStringUTFChars( addrString, c_bdaddr ); 
	
	btmtk_avrcp_send_connect_req(&nat->avrcp, 0, (U8 *)&btaddr, 0);

	
	ALOGI("[BT][AVRCP]-connectNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    disconnectNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_disconnectNative
  (JNIEnv *env, jobject object){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
     
      nat = get_native_data(env, object);      
      ALOGI("[BT][AVRCP]+disconnectNative");
      
      btmtk_avrcp_send_disconnect_req(&nat->avrcp, 0);
      
      ALOGI("[BT][AVRCP]-disconnectNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    disconnectBrowseNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_connectBrowseNative
  (JNIEnv *env, jobject object){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);      
      ALOGI("[BT][AVRCP]+connectBrowseNative");
      
      btmtk_avrcp_send_browse_connect_req(&nat->avrcp, 0, 0);
      
      ALOGI("[BT][AVRCP]-connectBrowseNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    disconnect_req
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_disconnectBrowseNative
  (JNIEnv *env, jobject object){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);      
      ALOGI("[BT][AVRCP]+disconnectBrowseNative");
      
      ALOGE("[BT][AVRCP]+disconnectBrowseNative  Not implement Yet");
      //btmtk_avrcp_send_disconnect_req(&nat->avrcp, 0);
      
      ALOGI("[BT][AVRCP]-disconnectBrowseNative ret:%d", ret);
      return ret;
}


/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    sendKeyReqNative
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_sendKeyReqNative
  (JNIEnv *env, jobject object, jint keycode){
      jboolean ret = JNI_FALSE;
      ALOGI("[BT][AVRCP]+sendKeyReqNative keycode:%d", keycode);
      
      ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-sendKeyReqNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    setup_playerapp_capabilities
 * Signature: (BB[B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getCapabilitiesRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte count, jbyteArray eventArr){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      jbyte data[AVRCP_MAX_EVENT_NUM];
      U8 events[AVRCP_MAX_EVENT_NUM];
      nat = get_native_data(env, object);      
      
      ALOGI("[BT][AVRCP]+getCapabilitiesRspNative error:%d count:%d arrlen:%d", error, count, env->GetArrayLength(eventArr));
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_CAPABILITIES_IND);
      if( count < 2 ){
      	  btmtk_avrcp_send_get_capabilities_rsp( &nat->avrcp, BT_AVRCP_ERRCODE_INTERNAL_ERROR, 0, events);
      }else{
      	  if( count > env->GetArrayLength(eventArr) ){
      	  	count = env->GetArrayLength(eventArr);
          }
          if( count > AVRCP_MAX_EVENT_NUM ){
	      	ALOGE("[BT][AVRCP] Too many events");
	      	count = AVRCP_MAX_EVENT_NUM;
          }
      	  env->GetByteArrayRegion(eventArr, (jsize)0, (jsize)count, (jbyte *)data);
	      for( jint i = 0; i< count; i++ ){
	      	events[i] = data[i];
	      }
	      btmtk_avrcp_send_get_capabilities_rsp( &nat->avrcp, 0, count, events);
	  }
	  ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-getCapabilitiesRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    setup_playerapp_attribute
 * Signature: (BB[B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_listPlayerappAttributeRspNative__B_3B
  (JNIEnv *env, jobject object, jbyte error, jbyte count, jbyteArray eventAttr){

      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      jbyte data[BT_AVRCP_MAX_ATTRIBUTE_NUM];
      U8 attrs[BT_AVRCP_MAX_ATTRIBUTE_NUM];
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+listPlayerappAttributeRspNative error:%d count:%d arrlen:%d", error, count, env->GetArrayLength(eventAttr));
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_IND);
      if( count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
      	ALOGE("[BT][AVRCP] Too many events");
      	count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
      }
  	  env->GetByteArrayRegion(eventAttr, (jsize)0, (jsize)count, (jbyte *)data); 
      for( jint i = 0; i< count; i++ ){
      	attrs[i] = data[i];
      }
      btmtk_avrcp_send_list_player_attrs_rsp( &nat->avrcp, 0, count, attrs);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-listPlayerappAttributeRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    setup_playerapp_values
 * Signature: (BBB[B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_listPlayerappValuesRspNative__BB_3B
  (JNIEnv *env, jobject object, jbyte error, jbyte attr_id, jbyte count, jbyteArray values){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      jbyte data[BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM];
      U8 attrValues[BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM];
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+listPlayerappValuesRspNative error:%d count:%d arrlen:%d", error, count, env->GetArrayLength(values));
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_IND);
      if( count > BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM ){
      	ALOGE("[BT][AVRCP] Too many events");
      	count = BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM;
      }
      if( count > 0 ){
	  	  env->GetByteArrayRegion(values, (jsize)0, (jsize)count, (jbyte *)data); 
	      for( jint i = 0; i< count; i++ ){
	      	attrValues[i] = data[i];
	      }
	  }
      btmtk_avrcp_send_list_player_values_rsp( &nat->avrcp, error, attr_id, count, attrValues);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-listPlayerappValuesRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    BluetoothAvrcpService_getCurplayerappValueRspNative
 * Signature: (BB[B[B)Z
 * MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_RSP
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getCurplayerappValueRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte count, jbyteArray ids, jbyteArray values){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      jbyte data_ids[BT_AVRCP_MAX_ATTRIBUTE_NUM];
      jbyte data_values[BT_AVRCP_MAX_ATTRIBUTE_NUM];
      
      U8 u8Attrs[BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM];
      U8 u8Values[BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM];
      nat = get_native_data(env, object);
            
      ALOGI("[BT][AVRCP]+getCurplayerappValueRspNative error:%d count:%d arrlen:%d vallen:%d", error, count, env->GetArrayLength(ids), env->GetArrayLength(values));
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_IND);
      if( count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
      	ALOGE("[BT][AVRCP] Too many events");
      	count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
      }
  	  env->GetByteArrayRegion(ids, (jsize)0, (jsize)count, (jbyte *)data_ids); 
  	  env->GetByteArrayRegion(values, (jsize)0, (jsize)count, (jbyte *)data_values); 
  	  
      for( jint i = 0; i< count; i++ ){
      	u8Attrs[i] = data_ids[i];
      	u8Values[i] = data_values[i];
      	ALOGI("[BT][AVRCP]+getCurplayerappValueRspNative attr_id:%d value:%d", u8Attrs[i], u8Values[i]);
      }
      btmtk_avrcp_send_get_curplayer_value_rsp( &nat->avrcp, error, count, u8Attrs, u8Values);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-getCurplayerappValueRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    setup_playerapp_attributetext
 * Signature: (BBILjava/lang/String;)Z
 * MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_RSP
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getPlayerappAttributeTextRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte item, jbyte total, jbyte attr_id, jint charset, jstring text){
    jboolean ret = JNI_FALSE;
    native_data_t *nat;
    int length = 0;
    U8 u8Text[BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE];
    const char* cstr;
    nat = get_native_data(env, object);
    
    ALOGI("[BT][AVRCP]+getPlayerappAttributeTextRspNative error:%d item:%d total:%d", error, item, total);
    nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_IND);
    if( 0 == error ){
		length = env->GetStringUTFLength(text);
		if( length > 0 ){
			cstr = env->GetStringUTFChars(text,0);
		
			if( length > BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE ){
				length = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE;
			}
			memset( u8Text, 0, sizeof(u8Text) );
			memcpy( u8Text, cstr, length );
			env->ReleaseStringUTFChars( text, cstr ); 
		}
		btmtk_avrcp_send_get_player_attr_text_rsp(&nat->avrcp, error, item, total, attr_id, 0x6a, length, u8Text);
	}else{
    	btmtk_avrcp_send_get_player_attr_text_rsp(&nat->avrcp, error, item, total, attr_id, 0x6a, length, u8Text);
    }
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getPlayerappAttributeTextRspNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    setup_playerapp_valuetext
 * Signature: (BBBBBLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getPlayerappValueTextRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte item, jbyte total, jbyte attr_id, jbyte value_id, jstring text){
 
    jboolean ret = JNI_FALSE;
    native_data_t *nat;
    int length = 0;
    U8 u8Text[BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE];
    const char* cstr;
    nat = get_native_data(env, object);
    
    ALOGI("[BT][AVRCP]+getPlayerappValueTextRspNative error:%d item:%d total:%d", error, item, total);
    nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_IND);
    if( 0 == error ){
		if( length > BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE ){
			length = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE;
		}
		
		length = env->GetStringUTFLength(text);
		if( length > 0 ){
			cstr = env->GetStringUTFChars(text,0);
			memset( u8Text, 0, sizeof(u8Text) );
			memcpy( u8Text, cstr, length );
			env->ReleaseStringUTFChars( text, cstr ); 
		}

		btmtk_avrcp_send_get_player_value_text_value_rsp(&nat->avrcp, error, item, total, attr_id, value_id, 0x6a, length, u8Text);
	}else{
    	btmtk_avrcp_send_get_player_value_text_value_rsp(&nat->avrcp, error, item, total, attr_id, value_id, 0x6a, length, u8Text);
    }
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getPlayerappValueTextRspNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    set_playerapp_ind
 * Signature: (B[B[B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_setPlayerappValueRspNative
  (JNIEnv *env, jobject object, jbyte error){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);      
      ALOGI("[BT][AVRCP]+setPlayerappValueRspNative error:%d ", error);
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_IND);
      btmtk_avrcp_send_set_player_value_rsp(&nat->avrcp, error);
      ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-setPlayerappValueRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    inform_display_charset_rsp
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_informDisplayCharsetRspNative
  (JNIEnv *env, jobject object, jbyte error){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);      
      ALOGI("[BT][AVRCP]+informDisplayCharsetRspNative error:%d ", error);
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_IND);
      btmtk_avrcp_send_inform_charsetset_rsp(&nat->avrcp, error);
      ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-informDisplayCharsetRspNative ret:%d", ret);
      return ret;
}


/*
 * Class:     com_android_bluetooth_avrcp_informBatteryStatusRspNative
 * Method:    informBatteryStatusRspNative
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_informBatteryStatusRspNative
  (JNIEnv *env, jobject object, jbyte error){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);      
      ALOGI("[BT][AVRCP]+informBatteryStatusRspNative error:%d ", error);
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_IND);
      btmtk_avrcp_send_battery_status_rsp(&nat->avrcp, error);
      ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-informBatteryStatusRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    get_element_attributes_rsp
 * Signature: (BBBBSLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getElementAttributesRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte item, jbyte total, jbyte attr_id, jshort charset, jstring text){
      //getElementAttributesRspNative(byte error, byte item, byte total, byte offset, byte attribute_id, int charset, String value);
    jboolean ret = JNI_FALSE;
    native_data_t *nat;
    int length = 0;
    U8 u8Text[1024];
    const char* cstr = NULL;
    nat = get_native_data(env, object);
    
    ALOGI("[BT][AVRCP]+getElementAttributesRspNative error:%d item:%d total:%d attr_id:%d ", error, item, total, attr_id);
    nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND);
    if( 0 == error ){
		length = env->GetStringUTFLength(text);
		if( length > 0 ){
			ALOGI("[BT][AVRCP]+getElementAttributesRspNative length:%d", length);
			cstr = env->GetStringUTFChars(text,0);
		}
		if( (unsigned int)length > sizeof(u8Text) ){
			length = sizeof(u8Text)-1 ;
		}
		memset( u8Text, 0, sizeof(u8Text) );
		memcpy( u8Text, cstr, length );
		if( length > 0 ){ // NOTE! it could be empty string
			env->ReleaseStringUTFChars( text, cstr ); 
		}

		btmtk_avrcp_send_get_element_attributes_rsp(&nat->avrcp, error,  item,  total,  attr_id, 0x6a, length, u8Text);
		
	}else{
    	btmtk_avrcp_send_get_element_attributes_rsp(&nat->avrcp, error,  item,  total,  attr_id, 0x6a, length, u8Text);
    }
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getElementAttributesRspNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    get_playerstatus_rsp
 * Signature: (BIIB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getPlayerstatusRspNative
  (JNIEnv *env, jobject object, jbyte error, jint length, jint position, jbyte status){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+getPlayerstatusRspNative error:%d ", error);
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_IND);
      btmtk_avrcp_send_get_playstatus_rsp( &nat->avrcp, 0, length, position, status);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-getPlayerstatusRspNative ret:%d", ret);
      return ret;      
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_status_changed
 * Signature: (BBB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationPlayStatusChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jbyte status){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	nat = get_native_data(env, object);  
	    
	ALOGI("[BT][AVRCP]+notificationPlaybackChangedNative error:%d interim:%d status:%d", error, isinterim, status);
	
	if( status == 2 ){ // AVRCP_PLAY_PAUSE 2
	    if( TRUE == g_avrcp_stopmusic ){
	    	ALOGI("[BT][AVRCP] change to STOP status from PAUSE status");
	    	status = 0; // AVRCP_PLAY_STOP 0
	    } 
	}else if(status == 1){
	    // music is playing
	    g_avrcp_stopmusic = FALSE;
	}
	
	nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_PLAYBACK_STATUS_CHANGED );     
	btmtk_avrcp_send_reg_notievent_playback_rsp( &nat->avrcp, error, isinterim, status);
	ret = JNI_TRUE;      
	ALOGI("[BT][AVRCP]-notificationPlaybackChangedNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_track_changed
 * Signature: (BBJ)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationTrackChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jlong id){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      U8 data[8];
	  U8 dataff[8];
      nat = get_native_data(env, object);  
            
      // update the se_id before send it
      ALOGI("[BT][AVRCP]+notificationTrackChangedNative error:%d isinterim:%d id:%ld", error, isinterim, (long)id);
      nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_TRACK_CHANGED );
	  avrcp_util_long2array(id, data);

      // pts tg_nfy_bv_08_c
	  if( FALSE == g_avrcp_browsing_connected ){
	  	// only allow return 0x0 or 0xffff ffff ffff ffff
	  	memset(dataff, 0xff, sizeof(dataff));
		if( 0 != memcmp( data, dataff, sizeof(dataff) ) ){
			ALOGI("[BT][AVRCP]+notificationTrackChangedNative 1.3 only allow 0");
			memset(data, 0x0, sizeof(dataff));
		}
	  }

      btmtk_avrcp_send_reg_notievent_track_changed_rsp( &nat->avrcp, error, isinterim, data);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-notificationTrackChangedNative ret:%d", ret);
      return ret;   
      return JNI_FALSE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_track_reached_end
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationTrackReachedEndNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      U8 data[8];
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationTrackReachedEndNative error:%d isinterim:%d", error, isinterim );
      nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_TRACK_REACHED_END );
      btmtk_avrcp_send_reg_notievent_reached_end_rsp( &nat->avrcp, error, isinterim);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-notificationTrackReachedEndNative ret:%d", ret);
      return ret;   
      return JNI_FALSE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_track_reached_start
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationTrackReachedStartNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      U8 data[8];
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationTrackReachedStartNative error:%d isinterim:%d", error, isinterim );
      nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_TRACK_REACHED_START );
      btmtk_avrcp_send_reg_notievent_reached_start_rsp( &nat->avrcp, error, isinterim);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-notificationTrackReachedStartNative ret:%d", ret);
      return ret;   
      return JNI_FALSE;
}


/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_status_changed
 * Signature: (BBI)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationPlayPosChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jint position){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationPlayPosChangedNative error:%d interim:%d position:%d", error, isinterim, position);
      nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_PLAYBACK_POS_CHANGED );
      btmtk_avrcp_send_reg_notievent_pos_changed_rsp( &nat->avrcp, error, isinterim, position);
     
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-notificationPlayPosChangedNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_status_changed
 * Signature: (BBB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationBatteryStatusChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jbyte status){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationBatteryStatusChangedNative error:%d interim:%d status:%d", error, isinterim, status);
      nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_BATT_STATUS_CHANGED );
      btmtk_avrcp_send_reg_notievent_battery_status_changed_rsp( &nat->avrcp, error, isinterim, status);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-notificationBatteryStatusChangedNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_system_status_changed
 * Signature: (BBB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationSystemStatusChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jbyte status){
      jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationSystemStatusChangedNative error:%d interim:%d status:%d", error, isinterim, status);
      nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_SYSTEM_STATUS_CHANGED );
      btmtk_avrcp_send_reg_notievent_system_status_changed_rsp( &nat->avrcp, error, isinterim, status);
	  ret = JNI_TRUE;      
      ALOGI("[BT][AVRCP]-notificationSystemStatusChangedNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notification_application_setting_changed
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationApplicationSettingChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jbyte count, jbyteArray attr_ids, jbyteArray value_ids){
jsize attrsize, valuesize;
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	nat = get_native_data(env, object);
	U8 attrs[BT_AVRCP_MAX_ATTRIBUTE_NUM];
	U8 values[BT_AVRCP_MAX_ATTRIBUTE_NUM];
	jbyte data_attr[BT_AVRCP_MAX_ATTRIBUTE_NUM];
	jbyte data_value[BT_AVRCP_MAX_ATTRIBUTE_NUM];
	int i = 0 ;
	
	ALOGI("[BT][AVRCP]+notificationSystemStatusChangedNative error:%d interim:%d count:%d", error, isinterim, count);
	nat->avrcp.seq_id = getSavedRegEventSeqId( AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED );
	attrsize = env->GetArrayLength(attr_ids);
	valuesize = env->GetArrayLength(value_ids);
	
	if( count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
		ALOGE( "[BT][AVRCP] too many %d", count );
		count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
	}
	if( count > 0 ){
		env->GetByteArrayRegion(attr_ids, (jsize)0, (jsize)count, (jbyte *)data_attr);
		env->GetByteArrayRegion(value_ids, (jsize)0, (jsize)count, (jbyte *)data_value);
	}
	for( i = 0; i< count; i++ ){
		attrs[i] = data_attr[i];
		values[i] = data_value[i];
	}
	btmtk_avrcp_send_reg_notievent_player_appsettings_changed_rsp( &nat->avrcp, error, isinterim, count, attrs, values);
	ret = JNI_TRUE;      
	ALOGI("[BT][AVRCP]-notificationSystemStatusChangedNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    set_absolute_volume_rsp
 * Signature: (BBB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_setAbsoluteVolumeRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte status, jbyte volume){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	nat = get_native_data(env, object);  
	    
	ALOGI("[BT][AVRCP]+setAbsoluteVolumeRspNative error:%d status:%d volume:%d ", error, status, volume);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_IND);
	if( error == 0  && volume < 0 ){
		ALOGE(" volume should between 0~0x7f(100 persent) volume:%d", volume );
		volume = volume & 0x7f;
	}
	btmtk_avrcp_send_set_absolute_volume_rsp( &nat->avrcp, error, status, volume);
	ret = JNI_TRUE;      
	ALOGI("[BT][AVRCP]-setAbsoluteVolumeRspNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    set_addressedplayer_ind
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_setAddressedplayerRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte status){
  	  jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+setAddressedplayerRspNative status:%d", status);
      nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_IND);
      btmtk_avrcp_send_set_addressedplayer_rsp(&nat->avrcp, error, status);
	  ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-setAddressedplayerRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    set_browsedplayer_rsp
 * Signature: (BBIBIB[Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_setBrowsedplayerRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte status, jshort uid_counter, jint num , jshort charset, jbyte depth, jobjectArray path){
	jboolean ret = JNI_FALSE;
	jbyte i;
	U8 *pathname = NULL;
	jshort len = 0;
	native_data_t *nat;
	jsize length = 0;
	nat = get_native_data(env, object);  
	jstring myObject;
	
	if( depth > 0){
		length = env->GetArrayLength( path );
	}
	ALOGI("[BT][AVRCP]+setBrowsedplayerRspNative status:%d uid_counter:%d num:%d depath:%d", status, uid_counter, num, depth);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_IND);
	if( length < depth ){
		ALOGE("[BT][AVRCP] wrong string array:%d require depth:%d", length, depth); 		
	}	
	
	if( 0 == error ){
		for( i = 0; i< depth; i++){
			// config
			myObject = (jstring) env->GetObjectArrayElement( path, i);
			/* Convert the object obtained into a String */
			pathname =(U8 *) env->GetStringUTFChars( myObject, 0);
			len = strlen((const char *)pathname);
			btmtk_avrcp_config_set_browsedplayer_rsp(&nat->avrcp, i, depth, len, pathname);
			env->ReleaseStringUTFChars(myObject, (const char *)pathname);
		}
		if( 0 == depth ){
			// config root
			btmtk_avrcp_config_set_browsedplayer_rsp(&nat->avrcp, 0, 0, 0, NULL);
		}
		
		// send response
		btmtk_avrcp_send_set_browsedplayer_rsp(&nat->avrcp, error, status, uid_counter, num, charset);
	}else{
		btmtk_avrcp_send_set_browsedplayer_rsp(&nat->avrcp, error, status, uid_counter, num, charset);
	}
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-setBrowsedplayerRspNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    change_path_rsp
 * Signature: (BBI)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_changePathRspNative
  (JNIEnv *env, jobject object, jbyte error , jbyte status, jint num){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	nat = get_native_data(env, object);  
	    
	ALOGI("[BT][AVRCP]+changePathRspNative error:%d status:%d num:%d", error, status, num);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_CHANGE_PATH_IND);
	btmtk_avrcp_send_change_path_rsp(&nat->avrcp, error, status, num);
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-changePathRspNative ret:%d", ret);
	return JNI_FALSE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    get_itemattributes_rsp
 * Signature: (BBB[I[S[Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getItemattributesRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte status, jbyte count, jintArray ids, jshortArray charsets, jobjectArray values){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	int len = 0, i = 0;
	U32 id = 0;
	U8 *attrText = NULL;
	jsize length = 0;
	jstring myObject;
	
	jint data_attr[BT_AVRCP_MAX_ATTRIBUTE_NUM];
	U16 u8Charsets[BT_AVRCP_MAX_ATTRIBUTE_NUM];
		
	nat = get_native_data(env, object);
	         
	ALOGI("[BT][AVRCP]+getItemattributesRspNative error:%d status:%d count:%d", error, status, count);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_IND);
  	if( count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
  		count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
  	}
  	if( count > 0 && error == 0 ){
	  	length = env->GetArrayLength(values);
	  	if( length < count ){
	  		count = length;
			ALOGI("[BT][AVRCP] no enough arry - values"); 
	  	}
	  	length = env->GetArrayLength(ids);
	  	if( length < count ){
	  		count = length;
			ALOGI("[BT][AVRCP] no enough arry - ids ");
	  	}
	  	
	  	env->GetIntArrayRegion(ids, (jsize)0, (jsize)count, (jint *)data_attr); 
	  	env->GetShortArrayRegion(charsets, (jsize)0, (jsize)count, (jshort *)u8Charsets);
	  	
		for( i = 0; i < count ; i++ ){
			myObject = (jstring) env->GetObjectArrayElement( values, i);
			attrText =(U8 *) env->GetStringUTFChars( myObject, 0);

			len = strlen( (const char *)attrText );
			id = data_attr[i];
			ALOGI("[BT][AVRCP] getItemattributesRspNative u8Attrs:%d data_attr:%d len:%d", (unsigned int)id, data_attr[i], len);
			
			btmtk_avrcp_send_get_itemattributes_rsp(&nat->avrcp, error, status, i , count, id, 0x6a, len, attrText);
			env->ReleaseStringUTFChars(myObject, (const char *)attrText);
		}
	}
	
	if( count == 0 || error != 0 ){
		i = 0;
		btmtk_avrcp_send_get_itemattributes_rsp(&nat->avrcp, error, status, i, count, id, 0x6a, len, attrText);
	}
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getItemattributesRspNative ret:%d", ret);
	return JNI_FALSE;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    play_items_rsp
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_playItemsRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte status){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	nat = get_native_data(env, object);  
	    
	ALOGI("[BT][AVRCP]+playItemsRspNative error:%d status:%d", error, status);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_PLAY_ITEMS_IND);
	btmtk_avrcp_send_play_items_rsp(&nat->avrcp, error, status);
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-playItemsRspNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    search_rsp
 * Signature: (BBCI)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_searchRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte status, jshort uid_counter, jint numItems){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	nat = get_native_data(env, object);  
	    
	ALOGI("[BT][AVRCP]+searchRspNative error:%d status:%d uid_counter:%d numItems:%d", error, status, uid_counter, numItems);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_SEARCH_IND);
	btmtk_avrcp_send_search_rsp(&nat->avrcp, error, status, uid_counter, numItems);
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-searchRspNative ret:%d", ret);
	return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    add_tonowplaying_rsp
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_addTonowplayingRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte status){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	nat = get_native_data(env, object);  
	    
	ALOGI("[BT][AVRCP]+addTonowplayingRspNative error:%d status:%d", error, status);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_IND);
	btmtk_avrcp_send_add_tonowplaying_rsp(&nat->avrcp, error, status);
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-addTonowplayingRspNative ret:%d", ret);
	return ret;
}


avrcp_folder_mixed_item g_folder_player_item;

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    add_tonowplaying_rsp
 * Signature: (BBBBSSBB[BLjava/lang/String;)Z
 (JNIEnv *env, jobject object, jbyte error, jbyte status, jbyte item , jbyte total, jshort uid_counter, jshort player_id, jbyte type, jbyte stype, jintArray mask, jstring name){
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getMediaPlayerListRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte item , jbyte total, jbyte status, jshort uid_counter, jshort player_id, jbyte type, jbyte stype, jbyteArray mask, jstring name){
  	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	int length;
	U8 *folderName = NULL;
	jbyte datamask[16];
	jsize len; // array length
	nat = get_native_data(env, object);  
	    
	ALOGI("[BT][AVRCP]+getMediaPlayerListRspNative error:%d status:%d", error, status);

        // there is no start/end getMediaPlayerListRsp. so use item==0 as start
	if( error != 0 || item == 0 ){
		// clean up the folderitme data first
		btmtk_avrcp_reset_send_get_folderitems_rsp(&nat->avrcp);
	}
	
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND);
	if( error == 0 ){
		length = env->GetStringUTFLength(name);
		if( length > 0 ){
			folderName = (U8 *) env->GetStringUTFChars(name, 0);
		}
		// copy data int g_folder_player_item
		g_folder_player_item.item_type = 0x01; // player
		g_folder_player_item.player_id = player_id;
		g_folder_player_item.specific_type = type;
		g_folder_player_item.sub_type = stype;
		g_folder_player_item.status = status;
		//g_folder_player_item.mask = 
		len =  env->GetArrayLength(mask);
		if( len > 16 ) len = 16;
		env->GetByteArrayRegion(mask, (jsize)0, (jsize)len, (jbyte *)datamask);
		memset( g_folder_player_item.mask, 0, 16);
		memcpy( g_folder_player_item.mask, datamask, len);
		
		// caculator size
		btmtk_avrcp_config_send_get_folderitems_rsp(&nat->avrcp, item, total,  &g_folder_player_item , length, folderName);
		
		if( length > 0 ){
			env->ReleaseStringUTFChars( name, (const char *)folderName );
		}
	}else{
		ALOGE("[BT][AVRCP] ignore the error:%d", error);
	}
	
	// there is no start/end getMediaPlayerListRsp. so use item+1==total as end
	if( error != 0 || (item+1) == total || total == 0 ){
		// send all data out
		btmtk_avrcp_send_get_folderitems_rsp(&nat->avrcp, error, error, uid_counter);
	}
		
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getMediaPlayerListRspNative ret:%d", ret);
	return ret;	
}

avrcp_folder_mixed_item g_folder_folder_item;
avrcp_folder_mixed_item g_folder_element_item;

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    getFileSystemItemFolderRspNative
 * Signature: (BBBJBBSSLjava/lang/String;)Z
 * ok, item, total, uid, foldertype, playable, charset, len, stinrg
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getFileSystemItemFolderRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte item , jbyte total, jlong uid, jbyte foldertype, jbyte playable, jshort charset, jshort len, jstring folername){
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	avrcp_folder_mixed_item *pitem;
	const char* cstr = NULL;
	int length = 0;

	nat = get_native_data(env, object); 

	ALOGI("[BT][AVRCP]+getFileSystemItemFolderRspNative error:%d item:%d len:%d uid:%ld", error, item, len, (long)uid);
	ALOGI("[BT][AVRCP]+getFileSystemItemFolderRspNative total:%d uid:%d foldertype:%d playable:%d charset:%d", total, (int)uid, foldertype, playable, charset);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND);
	if ( 0 == error ){
		if( folername != NULL ){
			length = env->GetStringUTFLength(folername);
			//length = 1;
		}
		if( length > 0 ){
			cstr = (const char *) env->GetStringUTFChars(folername, 0);
		}
		pitem = &g_folder_folder_item;
		pitem->item_type = 0x02; // folder type
		avrcp_util_long2array( uid, pitem->uid);
		pitem->specific_type = foldertype;
		pitem->is_playable = playable;
		pitem->charset = charset;
		pitem->name_length = length;
		pitem->attribute_count = 0; // folder dones't ahve attribute
	
		pitem->item_length = 8+1+1+2+2+pitem->name_length;// check the length composed by profile
		btmtk_avrcp_config_send_get_folderitems_rsp(&nat->avrcp, item, total, pitem, length, (const U8 *)cstr);
		if( length > 0 ){
			env->ReleaseStringUTFChars( folername, cstr ); 
		}
	}else{
		ALOGE("[BT][AVRCP] ignore the error:%d", error);
	}
		
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getFileSystemItemFolderRspNative ret:%d", ret);
	return ret;  	
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    getFileSystemItemFileAttrRspNative
 * Signature: (BBBISSLjava/lang/String;)Z
 * configure file's attribute		
 * status, item, attr_id(int), charset(short), len(short), string
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getFileSystemItemFileAttrRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte item , jbyte attr_index, jint attr_id, jshort charset, jshort len, jstring attrvalue){
  	// configure attribute of file item first
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	avrcp_folder_mixed_item *pitem;
	const char* cstr = NULL;	
	int length = 0;
		
	nat = get_native_data(env, object);
	    
	ALOGI("[BT][AVRCP]+getFileSystemItemFileAttrRspNative error:%d item:%d att-indx:%d id:%d", error, item, attr_index, attr_id);
	// fill data into item's attribute. fill the current g_folder_element_item
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND);
	pitem = &g_folder_element_item;
	if( attr_index > BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID ){
		ALOGI("[BT][AVRCP][ERR] too large attr_index:%d", attr_index);
	}else{
		if( attrvalue != NULL ){
			length = env->GetStringUTFLength(attrvalue);
		}
		if( length > 0 ){
			cstr = (const char *) env->GetStringUTFChars(attrvalue, 0);
		}
		
		// ignore it
		pitem->attributes[attr_index].attribute_id = attr_id;
		pitem->attributes[attr_index].charset = charset;
		pitem->attributes[attr_index].attribute_value_len = len;
		
		pitem->attribute_count = attr_index + 1; 
		
		btmtk_avrcp_config_send_get_folderitems_attribute_rsp(&nat->avrcp, item, attr_index, attr_id, charset, (short)strlen(cstr), (const U8 *)cstr);
		if( length > 0 ){
			env->ReleaseStringUTFChars( attrvalue, cstr ); 
		}		
	}
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getFileSystemItemFileAttrRspNative ret:%d", ret);
	return ret;  	  	
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    getFileSystemItemFolderRspNative
 * Signature: (BBBJBSSLjava/lang/String;)Z
 * uid(long), mediatype, charset, len, name, attribute, attribute
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getFileSystemItemFileRspNative
  (JNIEnv *env, jobject object, jbyte error, jbyte item , jbyte total, jlong uid, jbyte mediatype, jshort charset, jshort len, jstring folername){
  	//long uid, byte mediatype, short charset, short len, String filename);
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	avrcp_folder_mixed_item *pitem;
	const char* cstr = NULL;
	int length = 0;

	nat = get_native_data(env, object); 

	ALOGI("[BT][AVRCP]+getFileSystemItemFileRspNative error:%d item:%d len:%d uid:%ld", error, item, len, (long)uid);
	ALOGI("[BT][AVRCP]+getFileSystemItemFileRspNative total:%d mediatype:%d charset:%d", total, mediatype, charset);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND);
	if( 0 == error ){
		if( folername != NULL ){
			length = env->GetStringUTFLength(folername);
		}
		if( length > 0 ){
			cstr = (const char *) env->GetStringUTFChars(folername, 0);
		}
		pitem = &g_folder_element_item;
		pitem->item_type = 0x03; // element type
		avrcp_util_long2array( uid, pitem->uid);
		pitem->specific_type = mediatype;
		pitem->charset = charset;
		pitem->name_length = length;
	
		pitem->item_length = 8+1+2+2+pitem->name_length;// check the length composed by profile
		btmtk_avrcp_config_send_get_folderitems_rsp(&nat->avrcp, item, total, pitem, length, (const U8 *)cstr);
	
		if( length > 0 ){
			env->ReleaseStringUTFChars( folername, cstr ); 
		}
	}else{
		ALOGE("[BT][AVRCP] ignore the error:%d", error);	
	}
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getFileSystemItemFileRspNative ret:%d", ret);
	return ret;  	
}


/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    getFileSystemItemFolderStartRspNative
 * Signature: (B)Z
 * uid(long), mediatype, charset, len, name, attribute, attribute
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getFileSystemItemStartRspNative
	(JNIEnv *env, jobject object){
	// send the data out by folder
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	const char* cstr = NULL;
	int length = 0;
	jbyte item = 0;
	jbyte total = 0;
	
	avrcp_folder_mixed_item *pitem;
	
	nat = get_native_data(env, object);  
	pitem = &g_folder_element_item;
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND);
	ALOGI("[BT][AVRCP]+getFileSystemItemStartRspNative ");
	btmtk_avrcp_reset_send_get_folderitems_rsp(&nat->avrcp);
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getFileSystemItemStartRspNative ret:%d", ret);
	return ret;  		
	
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    getFileSystemItemFolderEndRspNative
 * Signature: (BBS)Z
 * uid(long), mediatype, charset, len, name, attribute, attribute
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_getFileSystemItemEndRspNative
	(JNIEnv *env, jobject object, jbyte error, jbyte status, jshort uid_counter){
	// send the data out by folder
	jboolean ret = JNI_FALSE;
	native_data_t *nat;
	const char* cstr = NULL;
	int length = 0;
	jbyte item = 0;
	jbyte total = 0;
	
	avrcp_folder_mixed_item *pitem;
	
	nat = get_native_data(env, object);  
	pitem = &g_folder_element_item;
	    
	ALOGI("[BT][AVRCP]+getFileSystemItemEndRspNative error:%d status:%d", error, status);
	nat->avrcp.seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND);
	if( error == 0x0 ) { //success
		btmtk_avrcp_send_get_folderitems_rsp(&nat->avrcp, error, 0x04, uid_counter); // 0x04 is STATUS_COMPLETE
	}else{ // fail
		btmtk_avrcp_send_get_folderitems_rsp(&nat->avrcp, error, status, uid_counter);
	}
	
	ret = JNI_TRUE;
	ALOGI("[BT][AVRCP]-getFileSystemItemEndRspNative ret:%d", ret);
	return ret;  		
	
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notificationVolumeChangedNative
 * Signature: (BBB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationVolumeChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jbyte volume){
  	  jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+setAddressedplayerRspNative volume:%d", volume);
      nat->avrcp.seq_id = getSavedRegEventSeqId(AVRCP_EVENT_VOLUME_CHANGED);
      btmtk_avrcp_send_reg_notievent_volume_changed_rsp(&nat->avrcp, error, isinterim, volume);
	  ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-setAddressedplayerRspNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notificationAddressedPlayerChangedNative
 * Signature: (BBSS)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationAddressedPlayerChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jshort player_id, jshort uid_counter){
  	  jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationAddressedPlayerChangedNative error:%d", error);
      nat->avrcp.seq_id = getSavedRegEventSeqId(AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED);
      btmtk_avrcp_send_reg_notievent_addredplayer_changed_rsp(&nat->avrcp, error, isinterim, player_id, uid_counter);
	  ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-notificationAddressedPlayerChangedNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notificationAvailPlayersChangedNative
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationAvailPlayersChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim){
  	  jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationAvailPlayersChangedNative error:%d", error);
      nat->avrcp.seq_id = getSavedRegEventSeqId(AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED);
      btmtk_avrcp_send_reg_notievent_availplayers_changed_rsp(&nat->avrcp, error, isinterim);
	  ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-notificationAvailPlayersChangedNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notificationUIDSChangedNative
 * Signature: (BBS)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationUIDSChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim, jshort uidcounter){
  	  jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationUIDSChangedNative error:%d uidcounter:%d", error, uidcounter);
      nat->avrcp.seq_id = getSavedRegEventSeqId(AVRCP_EVENT_UIDS_CHANGED);
      btmtk_avrcp_send_reg_notievent_uids_changed_rsp(&nat->avrcp, error, isinterim, uidcounter);
	  ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-notificationUIDSChangedNative ret:%d", ret);
      return ret;
}

/*
 * Class:     com_android_bluetooth_avrcp_BluetoothAvrcpService
 * Method:    notificationNowPlayingChangedNative
 * Signature: (BB)Z
 */
JNIEXPORT jboolean JNICALL BluetoothAvrcpService_notificationNowPlayingChangedNative
  (JNIEnv *env, jobject object, jbyte error, jbyte isinterim){
  	  jboolean ret = JNI_FALSE;
      native_data_t *nat;
      nat = get_native_data(env, object);  
            
      ALOGI("[BT][AVRCP]+notificationNowPlayingChangedNative error:%d", error);
      nat->avrcp.seq_id = getSavedRegEventSeqId(AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED);
      btmtk_avrcp_send_reg_notievent_now_playing_content_changed_rsp(&nat->avrcp, error, isinterim);
	  ret = JNI_TRUE;
      ALOGI("[BT][AVRCP]-notificationNowPlayingChangedNative ret:%d", ret);
      return ret;
}


/***********************************************************************************/


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
    { "connectReqNative", "(Ljava/lang/String;)Z",           (void*)BluetoothAvrcpService_connectReqNative },
    { "disconnectReqNative", "()Z",          (void*)BluetoothAvrcpService_disconnectReqNative},
    { "sendKeyReqNative", "(I)Z",            (void*)BluetoothAvrcpService_sendKeyReqNative},
    
    /* AVRCP 13 */
    { "getCapabilitiesRspNative", "(BB[B)Z",       (void*)BluetoothAvrcpService_getCapabilitiesRspNative},
    { "listPlayerappAttributeRspNative", "(BB[B)Z",          (void*)BluetoothAvrcpService_listPlayerappAttributeRspNative__B_3B},
    { "listPlayerappValuesRspNative", "(BBB[B)Z",             (void*)BluetoothAvrcpService_listPlayerappValuesRspNative__BB_3B},
    { "getCurplayerappValueRspNative", "(BB[B[B)Z",           (void*)BluetoothAvrcpService_getCurplayerappValueRspNative},
    { "getPlayerappAttributeTextRspNative", "(BBBBILjava/lang/String;)Z",  (void*)BluetoothAvrcpService_getPlayerappAttributeTextRspNative},
    { "getPlayerappValueTextRspNative", "(BBBBBLjava/lang/String;)Z",      (void*)BluetoothAvrcpService_getPlayerappValueTextRspNative},
    { "setPlayerappValueRspNative", "(B)Z",                (void*)BluetoothAvrcpService_setPlayerappValueRspNative},
    { "informBatteryStatusRspNative", "(B)Z",           (void*)BluetoothAvrcpService_informBatteryStatusRspNative},
    { "informDisplayCharsetRspNative", "(B)Z",           (void*)BluetoothAvrcpService_informDisplayCharsetRspNative},
    { "getElementAttributesRspNative", "(BBBBSLjava/lang/String;)Z",    (void*)BluetoothAvrcpService_getElementAttributesRspNative},
    { "getPlayerstatusRspNative", "(BIIB)Z",              (void*)BluetoothAvrcpService_getPlayerstatusRspNative},
    { "notificationPlayStatusChangedNative", "(BBB)Z",         (void*)BluetoothAvrcpService_notificationPlayStatusChangedNative},
    { "notificationTrackChangedNative", "(BBJ)Z",          (void*)BluetoothAvrcpService_notificationTrackChangedNative},
    { "notificationTrackReachedEndNative", "(BB)Z",       (void*)BluetoothAvrcpService_notificationTrackReachedEndNative},
    { "notificationTrackReachedStartNative", "(BB)Z",     (void*)BluetoothAvrcpService_notificationTrackReachedStartNative},
    
    { "notificationPlayPosChangedNative", "(BBI)Z",     (void*)BluetoothAvrcpService_notificationPlayPosChangedNative},
    { "notificationBatteryStatusChangedNative", "(BBB)Z",     (void*)BluetoothAvrcpService_notificationBatteryStatusChangedNative},
    
    { "notificationSystemStatusChangedNative", "(BBB)Z",                 (void*)BluetoothAvrcpService_notificationSystemStatusChangedNative},
    { "notificationApplicationSettingChangedNative", "(BBB[B[B)Z",           (void*)BluetoothAvrcpService_notificationApplicationSettingChangedNative},
    { "setAbsoluteVolumeRspNative", "(BBB)Z",              (void*)BluetoothAvrcpService_setAbsoluteVolumeRspNative},
    { "setAddressedplayerRspNative", "(BB)Z",             (void*)BluetoothAvrcpService_setAddressedplayerRspNative},

    /* AVRCP 14 */
    { "setBrowsedplayerRspNative", "(BBSISB[Ljava/lang/String;)Z",    (void*)BluetoothAvrcpService_setBrowsedplayerRspNative},
    { "changePathRspNative", "(BBI)Z",            (void*)BluetoothAvrcpService_changePathRspNative},
    { "getItemattributesRspNative", "(BBB[I[S[Ljava/lang/String;)Z",   (void*)BluetoothAvrcpService_getItemattributesRspNative},
    { "playItemsRspNative", "(BB)Z",             (void*)BluetoothAvrcpService_playItemsRspNative},
    { "searchRspNative", "(BBSI)Z",           (void*)BluetoothAvrcpService_searchRspNative},
    { "addTonowplayingRspNative", "(BB)Z",             (void*)BluetoothAvrcpService_addTonowplayingRspNative},
    { "getMediaPlayerListRspNative", "(BBBBSSBB[BLjava/lang/String;)Z",             (void*)BluetoothAvrcpService_getMediaPlayerListRspNative},
	{ "getFileSystemItemFolderRspNative", "(BBBJBBSSLjava/lang/String;)Z",             (void*)BluetoothAvrcpService_getFileSystemItemFolderRspNative},    
    { "getFileSystemItemFileAttrRspNative", "(BBBISSLjava/lang/String;)Z",             (void*)BluetoothAvrcpService_getFileSystemItemFileAttrRspNative},
    { "getFileSystemItemFileRspNative", "(BBBJBSSLjava/lang/String;)Z",             (void*)BluetoothAvrcpService_getFileSystemItemFileRspNative},
    { "getFileSystemItemStartRspNative", "()Z",             (void*)BluetoothAvrcpService_getFileSystemItemStartRspNative},
    { "getFileSystemItemEndRspNative", "(BBS)Z",             (void*)BluetoothAvrcpService_getFileSystemItemEndRspNative},
    
    { "notificationVolumeChangedNative", "(BBB)Z",                 (void*)BluetoothAvrcpService_notificationVolumeChangedNative},
    { "notificationAddressedPlayerChangedNative", "(BBSS)Z",                 (void*)BluetoothAvrcpService_notificationAddressedPlayerChangedNative},
    { "notificationAvailPlayersChangedNative", "(BB)Z",                 (void*)BluetoothAvrcpService_notificationAvailPlayersChangedNative},
    { "notificationUIDSChangedNative", "(BBS)Z",                 (void*)BluetoothAvrcpService_notificationUIDSChangedNative},
    { "notificationNowPlayingChangedNative", "(BB)Z",                 (void*)BluetoothAvrcpService_notificationNowPlayingChangedNative},
    
    /* connect test */
    { "connectNative", "(Ljava/lang/String;)Z",             (void*)BluetoothAvrcpService_connectNative},
    { "disconnectNative", "()Z",             (void*)BluetoothAvrcpService_disconnectNative},
    { "connectBrowseNative", "()Z",             (void*)BluetoothAvrcpService_connectBrowseNative},
    { "disconnectBrowseNative", "()Z",             (void*)BluetoothAvrcpService_disconnectBrowseNative},
    
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
        ALOGE("Native registration unable to find class '%s'\n", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
        ALOGE("RegisterNatives failed for '%s'\n", className);
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
        return JNI_FALSE;
    return JNI_TRUE;
}

