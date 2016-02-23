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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*******************************************************************************
 *
 * Filename:
 * ---------
 * android_server_BluetoothDunService.cpp
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to provide jni layer api
 *
 * Author:
 * -------
 * Ting Zheng
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
	 
#define LOG_TAG "extdun_jni"
	 
#include <sys/socket.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>	 
//#include <cutils/properties.h>
#include <sys/wait.h>

//#include "../../../mediatek/frameworks-ext/base/core/jni/android_bluetooth_common.h"
#include "android_runtime/AndroidRuntime.h"
#include "JNIHelp.h"
#include "jni.h"
#include "utils/Log.h"
#include "utils/misc.h"
#include "cutils/sockets.h"
	 
 
#ifdef __BTMTK__
extern "C"
{
#include "bt_spp_api.h"
#include "bt_spp_hdl.h"
#include "bt_spp_ashm.h"
#include "bt_dun_at.h"
#include <sys/mman.h>   // for madvise(), mmap()
#include <cutils/ashmem.h>
#include "socket_local.h"
}


static jmethodID method_onDunEnableCnf;
static jmethodID method_onDunDisableCnf;
static jmethodID method_onDunStateChanged;
static jmethodID method_onDunConnectReq;
static jmethodID method_onDunDialupReq;

#define PHASE_NONE		0
#define PHASE_DIALING		1
#define PHASE_PPP		2

#define PPP_FLAG	0x7e	/* Flag Sequence */

/* argv parameters for ppp daemon */
/*  
    eg. pppd slient pppbt-setsockname <sock name> 
    pppbt-setipaddr <network device name> 
    ms-dns <dns1> ms-dns <dns2>
*/
#define PPP_SET_NAME				"pppd"
#define PPP_SET_MODE				"silent"
#define PPP_SET_SOCKNAME		"pppbt-setsockname"
#define PPP_SET_IPADDR		"pppbt-setipaddr"
#define PPP_SET_DNSADDR		"ms-dns"
#define PPP_PARAM_NUM		10	

#define DUN_MAX_OBJECT_PATH_SIZE 18
#define DUN_PLUGOUT_TIMEOUT		1000	/* timeout is in milliseconds */
#define DUN_DISABLE_TIMEOUT		100	/* timeout is in milliseconds */

#define ANDROID_EV_DUN_PPPD_START		MSG_ID_BT_CUSTOM_MSG_ID_BEGIN + 100
#define ANDROID_EV_DUN_PPPD_STOP			ANDROID_EV_DUN_PPPD_START + 1

#define FREEIF(p)   do { if(p) free(p); p = NULL; } while(0)

typedef struct _packet_t packet_t;

struct _packet_t{
    packet_t *next;	
    U16 len;
    U8 data[0];
};

typedef struct {
    JavaVM *vm;
    int envVer;
    jobject me;  // for callbacks to java
    bool bEnabled;

    pthread_mutex_t mutex;
    pthread_t sppThread;
    pthread_t dunPppThread;
    pthread_cond_t waitDisable;
    pthread_cond_t waitPlugout;

    pid_t pppdPid;
    pthread_t pppdWaitTermThread;

    int sppSockServer;    // for receiving indication and confirmation
    int sppSockClient;           // for sending request and response
    int dunPppSockServer;	// Server socket to accept incoming connection from ppp daemon
    int dunPppSockClient;	// Client socket to send or receive ppp protocol packet to or from ppp daemon

    int dunPort;
    int dunPhase;	// PHASE_NONE, PHASE_DIALING, PHASE_PPP
    U8 dunPath[DUN_MAX_OBJECT_PATH_SIZE];
    U8 *readbuff;
    U8 *writebuff;	

    packet_t *pendingPacket;
    bool bPutDone;
} native_data_t;


static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;

static native_data_t *nat = NULL;  // global native data

static void dun_convert_bdaddr2objpath(char *dest, bt_addr_struct *source);
static void dun_convert_objpath2bdaddr(const char *source, bt_addr_struct *dest);
static void dun_convert_String2ATResponse(const char *source, AtResponse *dest);
static void dun_cond_timedwait(pthread_mutex_t *pmutex, pthread_cond_t *pcond, U16 timeout);

static void dunEventCallbackHandler(void* handle, BT_CALLBACK_EVENT event, void *ptr, unsigned short datasize);
static bool dunStartPppTransmission(char *ipbaseaddr, char *dns1addr, char *dns2addr);
static void dunStopPppTransmission(void);
static void *dunPppTransmissionMain(void *arg);
static int dunLoopPacket(U8 *buffer, U16 len);
static void dunSendPacket(U8 *buffer, U16 len);
static int dunPppListen(void);
static void dunPppClose(void);

static jboolean sppStartEventLoop(JNIEnv* env, jobject object);
static void *sppEventLoopMain(void *arg);
static void sppWakeupEventLoopMain(void);
static jboolean sppRegisterSocket(JNIEnv* env, jobject object);
static void sppDeregisterSocket(void);

