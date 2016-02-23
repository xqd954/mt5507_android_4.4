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

#include "cutils/sockets.h"

#include <cutils/xlog.h>
#include <bt_simulator.h>

#define PBAP_UTLOG(fmt, ...) XLOGI("[BT][PBAPUT][EXT]%s:" fmt, __FUNCTION__, ## __VA_ARGS__)

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
    PBAP_UTLOG("+classInitNative");
    method_onAuthorizeInd = env->GetMethodID(clazz, "onAuthorizeInd", "(Ljava/lang/String;)I");
    method_onConnectInd = env->GetMethodID(clazz, "onConnectInd", "(Ljava/lang/String;Ljava/lang/String;I)I");
    method_onDisconnectInd = env->GetMethodID(clazz, "onDisconnectInd", "()V");
    method_onSetPathInd = env->GetMethodID(clazz, "onSetPathInd", "(Ljava/lang/String;I)I");
    method_onPullVcardEntryInd = env->GetMethodID(clazz, "onPullVcardEntryInd", "(ILjava/lang/String;JZ)I");
    method_onPullPhonebookInd = env->GetMethodID(clazz, "onPullPhonebookInd", "(ILjava/lang/String;JZII)I");
    method_onPullVcardListingInd = env->GetMethodID(clazz, "onPullVcardListingInd", "(ILjava/lang/String;ILjava/lang/String;III)I");
    method_onAbortInd = env->GetMethodID(clazz, "onAbortInd", "()V");
    method_onAuthChallInd = env->GetMethodID(clazz, "onAuthChallInd", "(Ljava/lang/String;ZZ)V");
    
    field_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");
    PBAP_UTLOG("-classInitNative");
}

static void initializeNativeDataNative(JNIEnv* env, jobject object) 
{
    native_data_t *nat = (native_data_t *)calloc(1, sizeof(native_data_t));

    PBAP_UTLOG("[API]");
    if (NULL == nat) {
        PBAP_UTLOG("%s: out of memory!");
        return;
    }
    memset(nat, 0, sizeof(native_data_t));
    env->SetIntField(object, field_mNativeData, (jint)nat);
    PBAP_UTLOG("- finished");
	BTUTLog_next(UT_PROFILE_PBAP);
}

static void cleanupNativeDataNative(JNIEnv* env, jobject object) 
{
    native_data_t *nat = (native_data_t *)env->GetIntField(object, field_mNativeData);

    PBAP_UTLOG("[API] ");
    if (nat) {
        free(nat);
    }
    clean_up(UT_PROFILE_PBAP);
}
static jboolean enableNative(JNIEnv* env, jobject object)
{
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    jboolean bResult;
    if (curLog == NULL) return JNI_FALSE;
    
    PBAP_UTLOG("Value = %s", (curLog->retv));
    bResult = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    PBAP_UTLOG("JNI return %d", bResult);
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 0, "");
    return bResult;
}

static void wakeupListenerNative(JNIEnv* env, jobject object)
{
    PBAP_UTLOG("JNI");
    BTUTLog_next(UT_PROFILE_PBAP);
	//setEvent(env, object, PBAP_WAKEUP_EVENT);
}

static void disableNative(JNIEnv* env, jobject object)
{
    PBAP_UTLOG("JNI ");
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 9, "");
}

static jboolean connectRspNative(JNIEnv* env, jobject object, jint con_id, jboolean accept)
{
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    jboolean bResult;
    if (curLog == NULL) return JNI_TRUE;

    bResult = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    const char* resultString = (accept == JNI_TRUE)?"true":"false";
    PBAP_UTLOG("JNI return %d", bResult);
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 1, resultString);
    return bResult;
}

static void disconnectNative(JNIEnv* env, jobject object)
{
    PBAP_UTLOG("JNI ");
    BTUTLog_next(UT_PROFILE_PBAP);
}



