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
#include <fcntl.h>

extern "C"
{

#include "bt_mmi.h"
#include "bt_struct.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_pan_struct.h"
#include "bttypes.h"
#include "bt_pan_api.h"
#include "bt_pan_hdl.h"

}


typedef struct {
    JNIEnv* env;
    int apisock;
    int servsock;
    U32 con_id;
} native_data_t;

static JavaVM *g_jvm = NULL;

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


static jfieldID m_native_data;

static JNIEnv* g_jni_env;
static jobject g_service_object;

/* Inline function for getting native data address from a java instance field. */
static inline native_data_t* get_native_data(JNIEnv* env, jobject object) {
    return (native_data_t*) (env->GetIntField(object, m_native_data));
}

void bt_pan_jni_cb(void* handle, BT_CALLBACK_EVENT pan_ui_event, void *msg, unsigned short datasize) {
	char		temp[18];
	jstring	device_addr=NULL;
	jint		status;
	jint		service;
	jboolean	result;
	bool 		isAttached = false;
	status = pan_ui_event;

	PAN_LOG("get JNI environment");
	memset(temp, 0, sizeof(temp));    
	if(g_jvm->GetEnv((void **)&g_jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		PAN_LOG("failed to get JNI environment, assuming native thread");
		if(g_jvm->AttachCurrentThread(&g_jni_env, NULL) < 0)
		{
			PAN_ERR(" failed to attach current thread");
		}
		else
		{
			isAttached = true;
		}
	}

	switch(status)
	{
		case EVENT_PAN_MMI_ACTIVATE_CNF:
			{
				PAN_LOG("EVENT_PAN_MMI_ACTIVATE_CNF");
				bt_pan_activate_cnf_struct *msg_p = (bt_pan_activate_cnf_struct *)msg;
				result = msg_p->result;
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_activate_cnf,  result);				
			}
			break;
		case EVENT_PAN_MMI_DEACTIVATE_CNF:
			{
				PAN_LOG("EVENT_PAN_MMI_DEACTIVATE_CNF");			
				bt_pan_deactivate_cnf_struct *msg_p = (bt_pan_deactivate_cnf_struct *)msg;
				result = msg_p->result;
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_deactivate_cnf,  result);				
			}
			break;
		case EVENT_PAN_MMI_CONNECTION_AUTHORIZE_IND:
			{
				PAN_LOG("EVENT_PAN_MMI_CONNECTION_AUTHORIZE_IND");
				bt_pan_connection_authorize_ind_struct *msg_p = (bt_pan_connection_authorize_ind_struct *)msg;
				service = msg_p->service;
				if(msg_p->bt_addr.addr != NULL)
				{
					sprintf(temp,"%02X:%02X:%02X:%02X:%02X:%02X",
						 msg_p->bt_addr.addr[0],	 
						 msg_p->bt_addr.addr[1],	 
						 msg_p->bt_addr.addr[2],	 
						 msg_p->bt_addr.addr[3],	 
						 msg_p->bt_addr.addr[4],	 
						 msg_p->bt_addr.addr[5]);
					device_addr = g_jni_env->NewStringUTF((const char*) temp);
				}
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_connection_authorize_ind,  service,device_addr);				
			}
			break;
		case EVENT_PAN_MMI_CONNECT_IND:
			{
				PAN_LOG("EVENT_PAN_MMI_CONNECT_IND");
				bt_pan_connect_ind_struct *msg_p = (bt_pan_connect_ind_struct *)msg;
				service = msg_p->service;
				if(msg_p->bt_addr.addr != NULL)
				{
					sprintf(temp,"%02X:%02X:%02X:%02X:%02X:%02X",
						 msg_p->bt_addr.addr[0],	 
						 msg_p->bt_addr.addr[1],	 
						 msg_p->bt_addr.addr[2],	 
						 msg_p->bt_addr.addr[3],	 
						 msg_p->bt_addr.addr[4],	 
						 msg_p->bt_addr.addr[5]);
					device_addr = g_jni_env->NewStringUTF((const char*) temp);
				}
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_connect_ind,  service,device_addr,msg_p->unit);				
			}
			break;
		case EVENT_PAN_MMI_CONNECT_CNF:
			{
				PAN_LOG("EVENT_PAN_MMI_CONNECT_CNF");
				bt_pan_connect_cnf_struct *msg_p = (bt_pan_connect_cnf_struct *)msg;
				result = msg_p->result;
				if(msg_p->bt_addr.addr != NULL)
				{
					sprintf(temp,"%02X:%02X:%02X:%02X:%02X:%02X",
						 msg_p->bt_addr.addr[0],	 
						 msg_p->bt_addr.addr[1],	 
						 msg_p->bt_addr.addr[2],	 
						 msg_p->bt_addr.addr[3],	 
						 msg_p->bt_addr.addr[4],	 
						 msg_p->bt_addr.addr[5]);
					device_addr = g_jni_env->NewStringUTF((const char*) temp);
				}
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_connect_cnf,  result,device_addr,msg_p->unit);				
			}
			break;
		case EVENT_PAN_MMI_DISCONNECT_IND:
			{
				PAN_LOG("EVENT_PAN_MMI_DISCONNECT_IND");
				bt_pan_disconnect_ind_struct *msg_p = (bt_pan_disconnect_ind_struct *)msg;
				if(msg_p->bt_addr.addr != NULL)
				{
					sprintf(temp,"%02X:%02X:%02X:%02X:%02X:%02X",
						 msg_p->bt_addr.addr[0],	 
						 msg_p->bt_addr.addr[1],	 
						 msg_p->bt_addr.addr[2],	 
						 msg_p->bt_addr.addr[3],	 
						 msg_p->bt_addr.addr[4],	 
						 msg_p->bt_addr.addr[5]);
					device_addr = g_jni_env->NewStringUTF((const char*) temp);
				}
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_disconnect_ind, device_addr);				
			}
			break;
		case EVENT_PAN_MMI_DISCONNECT_CNF:
			{
				PAN_LOG("EVENT_PAN_MMI_DISCONNECT_CNF");
				bt_pan_disconnect_cnf_struct *msg_p = (bt_pan_disconnect_cnf_struct *)msg;
				result = msg_p->result;
				if(msg_p->bt_addr.addr != NULL)
				{
					sprintf(temp,"%02X:%02X:%02X:%02X:%02X:%02X",
						 msg_p->bt_addr.addr[0],	 
						 msg_p->bt_addr.addr[1],	 
						 msg_p->bt_addr.addr[2],	 
						 msg_p->bt_addr.addr[3],	 
						 msg_p->bt_addr.addr[4],	 
						 msg_p->bt_addr.addr[5]);
					device_addr = g_jni_env->NewStringUTF((const char*) temp);
				}
				g_jni_env->CallVoidMethod(g_service_object, callback_pan_handle_disconnect_cnf,  result,device_addr);				
			}
			break;
		default:
			PAN_LOG("Unknown PAN event");
			break;			
	}	

	g_jni_env->DeleteLocalRef(device_addr);

	// release resource
	if(isAttached)
	{
		g_jvm->DetachCurrentThread();
	}
}

