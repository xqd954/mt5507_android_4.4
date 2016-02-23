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


#define LOG_TAG "BT SIMAP_JNI"

//#include "android_bluetooth_common.h"
#include "android_runtime/AndroidRuntime.h"
#include "JNIHelp.h"
#include "jni.h"
#include "utils/Log.h"
#include "utils/misc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


extern "C"
{

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_simap_struct.h"
#include "bt_simap_api.h"
#include "bt_simap_mmi_event.h"
#include "bt_simap_porting.h"


#define TRANS_PRO_TO        0
#define TRANS_PRO_T1        1
#define TRANS_PRO_TO_T1     2

extern void btmtk_simap_handle_message(ilm_struct *message);

}

#define SIM_ACCESS_CHANNEL_RIL_SOCKET  1   // Send requests to modem via socket directly
#define SIM_ACCESS_CHANNEL_AF          2   // Send requests to the app framework and then the AF forwards the requests to modem

//#define SIM_ACCESS_CHANNEL_SELECT  SIM_ACCESS_CHANNEL_RIL_SOCKET
#define SIM_ACCESS_CHANNEL_SELECT  SIM_ACCESS_CHANNEL_AF

#if SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_RIL_SOCKET
    #include "BtSimapRILAdapter.h"
#elif SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_AF
    #include "BtSimapAfAdapter.h"
#else
    #error Invalid SIM_ACCESS_CHANNEL_SELECT
#endif

#define SIMAP_LOG(fmt, ...) ALOGI("[SAP JNI] %s:" fmt, __FUNCTION__, ## __VA_ARGS__)

#define SIMAP_ERR(fmt, ...) ALOGE("[SAP JNI] %s:" fmt, __FUNCTION__, ## __VA_ARGS__)



typedef struct {
	JNIEnv* env;
	int msg_sock; //send/receive msg to/from internal adapter
	U32 con_id;
} native_data_t;
	

JavaVM* g_jvm;

jobject g_service_object;

static jfieldID field_mNativeData;

static jboolean g_SIM_connected = JNI_FALSE;
static int g_SIM_index = 1;
static char g_remote_addr[18];

/*
 * Callback functions to BluetootSimapService
 * Direction: C-> Java
 */
static jmethodID cb_onEnableCnf;
static jmethodID cb_onDisableCnf;
static jmethodID cb_onAuthorizeInd;
static jmethodID cb_onConnectedInd;
static jmethodID cb_onDisconnectedInd;
static jmethodID cb_onDisconnectCnf;
static jmethodID cb_onBtResetInd;

#if SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_AF
    jmethodID cb_onBtSimapCmd;
#endif

static void bt_simap_jni_cb(void* handle, BT_CALLBACK_EVENT event, void *parms, unsigned short datasize);


static inline native_data_t * get_native_data(JNIEnv *env, jobject object) {
	return (native_data_t *)(env->GetIntField(object, field_mNativeData));
}

static jboolean initServiceNative(JNIEnv* env, jobject object) {
	ALOGV(__FUNCTION__);

    native_data_t* nat = NULL;

    SIMAP_LOG("Enter");

    /* Find class structure */
    jclass clazz = env->FindClass("com/mediatek/bluetooth/simap/BluetoothSimapService");
    if (clazz == NULL) {
		SIMAP_ERR("FindClass failed");
		goto init_fail;
    }

    if (NULL == g_service_object) {
        SIMAP_ERR("0. Try to get g_service_object");
        g_service_object = env->NewGlobalRef(object);
    
        if (NULL == g_service_object) {
            SIMAP_ERR("Failed to get g_service_object");
            goto init_fail;
        }
    }

    /* Retrieve the method id and field id */
	cb_onEnableCnf = env->GetMethodID(clazz, "onEnableCnf", "(Z)V");
	cb_onDisableCnf = env->GetMethodID(clazz, "onDisableCnf", "(Z)V");
	cb_onAuthorizeInd = env->GetMethodID(clazz, "onAuthorizeInd", "(Ljava/lang/String;)Z");
	cb_onConnectedInd = env->GetMethodID(clazz, "onConnectedInd", "(Ljava/lang/String;)V");
	cb_onDisconnectedInd = env->GetMethodID(clazz, "onDisconnectedInd", "()V");
	cb_onDisconnectCnf= env->GetMethodID(clazz, "onDisconnectCnf", "(Z)V");
	cb_onBtResetInd = env->GetMethodID(clazz, "onBtResetInd", "()V");

    #if SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_AF
        cb_onBtSimapCmd= env->GetMethodID(clazz, "onBtSimapCmd", "(IILjava/lang/String;)Lcom/mediatek/bluetooth/simap/AfAdapterResult;");
    #endif
	
	field_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");


    /* Allocate nat and store its address in object field */
	nat = (native_data_t *)calloc(1, sizeof(native_data_t));
	if (NULL == nat) {
		ALOGE("%s: out of memory!", __FUNCTION__);
		goto init_fail;
	}	
    memset(nat, 0, sizeof(native_data_t));
	env->SetIntField(object, field_mNativeData, (jint)nat);

	
	nat->msg_sock = bt_simap_init_socket();
	SIMAP_LOG("[BT SAP JNI]bt_simap_init_socket return %d", nat->msg_sock);

    SIMAP_LOG("Succeeded");
    return JNI_TRUE;

init_fail:
    if (nat) {
		free(nat);
    }

    SIMAP_ERR("Failed");
    return JNI_FALSE;	
}