/* -1 : failed */
static jboolean listenIndicationNative(JNIEnv* env, jobject object, jboolean noWait)
{
	int curCbFunc = -1;
	UTJniLog* curLog=NULL;
	char* cbFuncs[] = { 
                            "method_onAuthorizeInd",
    						"method_onConnectInd",
    						"method_onDisconnectInd",
    						"method_onAuthChallInd",
    						"method_onSetPathInd",
    						"method_onPullVcardEntryInd",
    						"method_onPullPhonebookInd",
    						"method_onPullVcardListingInd",
    						"method_onAbortInd",

                            //return false log
                            "method_PbaplistenIndicationNativeReturnFalse"
					    };

	int  cbFuncNum = 10;

	//get current callback function num.then goto the correct case.
	curCbFunc = BTUTLog_listenToCbFunc(cbFuncs,cbFuncNum);
	curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);

    if (curLog == NULL) return JNI_TRUE;
             
    switch(curCbFunc)
    {
        case 0:
        {
            char *addr= curLog ->params[0];

            PBAP_UTLOG("JNI Call back method_onAuthorizeInd param(%d) %s",curLog->paramLen,curLog->params[0]);

            BTUTLog_next(UT_PROFILE_PBAP);
            env->CallIntMethod(object, method_onAuthorizeInd,
                                            env->NewStringUTF((const char*)addr));
            PBAP_UTLOG("JNI method_onAuthorizeInd call back completed");
        }
            break;
        case 1:
        {
            char *addr = curLog ->params[0];
            char *name = curLog ->params[1];
            jint connectId = atoi(curLog ->params[2]);

            PBAP_UTLOG("JNI Call back method_onConnectInd param(%d) %s  %s  %s",
                curLog->paramLen,curLog->params[0], curLog ->params[1], curLog ->params[2]);

            BTUTLog_next(UT_PROFILE_PBAP);
            env->CallIntMethod(object, method_onConnectInd,
                                                    env->NewStringUTF((const char*)addr),
                                                    env->NewStringUTF((const char*)name),
                                                    connectId);
            PBAP_UTLOG("JNI method_onConnectInd call back completed");
        }
            break;
        case 2:
            PBAP_UTLOG("JNI Call back method_onDisconnectInd param(%d)", curLog->paramLen);

            BTUTLog_next(UT_PROFILE_PBAP);
            env->CallVoidMethod(object, method_onDisconnectInd);

            PBAP_UTLOG("JNI method_onDisconnectInd call back completed");
            break;
        case 3:
        {
            char *name = curLog ->params[0];
            jboolean isUserIdRequired = atoi(curLog ->params[1]);
            jboolean isFullAccess = atoi(curLog ->params[2]);

            PBAP_UTLOG("JNI Call back method_onAuthChallInd param(%d) %s  %s  %s",
                curLog->paramLen,curLog->params[0], curLog ->params[1], curLog ->params[2]);

            BTUTLog_next(UT_PROFILE_PBAP);
            env->CallVoidMethod(object, method_onAuthChallInd,
                                                env->NewStringUTF((const char*)name),
                                                isUserIdRequired,
                                                isFullAccess);
            PBAP_UTLOG("JNI method_onAuthChallInd call back completed");
        }
            break;
        case 4:
        {

            char * path = curLog ->params[0];
            jint type = atoi(curLog ->params[1]);
            
            PBAP_UTLOG("JNI Call back method_onSetPathInd param(%d) %s  %s ",
                curLog->paramLen,curLog->params[0], curLog ->params[1]);

            BTUTLog_next(UT_PROFILE_PBAP);
            env->CallIntMethod(object, method_onSetPathInd,
                                                    env->NewStringUTF((const char*)path),   // The message interface use UCS2
                                                    type);
            PBAP_UTLOG("JNI method_onSetPathInd call back completed");
        }
            break;
        case 5:
        {   
            jint conId = atoi(curLog ->params[0]);
            char* name = curLog ->params[1];
            jlong filter = atol(curLog ->params[2]);
            jboolean vcard21 = atoi(curLog ->params[3]);

            PBAP_UTLOG("JNI Call back method_onPullVcardEntryInd param(%d) %s  %s  %s  %s",
                curLog->paramLen, curLog->params[0], curLog ->params[1], curLog ->params[2], curLog ->params[3]);

            BTUTLog_next(UT_PROFILE_PBAP);    
            env->CallIntMethod(object, method_onPullVcardEntryInd,
                                                conId,
                                                env->NewStringUTF((const char*)name),
                                                filter,
                                                vcard21);
            PBAP_UTLOG("JNI method_onPullVcardEntryInd call back completed");
        }
            break;
        case 6:
        {
            jint conId = atoi(curLog ->params[0]);
            char* name = curLog ->params[1];
            jlong filter = atol(curLog ->params[2]);
            jboolean vcard21 = atoi(curLog ->params[3]);
            jint maxListCount = atoi(curLog ->params[4]);
            jint listStartOffset = atoi(curLog ->params[5]);

            PBAP_UTLOG("JNI Call back method_onPullPhonebookInd param(%d) %s  %s  %s  %s  %s  %s",
                curLog->paramLen, curLog->params[0], curLog ->params[1], curLog ->params[2], curLog ->params[3],
                curLog ->params[4], curLog ->params[5]);

            BTUTLog_next(UT_PROFILE_PBAP);
            env->CallIntMethod(object, method_onPullPhonebookInd,
                                                conId,
                                                env->NewStringUTF((const char*)name),
                                                filter,
                                                vcard21,
                                                maxListCount,
                                                listStartOffset);
            PBAP_UTLOG("JNI method_onPullPhonebookInd call back completed");
        }
            break;
        case 7:
        {
            jint conId = atoi(curLog ->params[0]);
            char* name= curLog ->params[1];
            jint order = atoi(curLog ->params[2]);
            char* searchVal = curLog ->params[3];
            jint searchAttr = atoi(curLog ->params[4]);
            jint maxListCount = atoi(curLog ->params[5]);
            jint listStartOffset = atoi(curLog ->params[6]);
            
            PBAP_UTLOG("JNI Call back method_onPullVcardListingInd param(%d) %s  %s  %s  %s  %s  %s  %s",
                curLog->paramLen, curLog->params[0], curLog ->params[1], curLog ->params[2], curLog ->params[3],
                curLog ->params[4], curLog ->params[5], curLog ->params[6]);

            BTUTLog_next(UT_PROFILE_PBAP);
            
            if (strcmp(searchVal, "empty") == 0) 
            {
                searchVal = "";
            }
            
            env->CallIntMethod(object, method_onPullVcardListingInd,
                                                conId,
                                                env->NewStringUTF((const char*)name),
                                                order,
                                                env->NewStringUTF((const char*)searchVal),
                                                searchAttr,
                                                maxListCount,
                                                listStartOffset);
            PBAP_UTLOG("JNI method_onPullVcardListingInd call back completed");
        }
            break;
        case 8:
            PBAP_UTLOG("JNI Call back method_onAbortInd param(%d)");

            BTUTLog_next(UT_PROFILE_PBAP);
            env->CallVoidMethod(object, method_onAbortInd);
            PBAP_UTLOG("JNI method_onAbortInd call back completed");
            break;
        case 9://no use
            return JNI_FALSE;
            break;
        default:
            //PBAP_UTLOG("Unexpected message : %s", curLog ->name);
            break;
    }
    
    return JNI_TRUE;
}