static jboolean initNative(JNIEnv *env, jobject object)
{
    ALOGV("[SPP][JNI] initNative");

    nat = (native_data_t *)calloc(1, sizeof(native_data_t));
    if (NULL == nat) {
        ALOGE("[SPP][ERR] %s: out of memory!", __FUNCTION__);
        return JNI_FALSE;
    }

    memset(nat,0,sizeof(native_data_t));
    nat->sppSockServer = -1;
    nat->sppSockClient = -1;	
    nat->dunPppSockServer = -1;
    nat->dunPppSockClient = -1;
    nat->pendingPacket = NULL;
    nat->bPutDone = TRUE;
    nat->pppdPid = -1;

    pthread_mutex_init(&nat->mutex, NULL);
    pthread_cond_init(&nat->waitDisable, NULL);
    pthread_cond_init(&nat->waitPlugout, NULL);

    env->GetJavaVM( &(nat->vm) );
    nat->envVer = env->GetVersion();
    nat->me = env->NewGlobalRef(object);

    /* register spp socket */
    if (!sppRegisterSocket(env, object))
    {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static void cleanupNative(JNIEnv *env, jobject object)
{
    ALOGI("[SPP][JNI] cleanupNative");

    if (nat->dunPhase != PHASE_NONE)
    {
        dun_cond_timedwait(&nat->mutex, &nat->waitPlugout, DUN_PLUGOUT_TIMEOUT);
        dunStopPppTransmission();
    }

    /*
    * A sent signal is lost if nobody's waiting for a condition when it's sent.
    * So if nat->bEnabled is false, it means that the waitDisable signal has
    * been sent before waiting so that it is unnecessary to wait any more.
    */
    if (nat->bEnabled)
    {
        dun_cond_timedwait(&nat->mutex, &nat->waitDisable, DUN_DISABLE_TIMEOUT);
    }
    
    sppWakeupEventLoopMain();

    /* deregister spp socket */
    if (nat->sppSockClient >= 0)
    {
        close(nat->sppSockClient);
        nat->sppSockClient = -1;		
    }

    /* Enter the critical section -- other threads are locked out */
    pthread_mutex_lock(&cs_mutex);

    pthread_mutex_destroy(&nat->mutex);
    pthread_cond_destroy(&nat->waitPlugout);
    pthread_cond_destroy(&nat->waitDisable);

    env->DeleteGlobalRef(nat->me);
    FREEIF(nat);
    
    pthread_mutex_unlock(&cs_mutex);
    /* Leave the critical section -- other threads can now pthread mutex lock */
}

static void dunEnableNative(JNIEnv *env, jobject object)
{
    ALOGI("[SPP][JNI] enable dun");
    /* send enable request to bt task */	
    btmtk_dun_register_mmi_callback_req((BTMTK_EventCallBack) dunEventCallbackHandler);
    btmtk_dun_activate_req();
}

static void dunDisableNative(JNIEnv *env, jobject object)
{
    btmtk_dun_deactivate_req(nat->dunPort);
}

static void dunDisconnectNative(JNIEnv *env, jobject object)
{
    btmtk_dun_disconnect_req(nat->dunPort);
}

static void dunConnectRspNative(JNIEnv *env, jobject object, jstring path, jboolean accept)
{
    char *c_path = (char *)env->GetStringUTFChars(path, NULL);
    bt_addr_struct addr;

    dun_convert_objpath2bdaddr(c_path, &addr);
    
    btmtk_dun_connect_ind_rsp(nat->dunPort, addr.lap, addr.uap, addr.nap, accept);
}

static void dunDialupRspNative(JNIEnv *env, jobject object, jstring response, jstring ipBase, jobjectArray dnsServers)
{
    AtResponse rsp_type;
    U16 rsp_len;
    char *c_response = (char *)env->GetStringUTFChars(response, NULL);

    dun_convert_String2ATResponse(c_response, &rsp_type);	
    
    btmtk_dun_at_encode(rsp_type, nat->writebuff, &rsp_len);
    
    ALOGI("at command response: %d, %d, %d, %d", nat->writebuff[0], nat->writebuff[1], nat->writebuff[2], nat->writebuff[3]);
    
    // notify bt task to read data from shared memory
    btmtk_spp_uart_put_bytes_req(nat->dunPort, NULL, rsp_len);
    
    if (rsp_type == AT_CONNECT)
    {
        char *c_ipbase = (char *)env->GetStringUTFChars(ipBase, NULL);
        char *c_dns1addr = (char *)env->GetStringUTFChars((jstring)env->GetObjectArrayElement(dnsServers, 0), NULL);
        char *c_dns2addr = (char *)env->GetStringUTFChars((jstring)env->GetObjectArrayElement(dnsServers, 1), NULL);

        if (dunStartPppTransmission(c_ipbase, c_dns1addr, c_dns2addr))
        {
            nat->dunPhase = PHASE_PPP;			   
            return;			   
        }
        btmtk_dun_disconnect_req(nat->dunPort);
    }
    else
    {
        U8 buff[6];

        // send "OK"
        btmtk_dun_at_encode(AT_OK, buff, &rsp_len);
        dunLoopPacket(buff, rsp_len);
        // disconnect?
    }
    
}

static void dun_cb_enable_cnf(JNIEnv *env, void *ptr)
{
    btmtk_dun_activate_cnf_struct *msg = (btmtk_dun_activate_cnf_struct *)ptr;

    ALOGI("[DUN][JNI] enable cnf result: %d", msg->result);

    if (msg->result)
    {
        nat->dunPort = msg->port;
        nat->bEnabled = true;
    }
    env->CallVoidMethod(nat->me, method_onDunEnableCnf, msg->result);
    	
}

static void dun_cb_disable_cnf(JNIEnv *env, void *ptr)
{
    btmtk_dun_deactivate_cnf_struct *msg = (btmtk_dun_deactivate_cnf_struct *)ptr;
    ALOGI("[DUN][JNI] disable cnf result: %d", msg->result);

    if (NULL == nat)
    {
        ALOGW("[DUN][JNI] dun_cb_disable_cnf, nat is NULL!");
        return;		
    }

    env->CallVoidMethod(nat->me, method_onDunDisableCnf, msg->result);

    pthread_cond_signal(&nat->waitDisable);
    nat->bEnabled = false;
}

static void dun_cb_connect_ind_req(JNIEnv *env, void *ptr)
{
    btmtk_dun_connect_ind_req_struct *msg = (btmtk_dun_connect_ind_req_struct *)ptr;
    char object_path[DUN_MAX_OBJECT_PATH_SIZE];
    bt_addr_struct addr;

    ALOGI("[DUN][JNI] connect ind req");

    addr.lap = msg->lap;
    addr.uap = msg->uap;
    addr.nap = msg->nap;
    dun_convert_bdaddr2objpath(object_path, &(addr));

    jstring path = env->NewStringUTF(object_path);
    env->CallVoidMethod(nat->me, method_onDunConnectReq,
    				 path);
    env->DeleteLocalRef(path);
}

/* DUN is connected */
static void dun_cb_connect_ind(JNIEnv *env, void *ptr)
{
    btmtk_dun_connect_ind_struct *msg = (btmtk_dun_connect_ind_struct *)ptr;
    char object_path[DUN_MAX_OBJECT_PATH_SIZE];
    bt_addr_struct addr;

    ALOGI("[DUN][JNI] connected");

    addr.lap = msg->lap;
    addr.uap = msg->uap;
    addr.nap = msg->nap;
    
    dun_convert_bdaddr2objpath(object_path, &(addr));

    memcpy(nat->dunPath, object_path, sizeof(nat->dunPath));
    
    jstring path = env->NewStringUTF(object_path);
    jstring state = env->NewStringUTF("connected");
    env->CallVoidMethod(nat->me, method_onDunStateChanged,
    				 path, state);
    env->DeleteLocalRef(path);
    env->DeleteLocalRef(state);

    nat->dunPhase = PHASE_DIALING;

}

/* DUN is disconnected */
static void dun_cb_disconnect_ind(JNIEnv *env, void *ptr)
{
    btmtk_dun_disconnect_ind_struct *msg = (btmtk_dun_disconnect_ind_struct *)ptr;

    ALOGI("[DUN][JNI] disconnected ind");
    
    if (!nat->bEnabled)
    {
        return;
    }
    
    jstring path = env->NewStringUTF((const char *)nat->dunPath);
    jstring state = env->NewStringUTF("disconnected");
    env->CallVoidMethod(nat->me, method_onDunStateChanged,
    				 path, state);
    env->DeleteLocalRef(path);
    env->DeleteLocalRef(state);

    if (nat->dunPhase == PHASE_PPP)
    {
        dunStopPppTransmission();
    }

    nat->dunPhase = PHASE_NONE;
}

static void dun_cb_disconnect_cnf(JNIEnv *env, void *ptr)
{
    btmtk_dun_disconnect_cnf_struct *msg = (btmtk_dun_disconnect_cnf_struct *)ptr;

    ALOGI("[DUN][JNI] disconnected cnf");

    if (!nat->bEnabled)
    {
        return;
    }

    jstring path = env->NewStringUTF((const char *)nat->dunPath);
    jstring state = env->NewStringUTF("disconnected");
    env->CallVoidMethod(nat->me, method_onDunStateChanged,
    				 path, state);
    env->DeleteLocalRef(path);
    env->DeleteLocalRef(state);

    if (nat->dunPhase == PHASE_PPP)
    {
        dunStopPppTransmission();
    }

    nat->dunPhase = PHASE_NONE;

}

static void dun_cb_data_available_ind(JNIEnv *env, void *ptr)
{
    btmtk_spp_uart_data_available_ind_struct *msg = (btmtk_spp_uart_data_available_ind_struct *)ptr;

    ALOGI("[DUN][JNI] available data ind, phase: %d, len: %d", nat->dunPhase, msg->length);

    if (nat->dunPhase == PHASE_DIALING)
    {
        AtResponse rsp_type;
        AtCommand at_cmd;		
        U16 rsp_len;
        bool bRet; 
		
		
        rsp_type = btmtk_dun_at_decode(nat->readbuff, msg->length, &at_cmd);

        // notify bt task to delete data from ring buffer
        btmtk_spp_uart_data_available_ind_rsp(nat->dunPort, msg->length);

        if ((at_cmd == AT_SIGNAL_DIAL) && 
		(rsp_type == AT_CONNECT))
        {
            env->CallVoidMethod(nat->me, method_onDunDialupReq);
            return;
        }

        bRet = btmtk_dun_at_encode(rsp_type, nat->writebuff, &rsp_len);

        ALOGI("at command response: %d, %d, %d, %d", nat->writebuff[0], nat->writebuff[1], nat->writebuff[2], nat->writebuff[3]);

        // notify bt task to read data from shared memory
        btmtk_spp_uart_put_bytes_req(nat->dunPort, NULL, rsp_len);
    }
    else if (nat->dunPhase == PHASE_PPP)
    {
        U8 buffer[SPP_PUT_DATA_SHARED_BUFFER_SIZE];

        memcpy(buffer, nat->readbuff, msg->length);
        // notify bt task to delete data from ring buffer
        btmtk_spp_uart_data_available_ind_rsp(nat->dunPort, msg->length);

        if (write(nat->dunPppSockClient, buffer, msg->length) < 0)
        {
            ALOGI("[DUN][PPP] write client socket failed");
            //dunPppClose();
        }
    }
}

static void dun_cb_assign_buffer_cnf(JNIEnv *env, void *ptr)
{
    ALOGI("[DUN][JNI] get shared memory...");
    nat->readbuff = (U8 *)btmtk_spp_obtain_ashm_get_buffer();
    nat->writebuff = (U8 *)btmtk_spp_obtain_ashm_put_buffer();
}

static void dun_cb_uart_plugout_ind(JNIEnv *env, void *ptr)
{
    ALOGI("[DUN][JNI] uart plugout ind");
    if (NULL == nat)
    {
        ALOGW("[DUN][JNI] dun_cb_uart_plugout_ind, nat is NULL!");
        return;		
    }

    if (!nat->bEnabled)
    {
        pthread_cond_signal(&nat->waitPlugout);
    }
}

static void dun_cb_put_bytes_cnf(JNIEnv *env, void *ptr)
{
    pthread_mutex_lock(&(nat->mutex));

    dunSendPacket(NULL, 0);
    nat->bPutDone = TRUE;

    pthread_mutex_unlock(&(nat->mutex));
}

/* To handle event from bt task, including indication and confirmation event */
static void dunEventCallbackHandler(void* handle, BT_CALLBACK_EVENT event, void *ptr, unsigned short datasize)
{
    JNIEnv *env;

    ALOGI("[DUN][JNI] event callback handler, event: %d, datalen: %d", event, datasize);

    /*
    * Add critical section check here for thread synchronization, in case that
    * the jobject global reference is deleted but nat memory is still existed.
    */  
    pthread_mutex_lock(&cs_mutex);

    if (NULL == nat)
    {
        ALOGW("[DUN][JNI] nat is NULL!");
        pthread_mutex_unlock(&cs_mutex);
        return;		
    }
    nat->vm->GetEnv((void**)&env, nat->envVer);

    switch (event)
    {
        case EVENT_SPP_MMI_ASSIGN_BUFFER_CNF:
            dun_cb_assign_buffer_cnf(env, ptr);
            break;			
        case EVENT_SPP_MMI_UART_PLUGOUT_IND:
            dun_cb_uart_plugout_ind(env, ptr);	 
            break;			
        case EVENT_DUN_MMI_ACTIVATE_CNF:
            dun_cb_enable_cnf(env, ptr);
            break;
        case EVENT_DUN_MMI_DEACTIVATE_CNF:
            dun_cb_disable_cnf(env, ptr);
            break;
        case EVENT_DUN_MMI_PUT_BYTES_CNF:
            dun_cb_put_bytes_cnf(env, ptr);			
            break;
        case EVENT_DUN_MMI_CONNECT_IND_REQ:
            dun_cb_connect_ind_req(env, ptr);
            break;
        case EVENT_DUN_MMI_CONNECT_IND:
            dun_cb_connect_ind(env, ptr);
            break;
        case EVENT_DUN_MMI_DISCONNECT_IND:
            dun_cb_disconnect_ind(env, ptr);
            break;
        case EVENT_DUN_MMI_DISCONNECT_CNF:
            dun_cb_disconnect_cnf(env, ptr);
            break;
        case EVENT_DUN_MMI_DATA_AVAILABLE_IND:
            dun_cb_data_available_ind(env, ptr);
            break;
        default:
            // Report error
            break;
    }

    pthread_mutex_unlock(&cs_mutex);

}

static void dun_convert_bdaddr2objpath(char *dest, bt_addr_struct *source)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
            (U8) ((source->nap & 0xFF00) >> 8),
            (U8) (source->nap & 0x00FF),
            source->uap,
            (U8) ((source->lap & 0x00FF0000) >> 16),
            (U8) ((source->lap & 0x0000FF00) >> 8),
            (U8) (source->lap & 0x000000FF));	
}