#if SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_RIL_SOCKET
static void resetSIMConnection() {
	int ret;
	ALOGI("[SIMAP_JNI]resetSIMConnection: g_SIM_connected=%d", g_SIM_connected);
	
	if (g_SIM_connected) {
		ret = bt_simap_process_client_simple_request(SIMAP_DISCONNECT_SIM_REQ);
		if (ret == SIMAP_RSP_ERR_BUSY) {
			while (ret == SIMAP_RSP_ERR_BUSY) {
			    ALOGI("[SIMAP_JNI]resetSIMConnection: SIMAP_DISCONNECT_SIM_REQ return SIMAP_RSP_ERR_BUSY, waiting...");
				sleep(200);
				
			    ALOGI("[SIMAP_JNI]resetSIMConnection: SIMAP_DISCONNECT_SIM_REQ again");
				ret = bt_simap_process_client_simple_request(SIMAP_DISCONNECT_SIM_REQ);
			}
		}
		
		bt_simap_deinit_ril_sock();
		
		g_SIM_connected = JNI_FALSE;
	}
}
#elif SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_AF
static void resetSIMConnection() {
    int ret;
    ALOGI("[SIMAP_JNI]resetSIMConnection: g_SIM_connected=%d", g_SIM_connected);

    if (g_SIM_connected) {
        ret = BT_SIMAP_Disconnect_SIM();
        if (SUCCESS == ret) {
            g_SIM_connected = JNI_FALSE;
        }
    }
}
#endif

static void cleanupNativeDataNative(JNIEnv* env, jobject object) {
	ALOGV(__FUNCTION__);
	native_data_t *nat =
		(native_data_t *)env->GetIntField(object, field_mNativeData);

	if(nat->msg_sock >= 0)
	{
		bt_simap_deinit_socket();
		nat->msg_sock = -1;
	}

	resetSIMConnection();
		
    if (nat) {
        free(nat);
    }

    if (NULL != g_service_object) {
        ALOGI("[SIMAP_JNI] free g_service_object");
        env->DeleteGlobalRef(g_service_object);
        g_service_object = NULL;
    }
}

static jboolean enableNative(JNIEnv *env, jobject obj) {
	ALOGV(__FUNCTION__);

		ALOGI("[SIMAP_JNI] enableNative [+]");
		
		btmtk_simap_register_mmi_callback_req(bt_simap_jni_cb);

		btmtk_simap_activate_req();

		ALOGI("[SIMAP_JNI] enableNative [-]");

		return JNI_TRUE;
}

static void disableNative(JNIEnv *env, jobject obj) {
    ALOGV(__FUNCTION__);
	
	ALOGI("[SIMAP_JNI] disableNative [+]");
	btmtk_simap_deactivate_req();

	resetSIMConnection();
	
	ALOGI("[SIMAP_JNI] disableNative [-]");
}