static jboolean authorizeRspNative(JNIEnv* env, jobject object, jboolean accept)
{
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    jboolean bResult;
    if (curLog == NULL) return JNI_TRUE;
    bResult = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    PBAP_UTLOG("JNI return %d", bResult);
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 3, accept?"true":"false");
    return bResult;
}

static jboolean authChallengeRspNative(JNIEnv* env, jobject object, 
                                                                              jboolean cancel,
										    jstring password,
										    jstring userID)
{
    const char *c_password, *c_userID;
    char checkLog[40] = "";
    jboolean ret;
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    if (curLog == NULL) return JNI_TRUE;
    
    
    ALOGI("[API] authChallengeRspNative : password=%p, userID=%p", password, userID);
    c_password = c_userID = NULL;
    if(password != NULL)
    c_password = env->GetStringUTFChars(password, NULL);
    if(userID != NULL)
    c_userID = env->GetStringUTFChars(userID, NULL);
    ALOGI("[API] authChallengeRspNative(%d, %s, %s)", cancel?1:0, c_password?c_password:"NULL", c_userID?c_userID:"NULL");
    
    strcat(checkLog, cancel?"true;":"false;");
    strcat(checkLog, c_password?c_password:"null");
    strcat(checkLog, ";");
    strcat(checkLog, c_userID?c_userID:"null");
    
    ret = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    PBAP_UTLOG("JNI return %d", ret);
    PBAP_UTLOG("JNI checkLog is (%s)", checkLog);
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 4, checkLog);

    if(password != NULL)
    env->ReleaseStringUTFChars(userID, c_userID);
    if(userID != NULL)
    env->ReleaseStringUTFChars(password, c_password);
    return ret;
}

static jboolean setPathRspNative(JNIEnv* env, jobject object, jint rsp)
{
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    jboolean bResult;
    if (curLog == NULL) return JNI_TRUE;
    
    bResult = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    PBAP_UTLOG("setPathRspNative is OK?", (rsp == 0)?"Yes":"No");
    PBAP_UTLOG("JNI return %d", bResult);
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 2, (rsp == 0)?"true":"false");;
    return bResult;
}

