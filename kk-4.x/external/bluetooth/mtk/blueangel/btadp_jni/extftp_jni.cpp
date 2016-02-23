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

#define LOG_TAG "BT_FTP_JNI"

#include <wchar.h>
#include <jni.h>
#include <cutils/jstring.h>
#include <utils/Log.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "cutils/sockets.h"

extern "C"
{
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"

#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_ftps_struct.h"
#include "bt_ext_utility.h"

// SH : workaround << start >>
#include "bt_adp_fs.h"
#include "bt_adp_fs_ucs2.h"
// SH : workaround <<  end  >>

#include "bt_ftp_porting.h"
#include "bt_ftps_struct.h"
#include "bt_ftps_api.h"
#include "bt_ftps_hdl.h"
#include "bt_ftpc_struct.h"
#include "bt_ftpc_api.h"
#include "bt_ftpc_hdl.h"

#include "ext_fs.h"
#include "ext_ucs2.h"
}

#define JNI_DEBUG 1

#ifdef JNI_DEBUG
#define FTP_LOG(fmt, ...) ALOGI("[BT][FTP] %s:" fmt, __FUNCTION__, ## __VA_ARGS__)
#define FTP_LOG1(fmt, ...) ALOGI("[BT][FTP] %s:" fmt, __FUNCTION__, ## __VA_ARGS__)
#define FTP_LOG2(fmt, ...) ALOGI("[BT][FTP] %s:" fmt, __FUNCTION__, ## __VA_ARGS__)
#define FTP_LOG3(fmt, ...) ALOGI("[BT][FTP] %s:" fmt, __FUNCTION__, ## __VA_ARGS__)
#else
#define FTP_LOG(fmt, ...) {}
#endif

#define FTP_ERR(fmt, ...) ALOGE("[BT][FTP] %s:" fmt, __FUNCTION__, ## __VA_ARGS__)

// Default root path format should be UCS2
// static const wchar_t* g_const_ftps_default_root = L"/sdcard";
static U8 g_ftps_default_root[128];

typedef struct {
	JNIEnv* env;
	int apisock;
	int servsock;
	U32 con_id;
} native_data_t;

/*
 * Callback functions to BluetootFtpService
 * Direction: C-> Java
 */
static jmethodID cb_server_enable_result;
static jmethodID cb_server_disable_result;
static jmethodID cb_server_authorize_ind;
static jmethodID cb_server_connect_ind;
static jmethodID cb_server_disconnect_ind;
static jmethodID cb_server_start_transferring;
static jmethodID cb_server_end_transferring;
static jmethodID cb_server_start_pushing;
static jmethodID cb_server_end_pushing;
static jmethodID cb_server_object_deleted;

static jmethodID cb_client_connected;
static jmethodID cb_client_disconnected;
static jmethodID cb_client_browsed;
static jmethodID cb_client_setpathed;
static jmethodID cb_client_pulling;
static jmethodID cb_client_pulled;
static jmethodID cb_client_pushing;
static jmethodID cb_client_pushed;
static jmethodID cb_client_deleted;
static jmethodID cb_client_create_folder_failed;

static jfieldID m_native_data;

static JNIEnv* g_jni_env;
static jobject g_service_object;

#define FTP_INTENAL_ERROR 99

/* Inline function for getting native data address from a java instance field. */
static inline native_data_t* get_native_data(JNIEnv* env, jobject object) {
	return (native_data_t*) (env->GetIntField(object, m_native_data));
}