static void authorizeRspNative(JNIEnv* env, jobject object, jboolean rsp) {
    ALOGV(__FUNCTION__);

	ALOGD("[SIMAP_JNI]authorizeRspNative, rsp=%d", rsp);
	btmtk_simap_auth_rsp(rsp);		
}

static void disconnectNative(JNIEnv *env, jobject obj, int discMode) {
	ALOGV(__FUNCTION__);

	resetSIMConnection();
	
	btmtk_simap_disconnect_req(discMode);
}

static jboolean selectSIMNative (JNIEnv* env, jobject object, int index) {

	ALOGD("[SIMAP_JNI]selectSIMNative, index=%d", index);

	g_SIM_index = index;
	
	return JNI_TRUE;
}

/*
 * Keeps listening to the socket for incoming ilm 
 */
static jboolean startListenNative(JNIEnv* env, jobject object) {
	int res = 1;
	int sockfd;
	unsigned int msg_id;	

	fd_set readfs;
	ilm_struct ilm;
	native_data_t* nat = NULL;

	nat = get_native_data(env, object);
	if (nat == NULL) {
		SIMAP_ERR("NULL nat");
		return JNI_FALSE;
	}

    if (NULL == g_service_object) {
        SIMAP_LOG("sln: Try to get g_service_object");
        g_service_object = env->NewGlobalRef(object);

        if (NULL == g_service_object) {
            SIMAP_ERR("Failed to get g_service_object");
            return JNI_FALSE;
        }
    }

	sockfd = nat->msg_sock;
	if (sockfd == 0) {
		SIMAP_ERR("Zero sockfd");
		return JNI_FALSE;
	}

	FD_ZERO(&readfs);
	FD_SET(sockfd, &readfs);

	// loop listening
    while (1) { 
		
	res = select(sockfd+1, &readfs, NULL, NULL, NULL);
	// SIMAP_LOG("returned from select, res = %d", res);

	if (res > 0) {
		res = recvfrom(sockfd, (void*) &ilm, sizeof(ilm_struct), 0, NULL, NULL);
	} else if (res < 0) {
		SIMAP_ERR("select() failed %s(%d)", strerror(errno), errno);
		if(errno == EINTR)
			continue;
		else
			return JNI_FALSE;
	} else {
		SIMAP_ERR("Waiting ILM timeout");
		return JNI_FALSE;
	}

	msg_id = ilm.msg_id;
	if (msg_id == MSG_ID_BT_SIMAP_TERMINATE_SERVICE) {
		SIMAP_LOG("MSG_ID_BT_SIMAP_TERMINATE_SERVICE.");
		return JNI_TRUE;
	}
	
	if (msg_id <= MSG_ID_BT_SIMAP_GROUP_END && msg_id >= MSG_ID_BT_SIMAP_GROUP_START) {
		SIMAP_LOG("call btmtk_simap_handle_message: %d", msg_id);
		btmtk_simap_handle_message(&ilm);
	} else {
		SIMAP_ERR("Invalid ILM, ID: %d", msg_id);
	}

	// SIMAP_LOG("Received ILM, ID: %lu", msg_id);
    }

	return JNI_TRUE;
}

static void stopListenNative(JNIEnv* env, jobject object) {

	bt_simap_send_msg_toself(MSG_ID_BT_SIMAP_TERMINATE_SERVICE, NULL, 0);
	
}

static jboolean prepareListentoSocketNative(JNIEnv* env, jobject object) {
    native_data_t* nat = get_native_data(env, object);

    if (bt_simap_clear_socket_data() < 0) {
		return JNI_FALSE;
    }

    return JNI_TRUE;
}	

static jboolean sendSIMUnaccessibleIndNative(JNIEnv *env, jobject obj) {
	ALOGV(__FUNCTION__);

		ALOGI("[SIMAP_JNI] sendSIMUnaccessibleIndNative [+]");
				
		btmtk_simap_send_status_ind_with_unavailable_req();
		
		ALOGI("[SIMAP_JNI] sendSIMUnaccessibleIndNative [-]");

		return JNI_TRUE;
}

