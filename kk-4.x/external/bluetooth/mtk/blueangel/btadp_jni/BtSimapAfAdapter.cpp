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

#define LOG_TAG "BT_SIMAP_AFADP"

#include "utils/Log.h"

#include "JNIHelp.h"
#include "jni.h"

#include "BtSimapAfAdapter.h"

#define SIMAP_OP_CONNECT_SIM      0
#define SIMAP_OP_DISCONNECT_SIM   1
#define SIMAP_OP_POWER_ON_SIM     2
#define SIMAP_OP_POWER_OFF_SIM    3
#define SIMAP_OP_RESET_SIM        4
#define SIMAP_OP_APDU_REQ         5

#define SIM_HEXSTR_BUF_LEN        528   // this value is from BtSimapRILAdapter.cpp

extern JavaVM* g_jvm;                   // It should be defined in extsimap_jni.cpp
extern jobject g_service_object;        // It should be defined in extsimap_jni.cpp
extern jmethodID cb_onBtSimapCmd;       // It should be defined in extsimap_jni.cpp

char hex_table[] = "0123456789ABCDEF";

static jboolean g_bLock = JNI_FALSE;
static jboolean g_bDisconnected = JNI_TRUE;

#if 0
void dumpData(void *p, int size) {
    int i;
    ALOGI("Addr = 0x%08X, size = 0x%08X", (int)p, size);
    
    if (p) {
        for (i = 0; i < size; i++) {
            ALOGI("   p[%d]: 0x%02X", i, ((char*)p)[i]);
        }
    }
}
#endif

static char * byte_array_to_hex_string(char array[], int alen, char *str, int slen) {
    int           i;
    unsigned char ch;
    char         *p;

    if (  array  &&  str  &&  slen >= (alen * 2 + 3)  ) {
        i = 0;
        while(i < alen) {
            ch = array[i];
            p = (str + i*2);
            *p     = hex_table[ch >>  4];
            *(p+1) = hex_table[ch & 0xF];
            i++;
        }
        str[i*2    ] = '\0';

        return str;
    }
    return NULL;
}

int afAdpter_getResultCode(JNIEnv* env, jclass jclazz, jobject jobj) {
    static jmethodID jmid = NULL;
    jint             iResultCode;

    if (NULL == jmid) {
        jmid = env->GetMethodID(jclazz, "getResultCode", "()I");
        if (NULL == jmid) {
            ALOGI("ERROR: getResultCode(): NULL jmid!");
            return ERR_OUT_OF_MEMORY;
        }
    }

    iResultCode = env->CallIntMethod(jobj, jmid);

    return (int)iResultCode;
}

int afAdpter_getCurType(JNIEnv* env, jclass jclazz, jobject jobj) {
    static jmethodID jmid = NULL;
    jint             iCurType;

    if (NULL == jmid) {
        jmid = env->GetMethodID(jclazz, "getCurType", "()I");
        if (NULL == jmid) {
            ALOGI("ERROR: getCurType(): NULL jmid!");
            return UNKNOWN_PROTOCOL_TYPE;
        }
    }

    iCurType = env->CallIntMethod(jobj, jmid);

    return (int)iCurType;
}

int afAdpter_getSupportType(JNIEnv* env, jclass jclazz, jobject jobj) {
    static jmethodID jmid = NULL;
    jint             iSupportType;

    if (NULL == jmid) {
        jmid = env->GetMethodID(jclazz, "getSupportType", "()I");
        if (NULL == jmid) {
            ALOGI("ERROR: getSupportType(): NULL jmid!");
            return UNKNOWN_PROTOCOL_TYPE;
        }
    }

    iSupportType = env->CallIntMethod(jobj, jmid);

    return (int)iSupportType;
}