void bt_ftps_jni_cb(U32 ftps_ui_event, U8* data) {
	char char_bd_addr[20];
	jstring jstr_data;

	switch (ftps_ui_event) {
		case BT_FTPSUI_READY:
			FTP_LOG("BT_FTPSUI_READY");
			/* Apply default root folder here */
			btmtk_ftps_set_root_folder((const kal_uint8*) g_ftps_default_root);
			if (data == 0) {
				// Succeed
				g_jni_env->CallVoidMethod(g_service_object,
						cb_server_enable_result, JNI_TRUE);
			} else {
				// Fail
				btmtk_ftps_force_clear();
				g_jni_env->CallVoidMethod(g_service_object,
						cb_server_enable_result, JNI_FALSE);
			}
			break;

		case BT_FTPS_STATE_IDLE:
			FTP_LOG("BT_FTPS_STATE_IDLE");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_disable_result, JNI_TRUE);
			break;

		case BT_FTPS_STATE_AUTHORIZING: {
			FTP_LOG("BT_FTPS_STATE_AUTHORIZING");
			btmtk_util_convert_bdaddr2string(char_bd_addr, (bt_addr_struct *) data);
			jstr_data = g_jni_env->NewStringUTF((const char*) char_bd_addr);
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_authorize_ind, jstr_data);
			g_jni_env->DeleteLocalRef(jstr_data);
			break;
		}

		case BT_FTPSUI_CONNECTED:
			FTP_LOG("BT_FTPSUI_CONNECTED");
			btmtk_util_convert_bdaddr2string(char_bd_addr, (bt_addr_struct *) data);
			jstr_data = g_jni_env->NewStringUTF((const char*) char_bd_addr);
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_connect_ind, jstr_data);
			g_jni_env->DeleteLocalRef(jstr_data);
			break;

		case BT_FTPSUI_SET_FOLDER_START:
			// Param: none
			FTP_LOG("BT_FTPSUI_SET_FOLDER_START");
			break;
		case BT_FTPSUI_SET_FOLDERED:
			// Param: rspcode
			FTP_LOG("BT_FTPSUI_SET_FOLDER_END");
			break;

		case BT_FTPSUI_BROWSE_START:
		case BT_FTPSUI_PULL_FILE_START:
			FTP_LOG("BT_FTPSUI_(BROWSE|PULL_FILE)_START");
			jstr_data = (data != NULL) ? g_jni_env->NewStringUTF((const char*) data) : NULL;
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_start_transferring, jstr_data);
			break;

		case BT_FTPSUI_PUSH_FILE_START:
			FTP_LOG("BT_FTPSUI_PUSH_FILE_START");
			jstr_data = (data != NULL) ? g_jni_env->NewStringUTF((const char*) data) : NULL;
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_start_pushing, jstr_data);
			break;

		case BT_FTPSUI_BROWSING:
		case BT_FTPSUI_PUSHING:
		case BT_FTPSUI_PULLING:
			break;

		case BT_FTPSUI_BROWSED:
		case BT_FTPSUI_PULLED:
			FTP_LOG("BT_FTPSUI_(BROWSE|PULL_FILE)_END");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_end_transferring, NULL);
			break;

		case BT_FTPSUI_PUSHED:
			FTP_LOG("BT_FTPSUI_PUSH_FILE_END");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_end_pushing, (jint) data);
			break;

		case BT_FTPSUI_FILE_DELETE:
			FTP_LOG("BT_FTPSUI_FILE_DELETE");
			jstr_data = g_jni_env->NewStringUTF((const char*) data);
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_object_deleted, jstr_data);
			g_jni_env->DeleteLocalRef(jstr_data);
			break;

		case BT_FTPSUI_FOLDER_DELETE:
			FTP_LOG("BT_FTPSUI_FOLDER_DELETE");
			break;

		case BT_FTPSUI_FILE_CREATE:
			FTP_LOG("BT_FTPSUI_FILE_CREATE");
			break;

		case BT_FTPSUI_FOLDER_CREAT_START:
			FTP_LOG("BT_FTPSUI_FOLDER_CREAT_START");
			break;

		case BT_FTPSUI_ABORTED:
			FTP_LOG("BT_FTPSUI_ABORTED");
			break;
		case BT_FTPSUI_DISCONNECTED:
			FTP_LOG("BT_FTPSUI_DISCONNECTED");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_server_disconnect_ind, NULL);
			break;
		case BT_FTPSUI_ERROR:
			FTP_LOG("BT_FTPSUI_ERROR");
			break;

		default:
			FTP_LOG("unknown_for_now");
			break;
	}
}