static void bt_simap_proc_bt_reset_ind(JNIEnv *env)
{	
	ALOGI("bt_simap_proc_bt_reset_ind [+]");

	if (g_service_object == NULL)
		return;
	
	native_data_t *nat =
		(native_data_t *)env->GetIntField(g_service_object, field_mNativeData);

	if(nat->msg_sock >= 0)
	{
		bt_simap_deinit_socket();
		nat->msg_sock = -1;
	}

	resetSIMConnection();
		
	if (nat)
		free(nat);

	ALOGI("bt_simap_proc_bt_reset_ind [-]");
}

#if SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_RIL_SOCKET
static void bt_simap_process_sim_event(BT_CALLBACK_EVENT event, void *data, unsigned short datasize)
{
	int ret; 
	char rsp_data[SIM_RSP_BYTEARRAY_BUF_LEN];
	int rsp_data_len = 0;
	int cur_type;
	int supp_type;

	ALOGI("bt_simap_process_sim_event: event=%d", event);

	switch(event)
	{			
		/* req */
		case EVENT_SIM_MMI_CONNECT_REQ: 
			
			ret = bt_simap_init_ril_sock(g_SIM_index);
			
			ret = bt_simap_process_connect_sim_request(&cur_type, &supp_type, &rsp_data_len, rsp_data);

			btmtk_sim_connect_cnf(ret, cur_type, supp_type, rsp_data_len, (U8*)rsp_data);

			if (ret == SIMAP_RSP_SUCCESS) {
				/*
				env->CallVoidMethod(g_service_object,
									cb_onConnectedInd,
									env->NewStringUTF(g_remote_addr));
				*/
				g_SIM_connected = JNI_TRUE;
			}
			else {				
				bt_simap_deinit_ril_sock();
			}

			break;
			
		case EVENT_SIM_MMI_DISCONNECT_REQ: 
			
			ret = bt_simap_process_client_simple_request(SIMAP_DISCONNECT_SIM_REQ);

			btmtk_sim_disconnect_cnf(ret);

			bt_simap_deinit_ril_sock();

			g_SIM_connected = JNI_FALSE;
			
			break;
			
		case EVENT_SIM_MMI_APDU_REQ: 
			
			ret = bt_simap_process_apdu_request((char*)data, datasize, rsp_data, &rsp_data_len);

			ALOGI("bt_simap_process_apdu_request return: ret=%d, rsp_data_len=%d", ret, rsp_data_len);
			
			btmtk_sim_apdu_cnf(ret, rsp_data_len, (U8*)rsp_data);
			
			break;
			
		case EVENT_SIM_MMI_POWER_OFF_REQ: 

			ret = bt_simap_process_client_simple_request(SIMAP_POWER_SIM_OFF_REQ);

			btmtk_sim_power_off_cnf(ret);

			break;
			
		case EVENT_SIM_MMI_RESET_REQ: 
			
			ret = bt_simap_process_reset_sim_request(&cur_type, &rsp_data_len, rsp_data);

			btmtk_sim_reset_cnf(ret, cur_type, rsp_data_len, (U8*)rsp_data);
			
			break;


		/* ind */
#if 0		
		case EVENT_SIMAP_MMI_ATR_RSP_SENT_IND: 
			break;
			
		case EVENT_SIMAP_MMI_APDU_RSP_SENT_IND: 
			break;
			
		case EVENT_SIMAP_MMI_SIM_ON_RSP_SENT_IND: 
			break;
			
		case EVENT_SIMAP_MMI_SIM_OFF_RSP_SENT_IND:
			break;
			
		case EVENT_SIMAP_MMI_SIM_RESET_RSP_SENT_IND:
			break;
			
		case EVENT_SIMAP_MMI_CARD_STATUS_IND_SENT_IND: 
			break;
			
		case EVENT_SIMAP_MMI_CARD_READER_STATUS_RSP_SENT_IND: 
			break;
			
		case EVENT_SIMAP_MMI_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND: 
			break;
#endif			
			
		default:
			break;
	}
}
#elif SIM_ACCESS_CHANNEL_SELECT==SIM_ACCESS_CHANNEL_AF
static void bt_simap_process_sim_event(BT_CALLBACK_EVENT event, void *data, unsigned short datasize) {
    int  ret;
    char rsp_data[SIM_RSP_BYTEARRAY_BUF_LEN];
    int  rsp_data_len = SIM_RSP_BYTEARRAY_BUF_LEN;
	int  cur_type;
	int  supp_type;

    ALOGI("bt_simap_process_sim_event: event=%d", event);
    
    switch (event) {
    case EVENT_SIM_MMI_CONNECT_REQ:
        ret = BT_SIMAP_Connect_SIM((BT_SIMAP_SIM_ID)g_SIM_index, cur_type, supp_type, rsp_data, rsp_data_len);
        btmtk_sim_connect_cnf(ret, cur_type, supp_type, rsp_data_len, (U8*)rsp_data);
        if (SUCCESS == ret) {
            g_SIM_connected = JNI_TRUE;
        }
        break;
    case EVENT_SIM_MMI_DISCONNECT_REQ:
        ret = BT_SIMAP_Disconnect_SIM();
        btmtk_sim_disconnect_cnf(ret);
        if (SUCCESS == ret) {
            g_SIM_connected = JNI_FALSE;
        }
        break;
    case EVENT_SIM_MMI_POWER_OFF_REQ:
        ret = BT_SIMAP_PowerOff_SIM();
        btmtk_sim_power_off_cnf(ret);
        break;
    case EVENT_SIM_MMI_RESET_REQ:
        ret = BT_SIMAP_Reset_SIM(0, cur_type, rsp_data, rsp_data_len);
        btmtk_sim_reset_cnf(ret, cur_type, rsp_data_len, (U8*)rsp_data);
        break;
    case EVENT_SIM_MMI_APDU_REQ:
        ret = BT_SIMAP_APDU_req(0, (char*)data, datasize, rsp_data, rsp_data_len);
        btmtk_sim_apdu_cnf(ret, rsp_data_len, (U8*)rsp_data);
        break;

		/* ind */
#if 0
		case EVENT_SIMAP_MMI_ATR_RSP_SENT_IND:
			break;

		case EVENT_SIMAP_MMI_APDU_RSP_SENT_IND:
			break;

		case EVENT_SIMAP_MMI_SIM_ON_RSP_SENT_IND:
			break;

		case EVENT_SIMAP_MMI_SIM_OFF_RSP_SENT_IND:
			break;

		case EVENT_SIMAP_MMI_SIM_RESET_RSP_SENT_IND:
			break;

		case EVENT_SIMAP_MMI_CARD_STATUS_IND_SENT_IND:
			break;

		case EVENT_SIMAP_MMI_CARD_READER_STATUS_RSP_SENT_IND:
			break;

		case EVENT_SIMAP_MMI_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND:
			break;
#endif

    default:
        ALOGI("ERROR: unknown event");
    } // End of switch (event)
} // End of bt_simap_process_sim_event(...)
#endif