jstring afAdpter_getAtrString(JNIEnv* env, jclass jclazz, jobject jobj) {
    static jmethodID jmid = NULL;
    jstring          jstr;
    //const char *utfChars = NULL;

    if (NULL == jmid) {
        jmid = env->GetMethodID(jclazz, "getAtrString", "()Ljava/lang/String;");
        if (NULL == jmid) {
            ALOGI("ERROR: getAtrString(): NULL jmid!");
            return NULL;
        }
    }

    jstr = (jstring) (env->CallObjectMethod(jobj, jmid));

    if (NULL == jstr) {
        ALOGI("ERROR: getAtrString(): NULL jstr!");
/*
    } else {
        utfChars = env->GetStringUTFChars(jstr, NULL);
        if (NULL == utfChars) {
            ALOGI("ERROR: getAtrString(): NULL utfChars!");
        } else {
            env->ReleaseStringUTFChars(jstr, utfChars);
        }

        env->DeleteLocalRef(jstr);
*/
    }
    return jstr;

}

jstring afAdpter_getApduString(JNIEnv* env, jclass jclazz, jobject jobj) {
    static jmethodID jmid = NULL;
    jstring          jstr;

    if (NULL == jmid) {
        jmid = env->GetMethodID(jclazz, "getApduString", "()Ljava/lang/String;");
        if (NULL == jmid) {
            ALOGI("ERROR: getApduString(): NULL jmid!");
            return NULL;
        }
    }

    jstr = (jstring) (env->CallObjectMethod(jobj, jmid));

    if (NULL == jstr) {
        ALOGI("ERROR: getApduString(): NULL jstr!");
    }
    return jstr;

}

static char * locate_hex_string(const char *utfChars, int *pHexStrLen) {
    char *pStart;
    char *pEnd;
    int   len = 0;

    pStart = strchr(utfChars, '"');
    if (NULL != pStart) {
        pStart++;
        pEnd = strchr(pStart, '"');
        if (NULL != pEnd) {
            len = pEnd - pStart;
        } else {
            pStart = NULL;
        }
    }
    if (NULL != pHexStrLen) {
        *pHexStrLen = len;
    }
    return pStart;
}

static int hex_char_to_digit(char ch)
{
	int dgt = 0;
	
	if (ch >= '0' && ch <= '9')
		dgt = ch - '0';
	else if (ch >= 'A' && ch <= 'Z')
		dgt = (ch - 'A') + 10;
	else if (ch >= 'a' && ch <= 'z')
		dgt = (ch - 'a') + 10;

	return dgt;
}

static void hex_string_to_byte_array(char *str, int slen, char array[], int &alen) {
    int i;

    if (NULL == str || NULL == array || slen <= 0 || alen <= 0) {
        alen = 0;
    } else {
        if (slen & 1) {
            ALOGI("WARNING: hexStr2array(): odd hex string length!");
        }
        if ((slen / 2) < alen) {
            alen = slen / 2;
        } else {
            ALOGI("WARNING: hexStr2array(): array size is not enough!");
        }
        for (i = 0; i < alen; i++) {
            array[i] = (char) (
                hex_char_to_digit(str[i * 2    ]) * 16 +
                hex_char_to_digit(str[i * 2 + 1])
            );
        }
    }
}

static void RilRspStr_get_byte_array(JNIEnv* env, jstring jstr, char array[], int &alen) {
    const char *utfChars = NULL;
    char       *hexStr;
    int         nHexStrLen = 0;

    if (NULL == jstr) {
        alen = 0;
    } else {
        utfChars = env->GetStringUTFChars(jstr, NULL);
        if (NULL == utfChars) {
            ALOGI("ERROR: getAtrString(): NULL utfChars!");
            alen = 0;
        } else {
            hexStr = locate_hex_string(utfChars, &nHexStrLen);
            hex_string_to_byte_array(hexStr, nHexStrLen, array, alen);

            env->ReleaseStringUTFChars(jstr, utfChars);
        }

        env->DeleteLocalRef(jstr);
    }
}

#if 0
static void wait_n_lock() {
    int count = 0;
    while (JNI_TRUE == g_bLock && count < 5) {
        sleep(1);
        count++;
        ALOGI("in lock: %d", count);
    }
    g_bLock = JNI_TRUE;
}

static void unlock() {
    g_bLock = JNI_FALSE;
}
#else
#define wait_n_lock()
#define unlock()
#endif

