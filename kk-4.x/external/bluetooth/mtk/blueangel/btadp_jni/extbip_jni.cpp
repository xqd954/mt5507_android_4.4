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
#include <string.h>
#include <cutils/sockets.h>

   
extern "C"
{
typedef bool BOOL;

#include "bt_mmi.h"
#include "bt_message.h"

#include "bluetooth_struct.h"

#include "bt_bip_porting.h"
#include "bt_bip_porting_datatype.h"
#include "bt_ext_utility.h"

#include "socket_local.h"
}

#undef LOG_TAG
#define LOG_TAG "JNI_BIP"

#define BT_SOCK_NAME_EXT_ADP_BIP "bt.ext.adp.bip"


#define BIP_JNI_DEBUG 1

#ifdef BIP_JNI_DEBUG
#define BIP_DBG(fmt, ...) XLOGD("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)
#else
#define BIP_DBG(fmt, ...) {}
#endif
#define BIP_ERR(fmt, ...) XLOGE("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)
#define BIP_WRN(fmt, ...) XLOGW("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)

//int g_apisock;

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

void bip_event_callback(U8 event, S8** parameters, U8 count); //Callback from native
//void bip_event_callback(U8 event, void* parameters, U8 count); //Callback from native
// ----------------------------------------------------------------------------
static void classInitNative(JNIEnv* env, jclass clazz)
{
    BIP_DBG("+");

    env->GetJavaVM(&g_jvm);

    field_mNativeData = env->GetFieldID(clazz, "mNativeData", "I");
 //   method_onCallback = env->GetMethodID(clazz, "onCallback", "(IIILjava/lang/Object;)V");
    method_onCallback = env->GetMethodID(clazz, "onCallback", "(III[Ljava/lang/String;)V");
 /*
    mStringClass = env->FindClass("java/lang/String");
	if( mStringClass != NULL )
	{
		mStringClass = (jclass)(env->NewGlobalRef(mStringClass));
	}
*/

    BIP_DBG("-");
}

static void initializeDataNative(JNIEnv* env, jobject object)
{
    BIP_DBG("+");

    g_callback_object = env->NewGlobalRef(object);

    native_data_t *nat = (native_data_t *)calloc(1, sizeof(native_data_t));
    if (NULL == nat) {
        BIP_ERR("out of memory!");
        return;
    }
    memset(nat, 0, sizeof(native_data_t));
    env->SetIntField(object, field_mNativeData, (jint)nat);

    BIP_DBG("-");
}

static void cleanupDataNative(JNIEnv* env, jobject object)
{
    native_data_t *nat = (native_data_t *)env->GetIntField(object, field_mNativeData);

    BIP_DBG("+");
    env->DeleteGlobalRef(g_callback_object);
    //env->DeleteGlobalRef(mStringClass);
    if (nat) {
        free(nat);
        nat = 0;
        env->SetIntField(object, field_mNativeData, (jint)nat);
    }
    else
    {
        BIP_ERR("nat is null!");
    }


    BIP_DBG("-");
}
// ----------------------------------------------------------------------------