void bt_ftpc_jni_cb(U32 ftpc_ui_event, U8* data) {
	int prog = 0;

	switch (ftpc_ui_event) {
		case BT_FTPCUI_EVENT_NONE:
			FTP_LOG("BT_FTPCUI_EVENT_NONE");
			break;
		case BT_FTPCUI_DEACTVE:
			FTP_LOG("BT_FTPCUI_DEACTVE");
			break;
		case BT_FTPCUI_ACTIVE:
			FTP_LOG("BT_FTPCUI_ACTIVE");
			break;

		case BT_FTPCUI_CONNECTING:
			FTP_LOG("BT_FTPCUI_CONNECTING");
			break;

		case BT_FTPCUI_AUTHEN_WAIT:
			FTP_LOG("BT_FTPCUI_AUTHEN_WAIT");
			break;
		case BT_FTPCUI_CONNECTED:
			jint conntype, mru, srm;
			conntype = mru = srm = 0;
			conntype = btmtk_ftpc_get_conntype();
			mru = btmtk_ftpc_get_mru();
			srm = btmtk_ftpc_get_srm();
		        
			FTP_LOG("BT_FTPCUI_CONNECTED");
                        FTP_LOG2("conntype:%d mru:%d", conntype, mru);
			// Connection with FTP server established.
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_connected, NULL);
			break;
		case BT_FTPCUI_DISCONNECTING:
			FTP_LOG("BT_FTPCUI_DISCONNECTING");
			break;
		case BT_FTPCUI_PUSHING: {
			jlong p, t;
			btmtk_ftpc_get_push_progress((long long *) &p, (long long *) &t);
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_pushing, p, t);

			// FTP_LOG("BT_FTPCUI_PUSHING, p = %ld, t = %ld", (long int) p, (long int) t);
			break;
		}

		case BT_FTPCUI_PUSHED:
			FTP_LOG("BT_FTPCUI_PUSHED");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_pushed, (jint) data);
			break;

		case BT_FTPCUI_BROWSING:
			FTP_LOG("BT_FTPCUI_BROWSING");
			break;
		case BT_FTPCUI_BROWSED:
			FTP_LOG("BT_FTPCUI_BROWSED");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_browsed, (jint) data);
			break;

		case BT_FTPCUI_PULLING: {
			jlong p = 0, t = 0;
			btmtk_ftpc_get_pull_progress((long long *) &p, (long long *) &t);
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_pulling, p, t);

			// FTP_LOG("BT_FTPCUI_PULLING, p = %ld, t = %ld", (long int) p, (long int) t);
			break;
		}

		case BT_FTPCUI_PULLED:
			FTP_LOG("BT_FTPCUI_PULLED");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_pulled, (jint) data);
			break;

		case BT_FTPCUI_SETPATHING:
			FTP_LOG("BT_FTPCUI_SETPATHING");
			break;
		case BT_FTPCUI_SETPATHED:
			FTP_LOG("BT_FTPCUI_SETPATHED");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_setpathed, (jint) data);
			break;
		case BT_FTPCUI_ABORTING:
			FTP_LOG("BT_FTPCUI_ABORTING");
			break;
		case BT_FTPCUI_FILE_DELETED:
			FTP_LOG("BT_FTPCUI_FILE_DELETED");
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_deleted, (jint) data);
			break;
		case BT_FTPCUI_FOLDER_DELETED:
			FTP_LOG("BT_FTPCUI_FOLDER_DELETED");
			break;
		case BT_FTPCUI_FOLDER_CREATED:
			FTP_LOG("BT_FTPCUI_FOLDER_CREATED");
			// This case is for failed creation.
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_create_folder_failed, NULL);
			break;
		case BT_FTPCUI_DISCONNECTED:
			FTP_LOG("BT_FTPCUI_DISCONNECTED");
			// In this situation, we are disconnected by server.
			g_jni_env->CallVoidMethod(g_service_object,
					cb_client_disconnected, NULL);
			break;
		case BT_FTPCUI_ERROR:
			FTP_LOG("BT_FTPCUI_ERROR");
			break;

		default:
			FTP_LOG("Unknown FTP client event");
			break;
	}
}

/*************************************************************************************
 * FTP Service (Common) Native Function Definitions
 *************************************************************************************/