static void bt_simap_CONVERT_BDADDRSRC2STRING(char *dest, U32 lap, U8 uap, U16 nap)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
		(U8) ((nap & 0xFF00) >> 8),
		(U8) (nap & 0x00FF),
		uap,
		(U8) ((lap & 0x00FF0000) >> 16),
		(U8) ((lap & 0x0000FF00) >> 8),
		(U8) (lap & 0x000000FF)
		);
}

static const char* get_event_string(BT_CALLBACK_EVENT event)
{
	switch(event)
	{
	case EVENT_SIMAP_MMI_ACTIVATE_CNF:
		return "EVENT_SIMAP_MMI_ACTIVATE_CNF";
		
	case EVENT_SIMAP_MMI_DEACTIVATE_CNF:
		return "EVENT_SIMAP_MMI_DEACTIVATE_CNF";
	case EVENT_SIMAP_MMI_CONNECT_IND: 
			return "EVENT_SIMAP_MMI_CONNECT_IND";
	case EVENT_SIMAP_MMI_DISCONNECT_IND: 
			return "EVENT_SIMAP_MMI_DISCONNECT_IND";
	case EVENT_SIMAP_MMI_DISCONNECT_CNF: 
			return "EVENT_SIMAP_MMI_DISCONNECT_CNF";
	case EVENT_SIMAP_MMI_AUTH_REQ: 
			return "EVENT_SIMAP_MMI_AUTH_REQ";
	case EVENT_SIMAP_MMI_ATR_RSP_SENT_IND: 
			return "EVENT_SIMAP_MMI_ATR_RSP_SENT_IND";
	case EVENT_SIMAP_MMI_APDU_RSP_SENT_IND: 
			return "EVENT_SIMAP_MMI_APDU_RSP_SENT_IND";
	case EVENT_SIMAP_MMI_SIM_ON_RSP_SENT_IND: 
			return "EVENT_SIMAP_MMI_SIM_ON_RSP_SENT_IND";
	case EVENT_SIMAP_MMI_SIM_OFF_RSP_SENT_IND:
			return "EVENT_SIMAP_MMI_SIM_OFF_RSP_SENT_IND";
	case EVENT_SIMAP_MMI_SIM_RESET_RSP_SENT_IND:
			return "EVENT_SIMAP_MMI_SIM_RESET_RSP_SENT_IND";
	case EVENT_SIMAP_MMI_CARD_STATUS_IND_SENT_IND: 
			return "EVENT_SIMAP_MMI_CARD_STATUS_IND_SENT_IND";
	case EVENT_SIMAP_MMI_CARD_READER_STATUS_RSP_SENT_IND: 
			return "EVENT_SIMAP_MMI_CARD_READER_STATUS_RSP_SENT_IND";
	case EVENT_SIMAP_MMI_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND: 
			return "EVENT_SIMAP_MMI_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND";
	case EVENT_SIMAP_MMI_AUTH_REQ_TIMEOUT_RFCLOSE_IND: 
			return "EVENT_SIMAP_MMI_AUTH_REQ_TIMEOUT_RFCLOSE_IND";

	case EVENT_SIM_MMI_CONNECT_REQ: 
			return "EVENT_SIM_MMI_CONNECT_REQ";
	case EVENT_SIM_MMI_DISCONNECT_REQ: 
			return "EVENT_SIM_MMI_DISCONNECT_REQ";
	case EVENT_SIM_MMI_APDU_REQ: 
			return "EVENT_SIM_MMI_APDU_REQ";
	case EVENT_SIM_MMI_POWER_OFF_REQ: 
			return "EVENT_SIM_MMI_POWER_OFF_REQ";
	case EVENT_SIM_MMI_RESET_REQ: 
			return "EVENT_SIM_MMI_RESET_REQ";
	default:
		    return "Unkown event";
		}
}