static jboolean enableServiceNative(JNIEnv* env, jobject object)
{
    native_data_t *nat;
    sockaddr_un name;
    socklen_t namelen;

    BIP_DBG("+");
    // reset socket
    //    GOPP(apisock) = -1;
    //    GOPP(servsock) = -1;
    // get native data
    nat = get_native_data(env, object);
    if(!nat)
    {
        BIP_ERR("nat is null");
        return JNI_FALSE;
    }

    nat->apisock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    BIP_DBG("nat->apisock==%d", nat->apisock);
    if (nat->apisock < 0)
    {
        BIP_ERR("create bpp api socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
	
	struct timeval timeout;
	timeout.tv_sec = 5; //5s timeout
        timeout.tv_usec = 0;
	if(setsockopt(nat->apisock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)))
	{
	    BIP_ERR("set apisock timeout fail");	

	}else{
		
            BIP_DBG("set apisock timeout success");
	}
	
    name.sun_family = AF_UNIX;
    strcpy (name.sun_path, BT_SOCK_NAME_INT_ADP); //BT_SERV_SOCK_ADDR
    namelen = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1);
    if ( connect(nat->apisock, (const struct sockaddr*)&name, namelen) < 0)
    {
        BIP_ERR("connect to %s failed : %s, errno=%d", name.sun_path, strerror(errno), errno);
        goto error_exit;
    }
    //g_apisock = nat->apisock;
    //GOPP(apisock) = nat->apisock;


    nat->servsock = socket_local_server(BT_SOCK_NAME_EXT_ADP_BIP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
    BIP_DBG("nat->servsock==%d", nat->servsock);
    if (nat->servsock < 0)
    {
        BIP_ERR("create bip server socket failed : %s, errno=%d", strerror(errno), errno);
        goto error_exit;
    }
    //GOPP(servsock) = nat->servsock;


    //btmtk_bpp_enable();
    // register call back function
    BIP_DBG("set jni callback for external-adaptation");
    if( btmtk_bip_set_jni_callback(bip_event_callback, nat->apisock) != TRUE )
    {
        BIP_ERR("set event callback fail");
        goto error_exit;
    }
    BIP_DBG("-");

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
    //env->CallVoidMethod(object, method_onBppEnable, errno);
    BIP_ERR("-error_exit");

    return JNI_FALSE;
}

static void disableServiceNative(JNIEnv* env, jobject object)
{
    BIP_DBG("+");
    native_data_t *nat;
    nat = get_native_data(env, object);

    if(nat)
    {
        //btmtk_bpp_disable(nat->apisock);
        if(nat->apisock >= 0)
        {
            BIP_DBG("closing apisock...");
            close(nat->apisock);
            nat->apisock = -1;
        }
        //if(nat->servsock >= 0)
        //{
        //    BIP_DBG("closing servsocket...");
        //    close(nat->servsock);
        //    nat->servsock = -1;
        //}
    //use join instaed of these
        //while(nat->servsock >= 0)
        //{
        //    BIP_DBG("wait for servsocket closing...");
        //    sleep(1);
        //}
    }
    else
    {
        BIP_ERR("null nat");
    }
    BIP_DBG("-");
}

static jboolean startListenNative(JNIEnv* env, jobject object)
{
    // startListenMessage(); implement in bt_bip_porting.c
    BIP_DBG("+");

    int res = -1;
    fd_set readfs;
    int sockfd;
    ilm_struct ilm;


    native_data_t *nat = get_native_data(env, object);
    if(!nat)
    {
        BIP_ERR("nat is NULL");
        return JNI_FALSE;
    }
    //sockfd = g_servsock;
    sockfd = nat->servsock;
    BIP_DBG("servsock=%d", sockfd);

    BIP_DBG("MSG_ID_BT_BIPI_GROUP_START: %d,  MSG_ID_BT_BIPR_GROUP_END: %d", MSG_ID_BT_BIPI_GROUP_START, MSG_ID_BT_BIPR_GROUP_END);

    while(1)
    {
        FD_ZERO(&readfs);
        if(sockfd)
        {
            FD_SET(sockfd, &readfs);
        }
        else
        {
            BIP_ERR("nat->servsock == 0. exit");
        }
        BIP_DBG("Start select : sockfd=%d", sockfd);
        res = select(sockfd+1, &readfs, NULL, NULL, NULL);
        BIP_DBG("Return from select : soresckfd=%d", res);

        if(res > 0)
        {
            res = recvfrom(sockfd, (void*)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
            BIP_DBG("Recv BIP IND or CNF : %lu", ilm.msg_id);
            if(res < 0)
            {
                if(errno == EAGAIN)
                {
                    res = 0;
                }
                else
                {
                    BIP_ERR("recvfrom failed : %s, %d", strerror(errno), errno);
                }
            }
            else
            {
                if (*((int*)&ilm) == 19810103 )
                {
                    BIP_DBG("receive magic number");
                    //return JNI_FALSE; //leave
                    break;
                }

                if ((ilm.msg_id >= MSG_ID_BT_BIPI_GROUP_START) && (ilm.msg_id <= (MSG_ID_BT_BIPR_GROUP_END)) )
                {
                        btmtk_bip_handle_message(&ilm);
                }
                //else if( ilm.msg_id == -1 )
                //{
                //    // terminate request
                //    bt_ext_log("[OPP][ADP][startListenMessage]: server shutdown request received.");
                //         break;
                //}
                else
                {
                    BIP_ERR("Unexpected message: %lu", ilm.msg_id);
                }
            }
        }
        else
        {
             BIP_ERR("select failed: %s(%d)", strerror(errno), errno);
		if(errno != EINTR)
             	break;
        }
    }
    // close socket after exit loop
    if( nat->servsock >= 0)
    {
        BIP_DBG("closing bip server socket...");
        close(nat->servsock);
        nat->servsock = -1;
    }

    BIP_DBG("-");
    return (res>0) ? JNI_TRUE : JNI_FALSE;
}


static jboolean stopListenNative(JNIEnv* env, jobject object)
{
    // stopListenMessage(); implement in bt_bip_porting.c
    BIP_DBG("+");

    int magicsock;
    sockaddr_un name;
    socklen_t   namelen;

    int ret = -1;
    int magicNumber= 19810103;
    native_data_t *nat = get_native_data(env, object);


    if(nat->servsock >= 0)
    {
        magicsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
        BIP_DBG("magicsock==%d", magicsock);

        if (magicsock < 0){
            BIP_ERR("create bip magic socket failed : %s, errno=%d", strerror(errno), errno);
            return -1;
        }
        if( socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_BIP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&name, &namelen) < 0)
        {
            BIP_ERR("bsocket_make_sockaddr_un failed : %s, errno=%d", strerror(errno), errno);
        }
        if ( connect(magicsock, (const struct sockaddr*)&name, namelen) < 0 )
        {
            BIP_ERR("connect to %s failed : %s, errno=%d", name.sun_path, strerror(errno), errno);
            close(magicsock);
            magicsock = -1;
            return -1;
        }

        ret = send(magicsock, (void*)&magicNumber, 4, 0);
        if( ret < 0 )
        {
            BIP_ERR("send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            BIP_DBG("send msg success : %d", ret);
        }
        close(magicsock);
        magicsock = -1;
    }
    BIP_DBG("-");
    return (ret >= 0) ? JNI_TRUE : JNI_FALSE;
}

// ----------------------------------------------------------------------------

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

void bip_event_callback(U8 event, S8** parameters, U8 count)
//void bip_event_callback(U8 event, void* parameters, U8 count)
{
    bool isAttached = false;
    JNIEnv *env;
    jarray args;

    BIP_DBG("+");

    // prepare env
    if(g_jvm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
    {

        BIP_WRN("failed to get JNI environment, assuming native thread");
        if(g_jvm->AttachCurrentThread(&env, NULL) < 0)
        {
            BIP_ERR("failed to attach current thread");
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
        BIP_DBG("try to build non-null parameters: size[%d]", count);
        args = newStringArray(env, parameters, count);
        if( args == 0 )
        {
            BIP_ERR("can't new StringArray for callback parameters(size:%d)", count);
        }
    }
    else
    {
        args = NULL;
    }


    // call java method via env, callback to MMI
    env->CallVoidMethod(g_callback_object, method_onCallback, event, 0, 0, args);
    //env->CallVoidMethod(mCallbackObject, mCallbackMethod, event, args);


    // TODO free args ? parameters ?

    if( args != NULL )
    {
        env->DeleteLocalRef(args);
    }

    // release resource
    if(isAttached)
    {
        g_jvm->DetachCurrentThread();
    }

    BIP_DBG("-");
}

// ----------------------------------------------------------------------------
static void getExtBDADDR(JNIEnv* env, jstring BdAddr, U8* r_bdaddr)
{
    T_BDADDR bdaddr;
    const char *c_bdaddr = env->GetStringUTFChars(BdAddr, NULL);
    BIP_DBG("c_bdaddr: %s", c_bdaddr);

    bip_util_convert_string2array(c_bdaddr, bdaddr);
    env->ReleaseStringUTFChars(BdAddr, c_bdaddr);

    bip_util_reverse_array(r_bdaddr,bdaddr);

    return;
}

static void getExtImageObject(JNIEnv* env, jobject imageObject, T_BIP_OBJECT* c_image_object)
{
    jclass clazz;
    clazz = env->FindClass("com/mediatek/bluetooth/bip/BipImage");

    jfieldID field_DirName = env->GetFieldID(clazz, "DirName", "Ljava/lang/String;");
    jstring DirName = (jstring)env->GetObjectField(imageObject, field_DirName);
    const char *c_DirName = env->GetStringUTFChars(DirName, NULL);

    jfieldID field_FileName = env->GetFieldID(clazz, "FileName", "Ljava/lang/String;");
    jstring FileName = (jstring)env->GetObjectField(imageObject, field_FileName);
    const char *c_FileName = env->GetStringUTFChars(FileName, NULL);

    jfieldID field_Version = env->GetFieldID(clazz, "Version", "Ljava/lang/String;");
    jstring Version = (jstring)env->GetObjectField(imageObject, field_Version);
    const char *c_Version = env->GetStringUTFChars(Version, NULL);

    jfieldID field_Encoding = env->GetFieldID(clazz, "Encoding", "Ljava/lang/String;");
    jstring Encoding = (jstring)env->GetObjectField(imageObject, field_Encoding);
    const char *c_Encoding = env->GetStringUTFChars(Encoding, NULL);

    jfieldID field_ThumbnailFullPath = env->GetFieldID(clazz, "ThumbnailFullPath", "Ljava/lang/String;");
    jstring ThumbnailFullPath = (jstring)env->GetObjectField(imageObject, field_ThumbnailFullPath);
    const char *c_ThumbnailFullPath = env->GetStringUTFChars(ThumbnailFullPath, NULL);

    jfieldID field_ObjectSize = env->GetFieldID(clazz, "ObjectSize", "I");
    jint ObjectSize = env->GetIntField(imageObject, field_ObjectSize);

    jfieldID field_Height = env->GetFieldID(clazz, "Height", "I");
    jint Height = env->GetIntField(imageObject, field_Height);

    jfieldID field_Width = env->GetFieldID(clazz, "Width", "I");
    jint Width = env->GetIntField(imageObject, field_Width);

    jfieldID field_Height2 = env->GetFieldID(clazz, "Height2", "I");
    jint Height2 = env->GetIntField(imageObject, field_Height2);

    jfieldID field_Width2 = env->GetFieldID(clazz, "Width2", "I");
    jint Width2 = env->GetIntField(imageObject, field_Width2);

  //  T_BIP_OBJECT c_image_object;
  //  memset( &c_image_object, 0, sizeof(c_image_object));

    BIP_DBG("dirname: %s, length:%d", c_DirName, strlen(c_DirName));
    memcpy(c_image_object->DirName, c_DirName, strlen(c_DirName));
    env->ReleaseStringUTFChars(DirName, c_DirName);
    BIP_DBG("filename: %s, length:%d", c_FileName, strlen(c_FileName));
    memcpy(c_image_object->FileName, c_FileName, strlen(c_FileName));
    env->ReleaseStringUTFChars(FileName, c_FileName);
    BIP_DBG("thumbnailfullpath: %s, length:%d", c_ThumbnailFullPath, strlen(c_ThumbnailFullPath));
    memcpy(c_image_object->ThumbnailFullPath, c_ThumbnailFullPath, strlen(c_ThumbnailFullPath));
    env->ReleaseStringUTFChars(ThumbnailFullPath, c_ThumbnailFullPath);

    c_image_object->ObjectSize = ObjectSize;
    BIP_DBG("ObjectSize: %d ImageObject.ObjectSize: %lu", ObjectSize, c_image_object->ObjectSize);


    BIP_DBG("version: %s", c_Version);
    memcpy(c_image_object->ImageDesc.Version, c_Version, 10);
    env->ReleaseStringUTFChars(Version, c_Version);
    BIP_DBG("encoding: %s", c_Encoding);
    memcpy(c_image_object->ImageDesc.Encoding, c_Encoding, 30);
    env->ReleaseStringUTFChars(Encoding, c_Encoding);

    c_image_object->ImageDesc.Size = ObjectSize;
    BIP_DBG("ObjectSize: %d ImageObject.ImageDesc.Size: %lu", ObjectSize, c_image_object->ImageDesc.Size);
    c_image_object->ImageDesc.Width = Width;
    BIP_DBG("Width: %d ImageObject.ImageDesc.Width: %lu", Width, c_image_object->ImageDesc.Width);
    c_image_object->ImageDesc.Height = Height;
    BIP_DBG("Height: %d ImageObject.ImageDesc.Height: %lu", Height, c_image_object->ImageDesc.Height);
    c_image_object->ImageDesc.Width2 = Width2;
    BIP_DBG("Width2: %d ImageObject.ImageDesc.Width2: %lu", Width2, c_image_object->ImageDesc.Width2);
    c_image_object->ImageDesc.Height2 = Height2;
    BIP_DBG("Height2: %d ImageObject.ImageDesc.Height2: %lu", Height2, c_image_object->ImageDesc.Height2);

   return;
}





static jboolean bipiEnableNative(JNIEnv* env, jobject object)
{
    bip_initiator_enable();
    
    return JNI_TRUE;
}

static jboolean bipiDisableNative(JNIEnv* env, jobject object)
{
    bip_initiator_disable();
  
    return JNI_TRUE;
}

static jboolean bipiDisconnectNative(JNIEnv* env, jobject object, jstring BdAddr)
{
/*
    T_BDADDR bdaddr, r_bdaddr;
    const char *c_bdaddr = env->GetStringUTFChars(BdAddr, NULL);
    BIP_DBG("c_bdaddr: %s", c_bdaddr);

    bip_util_convert_string2array(c_bdaddr, bdaddr);
    env->ReleaseStringUTFChars(BdAddr, c_bdaddr);

    bip_util_reverse_array(r_bdaddr,bdaddr);
*/
    T_BDADDR r_bdaddr;
    getExtBDADDR(env, BdAddr, r_bdaddr);

    bip_initiator_disconnect(r_bdaddr);

    return JNI_TRUE;
}

static jboolean bipiGetCapabilityReqNative(JNIEnv* env, jobject object, jstring BdAddr)
{
/*
    T_BDADDR bdaddr, r_bdaddr;
    const char *c_bdaddr = env->GetStringUTFChars(BdAddr, NULL);
    BIP_DBG("c_bdaddr: %s", c_bdaddr);

    bip_util_convert_string2array(c_bdaddr, bdaddr);
    env->ReleaseStringUTFChars(BdAddr, c_bdaddr);

    bip_util_reverse_array(r_bdaddr,bdaddr);
*/
    T_BDADDR r_bdaddr;
    getExtBDADDR(env, BdAddr, r_bdaddr);

    bip_initiator_getcapability_request(r_bdaddr); 

    return JNI_TRUE;
}

//static const char *classBipImagePathName = "com/mediatek/bluetooth/bip/BipImage";
static jboolean bipiPushImageNative(JNIEnv* env, jobject object, jstring BdAddr, jobject imageObject)
{
/*
    T_BDADDR bdaddr, r_bdaddr;
    const char *c_bdaddr = env->GetStringUTFChars(BdAddr, NULL);
    BIP_DBG("c_bdaddr: %s", c_bdaddr);

    bip_util_convert_string2array(c_bdaddr, bdaddr);
    env->ReleaseStringUTFChars(BdAddr, c_bdaddr);

    bip_util_reverse_array(r_bdaddr,bdaddr);
*/
    T_BDADDR r_bdaddr;
    getExtBDADDR(env, BdAddr, r_bdaddr);


    T_BIP_OBJECT c_image_object;
    memset( &c_image_object, 0, sizeof(c_image_object));
    getExtImageObject(env, imageObject, &c_image_object);

/*
//test code
    memcpy(BipObject.DirName, "/sdcard", 256);
    memcpy(BipObject.FileName, "adb.jpg", 256);
   
    BipObject.ObjectSize = 73220;
    memcpy(BipObject.ImageDesc.Version, "1.0", 10);
    memcpy(BipObject.ImageDesc.Encoding, "JPEG", 30);
    BipObject.ImageDesc.Height = 469;
    BipObject.ImageDesc.Width = 352;
    BipObject.ImageDesc.Height2 = 0;
    BipObject.ImageDesc.Width2 = 0;
    BipObject.ImageDesc.Size = 73220;
*/
    bip_initiator_pushimage(r_bdaddr, &c_image_object);

    return JNI_TRUE;
}

static jboolean bipiPushThumbnailNative(JNIEnv* env, jobject object, jstring BdAddr, jobject imageObject)
{
    T_BDADDR r_bdaddr;
    getExtBDADDR(env, BdAddr, r_bdaddr);


    T_BIP_OBJECT c_image_object;
    memset( &c_image_object, 0, sizeof(c_image_object));
    getExtImageObject(env, imageObject, &c_image_object);

    bip_initiator_pushthumbnail(r_bdaddr, &c_image_object);

    return JNI_TRUE;
}


static jboolean bipiAuthRspNative(JNIEnv* env, jobject object, jobject authReply){return JNI_TRUE;}





static jboolean biprEnableNative(JNIEnv* env, jobject object, jstring rootPath){

    const char *c_rootPath = env->GetStringUTFChars(rootPath, NULL);
    BIP_DBG("c_rootPath: %s", c_rootPath);

    char RootPath[MAX_FILE_NAME_LEN] = {0};
    memcpy(RootPath, c_rootPath, strlen(c_rootPath));

    env->ReleaseStringUTFChars(rootPath, c_rootPath);

    bip_responder_enable(RootPath);

    return JNI_TRUE;
}

static jboolean biprDisableNative(JNIEnv* env, jobject object){

    bip_responder_disable();

    return JNI_TRUE;
}

static jboolean biprDisconnectNative(JNIEnv* env, jobject object){

    bip_responder_disconnect();

    return JNI_TRUE;
}

static jboolean biprAuthorizeRspNative(JNIEnv* env, jobject object, jint auth){

    bip_responder_authorize_response(auth);

    return JNI_TRUE;
}

//static jboolean biprGetCapabilityRspNative(JNIEnv* env, jobject object, jobject replay, jobject capability){
static jboolean biprGetCapabilityRspNative(JNIEnv* env, jobject object, jint reply, jobject capability){


    jclass capa_clazz, format_clazz, descriptor_clazz;
    capa_clazz = env->FindClass("com/mediatek/bluetooth/bip/Capability");
    descriptor_clazz = env->FindClass("com/mediatek/bluetooth/bip/ImageDescriptor");
    format_clazz = env->FindClass("com/mediatek/bluetooth/bip/ImageFormat");
    int i = 0;


    T_BIP_IMAGING_CAPABILITY c_capa_object;
    memset( &c_capa_object, 0, sizeof(c_capa_object));


    jfieldID field_PreferFormat = env->GetFieldID(capa_clazz, "PreferFormat", "Lcom/mediatek/bluetooth/bip/ImageDescriptor;");
    jobject PreferFormat = env->GetObjectField(capability, field_PreferFormat);
    jfieldID field_ImageFormats = env->GetFieldID(capa_clazz, "ImageFormats", "[Lcom/mediatek/bluetooth/bip/ImageFormat;");
    jobjectArray ImageFormats = (jobjectArray)env->GetObjectField(capability, field_ImageFormats);
    //jobject FormatObjs = env->GetObjectField(capability, field_ImageFormats);
    //jobjectArray* ImageFormats = reinterpret_cast<jobjectArray*>(&FormatObjs); 
/*
  jobject mvdata = env->GetObjectField (jc, aID); 
  jdoubleArray * arr = reinterpret_cast<jdoubleArray*>(&mvdata) 
  double * data = env->GetDoubleArrayElements(*arr, NULL); 
  env->ReleaseDoubleArrayElements(*arr, data, 0); 
*/
    jfieldID field_NumImageFormats = env->GetFieldID(capa_clazz, "NumImageFormats", "I");
    jint NumImageFormats = env->GetIntField(capability, field_NumImageFormats);

    c_capa_object.NumImageFormats = NumImageFormats;



    jfieldID field_Version = env->GetFieldID(descriptor_clazz, "Version", "Ljava/lang/String;");
    jstring Version = (jstring)env->GetObjectField(PreferFormat, field_Version);
    const char *c_Version = env->GetStringUTFChars(Version, NULL);

    jfieldID field_Encoding = env->GetFieldID(descriptor_clazz, "Encoding", "Ljava/lang/String;");
    jstring Encoding = (jstring)env->GetObjectField(PreferFormat, field_Encoding);
    const char *c_Encoding = env->GetStringUTFChars(Encoding, NULL);

    jfieldID field_Height = env->GetFieldID(descriptor_clazz, "Height", "I");
    jint Height = env->GetIntField(PreferFormat, field_Height);

    jfieldID field_Width = env->GetFieldID(descriptor_clazz, "Width", "I");
    jint Width = env->GetIntField(PreferFormat, field_Width);

    jfieldID field_Height2 = env->GetFieldID(descriptor_clazz, "Height2", "I");
    jint Height2 = env->GetIntField(PreferFormat, field_Height2);

    jfieldID field_Width2 = env->GetFieldID(descriptor_clazz, "Width2", "I");
    jint Width2 = env->GetIntField(PreferFormat, field_Width2);

    jfieldID field_Size = env->GetFieldID(descriptor_clazz, "Size", "I");
    jint Size = env->GetIntField(PreferFormat, field_Size);

    jfieldID field_Transform = env->GetFieldID(descriptor_clazz, "Transform", "I");
    jint Transform = env->GetIntField(PreferFormat, field_Transform);


    BIP_DBG("PreferFormat.version: %s", c_Version);
    memcpy(c_capa_object.PreferFormat.Version, c_Version, 10);
    env->ReleaseStringUTFChars(Version, c_Version);
    BIP_DBG("PreferFormat.encoding: %s", c_Encoding);
    memcpy(c_capa_object.PreferFormat.Encoding, c_Encoding, 30);
    env->ReleaseStringUTFChars(Encoding, c_Encoding);

    c_capa_object.PreferFormat.Width = Width;
    BIP_DBG("PreferFormat.width: %d", Width);
    c_capa_object.PreferFormat.Height = Height;
    BIP_DBG("PreferFormat.width2: %d", Width2);
    c_capa_object.PreferFormat.Width2 = Width2;
    BIP_DBG("PreferFormat.height: %d", Height);
    c_capa_object.PreferFormat.Height2 = Height2; 
    BIP_DBG("PreferFormat.height2: %d", Height2);
    c_capa_object.PreferFormat.Size = Size;
    BIP_DBG("PreferFormat.size: %d", Size);
    c_capa_object.PreferFormat.Transform = (T_BIP_TRANS)Transform;
    BIP_DBG("PreferFormat.transform: %d", Transform);


    for (i=0; i<NumImageFormats; i++) {

        jobject ImageFormat = (jobject)env->GetObjectArrayElement(ImageFormats, i);
        //jobject ImageFormat = (jobject)env->GetObjectArrayElement(*ImageFormats, i);


        jfieldID field_Encoding = env->GetFieldID(format_clazz, "Encoding", "Ljava/lang/String;");
        jstring Encoding = (jstring)env->GetObjectField(ImageFormat, field_Encoding);
        const char *c_Encoding = env->GetStringUTFChars(Encoding, NULL);

        jfieldID field_Height = env->GetFieldID(format_clazz, "Height", "I");
        jint Height = env->GetIntField(ImageFormat, field_Height);

        jfieldID field_Width = env->GetFieldID(format_clazz, "Width", "I");
        jint Width = env->GetIntField(ImageFormat, field_Width);

        jfieldID field_Height2 = env->GetFieldID(format_clazz, "Height2", "I");
        jint Height2 = env->GetIntField(ImageFormat, field_Height2);

        jfieldID field_Width2 = env->GetFieldID(format_clazz, "Width2", "I");
        jint Width2 = env->GetIntField(ImageFormat, field_Width2);

        jfieldID field_Size = env->GetFieldID(format_clazz, "Size", "I");
        jint Size = env->GetIntField(ImageFormat, field_Size);


        BIP_DBG("ImageFormats[%d].encoding: %s", i, c_Encoding);
        memcpy(c_capa_object.ImageFormats[i].Encoding, c_Encoding, 30);
        env->ReleaseStringUTFChars(Encoding, c_Encoding);

        c_capa_object.ImageFormats[i].Width = Width;
        BIP_DBG("ImageFormats[%d].width: %d", i, Width);
        c_capa_object.ImageFormats[i].Height = Height;
        BIP_DBG("ImageFormats[%d].width2: %d", i, Width2);
        c_capa_object.ImageFormats[i].Width2 = Width2;
        BIP_DBG("ImageFormats[%d].height: %d", i, Height);
        c_capa_object.ImageFormats[i].Height2 = Height2;
        BIP_DBG("ImageFormats[%d].height2: %d", i, Height2);
        c_capa_object.ImageFormats[i].Size = Size;
        BIP_DBG("ImageFormats[%d].size: %d", i, Size);
//    env->ReleaseOaDoubleArrayElements(*ImageFormats, ImageFormat, 0); 
    }


    bip_responder_getcapability_response(reply, &c_capa_object);

    return JNI_TRUE;
}


//static jboolean biprAccessRspNative(JNIEnv* env, jobject object, jobject reply){
static jboolean biprAccessRspNative(JNIEnv* env, jobject object, jint reply, jint thumbnail, jstring rootPath){
	const char *c_rootPath = env->GetStringUTFChars(rootPath, NULL);
	BIP_DBG("c_rootPath: %s", c_rootPath);
	
	char RootPath[MAX_FILE_NAME_LEN] = {0};
	memcpy(RootPath, c_rootPath, strlen(c_rootPath));
	
	env->ReleaseStringUTFChars(rootPath, c_rootPath);

    bip_responder_access_response(reply, thumbnail, RootPath);

    return JNI_TRUE;
}

// 20120627 added by mtk71255 for avoiding duplicated name file override
static void biprObjRename(JNIEnv* env, jobject object, jstring newName) {

	const char* c_newName = env->GetStringUTFChars(newName, NULL);
	BIP_DBG("mtk71255 added : c_newName: %s", c_newName);

	char l_newName[MAX_FILE_NAME_LEN] = {0};
	memcpy(l_newName, c_newName, strlen(c_newName));

	bip_responder_obj_rename(l_newName);
	env->ReleaseStringUTFChars(newName, c_newName);
}



static const char *classAuthInfoPathName = "com/mediatek/bluetooth/bip/AuthInfo";
static jboolean bipAuthRspNative(JNIEnv* env, jobject object, jobject auth_reply, jboolean isResponder){

    native_data_t *nat;

    BIP_DBG("+bip bipAuthRspNative");
    nat = get_native_data(env, object);
    if(!nat)
    {
        BIP_ERR("[ERR] nat is null");
        return JNI_FALSE;
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

    if ( isResponder )
    {
        bip_responder_auth_response(&c_auth_reply);
    }
    else
    {
        bip_initiator_auth_response(&c_auth_reply);
    }

    BIP_DBG("c_bAuth: %d c_UserId: %s c_Passwd: %s",
          c_auth_reply.bAuth, c_auth_reply.UserId, c_auth_reply.Passwd);
    BIP_DBG("-bip bipAuthRspNative");

    return JNI_TRUE;
}




// ----------------------------------------------------------------------------
static const char *classPathName = "com/mediatek/bluetooth/bip/BluetoothBipServer";
static JNINativeMethod methods[] = {
  {"classInitNative", "()V", (void*)classInitNative},
  {"initializeDataNative", "()V", (void*)initializeDataNative},
  {"cleanupDataNative", "()V", (void*)cleanupDataNative},

  {"enableServiceNative", "()Z", (void*)enableServiceNative },
  {"startListenNative", "()Z", (void*)startListenNative },
  {"stopListenNative", "()Z", (void*)stopListenNative },
  {"disableServiceNative", "()V", (void*)disableServiceNative },

  {"bipiEnableNative", "()Z", (void*)bipiEnableNative},
  {"bipiDisableNative", "()Z", (void*)bipiDisableNative},
  //{"bipiAuthRspNative", "(Lcom/mediatek/bluetooth/bip/AuthInfo;)Z", (void*)bipiAuthRspNative},
  {"bipiDisconnectNative", "(Ljava/lang/String;)Z", (void*)bipiDisconnectNative},
  {"bipiGetCapabilityReqNative", "(Ljava/lang/String;)Z", (void*)bipiGetCapabilityReqNative},
  {"bipiPushImageNative", "(Ljava/lang/String;Lcom/mediatek/bluetooth/bip/BipImage;)Z", (void*)bipiPushImageNative},
  {"bipiPushThumbnailNative", "(Ljava/lang/String;Lcom/mediatek/bluetooth/bip/BipImage;)Z", (void*)bipiPushThumbnailNative},

  {"bipAuthRspNative", "(Lcom/mediatek/bluetooth/bip/AuthInfo;Z)Z", (void*)bipAuthRspNative},

  {"biprEnableNative", "(Ljava/lang/String;)Z", (void*)biprEnableNative},
  {"biprDisableNative", "()Z", (void*)biprDisableNative},
  //{"biprAuthRspNative", "(Lcom/mediatek/bluetooth/bip/AuthInfo;)Z", (void*)biprAuthRspNative},
  {"biprDisconnectNative", "()Z", (void*)biprDisconnectNative},
  {"biprAuthorizeRspNative", "(I)Z", (void*)biprAuthorizeRspNative},
  {"biprGetCapabilityRspNative", "(ILcom/mediatek/bluetooth/bip/Capability;)Z", (void*)biprGetCapabilityRspNative},
 // {"biprGetCapabilityRspNative",
 //  "(Lcom/mediatek/bluetooth/bip/AuthRes;Lcom/mediatek/bluetooth/bip/Capability;)Z",
 //  (void*)biprGetCapabilityRspNative},
  {"biprAccessRspNative", "(IILjava/lang/String;)Z", (void*)biprAccessRspNative},
 // {"biprAccessRspNative", "(Lcom/mediatek/bluetooth/bip/AuthRes;)Z", (void*)biprAccessRspNative},
 // {"listenIndicationNative", "(Z)Z", (void*)listenIndicationNative},
 	{"biprObjRename", "(Ljava/lang/String;)V", (void*)biprObjRename}, // mtk71255 added 20120216
};
/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        BIP_ERR("unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        BIP_ERR("failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}
/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}
/*
 * This is called by the VM when the shared library is first loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    BIP_DBG("+");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        BIP_ERR("failed(%d)", result);
        return result;
    }
    assert(env != NULL);

    if (!registerNatives(env)) {
        BIP_ERR("failed(%d)", result);
        return result;
    }

    result = JNI_VERSION_1_4;

    BIP_DBG("-");
    return result;
}