BT_SIMAP_RESULT_CODE BT_SIMAP_Connect_SIM(BT_SIMAP_SIM_ID simId,  int& curType, int& supportType, char ATR[], int& atrLen) {
    JNIEnv*              env = NULL;
    AF_SIM_ID            afSimID;
    jobject              joResult;
    BT_SIMAP_RESULT_CODE resultCode;

    ALOGI("BT_SIMAP_Connect_SIM(%d, %d)", simId, atrLen);

    wait_n_lock();

    if (JNI_FALSE == g_bDisconnected) {
        ALOGI("ERROR: 0. already connected");
        unlock();
        return ERR_ALREADY_CONNECTED;
    }

	if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	    ALOGE("ERROR: 0. GetEnv failed\n");
        unlock();
	    return ERR_OUT_OF_MEMORY;
	}

    switch (simId) {
    case SIM1:
        afSimID = AF_SIM1;
        break;
    case SIM2:
        afSimID = AF_SIM2;
        break;
	case SIM3:
		afSimID = AF_SIM3;
		break;
	case SIM4:
		afSimID = AF_SIM4;
		break;
    default:
        ALOGI("ERROR: 0. Invalid SIM ID: %d", simId);
        unlock();
        return ERR_NOT_SUPPORTED;
    }

    ALOGI("afSimID: %d", afSimID);

    joResult = env->CallObjectMethod(g_service_object,
                   cb_onBtSimapCmd, SIMAP_OP_CONNECT_SIM, afSimID, NULL);

    if (NULL == joResult) {
        ALOGI("Error: 0. NULL result");
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    jclass jclazz = env->GetObjectClass(joResult);
    if (NULL == jclazz) {
        ALOGI("Error: 0. NULL jclazz");
        env->DeleteLocalRef(joResult);
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    resultCode = (BT_SIMAP_RESULT_CODE)afAdpter_getResultCode(env, jclazz, joResult);
    ALOGI("resultCode: %d", resultCode);

    curType = afAdpter_getCurType(env, jclazz, joResult);
    supportType = afAdpter_getSupportType(env, jclazz, joResult);
    ALOGI("curType: %d", curType);
    ALOGI("supportType: %d", supportType);
    RilRspStr_get_byte_array(
        env,
        afAdpter_getAtrString(env, jclazz, joResult),
        ATR,
        atrLen
    );
    ALOGI("atrLen: %d", atrLen);
    #if 1
    {
        int xxi;
        for (xxi = 0; xxi < atrLen; xxi++) {
            ALOGI("  0x%02X", ATR[xxi]);
        }
    }
    #endif

    env->DeleteLocalRef(jclazz);
    env->DeleteLocalRef(joResult);
    if (SUCCESS == resultCode) {
        g_bDisconnected = JNI_FALSE;
    }
    unlock();
    return resultCode;
}

BT_SIMAP_RESULT_CODE BT_SIMAP_Disconnect_SIM(void) {
    JNIEnv*              env = NULL;
    jobject              joResult;
    BT_SIMAP_RESULT_CODE resultCode;

    ALOGI("BT_SIMAP_Disconnect_SIM()");

    wait_n_lock();

    if (JNI_TRUE == g_bDisconnected) {
        ALOGI("ERROR: 1. not connected");
        unlock();
        return ERR_NOT_CONNECTED;
    }

	if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	    ALOGE("ERROR: 1. GetEnv failed\n");
        unlock();
	    return ERR_OUT_OF_MEMORY;
	}

    joResult = env->CallObjectMethod(g_service_object,
                   cb_onBtSimapCmd, SIMAP_OP_DISCONNECT_SIM, 0, NULL);

    if (NULL == joResult) {
        ALOGI("Error: 1. NULL result");
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    jclass jclazz = env->GetObjectClass(joResult);
    if (NULL == jclazz) {
        ALOGI("Error: 1. NULL jclazz");
        env->DeleteLocalRef(joResult);
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    resultCode = (BT_SIMAP_RESULT_CODE)afAdpter_getResultCode(env, jclazz, joResult);
    ALOGI("resultCode: %d", resultCode);

    env->DeleteLocalRef(jclazz);
    env->DeleteLocalRef(joResult);
    if (SUCCESS == resultCode) {
        g_bDisconnected = JNI_TRUE;
    }
    unlock();
    return resultCode;
}

BT_SIMAP_RESULT_CODE BT_SIMAP_APDU_req(int type, char cmdAPDU[], int cmdLen,  char rspAPDU[], int& rspLen) {
    JNIEnv*              env = NULL;
    jobject              joResult;
    jstring              jstr_apdu;
    char                 cstr_apdu[SIM_HEXSTR_BUF_LEN];
    BT_SIMAP_RESULT_CODE resultCode;

    ALOGI("BT_SIMAP_APDU_req(%d, %d, %d)", type, cmdLen, rspLen);
    #if 1
    {
        int xxi;
        for (xxi = 0; xxi < cmdLen; xxi++) {
            ALOGI("  0x%02X", cmdAPDU[xxi]);
        }
    }
    #endif

    wait_n_lock();

    if (JNI_TRUE == g_bDisconnected) {
        ALOGI("ERROR: 5. not connected");
        unlock();
        return ERR_NOT_CONNECTED;
    }

	if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	    ALOGE("ERROR: 5. GetEnv failed\n");
        unlock();
	    return ERR_OUT_OF_MEMORY;
	}

    cstr_apdu[0] = '\0';
    if (NULL == byte_array_to_hex_string(cmdAPDU, cmdLen, cstr_apdu, sizeof(cstr_apdu)/sizeof(cstr_apdu[0]))) {
        ALOGI("Error: 5. cmdAPDU = 0x%08X, cmdLen = %d", (unsigned int)cmdAPDU, cmdLen);
        unlock();
        return ERR_NOT_SUPPORTED;
    }

    jstr_apdu = env->NewStringUTF((const char *)cstr_apdu);

    if (NULL == jstr_apdu) {
        ALOGI("Error: 5. NULL new string");
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    joResult = env->CallObjectMethod(g_service_object,
                   cb_onBtSimapCmd, SIMAP_OP_APDU_REQ, type, jstr_apdu);

    env->DeleteLocalRef(jstr_apdu);

    if (NULL == joResult) {
        ALOGI("Error: 5. NULL result");
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    jclass jclazz = env->GetObjectClass(joResult);
    if (NULL == jclazz) {
        ALOGI("Error: 5. NULL jclazz");
        env->DeleteLocalRef(joResult);
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    resultCode = (BT_SIMAP_RESULT_CODE)afAdpter_getResultCode(env, jclazz, joResult);
    ALOGI("resultCode: %d", resultCode);

    RilRspStr_get_byte_array(
        env,
        afAdpter_getApduString(env, jclazz, joResult),
        rspAPDU,
        rspLen
    );
    ALOGI("rspLen: %d", rspLen);
    #if 1
    {
        int xxi;
        for (xxi = 0; xxi < rspLen; xxi++) {
            ALOGI("  0x%02X", rspAPDU[xxi]);
        }
    }
    #endif

    env->DeleteLocalRef(jclazz);
    env->DeleteLocalRef(joResult);
    unlock();
    return resultCode;
}

BT_SIMAP_RESULT_CODE BT_SIMAP_Reset_SIM(int type,  int& curType, char ATR[], int& atrLen) {
    JNIEnv*              env = NULL;
    jobject              joResult;
    BT_SIMAP_RESULT_CODE resultCode;

    ALOGI("BT_SIMAP_Reset_SIM(%d, %d)", type, atrLen);

    wait_n_lock();

    if (JNI_TRUE == g_bDisconnected) {
        ALOGI("ERROR: 4. not connected");
        unlock();
        return ERR_NOT_CONNECTED;
    }

	if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	    ALOGE("ERROR: 4. GetEnv failed\n");
        unlock();
	    return ERR_OUT_OF_MEMORY;
	}

    joResult = env->CallObjectMethod(g_service_object,
                   cb_onBtSimapCmd, SIMAP_OP_RESET_SIM, type, NULL);
    if (NULL == joResult) {
        ALOGI("Error: 4. NULL result");
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    jclass jclazz = env->GetObjectClass(joResult);
    if (NULL == jclazz) {
        ALOGI("Error: 4. NULL jclazz");
        env->DeleteLocalRef(joResult);
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    resultCode = (BT_SIMAP_RESULT_CODE)afAdpter_getResultCode(env, jclazz, joResult);
    ALOGI("resultCode: %d", resultCode);

    curType = afAdpter_getCurType(env, jclazz, joResult);
    ALOGI("curType: %d", curType);
    RilRspStr_get_byte_array(
        env,
        afAdpter_getAtrString(env, jclazz, joResult),
        ATR,
        atrLen
    );
    ALOGI("atrLen: %d", atrLen);
    #if 1
    {
        int xxi;
        for (xxi = 0; xxi < atrLen; xxi++) {
            ALOGI("  0x%02X", ATR[xxi]);
        }
    }
    #endif

    env->DeleteLocalRef(jclazz);
    env->DeleteLocalRef(joResult);
    unlock();
    return resultCode;
}

BT_SIMAP_RESULT_CODE BT_SIMAP_PowerOn_SIM(int type,  int& curType, char ATR[], int& atrLen) {
    JNIEnv*              env = NULL;
    jobject              joResult;
    BT_SIMAP_RESULT_CODE resultCode;

    ALOGI("BT_SIMAP_PowerOn_SIM(%d, %d)", type, atrLen);

    wait_n_lock();

    if (JNI_TRUE == g_bDisconnected) {
        ALOGI("ERROR: 2. not connected");
        unlock();
        return ERR_NOT_CONNECTED;
    }

	if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	    ALOGE("ERROR: 2. GetEnv failed\n");
        unlock();
	    return ERR_OUT_OF_MEMORY;
	}

    joResult = env->CallObjectMethod(g_service_object,
                   cb_onBtSimapCmd, SIMAP_OP_POWER_ON_SIM, type, NULL);

    if (NULL == joResult) {
        ALOGI("Error: 2.NULL result");
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    jclass jclazz = env->GetObjectClass(joResult);
    if (NULL == jclazz) {
        ALOGI("Error: 2. NULL jclazz");
        env->DeleteLocalRef(joResult);
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    resultCode = (BT_SIMAP_RESULT_CODE)afAdpter_getResultCode(env, jclazz, joResult);
    ALOGI("resultCode: %d", resultCode);

    curType = afAdpter_getCurType(env, jclazz, joResult);
    ALOGI("curType: %d", curType);
    RilRspStr_get_byte_array(
        env,
        afAdpter_getAtrString(env, jclazz, joResult),
        ATR,
        atrLen
    );
    ALOGI("atrLen: %d", atrLen);
    #if 1
    {
        int xxi;
        for (xxi = 0; xxi < atrLen; xxi++) {
            ALOGI("  0x%02X", ATR[xxi]);
        }
    }
    #endif

    env->DeleteLocalRef(jclazz);
    env->DeleteLocalRef(joResult);
    unlock();
    return resultCode;
}

BT_SIMAP_RESULT_CODE BT_SIMAP_PowerOff_SIM(void) {
    JNIEnv*              env = NULL;
    jobject              joResult;
    BT_SIMAP_RESULT_CODE resultCode;

    ALOGI("BT_SIMAP_PowerOff_SIM()");

    wait_n_lock();

    if (JNI_TRUE == g_bDisconnected) {
        ALOGI("ERROR: 3. not connected");
        unlock();
        return ERR_NOT_CONNECTED;
    }

	if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
	    ALOGE("ERROR: 3. GetEnv failed\n");
        unlock();
	    return ERR_OUT_OF_MEMORY;
	}

    joResult = env->CallObjectMethod(g_service_object,
                   cb_onBtSimapCmd, SIMAP_OP_POWER_OFF_SIM, 0, NULL);

    if (NULL == joResult) {
        ALOGI("Error: 3. NULL result");
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    jclass jclazz = env->GetObjectClass(joResult);
    if (NULL == jclazz) {
        ALOGI("Error: 3. NULL jclazz");
        env->DeleteLocalRef(joResult);
        unlock();
        return ERR_OUT_OF_MEMORY;
    }

    resultCode = (BT_SIMAP_RESULT_CODE)afAdpter_getResultCode(env, jclazz, joResult);
    ALOGI("resultCode: %d", resultCode);

    env->DeleteLocalRef(jclazz);
    env->DeleteLocalRef(joResult);
    unlock();
    return resultCode;
}