static void dun_convert_objpath2bdaddr(const char *source, bt_addr_struct *dest)
{
    unsigned char addr[6];
    char *ptr;
    int i;


    /* pattern XX:XX:XX:XX:XX:XX */
    for(i = 0, ptr= (char*)source; i < 6; i++, ptr++)
    {
        addr[i] = strtoul(ptr, &ptr, 16);
    }
    dest->lap = addr[5] + (addr[4] << 8) + (addr[3] << 16);
    dest->uap = addr[2];
    dest->nap = addr[1] + (addr[0] << 8);	
}

static void dun_convert_String2ATResponse(const char *source, AtResponse *dest)
{
    int len = strlen(source);

    if (strcmp(source, "active") == 0)
    {
        *dest = AT_CONNECT;
    }
    else if (strcmp(source, "busy") == 0)
    {
        *dest = AT_BUSY;
    }
    else if (strcmp(source, "unavailable") == 0)	
    {
        *dest = AT_NO_CARRIER;
    }
    else
    {
        *dest = AT_NO_CARRIER;
    }
}

/* DUN send message to BluetoothEventLoop for gap to handle */
static bool dun_sendmsg(unsigned long msg_id, void *ptr, U16 size)
{
    int ret = -1;
    ilm_struct ilm;
    struct sockaddr_un name;
    socklen_t namelen;
    
    memset(&ilm, 0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.src_mod_id = MOD_MMI;
    ilm.dest_mod_id = MOD_BT;
    ALOGI("[DUN][JNI] send msg = %ld", ilm.msg_id);
    if(ptr != NULL)
        memcpy(ilm.ilm_data, ptr, size);

    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

/*
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, BT_SOCK_NAME_EXT_ADP);
    namelen = offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1;
*/    
    socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&name, &namelen);

    if (nat->sppSockServer >= 0)
    {
        ret = sendto(nat->sppSockServer, &ilm, size, 0, (const sockaddr *)&name, namelen);
        if (ret < 0)
        {
            ALOGE("[DUN][JNI] send msg fail: %s(%d)", strerror(errno), errno);
        }
        else
        {
            ALOGI("[DUN][JNI] send msg len: %d", ret);
        }
    }
    else
    {
        ALOGE("[DUN][JNI] server socket uninitialized");
    }
    return (ret >= 0) ? true : false;	
}

