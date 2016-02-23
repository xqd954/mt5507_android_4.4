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


/* Inline function for getting native data address from a java instance field. */
static inline native_data_t* get_native_data(JNIEnv* env, jobject object) {
    return (native_data_t*) (env->GetIntField(object, m_native_data));
}

void bt_hid_jni_cb(U32 hid_ui_event, BD_ADDR* addr) {
	char		temp[18];
	jstring	device_addr=NULL;
	jint		status;
       jint		servObj = 0;
	bool 		isAttached = false;
	status=hid_ui_event;

	HID_LOG("get JNI environment");
	if(g_jvm->GetEnv((void **)&g_jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		HID_LOG("failed to get JNI environment, assuming native thread");
		if(g_jvm->AttachCurrentThread(&g_jni_env, NULL) < 0)
		{
			HID_ERR(" failed to attach current thread");
		}
		else
		{
			isAttached = true;
		}
	}

	switch(hid_ui_event)
	{
		case MBTEVT_HID_HOST_ENABLE_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_ENABLE_SUCCESS");
			break;
		case MBTEVT_HID_HOST_ENABLE_FAIL:
			HID_LOG("MBTEVT_HID_HOST_ENABLE_FAIL");
			break;
		case MBTEVT_HID_HOST_DISABLE_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_DISABLE_SUCCESS");
			break;
		case MBTEVT_HID_HOST_DISABLE_FAIL:
			HID_LOG("MBTEVT_HID_HOST_DISABLE_FAIL");
			break;
		case MBTEVT_HID_HOST_CONNECT_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_CONNECT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_CONNECT_FAIL:
			HID_LOG("MBTEVT_HID_HOST_CONNECT_FAIL");
			break;
		case MBTEVT_HID_HOST_DISCONNECT_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_DISCONNECT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_DISCONNECT_FAIL:
			HID_LOG("MBTEVT_HID_HOST_DISCONNECT_FAIL");
			break;
		case MBTEVT_HID_HOST_GET_DESC_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_GET_DESC_SUCCESS");
			break;
		case MBTEVT_HID_HOST_GET_DESC_FAIL:
			HID_LOG("MBTEVT_HID_HOST_GET_DESC_FAIL");
			break;
		case MBTEVT_HID_HOST_RECEIVE_UNPLUG:
			HID_LOG("MBTEVT_HID_HOST_RECEIVE_UNPLUG");
			break;
		case MBTEVT_HID_HOST_SEND_CONTROL_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_SEND_CONTROL_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SEND_CONTROL_FAIL:
			HID_LOG("MBTEVT_HID_HOST_SEND_CONTROL_FAIL");
			break;
		case MBTEVT_HID_HOST_SEND_REPORT_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_SEND_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SEND_REPORT_FAIL:
			HID_LOG("MBTEVT_HID_HOST_SEND_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_SET_REPORT_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_SET_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SET_REPORT_FAIL:
			HID_LOG("MBTEVT_HID_HOST_SET_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_GET_REPORT_SUCCESS:
			HID_LOG("MBTEVT_HID_HOST_GET_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_GET_REPORT_FAIL:
			HID_LOG("MBTEVT_HID_HOST_GET_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_RECEIVE_AUTHORIZE:
			HID_LOG("MBTEVT_HID_HOST_RECEIVE_AUTHORIZE");
			break;
		default:
			HID_LOG("Unknown HID event");
			break;			
	}	

	if(addr!=0)
	{
		HID_LOG("device_addr=0x%02X%02X%02X%02X%02X%02X",				
					 addr->addr[0],	 
					 addr->addr[1],	 
					 addr->addr[2],	 
					 addr->addr[3],	 
					 addr->addr[4],	 
					 addr->addr[5]);
		sprintf(temp,"%02X:%02X:%02X:%02X:%02X:%02X",
					 addr->addr[5],	 
					 addr->addr[4],	 
					 addr->addr[3],	 
					 addr->addr[2],	 
					 addr->addr[1],	 
					 addr->addr[0]);
		device_addr = g_jni_env->NewStringUTF((const char*) temp);
		HID_LOG("temp=%s",temp);
	}

	g_jni_env->CallVoidMethod(g_object, sendServiceMsg,  status,device_addr);	
       if(device_addr != NULL){
	    g_jni_env->DeleteLocalRef(device_addr);
       }

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

    /* Create the socket */

    nat->servsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_HID, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    if (nat->servsock < 0) {
	HID_ERR("Create receiving socket failed : %s, errno=%d", strerror(errno), errno);
	goto init_fail;
    }
    
    

    if(bt_hid_init_socket()==0){
	HID_ERR("Create sending socket failed");
	goto init_fail;
    }
    HID_LOG("initServiceNative Succeeded");
    return JNI_TRUE;

init_fail:
    if (nat) {
	free(nat);
	env->SetIntField(object, m_native_data, 0);
    }
    HID_ERR("Failed");
    return JNI_FALSE;
}

static void cleanServiceNative(JNIEnv* env, jobject object) {
    native_data_t* nat = get_native_data(env, object);
    if (nat) {
	free(nat);
	env->SetIntField(object, m_native_data, 0);
    }

    env->DeleteGlobalRef(g_object);

    if (g_service_object) {
	free(g_service_object);
    }
}

static void forceClearServerNative(JNIEnv* env, jobject object) {
    HID_LOG("Force clear server.");
    btmtk_deinit_hid_host();
}

/*
 * Keeps listening to the socket for incoming ilm 
 */
static jboolean listentoSocketNative(JNIEnv* env, jobject object) {
    HID_LOG("listentoSocketNative");
    int res = 1;
    int sockfd;
    unsigned int msg_id;    

    fd_set readfs;
    ilm_struct ilm;
    native_data_t* nat = NULL;

    nat = get_native_data(env, object);
    if (nat == NULL) {
	HID_ERR("NULL nat");
	return JNI_FALSE;
    }

    /* 
     * NOTE: Only the socket-listener thread can set g_int_env!
     *       Or there might be VM aborting due to referencing env of
     *       different thread.
     */
    g_jni_env = env;
    if (g_service_object == NULL) {
	HID_ERR("g_service_object is NULL");
	return JNI_FALSE;
    }

    sockfd = nat->servsock;
    if (sockfd < 0) {
	HID_ERR("Zero sockfd");
	return JNI_FALSE;
    }
    HID_LOG("nat->servsock=%d.",nat->servsock);

    FD_ZERO(&readfs);
    FD_SET(sockfd, &readfs);
    res = select(sockfd+1, &readfs, NULL, NULL, NULL);
    HID_LOG("select res=%d.",res);

    if (res > 0) {
	res = recvfrom(sockfd, (void*) &ilm, sizeof(ilm_struct), 0, NULL, NULL);
         HID_LOG("recvfrom res=%d.",res);
  	if(res == 0)
		return JNI_TRUE;
    } else if (res < 0) {
	HID_ERR("select() failed:%s(%d)",strerror(errno),errno);
	if(errno != EINTR)
		return JNI_FALSE;
	else
		return JNI_TRUE;
    } else {
	HID_ERR("Waiting ILM timeout");
	return JNI_FALSE;
    }    
	
    msg_id = ilm.msg_id;

    if (msg_id <= MSG_ID_BT_HID_GROUP_END && msg_id >= MSG_ID_BT_HID_GROUP_START) {
	btmtk_hid_handle_message(NULL, &ilm);
    } else {
	HID_ERR("Invalid ILM, ID: %d", msg_id);
    }

    return JNI_TRUE;
}

static void wakeupListenerNative(JNIEnv* env, jobject object)
{
	HID_LOG("wakeupListenerNative");
	native_data_t *nat = get_native_data(env, object);
	int event=1;

		if(nat)
		{
		    sockaddr_un name;
		    socklen_t   namelen;
		    int ret;
		    
		    namelen = sizeof(sockaddr_un);
		    ret = getsockname(nat->servsock, (struct sockaddr*)&name, &namelen);
		    HID_LOG("nat->hfgservsock : name=%s, size=%d", &name.sun_path[1], namelen);
		    if(ret == 0 && namelen > 0)
		    {
		        //ret = sendto(nat->servsock, &event, sizeof(int), 0, (struct sockaddr*)&name, namelen);
		        ret = sendto(nat->servsock, 0, 0, 0, (struct sockaddr*)&name, namelen);
		        if(ret < 0)
		        {
		            HID_ERR("[ERR] sendto servsock itself failed : %s, errno=%d", strerror(errno), errno);
		        }
		    }
		    else
		    {
		        HID_ERR("[ERR] getsockname failed : %s, errno=%d", strerror(errno), errno);
		    }
		}
		else
		{
		    HID_ERR("[ERR] nat is null");
		}
}


static void stopListentoSocketNative(JNIEnv* env, jobject object) {
    native_data_t* nat = NULL;

    nat = get_native_data(env, object);
    if (nat == NULL) {
	HID_ERR("NULL nat");
	return;
    }

    if (nat->servsock < 0) {
	HID_ERR("Zero sockfd");
	return;
    }
	
    close(nat->servsock);
	nat->servsock = -1;
	bt_hid_close_socket();
    HID_LOG("Job done. Close the socket.");
    return;
}

void CONVERT_STRING2BDADDR(const char *source, BD_ADDR *dest)
{    

    HID_LOG("[API] CONVERT_STRING2BDADDR  start");    
    //unsigned char addr[6];    
    int i;     
    char *ptr = (char*)source;    
    for(i = 6;i > 0;)    
    {        
        dest->addr[--i] = strtoul(ptr, &ptr, 16);        
        ptr++;    
    }    
    HID_LOG("[API] CONVERT_STRING2BDADDR : %s==>0x%02X%02X%02X%02X%02X%02X",    
        source,     
        dest->addr[0],    
        dest->addr[1],    
        dest->addr[2],    
        dest->addr[3],    
        dest->addr[4],    
        dest->addr[5]);
}


static void serverAuthorizeReqNative(JNIEnv* env, jobject object,  jstring BT_Addr,jboolean result) {
	HID_LOG("serverAuthorizeReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);
	btmtk_hidh_authorize_rsp_ext(bdaddr,result);
}

static void serverActivateReqNative(JNIEnv* env, jobject object) {
	HID_LOG("serverActivateReqNative");
	U8	msg_c = BT_HIDH_ACTIVATE_REQ_EXT;
	bt_hidh_req_ext_struct  msg;
	msg.command = BT_HIDH_ACTIVATE_REQ_EXT;	
	btmtk_hidh_set_callback(bt_hid_jni_cb);
	btmtk_hidh_handle_req_ext(&msg);
}

static void serverDeactivateReqNative(JNIEnv* env, jobject object) {
	HID_LOG("serverDeactivateReqNative");
	bt_hidh_req_ext_struct  msg;
	msg.command = BT_HIDH_DEACTIVATE_REQ_EXT;
	btmtk_hidh_handle_req_ext(&msg);
}


static void serverConnectReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverConnectReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);
	
	bt_hidh_req_ext_struct  msg;
	memset(&msg, 0, sizeof(bt_hidh_req_ext_struct));
	/*msg.addr.addr[0] = bdaddr.addr[0];
	msg.addr.addr[1] = bdaddr.addr[1];    
	msg.addr.addr[2] = bdaddr.addr[2];    
	msg.addr.addr[3] = bdaddr.addr[3];   
	msg.addr.addr[4] = bdaddr.addr[4];      
	msg.addr.addr[5] = bdaddr.addr[5];*/
	memcpy(msg.addr.addr,bdaddr.addr,sizeof(BD_ADDR));
	msg.command = BT_HIDH_CONNECT_REQ_EXT;
	//msg.addr = bdaddr;
	HID_LOG("serverConnectReqNative::msg.addr=0x%02X%02X%02X%02X%02X%02X",              
		        msg.addr.addr[0],    
		        msg.addr.addr[1],    
		        msg.addr.addr[2],    
		        msg.addr.addr[3],    
		        msg.addr.addr[4],    
		        msg.addr.addr[5]);
	btmtk_hidh_handle_req_ext(&msg);
}

static  void serverDisconnectReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverDisconnectReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	bt_hidh_req_ext_struct msg;
	msg.command = BT_HIDH_DISCONNECT_REQ_EXT;
	memcpy(msg.addr.addr,bdaddr.addr,sizeof(BD_ADDR));
	btmtk_hidh_handle_req_ext(&msg);
}

static  void serverUnplugReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverUnplugReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	btmtk_hidh_send_control_req(bdaddr, hidd_ctrl_op_virtual_cable_unplug);
}

static  void serverSendReportReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jstring report) {
	HID_LOG("serverSendReportReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);
	const char *c_report = env->GetStringUTFChars(report, NULL);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	U8 	 *rpt_data;
	U16 	rpt_size =strlen(c_report);
	rpt_data = (U8*)malloc(rpt_size* sizeof(U8));
	memcpy(rpt_data, c_report, rpt_size* sizeof(U8));
	HID_LOG("rpt_data = %d %d\n",*rpt_data, *(rpt_data+1));
	btmtk_hidh_send_output_report(bdaddr, hidd_report_output, rpt_data, rpt_size);
	free(rpt_data);
}

static  void serverSetReportReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jint reportType, jstring report) {
	HID_LOG("serverSetReportReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);
	const char *c_report = env->GetStringUTFChars(report, NULL);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);
	U8 	 *rpt_data;
	U16 	rpt_size = strlen(c_report);
	rpt_data = (U8*)malloc(rpt_size* sizeof(U8));
	memcpy(rpt_data, c_report, rpt_size* sizeof(U8));
	HID_LOG("rpt_data = %d %d\n",*rpt_data, *(rpt_data+1));

	btmtk_hidh_set_report_req(bdaddr, (bt_hidd_report_type_enum)reportType, rpt_data, rpt_size);
	free(rpt_data);
}

static  void serverGetReportReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jint reportType, jint reportId) {
	HID_LOG("serverGetReportReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	btmtk_hidh_get_report_req(bdaddr, reportType, reportId);
}

static  void serverSetProtocolReqNative(JNIEnv* env, jobject object, jstring BT_Addr, jint protocolMode) {
	HID_LOG("serverSetProtocolReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	btmtk_hidh_set_protocol_req(bdaddr,protocolMode);
}

static  void serverGetProtocolReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverGetProtocolReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	btmtk_hidh_get_protocol_req(bdaddr);
}

static  void serverSetIdleReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverSetIdleReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	btmtk_hidh_set_idle_rate_req(bdaddr, 0);
}

static  void serverGetIdleReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	HID_LOG("serverGetIdleReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	HID_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);

	btmtk_hidh_get_idle_rate_req(bdaddr);
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