static jboolean initServiceNative(JNIEnv* env, jobject object) {

	native_data_t* nat = NULL;

	FTP_LOG("Enter");

	// Find class structure
	jclass clazz = env->FindClass("com/mediatek/bluetooth/ftp/BluetoothFtpService");
	if (clazz == NULL) {
		FTP_ERR("FindClass failed");
		goto init_fail;
	}

	// Keep the service object for invoking callback functions.
	g_service_object = env->NewGlobalRef(object);

	// Retrieve the method id and field id
	cb_server_enable_result = env->GetMethodID(clazz, "cbServerEnableResult", "(Z)V");
	cb_server_disable_result = env->GetMethodID(clazz, "cbServerDisableResult", "(Z)V");
	cb_server_authorize_ind = env->GetMethodID(clazz, "cbServerAuthorizeInd", "(Ljava/lang/String;)V");
	cb_server_connect_ind = env->GetMethodID(clazz, "cbServerConnectInd", "(Ljava/lang/String;)V");
	cb_server_disconnect_ind = env->GetMethodID(clazz, "cbServerDisconnectInd", "()V");
	cb_server_start_transferring = env->GetMethodID(clazz, "cbServerStartTransferring", "(Ljava/lang/String;)V");
	cb_server_end_transferring = env->GetMethodID(clazz, "cbServerEndTransferring", "()V");
	cb_server_start_pushing = env->GetMethodID(clazz, "cbServerStartPushing", "(Ljava/lang/String;)V");
	cb_server_end_pushing = env->GetMethodID(clazz, "cbServerEndPushing", "(I)V");
	cb_server_object_deleted = env->GetMethodID(clazz, "cbServerObjectDeleted", "(Ljava/lang/String;)V");

	cb_client_connected = env->GetMethodID(clazz, "cbClientConnected", "()V");
	cb_client_disconnected = env->GetMethodID(clazz, "cbClientDisconnected", "()V");
	cb_client_browsed = env->GetMethodID(clazz, "cbClientBrowsed", "(I)V");
	cb_client_setpathed = env->GetMethodID(clazz, "cbClientSetpathed", "(I)V");
	cb_client_pulling = env->GetMethodID(clazz, "cbClientPulling", "(JJ)V");
	cb_client_pulled = env->GetMethodID(clazz, "cbClientPulled", "(I)V");
	cb_client_pushing = env->GetMethodID(clazz, "cbClientPushing", "(JJ)V");
	cb_client_pushed = env->GetMethodID(clazz, "cbClientPushed", "(I)V");
	cb_client_deleted = env->GetMethodID(clazz, "cbClientDeleted", "(I)V");
	cb_client_create_folder_failed = env->GetMethodID(clazz, "cbClientCreateFolderFailed", "()V");

	m_native_data = env->GetFieldID(clazz, "mNativeData", "I");

	// Allocate nat and store its address in object field
	nat = (native_data_t*) calloc(1, sizeof(native_data_t));
	if (nat == NULL) {
		FTP_ERR("Allocate nat: out of memory");
		goto init_fail;
	}

	memset(nat, 0, sizeof(native_data_t));
	env->SetIntField(object, m_native_data, (jint) nat);

	// Create the socket
	nat->servsock = bt_ftp_init_socket();
	if (nat->servsock <= 0) {
		FTP_ERR("Create receiving socket failed");
		goto init_fail;
	}

	btmtk_ftps_forcefully_deactivate_server();

	FTP_LOG("Succeeded");
	return JNI_TRUE;

init_fail:
	if (nat) {
		free(nat);
	}

	FTP_ERR("Failed");
	return JNI_FALSE;
}

static void cleanServiceNative(JNIEnv* env, jobject object) {
	native_data_t* nat = get_native_data(env, object);
	int sockfd;

	if (nat) {
		sockfd = nat->servsock;
		close(sockfd);
		free(nat);
	}
       env->DeleteGlobalRef(g_service_object);
}

static void forceClearServerNative(JNIEnv* env, jobject object) {
	FTP_LOG("Force clear server.");
	btmtk_ftps_force_clear();
}

static void forceClearClientNative(JNIEnv* env, jobject object) {
    FTP_LOG("Force clear client.");
    btmtk_ftpc_force_clear();
}

