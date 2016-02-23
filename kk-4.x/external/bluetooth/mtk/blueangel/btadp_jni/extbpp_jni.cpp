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

#include <jni.h>
#ifdef  __MTK_BT_EXTERNAL_PLATFORM__
#include "bt_ext_debug.h"
#else
#include <cutils/xlog.h>
#endif
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>

#include <cutils/sockets.h>

   
extern "C"
{
#include "bt_mmi.h"
#include "bt_message.h"

#include "bluetooth_bpp_common.h"
#include "bluetooth_bpp_struct.h"

#include "bt_bpp_porting.h"

#include "socket_local.h"
}

#undef LOG_TAG
#define LOG_TAG "JNI_BPP"

#define BT_SOCK_NAME_EXT_ADP_BPP "bt.ext.adp.bpp"


#define BPP_JNI_DEBUG 1

#ifdef BPP_JNI_DEBUG
#define BPP_DBG(fmt, ...) XLOGD("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)
#else
#define BPP_DBG(fmt, ...) {}
#endif
#define BPP_ERR(fmt, ...) XLOGE("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)
#define BPP_WRN(fmt, ...) XLOGW("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)

extern const kal_char* bpp_media_size_self_desc_name[];
extern bpp_mime_id_str_struct bpp_mime_type_table[];

typedef struct {
    JNIEnv *env;
    int apisock;
    int servsock;
    U32 con_id;
} native_data_t;


static JavaVM *g_jvm = NULL;
static jobject g_callback_object;

static jfieldID field_mNativeData;
static jmethodID method_onCallback; //Callback to MMI
//static jclass mStringClass;


static inline native_data_t * get_native_data(JNIEnv *env, jobject object) {
    return (native_data_t *)(env->GetIntField(object, field_mNativeData));
}

void bpp_event_callback(U8 event, S8** parameters, U8 count); //Callback from native

// ----------------------------------------------------------------------------
void CONVERT_STRING2BDADDR(const char *source, T_BDADDR dest)
{    

    BPP_DBG("[API] CONVERT_STRING2BDADDR  start");    
    //unsigned char addr[6];    
    int i;    
    char tmp;    
    char *ptr = (char*)source;    
    for(i = 6;i > 0;)    
    {        
        dest[--i] = strtoul(ptr, &ptr, 16);        
        ptr++;    
    }    
    BPP_DBG("[API] CONVERT_STRING2BDADDR : %s==>0x%02X%02X%02X%02X%02X%02X",    
        source,     
        dest[0],    
        dest[1],    
        dest[2],    
        dest[3],    
        dest[4],    
        dest[5]);
}


static int bpp_utf8_to_ucs2_char(unsigned short *ucs2, const char * utf8)
{
    char c = utf8[0];

    if (c < 0x80)
    {
        *ucs2 = c;
        return 1;
    }
    else if (c < 0xe0)
    {
        *ucs2 = ((unsigned short) (c & 0x1f) << 6) | (unsigned short) (utf8[1] ^ 0x80);
        return 2;
    }
    else
    {
        *ucs2 = ((unsigned short) (c & 0x0f) << 12) | ((unsigned short) (utf8[1] ^ 0x80) << 6) | (unsigned short) (utf8[2] ^ 0x80);
        return 3;
    }
}

int bpp_utf8_to_ucs2_string(char *dest, int dest_size, const char * src)
{
    int pos = 0;

    while (*src)
    {
        src += bpp_utf8_to_ucs2_char((unsigned short*) (dest + pos), src);
        pos += 2;
        if (pos >= dest_size - 2)
        {
            break;
        }
    }
    dest[pos] = 0;
    dest[pos + 1] = 0;
    return pos + 2;
}

// ----------------------------------------------------------------------------
static void classInitNative(JNIEnv* env, jclass clazz)
{
    BPP_DBG("+");

    env->GetJavaVM(&g_jvm);

    field_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");
    method_onCallback = env->GetMethodID(clazz, "onCallback", "(III[Ljava/lang/String;)V");
	/*
    mStringClass = env->FindClass("java/lang/String");
	if( mStringClass != NULL )
	{
		mStringClass = (jclass)(env->NewGlobalRef(mStringClass));
	}
*/

    BPP_DBG("-");
}