static void dun_cond_timedwait(pthread_mutex_t *pmutex, pthread_cond_t *pcond, U16 timeout)
{
    int err;
    struct timespec ts;
    struct timeval tv;  
    
    gettimeofday(&tv, (struct timezone *) NULL);
    ts.tv_sec = tv.tv_sec + (timeout / 1000);
    ts.tv_nsec = (tv.tv_usec + (timeout % 1000) * 1000L ) * 1000L;
    
    pthread_mutex_lock(pmutex);
    err = pthread_cond_timedwait(pcond, pmutex, &ts);
    ALOGI("[SPP][JNI] cond timed wait signal ret: %d", err);  
    pthread_mutex_unlock(pmutex);
}
/*
#define SET_ARGV(param, file) { \
                     fwrite(param, strlen(param)+1, 1, file); \
                     fwrite("\n", 1, 1, file); \
}
*/
#define SET_ARGV(param, argv) { \
                         argv = param; \
    }


static void *dunPppWaitForTerm(void *arg)
{
    native_data_t *nat = (native_data_t *)arg;
    pid_t pid;
    int status;
    
    while ( (pid = waitpid(nat->pppdPid, &status, 0)) == -1 && errno == EINTR );
    ALOGI("[DUN][PPP] waitpid returned pid %d, status = %08x\n", pid, status);

    return NULL;
}