static jboolean prepareListentoSocketNative(JNIEnv* env, jobject object) {
	native_data_t* nat = get_native_data(env, object);

	if (bt_ftp_clear_socket() < 0) {
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

static void stopListentoSocketNative(JNIEnv* env, jobject object) {
	bt_ftp_send_msg_toself(MSG_ID_BT_FTP_TERMINATE_SERVICE, NULL, 0);
}

/* Keeps listening to the socket for incoming ilm */
static jboolean listentoSocketNative(JNIEnv* env, jobject object) {
	int res = 1;
	int sockfd;
	unsigned long msg_id;

	fd_set readfs;
	ilm_struct ilm;
	native_data_t* nat = NULL;

	nat = get_native_data(env, object);
	if (nat == NULL) {
		FTP_ERR("NULL nat");
		return JNI_FALSE;
	}

	// NOTE: Only the socket-listener thread can set g_int_env!
	//		 Or there might be VM aborting due to referencing env of
	//		 different thread.

	g_jni_env = env;

	sockfd = nat->servsock;
	if (sockfd <= 0) {
		FTP_ERR("Invalid sockfd");
		return JNI_FALSE;
	}

	FD_ZERO(&readfs);
	FD_SET(sockfd, &readfs);

	while (1) {
		res = select(sockfd+1, &readfs, NULL, NULL, NULL);

		if (res > 0) {
			res = recvfrom(sockfd, (void*) &ilm, sizeof(ilm_struct), 0, NULL, NULL);
		} else if (res < 0) {
			if (errno != EINTR) {
				FTP_ERR("select() failed. errno: %d", errno);
				return JNI_FALSE;
			} else {
				continue;
			}
		} else {
			FTP_ERR("Waiting ILM timeout");
			return JNI_FALSE;
		}

		msg_id = ilm.msg_id;
		if (msg_id == (unsigned long) MSG_ID_BT_FTP_TERMINATE_SERVICE) {
			FTP_LOG("Job done. Stop listening to socket.");
			return JNI_TRUE;
		}

		if (msg_id <= (U32) MSG_ID_BT_FTPC_GROUP_END && msg_id >= (U32) MSG_ID_BT_FTPC_GROUP_START) {
			btmtk_ftpc_handle_message(&ilm);

		} else if (msg_id <= (U32) MSG_ID_BT_FTPS_GROUP_END && msg_id >= (U32) MSG_ID_BT_FTPS_GROUP_START) {
			btmtk_ftps_handle_message(&ilm);

		} else {
			FTP_ERR("Invalid ILM, ID: %lu", msg_id);
		}
		// FTP_LOG("Received ILM, ID: %lu", msg_id);
	}

	return JNI_TRUE;
}

/*************************************************************************************
 * FTP Server Native Function Definitions
 *************************************************************************************/

static void serverEnableReqNative(JNIEnv* env, jobject object, jstring root) {
	FTP_LOG("Enable FTP server.");
	const char* c_root = env->GetStringUTFChars(root, NULL);

	FTP_LOG("Root path: %s", c_root);
	ext_chset_utf8_to_ucs2_string((U8*) g_ftps_default_root, sizeof(g_ftps_default_root), (const U8*) c_root);

	if (c_root != NULL) {
		env->ReleaseStringUTFChars(root, c_root);
	}

	btmtk_ftps_set_callback(bt_ftps_jni_cb);
#ifdef __BT_GOEP_V2__	
	btmtk_ftps_activate_server(2); // 2 is l2cap & rfcomm
#else
	btmtk_ftps_activate_server(0); // 0 is rfcomm
#endif
}

static void serverDisableReqNative(JNIEnv* env, jobject object) {
	FTP_LOG("Disable FTP server.");
	btmtk_ftps_deactivate_server();
}

static jboolean setPermissionNative(JNIEnv* env, jobject object, jboolean perm) {
	BT_BOOL read_only = (perm == JNI_TRUE);
	BT_BOOL result = FALSE;
	btmtk_ftps_setup_read_only(read_only, &result);
	return JNI_TRUE;
}

static void serverAuthorizeRspNative(JNIEnv* env, jobject object, jboolean rsp) {
	if (rsp) {
		// FTP_LOG("Accept");
		btmtk_ftps_authorize_accept();	
	} else {
		// FTP_LOG("Reject");
		btmtk_ftps_authorize_reject();
	}
}

static void serverDisconnectReqNative(JNIEnv* env, jobject object) {
	btmtk_ftps_disconnect();
}

static jboolean serverAbortReqNative(JNIEnv* env, jobject object) {

#ifdef __BT_GOEP_V2__	
	btmtk_ftps_abortfile_req();
#else
	btmtk_ftps_abort();
#endif
	return JNI_TRUE;
}

/*************************************************************************************
 * FTP Client Native Function Definitions
 *************************************************************************************/

/* NOTE: Enabling and disabling ftp client need not to send ILM to BT-task. */
static jboolean clientEnableReqNative(JNIEnv* env, jobject object) {
	jboolean ret = JNI_TRUE;

	FTP_LOG("Enable FTP client.");
	btmtk_ftpc_set_callback(bt_ftpc_jni_cb);

	if (btmtk_ftpc_client_enable() == FALSE) {
		ret = JNI_FALSE;
	}

	return ret;
}

static void clientDisableReqNative(JNIEnv* env, jobject object) {
	FTP_LOG("Disable FTP client.");
	btmtk_ftpc_client_disable();
}

static void clientConnectReqNative(JNIEnv* env, jobject object, jstring addr) {
	FTP_LOG("Connect to FTP server.");
	const char* c_bd_addr;
	bt_addr_struct bd_addr;

	if (addr == NULL) {
		FTP_ERR("NULL address.");
		return;
	}

	c_bd_addr = env->GetStringUTFChars(addr, NULL);

	btmtk_util_convert_string2bdaddr(c_bd_addr, &bd_addr);
#ifdef __BT_GOEP_V2__
        btmtk_ftpc_connect_server_ex(bd_addr, 2);
#else
	btmtk_ftpc_connect_server(bd_addr);
#endif
	// Release the UFT chars
	if (c_bd_addr != NULL) {
		env->ReleaseStringUTFChars(addr, c_bd_addr);
	}
}

static void clientDisconnectReqNative(JNIEnv* env, jobject object) {
	FTP_LOG("Disconnect FTP client.");
	btmtk_ftpc_disconnect();
}

static void clientRefreshReqNative(JNIEnv* env, jobject object) {
	FTP_LOG("To get folder listing object.");
    U8 result = TRUE;
        
#ifdef __BT_GOEP_V2__    
	result = btmtk_ftpc_get_folder_content_ex(NULL, NULL);
#else
	btmtk_ftpc_get_folder_content(NULL, NULL);
#endif    

#ifdef __BT_GOEP_V2__    
    if( FALSE == result ){
	    // fail handling
	    FTP_LOG("[ERR] Fail! ");
	    bt_ftpc_jni_cb(BT_FTPCUI_SETPATHED, (U8*)FTP_INTENAL_ERROR);
    }
#endif
}

static void clientGoForwardReqNative(JNIEnv* env, jobject object, jstring path) {
	const char* c_path = env->GetStringUTFChars(path, NULL);
	U16 ucs2_path[BTMTK_FS_MAX_FILEPATH * 2];

	FTP_LOG("Set path, forward: %s", c_path);
	ext_chset_utf8_to_ucs2_string((U8*) ucs2_path, sizeof(ucs2_path), (const U8*) c_path);

	btmtk_ftpc_enter_folder(ucs2_path);

	if (c_path != NULL) {
		env->ReleaseStringUTFChars(path, c_path);
	}
}

static void clientGoBackwardReqNative(JNIEnv* env, jobject object) {
	FTP_LOG("Set path. backward");
	btmtk_ftpc_back_folder();
}

static void clientGoToRootReqNative(JNIEnv* env, jobject object) {
	FTP_LOG("Set path. to root");
	btmtk_ftpc_goto_root_folder();
}

static void clientPullReqNative(JNIEnv* env, jobject object,
		jstring path, jstring name, jstring rename) {

	const char* c_path = env->GetStringUTFChars(path, NULL);
	const char* c_name = env->GetStringUTFChars(name, NULL);
	const char* c_rename = (rename != NULL) ? env->GetStringUTFChars(rename, NULL) : NULL;

	U8 ucs2_path[BTMTK_FS_MAX_FILEPATH * 2];
	U8 ucs2_name[BTMTK_FS_MAX_FILEPATH * 2];
	U8 ucs2_rename[BTMTK_FS_MAX_FILEPATH * 2];

	FTP_LOG("Client requests to pull, path: %s, target: %s, rename: %s",
	c_path, c_name, (c_rename != NULL) ? c_rename : "NULL");

	ext_chset_utf8_to_ucs2_string(ucs2_path, sizeof(ucs2_path), (const U8*) c_path);
	ext_chset_utf8_to_ucs2_string(ucs2_name, sizeof(ucs2_name), (const U8*) c_name);

#ifdef __BT_GOEP_V2__
	if (c_rename != NULL) {
                U8 mode=2;
	        ext_chset_utf8_to_ucs2_string(ucs2_rename, sizeof(ucs2_rename), (const U8*) c_rename);
	        btmtk_ftpc_pull_file_ex(ucs2_path, ucs2_name, ucs2_rename, mode>0?TRUE:FALSE, 0);
	        env->ReleaseStringUTFChars(rename, c_rename);
	} else {
   	        btmtk_ftpc_pull_file_ex(ucs2_path, ucs2_name, NULL, TRUE, 0);
	}
#else 
	if (c_rename != NULL) {
		ext_chset_utf8_to_ucs2_string(ucs2_rename, sizeof(ucs2_rename), (const U8*) c_rename);
		btmtk_ftpc_get_file_obj(ucs2_path, ucs2_name, ucs2_rename);
		env->ReleaseStringUTFChars(rename, c_rename);
	} else {
		btmtk_ftpc_get_file_obj(ucs2_path, ucs2_name, NULL);
	}
#endif
	if (c_path != NULL) {
		env->ReleaseStringUTFChars(path, c_path);
	}

	if (c_name != NULL) {
		env->ReleaseStringUTFChars(name, c_name);
	}
}

static jboolean clientPushReqNative(JNIEnv* env, jobject object, jstring name) {

	jboolean ret = JNI_TRUE;
	const char* c_name = env->GetStringUTFChars(name, NULL);
	U8 ucs2_name[BTMTK_FS_MAX_FILEPATH * 2];

	FTP_LOG("Client requests to push: %s", c_name);
	ext_chset_utf8_to_ucs2_string(ucs2_name, sizeof(ucs2_name), (const U8*) c_name);

#ifdef __BT_GOEP_V2__
        {
            U8 mode = 2;
	    if (btmtk_ftpc_push_file_ex(ucs2_name, NULL, mode>0?TRUE:FALSE) == FALSE) {
	        ret = JNI_FALSE;
	    }
        }
#else
	if (btmtk_ftpc_push_file(ucs2_name, NULL) == FALSE) {
		ret = JNI_FALSE;
	}
#endif    

	if (c_name != NULL) {
		env->ReleaseStringUTFChars(name, c_name);
	}

	return ret;
}

static void clientDeleteReqNative(JNIEnv* env, jobject object, jstring name) {

const char* c_name = env->GetStringUTFChars(name, NULL);
	U8 ucs2_name[BTMTK_FS_MAX_FILEPATH * 2];

	FTP_LOG("Client requests to delete: %s", c_name);
	ext_chset_utf8_to_ucs2_string(ucs2_name, sizeof(ucs2_name), (const U8*) c_name);

	btmtk_ftpc_delete_folder(ucs2_name);

	if (c_name != NULL) {
		env->ReleaseStringUTFChars(name, c_name);
	}
}

static void clientCreateFolderReqNative(JNIEnv* env, jobject object, jstring name) {
	const char* c_name = env->GetStringUTFChars(name, NULL);
	U8 ucs2_name[BTMTK_FS_MAX_FILEPATH * 2];

	FTP_LOG("Client requests to create folder: %s", c_name);
	ext_chset_utf8_to_ucs2_string(ucs2_name, sizeof(ucs2_name), (const U8*) c_name);

	btmtk_ftpc_make_folder(ucs2_name);

	if (c_name != NULL) {
		env->ReleaseStringUTFChars(name, c_name);
	}

}

static jboolean clientAbortReqNative(JNIEnv* env, jobject object) {
	jboolean ret = JNI_FALSE;

	FTP_LOG("Abort");
#ifdef __BT_GOEP_V2__
	if( TRUE == btmtk_ftpc_abortfile_req(1) ){
		ret = JNI_TRUE;
	}
#else
	if (btmtk_ftpc_abort() == TRUE) {
		ret = JNI_TRUE;
	}
#endif
	return ret;
}

static void clientCancelConnectNative(JNIEnv* env, jobject object) {
	FTP_LOG("Cancel connect");
	btmtk_ftpc_connect_cancel();
}

/*************************************************************************************
 * JNI Initialization
 *************************************************************************************/

/*
 * Native function table associated with BluetoothFtpService
 * Direction: Java -> C
 */
static JNINativeMethod sMethods[] = {

	/* FTP service relative native functions */
	{"initServiceNative", "()Z", (void*) initServiceNative},
	{"cleanServiceNative", "()V", (void*) cleanServiceNative},
	{"forceClearServerNative", "()V", (void*) forceClearServerNative},
	{"forceClearClientNative", "()V", (void*) forceClearClientNative},
	{"prepareListentoSocketNative", "()Z", (void*) prepareListentoSocketNative},
	{"stopListentoSocketNative", "()V", (void*) stopListentoSocketNative},
	{"listentoSocketNative", "()Z", (void*) listentoSocketNative},

	/* FTP server relative native functions */
	{"serverEnableReqNative", "(Ljava/lang/String;)V", (void*) serverEnableReqNative},
	{"serverDisableReqNative", "()V", (void*) serverDisableReqNative},
	{"setPermissionNative", "(Z)Z", (void*) setPermissionNative},
	{"serverAuthorizeRspNative", "(Z)V", (void*) serverAuthorizeRspNative},
	{"serverDisconnectReqNative", "()V", (void*) serverDisconnectReqNative},
	{"serverAbortReqNative", "()Z", (void*) serverAbortReqNative},

	/* FTP client relative native functions */
	{"clientEnableReqNative", "()Z", (void*) clientEnableReqNative},
	{"clientDisableReqNative", "()V", (void*) clientDisableReqNative},
	{"clientConnectReqNative", "(Ljava/lang/String;)V", (void*) clientConnectReqNative},
	{"clientDisconnectReqNative", "()V", (void*) clientDisconnectReqNative},
	{"clientRefreshReqNative", "()V", (void*) clientRefreshReqNative},
	{"clientGoForwardReqNative", "(Ljava/lang/String;)V", (void*) clientGoForwardReqNative},
	{"clientGoBackwardReqNative", "()V", (void*) clientGoBackwardReqNative},
	{"clientGoToRootReqNative", "()V", (void*) clientGoToRootReqNative},
	{"clientPullReqNative", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void*) clientPullReqNative},
	{"clientPushReqNative", "(Ljava/lang/String;)Z", (void*) clientPushReqNative},
	{"clientDeleteReqNative", "(Ljava/lang/String;)V", (void*) clientDeleteReqNative},
	{"clientCreateFolderReqNative", "(Ljava/lang/String;)V", (void*) clientCreateFolderReqNative},
	{"clientAbortReqNative", "()Z", (void*) clientAbortReqNative},
	{"clientCancelConnectNative", "()V", (void*) clientCancelConnectNative},
};

static int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* methods, int numMethods) {
	jclass clazz = env->FindClass(className);
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

static int registerNatives(JNIEnv* env) {
	if (!registerNativeMethods(env, "com/mediatek/bluetooth/ftp/BluetoothFtpService",
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
		ALOGE("ERROR: GetEnv failed\n");
		goto bail;
	}
	assert(env != null);

	if (!registerNatives(env)) {
		ALOGE("ERROR: BluetoothFtpService failed to register natives\n");
		goto bail;
	}

	result = JNI_VERSION_1_4;

bail:
	return result;
}