static void initializeDataNative(JNIEnv* env, jobject object)
{
    BPP_DBG("+");

    g_callback_object = env->NewGlobalRef(object);

    native_data_t *nat = (native_data_t *)calloc(1, sizeof(native_data_t));
    if (NULL == nat) {
        BPP_ERR("out of memory!");
        return;
    }
    memset(nat, 0, sizeof(native_data_t));
    env->SetIntField(object, field_mNativeData, (jint)nat);

    BPP_DBG("-");
}

static void cleanupDataNative(JNIEnv* env, jobject object)
{
    native_data_t *nat = (native_data_t *)env->GetIntField(object, field_mNativeData);

    BPP_DBG("+");
    env->DeleteGlobalRef(g_callback_object);
    //env->DeleteGlobalRef(mStringClass);
    if (nat) {
        free(nat);
        nat = 0;
        env->SetIntField(object, field_mNativeData, (jint)nat);
    }
    else
    {
        BPP_ERR("nat is null!");
    }

    BPP_DBG("-");
}

// ----------------------------------------------------------------------------

static jboolean enableServiceNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;
    sockaddr_un name;
    socklen_t namelen;

    BPP_DBG("+");
    // reset socket
    //    GOPP(apisock) = -1;
    //    GOPP(servsock) = -1;
    // get native data
    nat = get_native_data(env, object);
    if(!nat)
    {
        BPP_ERR("nat is null");
        return JNI_FALSE;
    }

    nat->apisock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    BPP_DBG("nat->apisock==%d", nat->apisock);
    if (nat->apisock < 0)
    {
        BPP_ERR("create bpp api socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    name.sun_family = AF_UNIX;
    strcpy (name.sun_path, BT_SOCK_NAME_INT_ADP); //BT_SERV_SOCK_ADDR
    namelen = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1);
    if ( connect(nat->apisock, (const struct sockaddr*)&name, namelen) < 0)
    {
        BPP_ERR("connect to %s failed : %s, errno=%d", name.sun_path, strerror(errno), errno);
        goto error_exit;
    }
    //g_apisock = nat->apisock;
    //GOPP(apisock) = nat->apisock;


    nat->servsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_BPP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    BPP_DBG("nat->servsock==%d", nat->servsock);
    if (nat->servsock < 0)
    {
        BPP_ERR("create bpp server socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    //GOPP(servsock) = nat->servsock;


    //btmtk_bpp_enable();
    // register call back function
    BPP_DBG("set jni callback for external-adaptation");
    if( btmtk_bpp_set_jni_callback(bpp_event_callback, nat->apisock) != TRUE )
    {
        BPP_ERR("set event callback fail");
        goto error_exit;
    }
    BPP_DBG("-");

    return JNI_TRUE;

error_exit:
    if(nat)
    {
        if(nat->apisock >= 0)
        {
            close(nat->apisock);
            nat->apisock = -1;
        }
        if(nat->servsock >= 0)
        {
            close(nat->servsock);
            nat->servsock = -1;
        }
    }
    BPP_ERR("-error_exit");

    return JNI_FALSE;
}

static void disableServiceNative(JNIEnv* env, jobject object)
{
    BPP_DBG("+");
    native_data_t *nat;
    nat = get_native_data(env, object);

    if(nat)
    {
        //btmtk_bpp_disable(nat->apisock);
        if(nat->apisock >= 0)
        {
            BPP_DBG("closing apisock...");
            close(nat->apisock);
            nat->apisock = -1;
        }
        //if(nat->servsock >= 0)
        //{
        //    BPP_DBG("closing servsocket...");
        //    close(nat->servsock);
        //    nat->servsock = -1;
        //}
    //use join instaed of these
        //while(nat->servsock >= 0)
        //{
        //    BPP_DBG("wait for servsocket closing...");
        //    sleep(1);
        //}
    }
    else
    {
        BPP_ERR("null nat");
    }
    BPP_DBG("-");
}

static jboolean startListenNative(JNIEnv* env, jobject object)
{
    BPP_DBG("+");

    int res = -1;
    fd_set readfs;
    int sockfd;
    ilm_struct ilm;


    native_data_t *nat = get_native_data(env, object);
    if(!nat)
    {
        BPP_ERR("nat is NULL");
        return JNI_FALSE;
    }
    //sockfd = g_servsock;
    sockfd = nat->servsock;
    BPP_DBG("servsock=%d", sockfd);

    BPP_DBG("MSG_ID_BT_BPP_GROUP_START: %d,  MSG_ID_BT_BPP_GROUP_END: %d", MSG_ID_BT_BPP_GROUP_START, MSG_ID_BT_BPP_GROUP_END);

    while(1)
    {
        FD_ZERO(&readfs);
        if(sockfd)
        {
            FD_SET(sockfd, &readfs);
        }
        else
        {
            BPP_ERR("nat->servsock == 0. exit");
        }
        BPP_DBG("Start select : sockfd=%d", sockfd);
        res = select(sockfd+1, &readfs, NULL, NULL, NULL);
        BPP_DBG("Return from select : soresckfd=%d", res);

        if(res > 0)
        {
            res = recvfrom(sockfd, (void*)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
            BPP_DBG("Recv BPP IND or CNF : %lu", ilm.msg_id);
            if(res < 0)
            {
                if(errno == EAGAIN)
                {
                    res = 0;
                }
                else
                {
                    BPP_ERR("recvfrom failed : %s, %d", strerror(errno), errno);
                }
            }
            else
            {
                if (*((int*)&ilm) == 19810103 )
                {
                    BPP_DBG("receive magic number");
                    //return JNI_FALSE; //leave
                    break;
                }

                if ((ilm.msg_id >= MSG_ID_BT_BPP_GROUP_START) && (ilm.msg_id <= (MSG_ID_BT_BPP_GROUP_END)) )
                {
                        btmtk_bpp_handle_message(&ilm);
                }
                //else if( ilm.msg_id == -1 )
                //{
                //    // terminate request
                //    bt_ext_log("[OPP][ADP][startListenMessage]: server shutdown request received.");
                //         break;
                //}
                else
                {
                    BPP_ERR("Unexpected message: %lu", ilm.msg_id);
                }
            }
        }
        else
        {
             BPP_ERR("select failed: %s(%d)", strerror(errno), errno);
		if(errno != EINTR)
             	break;
        }
    }
    // close socket after exit loop
    if( nat->servsock >= 0)
    {
        BPP_DBG("closing bpp server socket...");
        close(nat->servsock);
        nat->servsock = -1;
    }

    BPP_DBG("-");
    return (res>0) ? JNI_TRUE : JNI_FALSE;
}

static jboolean stopListenNative(JNIEnv* env, jobject object)
{
    BPP_DBG("+");

    int magicsock;
    sockaddr_un name;
    socklen_t   namelen;

    int ret = -1;
    int magicNumber= 19810103;
    native_data_t *nat = get_native_data(env, object);


    if(nat->servsock >= 0)
    {
        magicsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
        BPP_DBG("magicsock==%d", magicsock);

        if (magicsock < 0){
            BPP_ERR("create bpp magic socket failed : %s, errno=%d", strerror(errno), errno);
            return -1;
        }
        if( socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_BPP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&name, &namelen) < 0)
        {
            BPP_ERR("bsocket_make_sockaddr_un failed : %s, errno=%d", strerror(errno), errno);
        }
        if ( connect(magicsock, (const struct sockaddr*)&name, namelen) < 0 )
        {
            BPP_ERR("connect to %s failed : %s, errno=%d", name.sun_path, strerror(errno), errno);
            close(magicsock);
            magicsock = -1;
            return -1;
        }

        ret = send(magicsock, (void*)&magicNumber, 4, 0);
        if( ret < 0 )
        {
            BPP_ERR("send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            BPP_DBG("send msg success : %d", ret);
        }
        close(magicsock);
        magicsock = -1;
    }
    BPP_DBG("-");
    return (ret >= 0) ? 0 : -1;
}

jarray newStringArray(JNIEnv *env, char **c_string, int count)
{
    int i;
    jobjectArray ary;
	jclass mStringClass;
	
    mStringClass = env->FindClass("java/lang/String");

    // new java string array
    ary = env->NewObjectArray(count, mStringClass, 0);
    if( ary == 0 )  
	{
		env->DeleteLocalRef(mStringClass);
		return NULL;
    }

    // fill string array element
    for (i = 0; i < count; i++)
    {
        jstring str = env->NewStringUTF(*c_string++);
        if( str == 0 )  return NULL;
        env->SetObjectArrayElement(ary, i, str);
        env->DeleteLocalRef(str);
    }
	env->DeleteLocalRef(mStringClass);

    return ary;
}

void bpp_event_callback(U8 event, S8** parameters, U8 count)
{
    bool isAttached = false;
    JNIEnv *env;
    jarray args;

    BPP_DBG("+");

    // prepare env
    if(g_jvm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
    {

        BPP_WRN("failed to get JNI environment, assuming native thread");
        if(g_jvm->AttachCurrentThread(&env, NULL) < 0)
        {
            BPP_ERR("failed to attach current thread");
        }
        else
        {
            isAttached = true;
        }
    }

    assert(env != NULL);

    // prepare parameter for java method
    if( parameters != NULL )
    {
        BPP_DBG("try to build non-null parameters: size[%d]", count);
        args = newStringArray(env, parameters, count);
        if( args == 0 )
        {
            BPP_ERR("can't new StringArray for callback parameters(size:%d)", count);
        }
    }
    else
    {
        args = NULL;
    }


    // call java method via env, callback to MMI
    env->CallVoidMethod(g_callback_object, method_onCallback, event, 0, 0, args);


    // TODO free args ? parameters ?

    if( args != NULL )
    {
        env->DeleteLocalRef(args);
    }
    if(isAttached)
    {
        g_jvm->DetachCurrentThread();
    }

    BPP_DBG("-");
}

/*----------------------------------------
call native api in external layer
----------------------------------------*/
static jboolean bppEnableNative(JNIEnv* env, jobject object)
{
    btmtk_bpp_enable();

    return JNI_TRUE;
}

static jboolean bppDisableNative(JNIEnv* env, jobject object)
{
    btmtk_bpp_disable();

    return JNI_TRUE;
}

static const char *classAuthInfoPathName = "com/mediatek/bluetooth/bpp/AuthInfo";
static void bppAuthRspNative(JNIEnv* env, jobject object, jobject auth_reply)
{
    native_data_t *nat;

    BPP_DBG("+bpp bppAuthRspNative");
    nat = get_native_data(env, object);
    if(!nat)
    {
        BPP_ERR("[ERR] nat is null");
        return;
    }

    jclass clazz;
    clazz = env->FindClass(classAuthInfoPathName);

    jfieldID field_bAuth = env->GetFieldID(clazz, "bAuth", "Z");
    jboolean bAuth = env->GetBooleanField(auth_reply, field_bAuth);
    
    jfieldID field_UserId = env->GetFieldID(clazz, "UserId", "Ljava/lang/String;");
    jstring UserId = (jstring)env->GetObjectField(auth_reply, field_UserId);
    const char *c_UserId = env->GetStringUTFChars(UserId, NULL);

    jfieldID field_Passwd = env->GetFieldID(clazz, "Passwd", "Ljava/lang/String;");
    jstring Passwd = (jstring)env->GetObjectField(auth_reply, field_Passwd);
    const char *c_Passwd = env->GetStringUTFChars(Passwd, NULL);

    T_OBEX_AUTH c_auth_reply;
    c_auth_reply.bAuth = bAuth;
    memcpy(c_auth_reply.UserId, c_UserId, OBEX_AUTH_USERID_LEN);
    env->ReleaseStringUTFChars(UserId, c_UserId);
    memcpy(c_auth_reply.Passwd, c_Passwd, OBEX_AUTH_PASSWD_LEN);
    env->ReleaseStringUTFChars(Passwd, c_Passwd);


    btmtk_bpp_auth_response(&c_auth_reply);


    BPP_DBG("c_bAuth: %d c_UserId: %s c_Passwd: %s", 
          c_auth_reply.bAuth, c_auth_reply.UserId, c_auth_reply.Passwd);
    BPP_DBG("-bpp bppAuthRspNative");

}

static void bppGetPrinterAttrNative(JNIEnv* env, jobject object, jstring address, jint bitmask_attr)
{
    native_data_t *nat;

    BPP_DBG("+bpp bppGetPrinterAttrNative");
    nat = get_native_data(env, object);
    if(!nat)
    {
        BPP_ERR("[ERR] nat is null");
        return;
    }

    T_BDADDR bdaddr;
    const char *c_bdaddr = env->GetStringUTFChars(address, NULL);
    BPP_DBG("c_bdaddr: %s", c_bdaddr);
    //for(int i=0;i<6;i++)
    //  bdaddr[i] = c_bdaddr[i];

    //btmtk_util_convert_string2bdaddr(c_bdaddr, bdaddr);
    CONVERT_STRING2BDADDR(c_bdaddr, bdaddr);
    env->ReleaseStringUTFChars(address, c_bdaddr);
    //get rid of the LSB->MSB conversion function in functions

    btmtk_bpp_getprinterattribute(bdaddr, bitmask_attr);


    BPP_DBG("-bpp bppGetPrinterAttrNative");

}


static const char *classPrintObjectPathName = "com/mediatek/bluetooth/bpp/PrintObject";
static void bppPrintNative(JNIEnv* env, jobject object, jstring address, jobject print_object)
{
    native_data_t *nat;

    BPP_DBG("+bpp bppPrintNative");
    nat = get_native_data(env, object);
    if(!nat)
    {
        BPP_ERR("[ERR] nat is null");
        return;
    }



    T_BDADDR bdaddr;
    const char *c_bdaddr = env->GetStringUTFChars(address, NULL);
    BPP_DBG("c_bdaddr: %s", c_bdaddr);

    CONVERT_STRING2BDADDR(c_bdaddr, bdaddr);
    env->ReleaseStringUTFChars(address, c_bdaddr);



    jclass clazz;
    clazz = env->FindClass(classPrintObjectPathName);

    jfieldID field_DirName = env->GetFieldID(clazz, "DirName", "Ljava/lang/String;");
    jstring DirName = (jstring)env->GetObjectField(print_object, field_DirName);
    const char *c_DirName = env->GetStringUTFChars(DirName, NULL);
    
    jfieldID field_FileName = env->GetFieldID(clazz, "FileName", "Ljava/lang/String;");
    jstring FileName = (jstring)env->GetObjectField(print_object, field_FileName);
    const char *c_FileName = env->GetStringUTFChars(FileName, NULL);

    jfieldID field_MimeType = env->GetFieldID(clazz, "nMimeType", "I");
    jint MimeType = env->GetIntField(print_object, field_MimeType);

    jfieldID field_ObjectSize = env->GetFieldID(clazz, "nObjectSize", "I");
    jint ObjectSize = env->GetIntField(print_object, field_ObjectSize);

    jfieldID field_JobBased = env->GetFieldID(clazz, "bJobBased", "Z");
    jboolean JobBased = env->GetBooleanField(print_object, field_JobBased);

    jfieldID field_Copies = env->GetFieldID(clazz, "Copies", "I");
    jint Copies = env->GetIntField(print_object, field_Copies);

    jfieldID field_NumberUp = env->GetFieldID(clazz, "nNumberUp", "I");
    jint NumberUp = env->GetIntField(print_object, field_NumberUp);

    jfieldID field_Sides = env->GetFieldID(clazz, "nSides", "I");
    jint Sides = env->GetIntField(print_object, field_Sides);

    jfieldID field_Orient = env->GetFieldID(clazz, "nOrient", "I");
    jint Orient = env->GetIntField(print_object, field_Orient);

    jfieldID field_Quality = env->GetFieldID(clazz, "nQuality", "I");
    jint Quality = env->GetIntField(print_object, field_Quality);
   
    jfieldID field_MediaSize = env->GetFieldID(clazz, "nMediaSize", "I");
    jint MediaSize = env->GetIntField(print_object, field_MediaSize);


    T_BPP_OBJECT c_print_object;
    memset( &c_print_object, 0, sizeof(c_print_object));

    BPP_DBG("bppPrintNative: dirname %s", c_DirName);
    //int len = 2*(strlen(c_DirName)+1);
    //char* cw_DirName = (char*)malloc(len);
    //bpp_utf8_to_ucs2_string(cw_DirName, len, c_DirName);
    //memcpy(c_print_object.DirName, cw_DirName, MAX_FILE_NAME_LEN);
    memcpy(c_print_object.DirName, c_DirName, MAX_FILE_NAME_LEN);
    env->ReleaseStringUTFChars(DirName, c_DirName);
    BPP_DBG("bppPrintNative: filename %s", c_FileName);
    memcpy(c_print_object.FileName, c_FileName, MAX_FILE_NAME_LEN);
    env->ReleaseStringUTFChars(FileName, c_FileName);

    c_print_object.MimeType = (T_MIME_MEDIA)MimeType; 
    BPP_DBG("bppPrintNative: mime_type(%d) cmime_type(%d)", MimeType, c_print_object.MimeType );
    c_print_object.bJobBasedPrinting = JobBased;
    BPP_DBG("bppPrintNative: job_based(%d) cjob_based(%d)", JobBased, c_print_object.bJobBasedPrinting );
    c_print_object.ObjectSize = ObjectSize; 
    BPP_DBG("bppPrintNative: objectsize(%d) cobjectsize(%lu)", ObjectSize, c_print_object.ObjectSize );
    
    c_print_object.PrintingAttribute.Copies = Copies; 
    BPP_DBG("bppPrintNative: copies(%d) ccopies(%d)", Copies, c_print_object.PrintingAttribute.Copies );
    c_print_object.PrintingAttribute.NumberUp = NumberUp; 
    BPP_DBG("bppPrintNative: numberup(%d) cnumberup(%d)", NumberUp, c_print_object.PrintingAttribute.NumberUp );
    c_print_object.PrintingAttribute.Sides = Sides; 
    BPP_DBG("bppPrintNative: sides(%d) csides(%d)", Sides, c_print_object.PrintingAttribute.Sides );
    c_print_object.PrintingAttribute.Orient = Orient; 
    BPP_DBG("bppPrintNative: orient(%d) corient(%d)", Orient, c_print_object.PrintingAttribute.Orient );
    c_print_object.PrintingAttribute.Quality = Quality;
    BPP_DBG("bppPrintNative: quality(%d) cquality(%d)", Quality, c_print_object.PrintingAttribute.Quality );

    strcpy(c_print_object.PrintingAttribute.MediaSize, bpp_media_size_self_desc_name[MediaSize]);
    BPP_DBG("bppPrintNative: media size(%s)", c_print_object.PrintingAttribute.MediaSize );
    
    strcpy(c_print_object.PrintingAttribute.DocFmt,(char*)bpp_mime_type_table[MimeType+1].type_str);
    BPP_DBG("bppPrintNative: mime type(%s)", c_print_object.PrintingAttribute.DocFmt );
//test code


    //c_print_object.bJobBasedPrinting = true;
    //c_print_object.PrintingAttribute.Copies = 1; 
    //c_print_object.PrintingAttribute.NumberUp = 1;  
    //c_print_object.PrintingAttribute.Sides =  0; //IGNORED
    //c_print_object.PrintingAttribute.Orient = 0; //IGNORED 
    //c_print_object.PrintingAttribute.Quality = 0; //IGNORED 

    //Image 
    //c_print_object.MimeType = (T_MIME_MEDIA)10;
    //c_print_object.PrintingAttribute.DocFmt =  "image/jpeg"; //IMAGE_JPEG
    //strcpy(c_print_object.PrintingAttribute.DocFmt, "application/vnd.pwg-multiplexed");
    //strcpy(c_print_object.PrintingAttribute.DocFmt, "image/jpeg");

    //Vcard
    //memcpy(c_print_object.DirName, "/sdcard", MAX_FILE_NAME_LEN);
    //memcpy(c_print_object.FileName, "paul.vcf", MAX_FILE_NAME_LEN);
    //c_print_object.ObjectSize = 1317;
    //c_print_object.MimeType = (T_MIME_MEDIA)5;
    //strcpy(c_print_object.PrintingAttribute.DocFmt, "text/x-vcard:3.0");
    //strcpy(c_print_object.PrintingAttribute.MediaSize, "na_index-4x6_4x6in");
    //c_print_object.PrintingAttribute.MediaType = (T_BPP_MEDIA)0; //UNDEF
    

    btmtk_bpp_print(bdaddr, &c_print_object);


    BPP_DBG("-bpp bppPrintNative");

}

/*
static void bppCancelNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;

    BPP_DBG("+bpp bppCancelNative");
    nat = get_native_data(env, object);
    if(!nat)
    {
        BPP_ERR("[ERR] nat is null");
        return;
    }

    btmtk_bpp_cancel(nat->apisock);

    BPP_DBG("-bpp bppCancelNative");
}
*/

static void bppDisconnectNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;

    BPP_DBG("+bpp bppDisconnectNative");
    nat = get_native_data(env, object);
    if(!nat)
    {
        BPP_ERR("[ERR] nat is null");
        return;
    }

    btmtk_bpp_disconnect();

    //env->CallVoidMethod(object,method_onBppDisconnectTimeout);

    BPP_DBG("-bpp bppDisconnectNative");
}

// ----------------------------------------------------------------------------

static const char *classPathName = "com/mediatek/bluetooth/bpp/BluetoothBppServer";

static JNINativeMethod methods[] = {
  {"classInitNative", "()V", (void*)classInitNative},
  {"initializeDataNative", "()V", (void*)initializeDataNative},
  {"cleanupDataNative", "()V", (void*)cleanupDataNative},

  {"enableServiceNative", "()Z", (void*)enableServiceNative },
  {"startListenNative", "()Z", (void*)startListenNative },
  {"stopListenNative", "()Z", (void*)stopListenNative },
  {"disableServiceNative", "()V", (void*)disableServiceNative },

  {"bppEnableNative", "()Z", (void*)bppEnableNative},
  {"bppDisableNative", "()Z", (void*)bppDisableNative},
  {"bppDisconnectNative", "()V", (void*)bppDisconnectNative},
  {"bppAuthRspNative", "(Lcom/mediatek/bluetooth/bpp/AuthInfo;)V", (void*)bppAuthRspNative},
  {"bppGetPrinterAttrNative", "(Ljava/lang/String;I)V", (void*)bppGetPrinterAttrNative},
  {"bppPrintNative", "(Ljava/lang/String;Lcom/mediatek/bluetooth/bpp/PrintObject;)V", (void*)bppPrintNative},
  //{"bppCancelNative", "()V", (void*)bppCancelNative},
};

// Register several native methods for one class.
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        BPP_ERR("unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        BPP_ERR("failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

// Register native methods for all classes we know about.
// returns JNI_TRUE on success.
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}
// ----------------------------------------------------------------------------

// This is called by the VM when the shared library is first loaded.
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    BPP_DBG("+");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        BPP_ERR("failed(%d)", result);
        return result;
    }
    assert(env != NULL);

    if (!registerNatives(env)) {
        BPP_ERR("failed(%d)", result);
        return result;
    }

    result = JNI_VERSION_1_4;

    BPP_DBG("-");
    return result;
}


