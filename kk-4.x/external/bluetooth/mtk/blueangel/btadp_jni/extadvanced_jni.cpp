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
 * extadvanced_jni.cpp
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to provide jni layer api for advanced service(profiles)
 *
 * Author:
 * -------
 * Dexiang Jiang
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
	 
#define LOG_TAG "extadvanced_jni"
	 
#include <sys/socket.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>	 
//#include <cutils/properties.h>

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
#include "bt_mmi.h"
//#include "bt_message.h"
//#include "bluetooth_struct.h"
#include "bt_gap_struct.h"
#include "socket_local.h"
}


static jmethodID method_onAdvancedEnableCnf;
static jmethodID method_onAdvancedDisableCnf;
static jmethodID method_onAdvancedStartRequestInd;
static jmethodID method_onAdvancedStopRequestInd;


#define FREEIF(p)   do { if(p) free(p); p = NULL; } while(0)

typedef struct {
    JavaVM *vm;
    int envVer;
    jobject me;         // for callbacks to java
    bool bEnabled;
	
	pthread_t rxThread; //thread for rx message from socket
    int sockServer;     // for receiving power on/off indication
} native_data_t;


static native_data_t *nat = NULL;  // global native data

static jboolean advancedStartEventLoop(JNIEnv* env, jobject object);
static void *advancedEventLoopMain(void *arg);
static void advancedWakeupEventLoopMain(void);
static jboolean advancedRegisterSocket(JNIEnv* env, jobject object);
static void advancedDeregisterSocket(void);