static bool dunStartPppTransmission(char *ipbaseaddr, char *dns1addr, char *dns2addr)
{
    ALOGI("[DUN][PPP] dunStartPppTransmission");
    //int ret;
/*
    FILE *file;
    
    file = fopen("/data/misc/ppp/ppp_options", "w");
    if (file == NULL)
    {
        ALOGE("[DUN][PPP] open ppp option file failed");
        return FALSE;		
    }


    SET_ARGV(PPP_SET_MODE, file);
    SET_ARGV(PPP_SET_SOCKNAME, file);
    SET_ARGV(BT_SOCK_NAME_EXT_ADP_SPP_DATA, file);
    SET_ARGV(PPP_SET_IPADDR, file);
    SET_ARGV(ipbaseaddr, file);
    SET_ARGV(PPP_SET_DNSADDR, file);
    SET_ARGV(dns1addr, file);
    SET_ARGV(PPP_SET_DNSADDR, file);
    SET_ARGV(dns2addr, file);

    fclose(file);

	
    //ret = property_set("ctl.start", "pppd");	

    if (!dun_sendmsg(ANDROID_EV_DUN_PPPD_START, NULL, 0))
    {
        ALOGE("[DUN][PPP] failed to start ppp daemon process");
        return FALSE;		
    }
*/
    //char *argv[] = {"file", "/data/misc/ppp/ppp_options"};
    char *argv[11] = {NULL};
    int i = 0;
    
    SET_ARGV(PPP_SET_NAME, argv[i++]);
    SET_ARGV(PPP_SET_MODE, argv[i++]);
    SET_ARGV(PPP_SET_SOCKNAME, argv[i++]);
    SET_ARGV(BT_SOCK_NAME_EXT_ADP_SPP_DATA, argv[i++]);
    SET_ARGV(PPP_SET_IPADDR, argv[i++]);
    SET_ARGV(ipbaseaddr, argv[i++]);
    SET_ARGV(PPP_SET_DNSADDR, argv[i++]);
    SET_ARGV(dns1addr, argv[i++]);
    SET_ARGV(PPP_SET_DNSADDR, argv[i++]);
    SET_ARGV(dns2addr, argv[i++]);
    
    
    pid_t pid = fork();
    if (pid == 0)
    {
        int ret = execve("/system/bin/pppd", argv, NULL);
        if (ret < 0)
        {
            ALOGE("[DUN][PPP] execute pppd failed: %s(%d)", strerror(errno), errno);
        }
    }
    else if (pid < 0)
    {
        ALOGE("[DUN][PPP] fork pppd failed: %s(%d)", strerror(errno), errno);
        return FALSE;
    }
    else
    {
        ALOGI("[DUN][PPP] save pppd pid: %d", pid);
        nat->pppdPid = pid;
        pthread_create(&(nat->pppdWaitTermThread), NULL, dunPppWaitForTerm, nat);
    }


    pthread_mutex_lock(&(nat->mutex));
    pthread_create(&(nat->dunPppThread), NULL, dunPppTransmissionMain, nat);
    pthread_mutex_unlock(&(nat->mutex));

    return TRUE;
}

