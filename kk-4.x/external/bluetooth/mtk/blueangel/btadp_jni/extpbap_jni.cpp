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

#define LOG_TAG "EXT_PBAP"

#include <wchar.h>
#include <jni.h>
#include <cutils/jstring.h>
#include <utils/Log.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

extern "C"
{
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_pbap_struct.h"
#include "bt_pbap_api.h"
}

#define IsValidRspCode(code) (code==PBAP_CNF_SUCCESS)

typedef struct {
    JNIEnv *env;
    int apisock;
    int servsock;
    U32 con_id;
} native_data_t;

static jfieldID field_mNativeData;

static jmethodID method_onAuthorizeInd;
static jmethodID method_onConnectInd;
static jmethodID method_onDisconnectInd;
static jmethodID method_onSetPathInd;
static jmethodID method_onPullVcardEntryInd;
static jmethodID method_onPullPhonebookInd;
static jmethodID method_onPullVcardListingInd;
static jmethodID method_onAbortInd;
static jmethodID method_onAuthChallInd;
static jmethodID method_onPullPhonebookContinueInd;
    
#define PBAP_WAKEUP_EVENT 1

static inline native_data_t * get_native_data(JNIEnv *env, jobject object) {
    return (native_data_t *)(env->GetIntField(object, field_mNativeData));
}

static void setEvent(JNIEnv* env, jobject object, int event)
{
	switch(event)
	{
	case PBAP_WAKEUP_EVENT:
	{
		// send event
		native_data_t *nat = get_native_data(env, object);

		ALOGI("[API] setEventNative(%d)", event);
		if(nat)
		{
		    sockaddr_un name;
		    socklen_t   namelen;
		    int ret;
		    
		    namelen = sizeof(sockaddr_un);
		    ret = getsockname(nat->servsock, (struct sockaddr*)&name, &namelen);
		    ALOGI("nat->hfgservsock : name=%s, size=%d", &name.sun_path[1], namelen);
		    if(ret == 0 && namelen > 0)
		    {
		        ret = sendto(nat->apisock, &event, sizeof(int), 0, (struct sockaddr*)&name, namelen);
		        if(ret < 0)
		        {
		            ALOGE("[ERR] sendto servsock itself failed : %s, errno=%d", strerror(errno), errno);
		        }
		    }
		    else
		    {
		        ALOGE("[ERR] getsockname failed : %s, errno=%d", strerror(errno), errno);
		    }
		}
		else
		{
		    ALOGE("[ERR] nat is null");
		}
	}
		break;
	default:
		ALOGE("[ERR] unsupported event");
		break;
	}
}

S32 bt_ucs2strlen(const S8 *arrOut){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/      
    U32 nCount;
    U32 nLength;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Check for NULL character only at the odd no. of bytes 
       assuming forst byte start from zero */
    nCount = 0;
    nLength = 0;
    if( NULL == arrOut ){
        return 0;
    }
    // Avoid using U16*. Arm data pointer (4 byte aligment) may cause data abort exception    
    for ( nCount = 0; (0 != *arrOut || 0 != *(arrOut+1) ) ; nCount++){
        arrOut++;arrOut++;
    }
    return nCount; /* One is added to count 0th byte */
}

static void CONVERT_BDADDRSRC2STRING(char *dest, bt_pbap_bd_addr_struct *source)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
            (U8) ((source->nap & 0xFF00) >> 8),
            (U8) (source->nap & 0x00FF),
            source->uap,
            (U8) ((source->lap & 0x00FF0000) >> 16),
            (U8) ((source->lap & 0x0000FF00) >> 8),
            (U8) (source->lap & 0x000000FF));
}