static void bt_simap_jni_cb(void* handle, BT_CALLBACK_EVENT event, void *parms, unsigned short datasize)
{
    JNIEnv* env = NULL;

    if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: bt_simap_jni_cb(): GetEnv failed\n");
        return;
    }

	switch(event)
	{
		/* To SIMAP MMI */
		case EVENT_SIMAP_MMI_ACTIVATE_CNF:
			{
			bt_simap_activate_cnf_struct *msg = (bt_simap_activate_cnf_struct *)parms;
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_ACTIVATE_CNF, result=%d", msg->result);
			
			env->CallVoidMethod(g_service_object,
								cb_onEnableCnf, msg->result);
			}
			break;
			
		case EVENT_SIMAP_MMI_DEACTIVATE_CNF:
			{
			bt_simap_deactivate_cnf_struct *msg = (bt_simap_deactivate_cnf_struct *)parms;
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_DEACTIVATE_CNF, result=%d", msg->result);
			
			env->CallVoidMethod(g_service_object,
								cb_onDisableCnf, msg->result);
			}
			break;
			
		case EVENT_SIMAP_MMI_CONNECT_IND:
			{
				bt_simap_connect_ind_struct *msg = (bt_simap_connect_ind_struct *)parms;
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_CONNECT_IND");
			bt_simap_CONVERT_BDADDRSRC2STRING(g_remote_addr, msg->lap, msg->uap, msg->nap);
			
			env->CallVoidMethod(g_service_object,
								cb_onConnectedInd,
								env->NewStringUTF(g_remote_addr));
			}
			break;
			
		case EVENT_SIMAP_MMI_DISCONNECT_IND:
			{
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_DISCONNECT_IND");
			
			env->CallVoidMethod(g_service_object,
								cb_onDisconnectedInd);

			resetSIMConnection();
			
			}
			break;
			
		case EVENT_SIMAP_MMI_DISCONNECT_CNF:
			{
			bt_simap_disconnect_cnf_struct *msg = (bt_simap_disconnect_cnf_struct *)parms;
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_DISCONNECT_CNF, result=%d", msg->result);
			env->CallVoidMethod(g_service_object,
								cb_onDisconnectCnf, msg->result);
			}
			break;
			
		case EVENT_SIMAP_MMI_AUTH_REQ:
			{
			bt_simap_auth_req_struct *msg = (bt_simap_auth_req_struct *)parms;
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_AUTH_REQ");
			bt_simap_CONVERT_BDADDRSRC2STRING(g_remote_addr, msg->lap, msg->uap, msg->nap);
			env->CallVoidMethod(g_service_object,
								cb_onAuthorizeInd,
								env->NewStringUTF(g_remote_addr));
			}
			break;
			
		case EVENT_SIMAP_MMI_AUTH_REQ_TIMEOUT_RFCLOSE_IND:
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_AUTH_REQ_TIMEOUT_RFCLOSE_IND");
			env->CallVoidMethod(g_service_object,
								cb_onDisconnectedInd);
			break;
				
	
		case EVENT_SIMAP_MMI_BT_RESET_IND:
			SIMAP_LOG("bt_simap_jni_cb: EVENT_SIMAP_MMI_BT_RESET_IND");

			bt_simap_proc_bt_reset_ind(env);
			
			SIMAP_LOG("bt_simap_jni_cb: call cb_onBtResetInd");
			
			if (NULL != g_service_object) {
			        SIMAP_LOG("free g_service_object by EVENT_SIMAP_MMI_BT_RESET_IND");
				env->CallVoidMethod(g_service_object,
								cb_onBtResetInd);
			
				env->DeleteGlobalRef(g_service_object);
				g_service_object = NULL;
			}

			break;
	
		default:
			SIMAP_LOG("bt_simap_jni_cb: event: %d", event);
			SIMAP_LOG("event=%s", get_event_string(event));
			if ((event >= EVENT_SIM_MMI_CONNECT_REQ) &&(event <= EVENT_SIM_MMI_RESET_REQ))
			{
			    char *data = NULL;
				int data_len = 0;

				if (event == EVENT_SIM_MMI_APDU_REQ)
				{
					data = (char *)(((bt_sim_apdu_req_struct *)parms)->apdu_req);
					data_len = ((bt_sim_apdu_req_struct *)parms)->apdu_req_len;
				}
				
				bt_simap_process_sim_event(event, data, data_len);
			}
			
			break;
	}

}