static void dunStopPppTransmission(void)
{
    packet_t *tmp;

    // free pending packet list
    while (nat->pendingPacket)
    {
        tmp = nat->pendingPacket;
        nat->pendingPacket = tmp->next;
        free(tmp);
    }

	
    // close ppp socket
    dunPppClose();
	
    // kill pppd process
    //property_set("ctl.stop", "pppd");
    //dun_sendmsg(ANDROID_EV_DUN_PPPD_STOP, NULL, 0);
    ALOGI("[DUN][PPP] kill pppd pid: %d", nat->pppdPid);
    if (nat->pppdPid > 0)
    {
        int ret = kill(nat->pppdPid, SIGTERM);
        if (ret < 0)
        {
            ALOGE("[DUN][PPP] kill pppd failed: %s(%d)", strerror(errno), errno);
        }
        nat->pppdPid = -1;
    }
}

static void *dunPppTransmissionMain(void *arg)
{
    JNIEnv *env;
    JavaVMAttachArgs thr_args;
    char thr_name[] = "BT DUN PPP";
    int sockClient;
    int dunPort = nat->dunPort;
    S16 len;
    JavaVM *vm = nat->vm;

    thr_args.version = nat->envVer;
    thr_args.name = thr_name;
    thr_args.group = NULL;

    vm->AttachCurrentThread(&env, &thr_args);

    ALOGI("[DUN][PPP] dunPppTransmissionMain");

    sockClient = dunPppListen();
    if (sockClient < 0)
    {
        goto exit;
    }

    nat->dunPppSockClient = sockClient;

    while (1)
    {
        U8 buffer[SPP_PUT_DATA_SHARED_BUFFER_SIZE];

        // a thread to wait for packet from ppp daemon
        len = read(sockClient, &buffer, SPP_PUT_DATA_SHARED_BUFFER_SIZE);
        ALOGI("[DUN][PPP] read packet len: %d", len);
        if (len < 0)
        {
            ALOGE("[DUN][PPP] read failed!");
            break;
        }

        if (len == 0)
        {
            ALOGI("[DUN][PPP] client socket disconnect...");
            break;
        }
		
        pthread_mutex_lock(&(nat->mutex));
        if (nat->bPutDone)
        {
            dunSendPacket(buffer, len);
            nat->bPutDone = FALSE;		
        }
        else
        {
            dunLoopPacket(buffer, len);
        }
        pthread_mutex_unlock(&(nat->mutex));

    }

    if (sockClient >= 0)
    {
        close(sockClient);
        sockClient = -1;		
    }

exit:
	
    vm->DetachCurrentThread();
    if ((nat != NULL) && (nat->dunPhase != PHASE_NONE))
    {
        btmtk_dun_disconnect_req(dunPort);
    }
	
    return NULL;
}

static int dunLoopPacket(U8 *buffer, U16 len)
{
    packet_t *newPacket;


    ALOGI("dunLoopPacket: len: %d, %x, %x, %x", len, buffer[0], buffer[1], buffer[2]);

    newPacket = (packet_t *)malloc(sizeof(packet_t) + len);		
    if (newPacket == NULL)
    {
        return -1;
    }
    newPacket->len = len;
    memcpy(((U8 *)newPacket) + sizeof(packet_t), buffer, len);
    newPacket->next = NULL;
	
    if (nat->pendingPacket == NULL)
    {
        nat->pendingPacket = newPacket;
    }
    else
    {
        packet_t *tmp = nat->pendingPacket;

        while (tmp->next)
            tmp = tmp->next;

        tmp->next = newPacket;
    }


    return 0;
}