static void classInitNative(JNIEnv* env, jclass clazz) 
{
    ALOGI("+classInitNative");
    method_onAuthorizeInd = env->GetMethodID(clazz, "onAuthorizeInd", "(Ljava/lang/String;)I");
    method_onConnectInd = env->GetMethodID(clazz, "onConnectInd", "(Ljava/lang/String;Ljava/lang/String;I)I");
    method_onDisconnectInd = env->GetMethodID(clazz, "onDisconnectInd", "()V");
    method_onSetPathInd = env->GetMethodID(clazz, "onSetPathInd", "(Ljava/lang/String;I)I");
    method_onPullVcardEntryInd = env->GetMethodID(clazz, "onPullVcardEntryInd", "(ILjava/lang/String;JZ)I");
    method_onPullPhonebookInd = env->GetMethodID(clazz, "onPullPhonebookInd", "(ILjava/lang/String;JZII)I");
    method_onPullVcardListingInd = env->GetMethodID(clazz, "onPullVcardListingInd", "(ILjava/lang/String;ILjava/lang/String;III)I");
    method_onAbortInd = env->GetMethodID(clazz, "onAbortInd", "()V");
    method_onAuthChallInd = env->GetMethodID(clazz, "onAuthChallInd", "(Ljava/lang/String;ZZ)V");
    method_onPullPhonebookContinueInd = env->GetMethodID(clazz, "onPullPhonebookContinueInd", "(I)I");
    
    field_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");
    ALOGI("-classInitNative");
}

static void initializeNativeDataNative(JNIEnv* env, jobject object) 
{
    native_data_t *nat = (native_data_t *)calloc(1, sizeof(native_data_t));

    ALOGI("[API] initializeNativeDataNative");
    if (NULL == nat) {
        ALOGE("%s: out of memory!", __FUNCTION__);
        return;
    }
    memset(nat, 0, sizeof(native_data_t));
    env->SetIntField(object, field_mNativeData, (jint)nat);
    ALOGI("-initializeNativeDataNative");
}