// if vcard == null, 
static jboolean pullPhonebookRspNative(JNIEnv* env, jobject object, jint rsp, jint pbSize, jint newMissed, jstring vcard)
{
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    jboolean ret;
    char checkLog[100] = ""; 
    char pbSizeString[10];
    char newMissedString[10];
    const char* vcardString;
    if (curLog == NULL) return JNI_TRUE;
    
    ret = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    PBAP_UTLOG("JNI return %d", ret);
    
    sprintf(pbSizeString, "%d", pbSize);
    sprintf(newMissedString, "%d", newMissed);
    if ((vcard != NULL)&&(rsp == 0))
    {
        vcardString = env->GetStringUTFChars(vcard, NULL);
    }
    else
    {
        vcardString = "null";
    }
    
    strcat(checkLog, (rsp == 0)?"Success":"Failed");
    strcat(checkLog, ",");
    strcat(checkLog, pbSizeString);
    strcat(checkLog, ",");
    strcat(checkLog, newMissedString);
    strcat(checkLog, ";");
    strcat(checkLog, vcardString);
    
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 5, checkLog);
    
    if ((vcard != NULL)&&(rsp == 0))
    {
        env->ReleaseStringUTFChars(vcard, vcardString);
    }
    return ret;
}

static jboolean pullVcardListingRspNative(JNIEnv* env, jobject object, jint rsp, jint pbSize, jint newMissed, jstring vcardList)
{
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    jboolean ret;
    char checkLog[100] = ""; 
    char pbSizeString[10];
    char newMissedString[10];
    const char* vcardString;
    if (curLog == NULL) return JNI_TRUE;
    
    ret = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    PBAP_UTLOG("JNI return %d", ret);
    
    sprintf(pbSizeString, "%d", pbSize);
    sprintf(newMissedString, "%d", newMissed);
    if ((vcardList != NULL)&&(rsp == 0))
    {
        vcardString = env->GetStringUTFChars(vcardList, NULL);
    }
    else
    {
        vcardString = "null";
    }
    
    PBAP_UTLOG("JNI return %d", ret);
    
    strcat(checkLog, (rsp == 0)?"Success":"Failed");
    strcat(checkLog, ",");
    strcat(checkLog, pbSizeString);
    strcat(checkLog, ",");
    strcat(checkLog, newMissedString);
    strcat(checkLog, ";");
    strcat(checkLog, vcardString);
    
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 5, checkLog);
    
    if ((vcardList != NULL)&&(rsp == 0))
    {
        env->ReleaseStringUTFChars(vcardList, vcardString);
    }
    return ret;
}

static jboolean pullVcardEntryRspNative(JNIEnv* env, jobject object, jint rsp, jstring vcard)
{
    UTJniLog* curLog = BTUTLog_getInfo(UT_PROFILE_PBAP);
    jboolean ret;
    char checkLog[100] = ""; 
    const char* vcardString;
    if (curLog == NULL) return JNI_TRUE;
    
    ret = (atoi(curLog->retv)==1)?JNI_TRUE : JNI_FALSE;
    PBAP_UTLOG("JNI return %d", ret);
    
    if ((vcard != NULL)&&(rsp == 0))
    {
        vcardString = env->GetStringUTFChars(vcard, NULL);
    }
    else
    {
        vcardString = "null";
    }
    
    PBAP_UTLOG("JNI return %d", ret);
    
    strcat(checkLog, (rsp == 0)?"Success":"Failed");
    strcat(checkLog, ";");
    strcat(checkLog, vcardString);
    
    BTUTLog_notifyStateChange(UT_PROFILE_PBAP, 5, checkLog);
    
    if ((vcard != NULL)&&(rsp == 0))
    {
        env->ReleaseStringUTFChars(vcard, vcardString);
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
    { "pullPhonebookRspNative", "(IIILjava/lang/String;)Z",    (void*) pullPhonebookRspNative },
    { "pullVcardListingRspNative", "(IIILjava/lang/String;)Z",  (void*) pullVcardListingRspNative },
    { "pullVcardEntryRspNative", "(ILjava/lang/String;)Z",    (void*) pullVcardEntryRspNative },
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
        ALOGI("Native registration unable to find class '%s'\n", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
        ALOGI("RegisterNatives failed for '%s'\n", className);
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