static void dunSendPacket(U8 *buffer, U16 len)
{
    packet_t *tmp;
    U16 totalLen = 0;

    if ((nat->pendingPacket == NULL) && (buffer == NULL))
    {
        ALOGI("[DUN][PPP] send packet, no more to be sent");
        return;		
    }


    if ((nat->pendingPacket == NULL) && (buffer != NULL))
    {
        // notify bt task to read data from shared memory
        ALOGI("[DUN][PPP] send packet, no pending packet...");
        memcpy(nat->writebuff, buffer, len); 
        btmtk_spp_uart_put_bytes_req(nat->dunPort, NULL, len);
        return;		
    }

    ALOGI("[DUN][PPP] send packet to bt task...");
    /* if there is any pending packet, it shall be sent first */
    while (nat->pendingPacket && (totalLen < SPP_PUT_DATA_SHARED_BUFFER_SIZE))
    {
        memcpy(nat->writebuff + totalLen, ((U8 *)nat->pendingPacket) + sizeof(packet_t), nat->pendingPacket->len); 
        totalLen += nat->pendingPacket->len;
		
        tmp = nat->pendingPacket;
        nat->pendingPacket = tmp->next;
        free(tmp);
    }


    if (buffer != NULL)
    {
        if ((!nat->pendingPacket) ||
              (totalLen + len < SPP_PUT_DATA_SHARED_BUFFER_SIZE))
        {
            ALOGI("[DUN][PPP] send packet, send current buffer anyway, total len: %d", totalLen);
            memcpy(nat->writebuff + totalLen, buffer, len); 
            totalLen += len;
        }
        else
        {
            dunLoopPacket(buffer, len);
        }
    }
	
    // notify bt task to read data from shared memory
    btmtk_spp_uart_put_bytes_req(nat->dunPort, NULL, totalLen);
    ALOGI("[DUN][PPP] send packet, total len: %d", totalLen);
	
}

static int dunPppListen(void)
{
    struct sockaddr_un btSrvAddr;
    socklen_t btSrvAddrlen;
    struct sockaddr_un btCliAddr;
    socklen_t btCliAddrlen;
    int sockClient;

    nat->dunPppSockServer = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (nat->dunPppSockServer < 0)
    {
        ALOGE("[DUN][PPP] create bt data socket failed: %s(%d)", strerror(errno), errno);
        return -1;
    }
    
    btSrvAddr.sun_family = AF_LOCAL;
    btSrvAddr.sun_path[0] = 0;
    strcpy(btSrvAddr.sun_path + 1, BT_SOCK_NAME_EXT_ADP_SPP_DATA);
    btSrvAddrlen = offsetof(struct sockaddr_un, sun_path) + strlen(btSrvAddr.sun_path) + 1;	

    if (bind(nat->dunPppSockServer, (struct sockaddr *) &btSrvAddr, btSrvAddrlen) < 0)
    {
        ALOGE("[DUN][PPP] bind failed: %s(%d)", strerror(errno), errno);
        goto exit;
    }

    if (listen(nat->dunPppSockServer, 1) < 0)
    {
        ALOGE("[DUN][PPP] listen failed: %s(%d)", strerror(errno), errno);
        goto exit;
    }

    sockClient = accept(nat->dunPppSockServer, (struct sockaddr *) &btCliAddr, &btCliAddrlen);
    if (sockClient < 0)
    {
        ALOGE("[DUN][PPP] accept failed: %s(%d)", strerror(errno), errno);
        goto exit;
    }

    return sockClient;

exit:

    if (nat->dunPppSockServer >= 0)
    {
        close(nat->dunPppSockServer);
        nat->dunPppSockServer = -1;		
    }
    return -1;
	
}

static void dunPppClose(void)
{
    if (nat->dunPppSockClient >= 0)
    {
        close(nat->dunPppSockClient);
        nat->dunPppSockClient = -1;
    }

    if (nat->dunPppSockServer >= 0)
    {
        close(nat->dunPppSockServer);
        nat->dunPppSockServer = -1; 
    }

    // close connection?	

}

static jboolean sppStartEventLoop(JNIEnv* env, jobject object)
{
    ALOGI("[SPP][JNI] startEventLoop");

    pthread_mutex_lock(&(nat->mutex));
    pthread_create(&(nat->sppThread), NULL, sppEventLoopMain, nat);
    pthread_mutex_unlock(&(nat->mutex));

    return JNI_TRUE;
}