static void cleanupNativeDataNative(JNIEnv* env, jobject object) 
{
    native_data_t *nat = (native_data_t *)env->GetIntField(object, field_mNativeData);

    ALOGI("[API] cleanupNativeDataNative");
    if (nat) {
        free(nat);
    }
}
static jboolean enableNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;
    sockaddr_un name;
    socklen_t   namelen;
    struct sockaddr_un btname;
    socklen_t   btnamelen;

    ALOGI("[API] enableNative");
    // get native data
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }
    // Create apisock and servsock
    // Setup hfg service socket
    name.sun_family = AF_LOCAL;
    namelen = sizeof(short);
    nat->servsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    ALOGI("nat->servsock==%d", nat->servsock);
    if (nat->servsock < 0)
    {
        ALOGE("[ERR] create pbap server socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    if (bind (nat->servsock, (struct sockaddr *) &name, namelen) < 0)
    {
        ALOGE("[ERR] bind pbap service socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    
    // Setup hfg api socket
    namelen = sizeof(short);
    nat->apisock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    ALOGI("nat->apisock==%d", nat->apisock);
    if (nat->apisock < 0)
    {
        ALOGE("[ERR] create pbap api socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    if (bind (nat->apisock, (struct sockaddr *) &name, namelen) < 0)
    {
        ALOGE("[ERR] bind pbap api socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    // connect to set bt task socket
    name.sun_family = AF_UNIX;
    strcpy (name.sun_path, /*BT_SERV_SOCK_ADDR*/BT_SOCK_NAME_INT_ADP);
    namelen = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1);     
    if ( connect(nat->apisock, (const struct sockaddr*)&name, namelen) < 0)
    {
        ALOGE("[ERR] connect to %s failed : %s, errno=%d", name.sun_path, strerror(errno), errno);
        goto error_exit;
    }    
    if( getsockname(nat->servsock, (struct sockaddr*)&name, &namelen) < 0)
    {
        ALOGE("[ERR] getsockname failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    if( !btmtk_pbap_send_active_req(nat->apisock, 0x02, 0x03 /* PB only */, &name, namelen) )
    {
        goto error_exit;
    }    
    return JNI_TRUE;
error_exit:
    if(nat)
    {
        if(nat->apisock >= 0)
        {
            close(nat->apisock);
            nat->apisock = 0;
        }
        if(nat->servsock >= 0)
        {
            close(nat->servsock);
            nat->servsock = 0;
        }
    }
    return JNI_FALSE;
}

static void wakeupListenerNative(JNIEnv* env, jobject object)
{
	setEvent(env, object, PBAP_WAKEUP_EVENT);
}

static void disableNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;

    ALOGI("[API] disableNative");
    nat = get_native_data(env, object);
    if(nat)
    {
        btmtk_pbap_send_deactive_req(nat->apisock);
        if(nat->apisock >= 0)
        {
            close(nat->apisock);
            nat->apisock = 0;
        }
        if(nat->servsock >= 0)
        {
            close(nat->servsock);
            nat->servsock = 0;
        }
    }
    else
    {
        ALOGE("[ERR] nat is null");
    }    
}

static jboolean connectRspNative(JNIEnv* env, jobject object, jint con_id, jboolean accept)
{
    native_data_t *nat;

    ALOGI("[API] connectRspNative(%d, %d)", con_id, accept?1:0);
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }
    if( btmtk_pbap_send_connect_rsp(nat->apisock, (U32)nat->con_id, (accept ? PBAP_CNF_SUCCESS : PBAP_CNF_FAILED)) )
    {
        //nat->con_id = (U32)con_id;
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

static void disconnectNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;

    ALOGI("[API] disconnectNative");
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return;
    }
    btmtk_pbap_send_disconnect_req(nat->apisock, nat->con_id, 0x01);
    nat->con_id = 0;
}

/* -1 : failed */
static jboolean listenIndicationNative(JNIEnv* env, jobject object, jboolean noWait)
{
    int res = 1;
    fd_set readfs;
    int sockfd;
    ilm_struct ilm;

    ALOGI("[API] listenIndicationNative(%d)", noWait?1:0);
    native_data_t *nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is NULL");
        return JNI_FALSE;
    }

    sockfd = nat->servsock;
    if(!noWait)
    {
        FD_ZERO(&readfs);
        if(sockfd)
        {
            FD_SET(sockfd, &readfs);
        }
        else
        {
            ALOGE("[ERR] nat->servsock == 0. exit");
        }
        ALOGI("Start select : sockfd=%d", sockfd);
        res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, NULL));
        ALOGI("Return from select : res=%d", res);
    }

    if(res > 0)
    {
        res = recvfrom(sockfd, (void*)&ilm, sizeof(ilm_struct), noWait ? MSG_DONTWAIT : 0, NULL, NULL);
        ALOGI("recvfrom returns %d", res);
        if(res < 0)
        {
            if(errno == EAGAIN)
            {
                res = 0;
            }
            else
            {
                ALOGE("[ERR] recvfrom failed : %s, %d", strerror(errno), errno);
            }
        }else if(res == sizeof(int)) {
            // PBAP internal events
            int evt = *(int*)&ilm;
            ALOGI("Recv PBAP internal event : %d", evt);
            switch(evt) {
            case PBAP_WAKEUP_EVENT:
                // do nothing
                break;
            default:
                ALOGE("[ERR] unsupported event");
                break;
            }
        }else {
            ALOGI("Recv PBAP indication : %lu", ilm.msg_id);        
            switch(ilm.msg_id)
            {
            case MSG_ID_BT_PBAP_AUTHORIZE_IND:
            {
                bt_pbap_authorize_ind_struct *ind_p = (bt_pbap_authorize_ind_struct*)ilm.ilm_data;
                char addr[18];
                CONVERT_BDADDRSRC2STRING(addr, &ind_p->bd_addr);
                env->CallIntMethod(object,
                                                method_onAuthorizeInd,
                                                env->NewStringUTF(addr));
            }
                break;
            case MSG_ID_BT_PBAP_CLIENT_CONNECT_IND:
            {
                bt_pbap_client_connect_ind_struct *ind_p = (bt_pbap_client_connect_ind_struct*)ilm.ilm_data;
                char addr[18];
                CONVERT_BDADDRSRC2STRING(addr, &ind_p->bd_addr);
                nat->con_id = ind_p->cm_conn_id;
                env->CallIntMethod(object,
                                                        method_onConnectInd,
                                                        env->NewStringUTF(addr),
                                                        env->NewStringUTF((const char*)ind_p->dev_name),
                                                        ind_p->cm_conn_id);
            }
                break;
            case MSG_ID_BT_PBAP_DISCONNECT_IND:
                env->CallVoidMethod(object, method_onDisconnectInd);
                break;
            case MSG_ID_BT_PBAP_CLIENT_AUTH_CHALLENGE_IND:
            {
                bt_pbap_client_auth_challenge_ind_struct *ind_p = (bt_pbap_client_auth_challenge_ind_struct*)ilm.ilm_data;
                ALOGI("MSG_ID_BT_PBAP_CLIENT_AUTH_CHALLENGE_IND : dev_name=%s, option=%d", ind_p->dev_name, ind_p->auth_option);
                env->CallVoidMethod(object, method_onAuthChallInd,
                                                    env->NewStringUTF((const char*)ind_p->dev_name),
                                                    (ind_p->auth_option&0x1) ? JNI_TRUE : JNI_FALSE,
                                                    (ind_p->auth_option&0x2) ? JNI_TRUE : JNI_FALSE);
            }
                break;
            case MSG_ID_BT_PBAP_SET_PATH_IND:
            {
                bt_pbap_set_path_ind_struct *ind_p = (bt_pbap_set_path_ind_struct*)ilm.ilm_data;
                ALOGI("MSG_ID_BT_PBAP_SET_PATH_IND : type=%d, strlen=%d", ind_p->type, bt_ucs2strlen((const char*)ind_p->name));
                if(ind_p->type == PBAP_ROOT_FOLDER)
                {
                    ALOGI("ind_p->type = PBAP_ROOT_FOLDER");
                    //ind_p->name[0] = '\0';
			              env->CallIntMethod(object,
			                                method_onSetPathInd,
			                                NULL,   // The message interface use UCS2
			                                ind_p->type);
                }
                else
               	{
                env->CallIntMethod(object,
                                                        method_onSetPathInd,
                                                        env->NewString((const jchar*)ind_p->name, bt_ucs2strlen((const char*)ind_p->name)),   // The message interface use UCS2
                                                        ind_p->type);
            }
            }
                break;
            case MSG_ID_BT_PBAP_READ_ENTRY_IND:
            {
                int i;
                jlong filter;
                bt_pbap_read_entry_ind_struct *ind_p = (bt_pbap_read_entry_ind_struct*)ilm.ilm_data;
                
                for(i = 0, filter = 0;i < 8;i++)
                    filter += (((jlong)ind_p->filter.byte[i])<<(i<<3));
                env->CallIntMethod(object,
                                                    method_onPullVcardEntryInd,
                                                    nat->con_id,
                                                    env->NewString((const jchar*)ind_p->objectName, bt_ucs2strlen((const char*)ind_p->objectName)),
                                                    filter,
                                                    ind_p->format == VCARD_FORMAT_21 ? JNI_TRUE : JNI_FALSE);
            }
                break;
            case MSG_ID_BT_PBAP_READ_FOLDER_IND:
            {
                int i;
                jlong filter;
                bt_pbap_read_folder_ind_struct *ind_p = (bt_pbap_read_folder_ind_struct*)ilm.ilm_data;

                for(i = 0, filter = 0;i < 8;i++)
                    filter += (((jlong)ind_p->filter.byte[i])<<(i<<3));
                env->CallIntMethod(object,
                                                    method_onPullPhonebookInd,
                                                    nat->con_id,
                                                    env->NewString((const jchar*)ind_p->pbName, bt_ucs2strlen((const char*)ind_p->pbName)),
                                                    filter,
                                                    ind_p->format == VCARD_FORMAT_21 ? JNI_TRUE : JNI_FALSE,
                                                    ind_p->maxListCount,
                                                    ind_p->listStartOffset);
            }
                break;
            case MSG_ID_BT_PBAP_READ_CONTINUE_IND:
                env->CallIntMethod(object,
                                                    method_onPullPhonebookContinueInd,
                                                    nat->con_id);
                break;
            case MSG_ID_BT_PBAP_READ_LIST_IND:
            {
                bt_pbap_read_list_ind_struct *ind_p = (bt_pbap_read_list_ind_struct*)ilm.ilm_data;
                ALOGI("MSG_ID_BT_PBAP_READ_LIST_IND : searchValue=%s", ind_p->searchValue);
                env->CallIntMethod(object,
                                                    method_onPullVcardListingInd,
                                                    nat->con_id,
                                                    env->NewString((const jchar*)ind_p->folderName, bt_ucs2strlen((const char*)ind_p->folderName)),
                                                    ind_p->order,
                                                    env->NewStringUTF((const char*)ind_p->searchValue),
                                                    ind_p->searchAttribute,
                                                    ind_p->maxListCount,
                                                    ind_p->listStartOffset);
            }
                break;
            case MSG_ID_BT_PBAP_ABORT_IND:
                env->CallVoidMethod(object, method_onAbortInd);
                break;
            case MSG_ID_BT_PBAP_OPER_SUCCESS_IND:
                break;
            default:
                ALOGE("Unexpected message : %lu", ilm.msg_id);
                break;
            }
        }
    }
    else if(res == 0)
    {
        ALOGW("[ERR] timeout waiting indication");
    }
    else
    {
        ALOGE("[ERR] select failed : %s, %d", strerror(errno), errno);
    }
    return (res>0) ? JNI_TRUE : JNI_FALSE;
}

static jboolean authorizeRspNative(JNIEnv* env, jobject object, jboolean accept)
{
    native_data_t *nat;

    ALOGI("[API] authorizeRspNative(%d)", accept?1:0);
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }
    if( btmtk_pbap_send_authorize_rsp(nat->apisock, (accept ? PBAP_CNF_SUCCESS : PBAP_CNF_FAILED)) )
    {
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

static jboolean authChallengeRspNative(JNIEnv* env, jobject object, 
                                                                              jboolean cancel,
										    jstring password,
										    jstring userID)
{
    native_data_t *nat;
    const char *c_password, *c_userID;
    jboolean ret = JNI_FALSE;

    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }
    ALOGI("[API] authChallengeRspNative : password=%p, userID=%p", password, userID);
    c_password = c_userID = NULL;
    if(password != NULL)
    c_password = env->GetStringUTFChars(password, NULL);
    if(userID != NULL)
    c_userID = env->GetStringUTFChars(userID, NULL);
    ALOGI("[API] authChallengeRspNative(%d, %s, %s)", cancel?1:0, c_password?c_password:"NULL", c_userID?c_userID:"NULL");
    if( btmtk_pbap_send_obex_auth_challege_rsp(nat->apisock, cancel?1:0, c_password, c_password?strlen(c_password):0, c_userID, c_userID?strlen(c_userID):0) )
    {
        ret = JNI_TRUE;
    }
    if(password != NULL)
    env->ReleaseStringUTFChars(userID, c_userID);
    if(userID != NULL)
    env->ReleaseStringUTFChars(password, c_password);
    return ret;
}

static jboolean setPathRspNative(JNIEnv* env, jobject object, jint rsp)
{
    native_data_t *nat;

    ALOGI("[API] setPathRspNative(%d)", rsp);
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }

    if( btmtk_pbap_send_set_path_rsp(nat->apisock, rsp) )
    {
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

// if vcard == null, 
static jboolean pullPhonebookRspNative(JNIEnv* env, jobject object, jint rsp, jint pbSize, jint newMissed, jstring vcard, jboolean cont)
{
    native_data_t *nat;
    const jchar *c_vcard = NULL;
    jboolean ret = JNI_FALSE;

    ALOGI("[API] pullPhonebookRspNative(%d, %d, %d, %d)", rsp, pbSize, newMissed, cont?1:0); 
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }
    
    if(vcard)
    {
        c_vcard = env->GetStringChars(vcard, NULL);
        if (btmtk_pbap_send_read_folder_rsp(nat->apisock, rsp, pbSize, newMissed, (U8*)c_vcard, sizeof(jchar)*env->GetStringLength(vcard), cont?1:0))
            ret = JNI_TRUE;
        env->ReleaseStringChars(vcard, c_vcard);
    }
    else
    {
        ALOGI("vcard=null");
        if(btmtk_pbap_send_read_folder_rsp(nat->apisock, rsp, pbSize, newMissed, NULL, 0, 0))
            ret = JNI_TRUE;
    }
    return ret;
}

static jboolean pullVcardListingRspNative(JNIEnv* env, jobject object, jint rsp, jint pbSize, jint newMissed, jstring vcardList, jboolean cont)
{
    native_data_t *nat;
    jboolean ret = JNI_FALSE;
    //const char *c_vcardList = NULL;
    const jchar *c_vcardList = NULL;    // the format of file name is wide-char
    
    ALOGI("[API] pullVcardListingRspNative(%d, %d, %d, %d)", rsp, pbSize, newMissed, cont?1:0);
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }
    if(IsValidRspCode(rsp) && vcardList)
    {
        c_vcardList = env->GetStringChars(vcardList, NULL);
        if (btmtk_pbap_send_read_list_rsp(nat->apisock, rsp, pbSize, newMissed, (U8*)c_vcardList, sizeof(jchar)*env->GetStringLength(vcardList), cont?1:0))
            ret = JNI_TRUE;
        env->ReleaseStringChars(vcardList, c_vcardList);
    }
    else
    {
        ALOGI("vcardList=null");
        if(btmtk_pbap_send_read_list_rsp(nat->apisock, rsp, pbSize, newMissed, NULL, 0, 0))
            ret = JNI_TRUE;
    }
    return ret;    
}

static jboolean pullVcardEntryRspNative(JNIEnv* env, jobject object, jint rsp, jstring vcard, jboolean cont)
{
    native_data_t *nat;
    const jchar *c_vcard = NULL;
    jboolean ret = JNI_FALSE;

    ALOGI("[API] pullVcardListingRspNative(%d)", rsp);
    nat = get_native_data(env, object);
    if(!nat)
    {
        ALOGE("[ERR] nat is null");
        return JNI_FALSE;
    }
    if(vcard)
    {
        c_vcard = env->GetStringChars(vcard, NULL);
        if (btmtk_pbap_send_read_entry_rsp(nat->apisock, rsp, (U8*)c_vcard, sizeof(jchar)*env->GetStringLength(vcard), cont?1:0))
            ret = JNI_TRUE;
        env->ReleaseStringChars(vcard, c_vcard);
    }
    else
    {
        ALOGI("vcard=null");
        rsp = PBAP_CNF_FAILED;
        if (btmtk_pbap_send_read_entry_rsp(nat->apisock, rsp, NULL, 0, 0))        
            ret = JNI_TRUE;
    }
    return ret;
}

/**
 * Table of methods associated with BluetoothPbapServer.
 */
static JNINativeMethod sMethods[] = {
    {"classInitNative", "()V",                                                (void*)classInitNative},
    {"initializeNativeDataNative", "()V",                               (void *)initializeNativeDataNative},
    {"cleanupNativeDataNative", "()V",                                (void *)cleanupNativeDataNative},
    { "enableNative", "()Z",                                                  (void*) enableNative },
    { "wakeupListenerNative", "()V",                                                 (void*) wakeupListenerNative },
    { "disableNative", "()V",                                                 (void*) disableNative },
    { "disconnectNative", "()V",                                            (void*) disconnectNative },
    { "listenIndicationNative", "(Z)Z",                                      (void*) listenIndicationNative },
    { "connectRspNative", "(IZ)Z",                                            (void*) connectRspNative },
    { "authorizeRspNative", "(Z)Z",                                       (void*) authorizeRspNative },
    { "authChallengeRspNative", "(ZLjava/lang/String;Ljava/lang/String;)Z", (void*) authChallengeRspNative },
    { "setPathRspNative", "(I)Z",                                           (void*) setPathRspNative },
    { "pullPhonebookRspNative", "(IIILjava/lang/String;Z)Z",    (void*) pullPhonebookRspNative },
    { "pullVcardListingRspNative", "(IIILjava/lang/String;Z)Z",  (void*) pullVcardListingRspNative },
    { "pullVcardEntryRspNative", "(ILjava/lang/String;Z)Z",    (void*) pullVcardEntryRspNative },
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
    if (!registerNativeMethods(env, "com/mediatek/bluetooth/pbap/BluetoothPbapServer",
            sMethods, sizeof(sMethods) / sizeof(sMethods[0])))
        return JNI_FALSE;
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
        goto bail;
    }
    assert(env != NULL);

    if (!registerNatives(env)) {
        ALOGE("ERROR: BluetoothPbapServer native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;
bail:
    ALOGI("-JNI_OnLoad: %d", result);
    return result;
}