static jboolean initNative(JNIEnv *env, jobject object)
{
    ALOGV("[Advanced][JNI] initNative");

    nat = (native_data_t *)calloc(1, sizeof(native_data_t));
    if (NULL == nat) {
        ALOGE("[Advanced][ERR] %s: out of memory!", __FUNCTION__);
        return JNI_FALSE;
    }

    memset(nat,0,sizeof(native_data_t));
    nat->sockServer = -1;

    env->GetJavaVM( &(nat->vm) );
    nat->envVer = env->GetVersion();
    nat->me = env->NewGlobalRef(object);

    /* register advanced socket */
    if (!advancedRegisterSocket(env, object))
    {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static void cleanupNative(JNIEnv *env, jobject object)
{
    ALOGI("[Advanced][JNI] cleanupNative");
    
    advancedWakeupEventLoopMain();

    env->DeleteGlobalRef(nat->me);
    FREEIF(nat);
}

static void advancedEnableNative(JNIEnv *env, jobject object)
{
    ALOGI("[Advanced][JNI] enable Advanced");
    nat->bEnabled = true; 
    env->CallVoidMethod(nat->me, method_onAdvancedEnableCnf, true);    
}

static void advancedDisableNative(JNIEnv *env, jobject object)
{
    ALOGI("[Advanced][JNI] disable Advanced");
    nat->bEnabled = false;
    env->CallVoidMethod(nat->me, method_onAdvancedDisableCnf, true);    
}

/* Power on done */
static void advanced_cb_start_ind(JNIEnv *env, bool result)
{
    ALOGI("[Advanced][JNI] power on ind, result[%d]", result);
    if (!nat->bEnabled)
    {
    	ALOGI("[Advanced][JNI][ERR] No enabled before advanced_cb_start_ind!");
        return;
    }    
    env->CallVoidMethod(nat->me, method_onAdvancedStartRequestInd, result);
}

/* Power off done */
static void advanced_cb_stop_ind(JNIEnv *env, bool result)
{
    ALOGI("[Advanced][JNI] disconnected ind");  
    if (!nat->bEnabled)
    {
    	ALOGI("[Advanced][JNI][ERR] No enabled before advanced_cb_stop_ind!");
        return;
    }

    env->CallVoidMethod(nat->me, method_onAdvancedStopRequestInd);
}

void advanced_service_handle_message(ilm_struct *ilm)
{
    JNIEnv *env;

    ALOGI("[Advanced][JNI] advanced_service_handle_message, msg: %d", (int)ilm->msg_id);

    /*
    * Add critical section check here for thread synchronization, in case that
    * the jobject global reference is deleted but nat memory is still existed.
    */  

    if (NULL == nat)
    {
        ALOGW("[Advanced][JNI][ERR] nat is NULL!");
        return;		
    }
    nat->vm->GetEnv((void**)&env, nat->envVer);

    switch (ilm->msg_id)
    {
        case EV_BT_CB_ENABLE_ADVANCE_SERVICE:
        {
            bt_poweron_cnf_struct *cnf = (bt_poweron_cnf_struct*)ilm;	
            ALOGW("[Advanced][JNI] receive power on cnf, result[%d]", cnf->result);
            advanced_cb_start_ind(env, true);
            break;
        }
        case EV_BT_CB_DISABLE_ADVANCE_SERVICE:
        {
        	bt_poweroff_cnf_struct *cnf = (bt_poweroff_cnf_struct*)ilm;	
        	ALOGW("[Advanced][JNI] receive power off cnf, result[%d]", cnf->result);
            advanced_cb_stop_ind(env, true);
        	break;
        }
        	
        default:
        	ALOGW("[Advanced][JNI][ERR] Un-expect Message[%d]!", (int)ilm->msg_id);
        	break;
    }  
}

static jboolean advancedStartEventLoop(JNIEnv* env, jobject object)
{
    ALOGI("[Advanced][JNI] startEventLoop");

    //Need mutex?
    pthread_create(&(nat->rxThread), NULL, advancedEventLoopMain, nat);

    return JNI_TRUE;
}

static void *advancedEventLoopMain(void *arg)
{
    JNIEnv *env;
    JavaVM *vm = nat->vm;

    ilm_struct ilm;
    fd_set readfs;
    int res;	
    int socksrv;
	
    JavaVMAttachArgs thr_args;
    char thr_name[] = "BT Advanced EventLoop";

    thr_args.version = nat->envVer;
    thr_args.name = thr_name;
    thr_args.group = NULL;

   vm->AttachCurrentThread(&env, &thr_args);

    ALOGI("[Advanced][JNI] advancedEventLoopMain");

    socksrv = nat->sockServer;

    while (1)
    {
        FD_ZERO(&readfs);
        if (socksrv > 0)
        {
            FD_SET(socksrv, &readfs);
        }
        res = select(socksrv + 1, &readfs, NULL, NULL, NULL);
        ALOGI("[Advanced][JNI] select result: %d", res);

        if (res > 0)
        {
            res = recvfrom(socksrv, (void *)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
            if (res <= 0)
            {
                ALOGW("[Advanced][JNI] receive message error, res: %d, error: %s(%d)", res, strerror(errno), errno);
                break;
            }
            ALOGI("[Advanced][JNI] receive message length: %d", res);			
            advanced_service_handle_message(&ilm);
        }
        else if (res == 0)
        {
            ALOGW("[Advanced][JNI] select timeout waiting...");
            break;			
        }
        else
        {
            if (errno == EINTR)
            {
                ALOGI("[Advanced][JNI] system interrupt, reselect");
                continue;
            }
            else
            {
                ALOGE("[Advanced][ERR] select failed: %s(%d)", strerror(errno), errno);
                break;          
            }
        }
    }

    ALOGI("[Advanced][JNI] event loop main thread detach: %d", res); 		
    if (socksrv >= 0)
    {
        close(socksrv);
        socksrv = -1;
    }

    vm->DetachCurrentThread();
    return NULL;
}

static void advancedWakeupEventLoopMain(void)
{
    struct sockaddr_un addr;
    socklen_t addrlen = sizeof(struct sockaddr_un);
    int ret;

    addr.sun_path[0] = '\0';
    if (getsockname(nat->sockServer, (struct sockaddr *)&addr, &addrlen) < 0)
    {
        ALOGE("[Advanced][ERR] get advanced server socket address failed: %s(%d)", strerror(errno), errno);
        return;
    }
    ret = sendto(nat->sockServer, 0, 0, 0, (struct sockaddr *)&addr, addrlen);
}

static jboolean advancedRegisterSocket(JNIEnv* env, jobject object)
{
    //struct sockaddr_un extSockAddr;
    //socklen_t extSockAddrLen = sizeof(struct sockaddr_un);

    ALOGI("[Advanced][JNI] advancedRegisterSocket");

    /**************************/
    /*             create socket                 */
    /**************************/
    /* create advanced external adp server socket, and bind to indicated address */
    nat->sockServer = socket_local_server(BT_SOCK_NAME_EXT_ADP_ADVANCED,
		                                  ANDROID_SOCKET_NAMESPACE_ABSTRACT,
		                                  SOCK_DGRAM);

    if (nat->sockServer < 0)
    {
        ALOGE("[Advanced][ERR] create advanced server socket failed: %s(%d)", strerror(errno), errno);
        return JNI_FALSE;
    }

    /**************************/
    /*		start event looper	         */
    /**************************/
    advancedStartEventLoop(env, object);

    return JNI_TRUE;
}

static void advancedDeregisterSocket(void)
{
    if (nat->sockServer >= 0)
    {
        close(nat->sockServer);
        nat->sockServer = -1;
    }
}


static JNINativeMethod advancedMethods[] = {
    {"initNative", "()Z", (void *)initNative},
    {"cleanupNative", "()V", (void *)cleanupNative},
    {"advancedEnableNative", "()V", (void *)advancedEnableNative},
    {"advancedDisableNative", "()V", (void *)advancedDisableNative}
};

static int register_BluetoothAdvancedService(JNIEnv *env)
{
    int methodsNum = 0;
    ALOGE("[Advanced] register_BluetoothAdvancedService");
    jclass clazz = env->FindClass("com/mediatek/bluetooth/AdvancedService");
    if (clazz == NULL)
    {
        ALOGE("[Advanced][ERR] Can't find com/mediatek/bluetooth/AdvancedService");
		return JNI_FALSE;
    }

	methodsNum = sizeof(advancedMethods) / sizeof(advancedMethods[0]);
	ALOGE("[Advanced]Native methods number:%d, NELEM:%d", methodsNum, NELEM(advancedMethods));
    if (env->RegisterNatives(clazz, advancedMethods, methodsNum) < 0) 
    {
        ALOGE("[Advanced][ERR] Register BluetoothAdvancedService native failed");
        return JNI_FALSE;
    }

    method_onAdvancedEnableCnf = env->GetMethodID(clazz, "onAdvancedEnableCnf", "(Z)V");
    method_onAdvancedDisableCnf = env->GetMethodID(clazz, "onAdvancedDisableCnf", "(Z)V");
    method_onAdvancedStartRequestInd = env->GetMethodID(clazz, "onAdvancedStartRequestInd", "(Z)V");
    method_onAdvancedStopRequestInd = env->GetMethodID(clazz, "onAdvancedStopRequestInd", "(Z)V");
    
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

    if (!register_BluetoothAdvancedService(env)) {
        ALOGE("ERROR: AdvancedService native registration failed\n");
        goto exit;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;
exit:
    ALOGI("-JNI_OnLoad: %d", result);
    return result;
}

#endif