static JNINativeMethod sMethods[] = {
	 /* name, signature, funcPtr */
	{"initServiceNative", "()Z", (void *)initServiceNative},
	{"cleanupNativeDataNative", "()V", (void *)cleanupNativeDataNative},
    {"enableNative", "()Z", (void *)enableNative},
    {"disableNative", "()V", (void *)disableNative},
	{"disconnectNative", "(I)V", (void *)disconnectNative},
	{"authorizeRspNative", "(Z)V", (void *)authorizeRspNative},
	{"selectSIMNative", "(I)Z", (void *)selectSIMNative},
 	{"startListenNative", "()Z", (void *)startListenNative},
    {"stopListenNative", "()V", (void *)stopListenNative},
 	{"prepareListentoSocketNative", "()Z", (void*) prepareListentoSocketNative},
    {"sendSIMUnaccessibleIndNative", "()V", (void *)sendSIMUnaccessibleIndNative}
    
};


static int registerNativeMethods(JNIEnv* env, const char* className,
	JNINativeMethod* methods, int numMethods) 
{
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
	if (!registerNativeMethods(env, "com/mediatek/bluetooth/simap/BluetoothSimapService",
		sMethods, sizeof(sMethods) / sizeof(sMethods[0])))
	{
		ALOGE("[SIMAP_JNI] registerNativeMethods failed");
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
	
	ALOGI("[SIMAP_JNI]JNI_OnLoad [+]");
	
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	ALOGE("ERROR: GetEnv failed\n");
	goto bail;
	}
	assert(env != null);

	if (!registerNatives(env)) {
	ALOGE("ERROR: BluetoothSimapService failed to register natives\n");
	goto bail;
	}

	result = JNI_VERSION_1_4;
	g_jvm = vm;

bail:

	ALOGI("[SIMAP_JNI]JNI_OnLoad [-]");
	return result;;
}