static void *sppEventLoopMain(void *arg)
{
    JNIEnv *env;
    JavaVM *vm = nat->vm;

    ilm_struct ilm;
    fd_set readfs;
    int res;	
    int socksrv;
	
    JavaVMAttachArgs thr_args;
    char thr_name[] = "BT SPP EventLoop";

    thr_args.version = nat->envVer;
    thr_args.name = thr_name;
    thr_args.group = NULL;

   vm->AttachCurrentThread(&env, &thr_args);

    ALOGI("[SPP][JNI] sppEventLoopMain");

    socksrv = nat->sppSockServer;

    while (1)
    {
        FD_ZERO(&readfs);
        if (socksrv > 0)
        {
            FD_SET(socksrv, &readfs);
        }
        res = select(socksrv + 1, &readfs, NULL, NULL, NULL);
        ALOGI("[SPP][JNI] select result: %d", res);

        if (res > 0)
        {
            res = recvfrom(socksrv, (void *)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
            if (res <= 0)
            {
                ALOGW("[SPP][JNI] receive message error, res: %d, error: %s(%d)", res, strerror(errno), errno);
                break;
            }
            ALOGI("[SPP][JNI] receive message length: %d", res);			
            btmtk_spp_handle_message(&ilm);
        }
        else if (res == 0)
        {
            ALOGW("[SPP][JNI] select timeout waiting...");
            break;			
        }
        else
        {
            if (errno == EINTR)
            {
                ALOGI("[SPP][JNI] system interrupt, reselect");
                continue;
            }
            else
            {
                ALOGE("[SPP][ERR] select failed: %s(%d)", strerror(errno), errno);
                break;          
            }
        }
    }

    ALOGI("[SPP][JNI] event loop main thread detach: %d", res); 		
    if (socksrv >= 0)
    {
        close(socksrv);
        socksrv = -1;
    }

    vm->DetachCurrentThread();
    return NULL;
}

static void sppWakeupEventLoopMain(void)
{
    struct sockaddr_un addr;
    socklen_t addrlen = sizeof(struct sockaddr_un);
    int ret;

    addr.sun_path[0] = '\0';
    if (getsockname(nat->sppSockServer, (struct sockaddr *)&addr, &addrlen) < 0)
    {
        ALOGE("[SPP][ERR] get dun server socket address failed: %s(%d)", strerror(errno), errno);
        return;
    }
    ret = sendto(nat->sppSockServer, 0, 0, 0, (struct sockaddr *)&addr, addrlen);
}

static jboolean sppRegisterSocket(JNIEnv* env, jobject object)
{
    struct sockaddr_un dunextaddr;
    socklen_t dunextaddrlen = sizeof(struct sockaddr_un);
    struct sockaddr_un btintaddr;
    socklen_t btintaddrlen;

    ALOGI("[SPP][JNI] sppRegisterSocket");

    /**************************/
    /*             create socket                 */
    /**************************/
    /* create dun external adp server socket, and bind to indicated address */
    nat->sppSockServer = socket_local_server(BT_SOCK_NAME_EXT_ADP_SPP,
		                                                  ANDROID_SOCKET_NAMESPACE_ABSTRACT,
		                                                  SOCK_DGRAM);

    if (nat->sppSockServer < 0)
    {
        ALOGE("[SPP][ERR] create dun server socket failed: %s(%d)", strerror(errno), errno);
        return JNI_FALSE;
    }


    /* create dun external adp client socket */
    nat->sppSockClient = socket(AF_LOCAL, SOCK_DGRAM, 0);

    if (nat->sppSockClient < 0)
    {
        ALOGE("[SPP][ERR] create dun client socket failed: %s(%d)", strerror(errno), errno);
        goto exit;
    }

    btintaddr.sun_family = AF_LOCAL;
    strcpy(btintaddr.sun_path, BT_SOCK_NAME_INT_ADP);
    btintaddrlen = offsetof(struct sockaddr_un, sun_path) + strlen(btintaddr.sun_path) + 1;	

    if (connect(nat->sppSockClient, (struct sockaddr *)&btintaddr, btintaddrlen) < 0)
    {
        ALOGE("[SPP][ERR] connect to /data/bt.int.adp failed: %s(%d)", strerror(errno), errno);
        goto exit;
    }

    btmtk_spp_set_sockfd(nat->sppSockClient);

    /**************************/
    /*		start event looper	         */
    /**************************/
    sppStartEventLoop(env, object);

    return JNI_TRUE;

exit:
    sppDeregisterSocket();
	
    return JNI_FALSE;
}

static void sppDeregisterSocket(void)
{
    if (nat->sppSockServer >= 0)
    {
        close(nat->sppSockServer);
        nat->sppSockServer = -1;
    }
    if (nat->sppSockClient >= 0)
    {
        close(nat->sppSockClient);
        nat->sppSockClient = -1;		
    }
}


static JNINativeMethod sMethods[] = {
    {"initNative", "()Z", (void *)initNative},
    {"cleanupNative", "()V", (void *)cleanupNative},

    {"dunEnableNative", "()V", (void *)dunEnableNative},
    {"dunDisableNative", "()V", (void *)dunDisableNative},
    {"dunDisconnectNative", "()V", (void *)dunDisconnectNative},
    {"dunConnectRspNative", "(Ljava/lang/String;Z)V", (void*)dunConnectRspNative},
    {"dunDialupRspNative", "(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V", (void *)dunDialupRspNative}
};

static int register_BluetoothDunService(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/mediatek/bluetooth/dun/BluetoothDunService");
    if (clazz == NULL)
    {
        ALOGE("[SPP][ERR] Can't find com/mediatek/bluetooth/dun/BluetoothDunService");
        return JNI_FALSE;
    }
	
    if (env->RegisterNatives(clazz, sMethods, NELEM(sMethods)) < 0) 
    {
        ALOGE("[SPP][ERR] Register BluetoothDunService native failed");
        return JNI_FALSE;
    }

    method_onDunConnectReq = env->GetMethodID(clazz, "onDunConnectReq", "(Ljava/lang/String;)V");
    method_onDunStateChanged = env->GetMethodID(clazz, "onDunStateChanged",
		                 "(Ljava/lang/String;Ljava/lang/String;)V");
    method_onDunDialupReq = env->GetMethodID(clazz, "onDunDialupReq", "()V");
    method_onDunEnableCnf = env->GetMethodID(clazz, "onDunEnableCnf", "(Z)V");
    method_onDunDisableCnf = env->GetMethodID(clazz, "onDunDisableCnf", "(Z)V");

    return JNI_TRUE;
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

    ALOGI("+JNI_OnLoad");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto exit;
    }
    assert(env != NULL);

    if (!register_BluetoothDunService(env)) {
        ALOGE("ERROR: BluetoothDunService native registration failed\n");
        goto exit;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;
exit:
    ALOGI("-JNI_OnLoad: %d", result);
    return result;
}

#endif