U8  bt_pan_init_socket(native_data_t* nat)
{
    char buff[20];
    int err, sock_fl;
    static struct sockaddr_un g_int_adp_sockaddr;
    static socklen_t g_int_adp_sockaddr_len;
	
     g_int_adp_sockaddr.sun_family = AF_UNIX;
    strcpy(g_int_adp_sockaddr.sun_path, BT_SOCK_NAME_INT_ADP);
    g_int_adp_sockaddr_len = (offsetof(struct sockaddr_un, sun_path) + strlen(g_int_adp_sockaddr.sun_path) + 1);

    nat->apisock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (nat->apisock < 0) {
	PAN_ERR("Create server socket failed.");
	return 0;
    }
    //if (0 > bind(nat->apisock, (struct sockaddr*) &g_int_adp_sockaddr, g_int_adp_sockaddr_len)) {
//	PAN_ERR("Bind server socket failed.");
//	return 0;
  //  }

    /* Get current socket flags */
    sock_fl = fcntl(nat->apisock, F_GETFL);
    if (sock_fl == -1) {
	err = errno;
	PAN_ERR("Get socket flags failed. Reason: [%d]%s", err, strerror(err));
	return 0;
    }

    /* Set ftp socket as non-block */
    if (fcntl(nat->apisock, F_SETFL, sock_fl | O_NONBLOCK) == -1) {
	err = errno;
	PAN_ERR("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
	return 0;
    }

    /* Read data from socket till empty */
    while(0 < recvfrom(nat->apisock, (void*) buff, sizeof(buff), 0, NULL, NULL)) {
	PAN_LOG("Clearing socket...");
    }
    PAN_LOG("[BT_PAN]Socket clear.");

    /* Set back original flags */
    if (fcntl(nat->apisock, F_SETFL, sock_fl) == -1) {
	err = errno;
	PAN_ERR("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
	return 0;
    }

    if (connect(nat->apisock, (struct sockaddr *)&g_int_adp_sockaddr, g_int_adp_sockaddr_len) < 0)
    {
        err = errno;
        PAN_ERR("[SPP][ERR] connect to /data/bt.int.adp failed: %s(%d)", strerror(err), err);
        return 0;
    }
    return 1;
}

 U8 bt_pan_close_socket(native_data_t* nat)
 {
	if(nat->apisock<0)
	{
		PAN_ERR("Invalid g_ext_pan_sockfd");
		return 0;
	}
	close(nat->apisock);
	nat->apisock=-1;
	PAN_LOG("Close g_ext_pan_sockfd OK");
	return 1;
 }


static jboolean initServiceNative(JNIEnv* env, jobject object) {

    native_data_t* nat = NULL;

    PAN_LOG("initServiceNative Enter");

   // jvm
   env->GetJavaVM(&g_jvm);

    /* Find class structure */
    jclass clazz = env->FindClass("com/mediatek/bluetooth/pan/BluetoothPanService");
    if (clazz == NULL) {
	PAN_ERR("FindClass failed");
	goto init_fail;
    }

    g_service_object = env->NewGlobalRef(object);

    /* Retrieve the method id and field id */
	callback_pan_handle_activate_cnf = env->GetMethodID(clazz, "callback_pan_handle_activate_cnf", "(Z)V");
	callback_pan_handle_deactivate_cnf = env->GetMethodID(clazz, "callback_pan_handle_deactivate_cnf", "(Z)V");
	callback_pan_handle_connection_authorize_ind = env->GetMethodID(clazz, "callback_pan_handle_connection_authorize_ind", "(ILjava/lang/String;)V");
	callback_pan_handle_connect_ind = env->GetMethodID(clazz, "callback_pan_handle_connect_ind", "(ILjava/lang/String;I)V");
	callback_pan_handle_connect_cnf = env->GetMethodID(clazz, "callback_pan_handle_connect_cnf", "(ZLjava/lang/String;I)V");
	callback_pan_handle_disconnect_ind = env->GetMethodID(clazz, "callback_pan_handle_disconnect_ind", "(Ljava/lang/String;)V");
	callback_pan_handle_disconnect_cnf = env->GetMethodID(clazz, "callback_pan_handle_disconnect_cnf", "(ZLjava/lang/String;)V");

    m_native_data = env->GetFieldID(clazz, "mNativeData", "I");

    /* Allocate nat and store its address in object field */
    nat = (native_data_t*) calloc(1, sizeof(native_data_t));
    if (nat == NULL) {
	PAN_ERR("Allocate nat: out of memory");
	goto init_fail;
    }

    memset(nat, 0, sizeof(native_data_t));
    env->SetIntField(object, m_native_data, (jint) nat);

    /* Create the socket */

    nat->servsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_PAN, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    if (nat->servsock < 0) {
	PAN_ERR("Create receiving socket failed : %s, errno=%d", strerror(errno), errno);
	goto init_fail;
    }
    
    

    if(bt_pan_init_socket(nat)==0){
	PAN_ERR("Create sending socket failed");
	goto init_fail;
    }
    PAN_LOG("initServiceNative Succeeded");
    return JNI_TRUE;

init_fail:
    if (nat) {
			free(nat);
			env->SetIntField(object, m_native_data, 0);
    }
    PAN_ERR("Failed");
    return JNI_FALSE;
}

static void cleanServiceNative(JNIEnv* env, jobject object) {
    native_data_t* nat = get_native_data(env, object);
    if (nat) {
	free(nat);
    }
    env->DeleteGlobalRef(g_service_object);
}

static void forceClearServerNative(JNIEnv* env, jobject object) {
    PAN_LOG("Force clear server.");
    btmtk_deinit_pan();
}

/*
 * Keeps listening to the socket for incoming ilm 
 */
static jboolean listentoSocketNative(JNIEnv* env, jobject object) {
    PAN_LOG("listentoSocketNative");
    int res = 1;
    int sockfd;
    unsigned int msg_id;    

    fd_set readfs;
    ilm_struct ilm;
    native_data_t* nat = NULL;

    nat = get_native_data(env, object);
    if (nat == NULL) {
	PAN_ERR("NULL nat");
	return JNI_FALSE;
    }

    /* 
     * NOTE: Only the socket-listener thread can set g_int_env!
     *       Or there might be VM aborting due to referencing env of
     *       different thread.
     */
    g_jni_env = env;

    sockfd = nat->servsock;
    if (sockfd < 0) {
	PAN_ERR("Zero sockfd");
	return JNI_FALSE;
    }
    PAN_LOG("nat->servsock=%d.",nat->servsock);

    FD_ZERO(&readfs);
    FD_SET(sockfd, &readfs);
    res = select(sockfd+1, &readfs, NULL, NULL, NULL);
    PAN_LOG("select res=%d.",res);

    if (res > 0) {
	res = recvfrom(sockfd, (void*) &ilm, sizeof(ilm_struct), 0, NULL, NULL);
         PAN_LOG("recvfrom res=%d.",res);
  	if(res == 0)
		return JNI_TRUE;
    } else if (res < 0) {
  	if(errno == EINTR) {
  		PAN_LOG("select system interrupt");
  		return JNI_TRUE;
	} else {
  		PAN_ERR("select() failed");
  		return JNI_FALSE;
	}
    } else {
	PAN_ERR("Waiting ILM timeout");
	return JNI_FALSE;
    }    
	
    msg_id = ilm.msg_id;

    if (msg_id <= MSG_ID_BT_PAN_GROUP_END && msg_id >= MSG_ID_BT_PAN_GROUP_START) {
	btmtk_pan_handle_message(&ilm);
    } else {
	PAN_ERR("Invalid ILM, ID: %d", msg_id);
    }

    return JNI_TRUE;
}

static void wakeupListenerNative(JNIEnv* env, jobject object)
{
	PAN_LOG("wakeupListenerNative");
	native_data_t *nat = get_native_data(env, object);
	int event=1;

		if(nat)
		{
		    sockaddr_un name;
		    socklen_t   namelen;
		    int ret;
		    
		    namelen = sizeof(sockaddr_un);
		    ret = getsockname(nat->servsock, (struct sockaddr*)&name, &namelen);
		    PAN_LOG("nat->servsock : name=%s, size=%d", &name.sun_path[1], namelen);
		    if(ret == 0 && namelen > 0)
		    {
		        //ret = sendto(nat->servsock, &event, sizeof(int), 0, (struct sockaddr*)&name, namelen);
		        ret = sendto(nat->servsock, 0, 0, 0, (struct sockaddr*)&name, namelen);
		        if(ret < 0)
		        {
		            PAN_ERR("[ERR] sendto servsock itself failed : %s, errno=%d", strerror(errno), errno);
		        }
		    }
		    else
		    {
		        PAN_ERR("[ERR] getsockname failed : %s, errno=%d", strerror(errno), errno);
		    }
		}
		else
		{
		    PAN_ERR("[ERR] nat is null");
		}
}


static void stopListentoSocketNative(JNIEnv* env, jobject object) {
    native_data_t* nat = NULL;

    nat = get_native_data(env, object);
    if (nat == NULL) {
	PAN_ERR("NULL nat");
	return;
    }

    if (nat->servsock < 0) {
	PAN_ERR("Zero sockfd");
	return;
    }
	
    close(nat->servsock);
    nat->servsock = -1;
    bt_pan_close_socket(nat);
    PAN_LOG("Job done. Close the socket.");
    return;
}

void CONVERT_STRING2BDADDR(const char *source, BD_ADDR *dest)
{    

    PAN_LOG("[API] CONVERT_STRING2BDADDR  start");    
    //unsigned char addr[6];    
    int i;     
    char *ptr = (char*)source;    
    for(i = 0;i < 6;)    
    {        
        dest->addr[i++] = strtoul(ptr, &ptr, 16);        
        ptr++;    
    }    
    PAN_LOG("[API] CONVERT_STRING2BDADDR : %s==>0x%02X%02X%02X%02X%02X%02X",    
        source,     
        dest->addr[0],    
        dest->addr[1],    
        dest->addr[2],    
        dest->addr[3],    
        dest->addr[4],    
        dest->addr[5]);
}

static void serverAuthorizeRspNative(JNIEnv* env, jobject object, jstring BT_Addr,jboolean result) {
	PAN_LOG("serverAuthorizeRspNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	PAN_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);
	btmtk_pan_connection_authorize_rsp((U8*)bdaddr.addr, result);
}

static void serverActivateReqNative(JNIEnv* env, jobject object) {
	PAN_LOG("serverActivateReqNative");
	native_data_t* nat = NULL;

    	nat = get_native_data(env, object);
   	 if (nat == NULL) {
		PAN_ERR("NULL nat");
		return;
    	}
	btmtk_pan_register_mmi_callback(bt_pan_jni_cb);
	btmtk_pan_set_sockfd(nat->apisock);
	btmtk_pan_activate_req();
}

static void serverDeactivateReqNative(JNIEnv* env, jobject object) {
	PAN_LOG("serverDeactivateReqNative");
	btmtk_pan_deactivate_req();
}


static void serverConnectReqNative(JNIEnv* env, jobject object,jint service, jstring BT_Addr) {
	PAN_LOG("serverConnectReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	PAN_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);
	btmtk_pan_connect_req((bt_pan_service_enum)service, (U8*)bdaddr.addr);
}

static  void serverDisconnectReqNative(JNIEnv* env, jobject object, jstring BT_Addr) {
	PAN_LOG("serverDisconnectReqNative");
	BD_ADDR bdaddr;
	const char *c_bdaddr = env->GetStringUTFChars(BT_Addr, NULL);
	PAN_LOG("c_bdaddr: %s", c_bdaddr);

	CONVERT_STRING2BDADDR(c_bdaddr, &bdaddr);
	env->ReleaseStringUTFChars(BT_Addr, c_bdaddr);
	btmtk_pan_disconnect_req((U8*)bdaddr.addr);
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
	PAN_ERR("Native registration unable to find class '%s'\n", className);
	return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
	PAN_ERR("RegisterNatives failed for '%s'\n", className);
	return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env) {
    if (!registerNativeMethods(env, "com/mediatek/bluetooth/pan/BluetoothPanService",
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
	PAN_ERR("ERROR: GetEnv failed\n");
	goto bail;
    }
    assert(env != null);

    if (!registerNatives(env)) {
	PAN_ERR("ERROR: BluetoothPanService failed to register natives\n");
	goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;;
}

