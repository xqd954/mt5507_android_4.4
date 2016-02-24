

//#define LOG_NDEBUG 0
#define LOG_TAG "TMediaPlayer-JNI"
#include "utils/Log.h"

#include <media/mediaplayer.h>
#include <media/MediaPlayerInterface.h>
#include <media/stagefright/MediaErrors.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <utils/threads.h>
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/android_view_Surface.h"
#include "utils/Errors.h"  // for status_t
#include "utils/KeyedVector.h"
#include "utils/String8.h"
//#include "android_media_Utils.h"
#include "android_os_Parcel.h"
#include "android_util_Binder.h"
#include <binder/Parcel.h>
//#include <gui/ISurfaceTexture.h>
#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/MemoryBase.h>
#include <core/SkBitmap.h>
using namespace android;

struct fields_t {
    jfieldID    context;
    jfieldID    surface_texture;
    jfieldID    surface_subtitle;
    jclass bitmapClazz;  // Must be a global ref
    jfieldID nativeBitmap;
    jmethodID createBitmapMethod;
    jclass configClazz;  // Must be a global ref
    jmethodID createConfigMethod;
    jmethodID   post_event;
};
static fields_t fields;

static Mutex sLock;

#define ARRAY_LENGTH   0x100000

// ----------------------------------------------------------------------------

static sp<MediaPlayer> getMediaPlayer(JNIEnv* env, jobject thiz)
{
    Mutex::Autolock l(sLock);
    MediaPlayer* const p = (MediaPlayer*)env->GetIntField(thiz, fields.context);
    return sp<MediaPlayer>(p);
}

static sp<MediaPlayer> setMediaPlayer(JNIEnv* env, jobject thiz, const sp<MediaPlayer>& player)
{
    Mutex::Autolock l(sLock);
    sp<MediaPlayer> old = (MediaPlayer*)env->GetIntField(thiz, fields.context);
    if (player.get()) {
        player->incStrong(thiz);
    }
    if (old != 0) {
        old->decStrong(thiz);
    }
    env->SetIntField(thiz, fields.context, (int)player.get());
    return old;
}

// If exception is NULL and opStatus is not OK, this method sends an error
// event to the client application; otherwise, if exception is not NULL and
// opStatus is not OK, this method throws the given exception to the client
// application.
static void process_media_player_call(JNIEnv *env, jobject thiz, status_t opStatus, const char* exception, const char *message)
{
    if (exception == NULL) {  // Don't throw exception. Instead, send an event.
        if (opStatus != (status_t) OK) {
            sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
            if (mp != 0) mp->notify(MEDIA_ERROR, opStatus, 0);
        }
    } else {  // Throw exception!
        if ( opStatus == (status_t) INVALID_OPERATION ) {
            jniThrowException(env, "java/lang/IllegalStateException", NULL);
        } else if ( opStatus == (status_t) PERMISSION_DENIED ) {
            jniThrowException(env, "java/lang/SecurityException", NULL);
        } else if ( opStatus != (status_t) OK ) {
            if (strlen(message) > 230) {
                // if the message is too long, don't bother displaying the status code
                jniThrowException( env, exception, message);
            } else {
                char msg[256];
                // append the status code to the message
                sprintf(msg, "%s: status=0x%X", message, opStatus);
                jniThrowException( env, exception, msg);
            }
        }
    }
}

static void process_bitmap_env(JNIEnv *env)
{
    jclass bitmapClazz = env->FindClass("android/graphics/Bitmap");
    if (bitmapClazz == NULL) {
        return;
    }
    fields.bitmapClazz = (jclass) env->NewGlobalRef(bitmapClazz);
    if (fields.bitmapClazz == NULL) {
        return;
    }
    fields.createBitmapMethod =
    env->GetStaticMethodID(fields.bitmapClazz, "createBitmap",
                           "(IILandroid/graphics/Bitmap$Config;)"
                           "Landroid/graphics/Bitmap;");
    if (fields.createBitmapMethod == NULL) {
        return;
    }

    fields.nativeBitmap = env->GetFieldID(fields.bitmapClazz, "mNativeBitmap", "I");
    if (fields.nativeBitmap == NULL) {
        return;
    }

    jclass configClazz = env->FindClass("android/graphics/Bitmap$Config");
    if (configClazz == NULL) {
        return;
    }
    fields.configClazz = (jclass) env->NewGlobalRef(configClazz);
    if (fields.configClazz == NULL) {
        return;
    }
    fields.createConfigMethod =
    env->GetStaticMethodID(fields.configClazz, "nativeToConfig",
                           "(I)Landroid/graphics/Bitmap$Config;");
    if (fields.createConfigMethod == NULL) {
        return;
    }
}


static void
android_media_TMediaPlayer_native_init(JNIEnv *env)
{
    jclass clazz;

    clazz = env->FindClass("com/tcl/media/TMediaPlayer");
    if (clazz == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find com/tcl/media/TMediaPlayer");
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (fields.context == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find MediaPlayer.mNativeContext");
        return;
    }

    fields.surface_subtitle = env->GetFieldID(clazz, "mNaticeSubtitleSurfaceTexture", "I");

    if (fields.surface_subtitle == NULL) {
        jniThrowException(env, "java/lang/RuntimeException",
                          "Can't find surface_subtitle");
        return;
    }

    process_bitmap_env(env);

}

static void
android_media_TMediaPlayer_getAudioTrackInfo(JNIEnv *env, jobject thiz, int type, jobject audioTrackInfo)
{
    ALOGD("JNI--getAudioTrackInfo");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    Parcel *metadata = parcelForJavaObject(env, audioTrackInfo);
    if (metadata == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "audioTrackInfo parcel is null");
        return;
    }

    metadata->freeData();

//    process_media_player_call(env, thiz, mp->getAudioTrackInfo(type, metadata), "java/io/IOException", "getAudioTrackInfo failed");
}

static void
android_media_TMediaPlayer_setAudioTrack(JNIEnv *env,  jobject thiz, jint track)
{
    ALOGD("setAudioTrack");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

//    process_media_player_call(env, thiz, mp->setAudioTrack(track), NULL, NULL);
}

static void
android_media_TMediaPlayer_setSubtitleTrack(JNIEnv *env,  jobject thiz, jint track)
{
    ALOGD("setSubtitleTrack");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

//    process_media_player_call(env, thiz, mp->setSubtitleTrack(track), NULL, NULL);
}

static void
android_media_TMediaPlayer_setSubtitleDataSource(JNIEnv *env,  jobject thiz, jstring path)
{
    ALOGD("setSubtitleDataSource");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    char* str_path = (char*)env->GetStringUTFChars(path, JNI_FALSE);
 //   process_media_player_call(env, thiz, mp->setSubtitleDataSource(str_path), NULL, NULL);
}

static void
android_media_TMediaPlayer_getSubtitleTrackInfo(JNIEnv *env, jobject thiz, int trackIndex, jobject subtitleTrackInfo)
{
    ALOGD("JNI--getSubtitleTrackInfo");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    Parcel *metadata = parcelForJavaObject(env, subtitleTrackInfo);
    if (metadata == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "subtitleTrackInfo parcel is null");
        return;
    }

    metadata->freeData();

 //   process_media_player_call(env, thiz, mp->getSubtitleTrackInfo(trackIndex, metadata), "java/io/IOException", "getSubtitleTrackInfo failed");
}

static jint
android_media_TMediaPlayer_getAllSubtitleTrackInfo(JNIEnv *env, jobject thiz, jobject allSubtitleTrackInfo)
{
    ALOGD("JNI--getAllSubtitleTrackInfo");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }

    Parcel *metadata = parcelForJavaObject(env, allSubtitleTrackInfo);
    if (metadata == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "allSubtitleTrackInfo parcel is null");
        return 0;
    }

    metadata->freeData();
 //   return mp->getAllSubtitleTrackInfo(metadata);
    return 0;
}

static jboolean
android_media_TMediaPlayer_setPlayMode(JNIEnv *env, jobject thiz, int speed)
{
    ALOGV("android_media_MediaPlayer_setPlayMode, speed(%d)", speed);
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateExcepption", NULL);
        return false;
    }

 //   return mp->setPlayMode(speed);
     return false;
}

static jint
android_media_TMediaPlayer_getPlayMode(JNIEnv *env, jobject thiz)
{
    ALOGV("android_media_MediaPlayer_getPlayMode");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateExcepption", NULL);
        return 0;
    }

 //   return mp->getPlayMode();
    return 0;
}

static void
android_media_TMediaPlayer_onSubtitleTrack(JNIEnv *env, jobject thiz)
{
    ALOGV("android_media_MediaPlayer_onSubtitleTrack");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateExcepption", NULL);
        return;
    }
//    mp->onSubtitleTrack();
    return;
}

static void
android_media_TMediaPlayer_offSubtitleTrack(JNIEnv *env, jobject thiz)
{
    ALOGV("android_media_MediaPlayer_offSubtitleTrack");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateExcepption", NULL);
        return;
    }
 //   mp->offSubtitleTrack();
    return;
}

static jstring
android_media_TMediaPlayer_getSubtitleData(JNIEnv *env, jobject thiz)
{

    ALOGV("android_media_MediaPlayer_getSubtitleData");
    return false;
    /*
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateExcepption", NULL);
        
    }
    int size = 0;
    char *pStr = mp->getSubtitleData(&size);
    ALOGD("android_media_MediaPlayer_getSubtitleData, ret %s", pStr);

    jstring jstrBuf = env->NewString((jchar *)pStr, size / 2);
    ////////////maybe wrong!!!!!!!
    //but have to free the memory!!!!!!!!!!!!
    if (pStr != NULL)
        free((void *)pStr);
    /////////////////
    return jstrBuf;
    */
}

static jstring
android_media_TMediaPlayer_getAudioTrackStringData(JNIEnv* env, jobject thiz, int infoType){
    ALOGV("android_media_getAudioTrackStringData");
    return false;
    /*
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateExcepption", NULL);
        return false;
    }
    int size = 0;
    char *pStr = mp->getAudioTrackStringData(&size,infoType);
    ALOGD("android_media_getAudioTrackStringData, ret %s", pStr);

    jstring jstrBuf = env->NewString((jchar *)pStr, size / 2);

    if (pStr != NULL)
        free((void *)pStr);

    return jstrBuf;
    */
}

static sp<IGraphicBufferProducer>
getSubtitleSurfaceTexture(JNIEnv* env, jobject thiz) {
    IGraphicBufferProducer * const p = (IGraphicBufferProducer*)env->GetIntField(thiz, fields.surface_subtitle);
    return sp<IGraphicBufferProducer>(p);
}

static void
decSubtitleSurfaceRef(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        return;
    }

    sp<IGraphicBufferProducer> old_st = getSubtitleSurfaceTexture(env, thiz);
    if (old_st != NULL) {
        old_st->decStrong(thiz);
    }
}

static void
setSubtitleSurface(JNIEnv *env, jobject thiz, jobject jsurface, jboolean mediaPlayerMustBeAlive)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        if (mediaPlayerMustBeAlive) {
            jniThrowException(env, "java/lang/IllegalStateException", NULL);
        }
        return;
    }

    decSubtitleSurfaceRef(env, thiz);

    sp<IGraphicBufferProducer> new_st;
    if (jsurface) {
        sp<Surface> surface(android_view_Surface_getSurface(env, jsurface));
        if (surface != NULL) {
            new_st = surface->getIGraphicBufferProducer();
            new_st->incStrong(thiz);
        } else {
            jniThrowException(env, "java/lang/IllegalArgumentException",
                              "The surface has been released");
            return;
        }
    }

    env->SetIntField(thiz, fields.surface_subtitle, (int)new_st.get());

    // This will fail if the media player has not been initialized yet. This
    // can be the case if setDisplay() on MediaPlayer.java has been called
    // before setDataSource(). The redundant call to setVideoSurfaceTexture()
    // in prepare/prepareAsync covers for this case.

   // mp->setSubtitleSurface(new_st);

}

static void
android_media_TMediaPlayer_setSubtitleSurface(JNIEnv *env, jobject thiz, jobject jsurface)
{
    setSubtitleSurface(env, thiz, jsurface, true);
}

static jint
android_media_TMediaPlayer_setSubtitleSync(JNIEnv *env, jobject thiz, int timeMs)
{
    ALOGD("setSubtitleSync");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

    int CurTime = 0;
   // CurTime = mp->setSubtitleSync(timeMs);
    ALOGV("getCurrentTime: %d (msec)", CurTime);
    return CurTime;
}


static jint android_media_TMediaPlayer_GetVideoInfo(JNIEnv *env, jobject thiz, jobject pstVedioInfo)
{
    ALOGD("android_media_MediaPlayer_GetVideoInfo");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }

    Parcel *metadata = parcelForJavaObject(env, pstVedioInfo);
    if (metadata == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "GetVideoInfo parcel is null");
        return UNKNOWN_ERROR;
    }
    metadata->freeData();
 //   return mp->getVideoInfo(metadata);
    return UNKNOWN_ERROR;
}

static jint android_media_TMediaPlayer_GetAudioCodecType(JNIEnv *env, jobject thiz, jobject pstAudioCodecType)
{
    ALOGD("android_media_MediaPlayer_GetAudioCodecType");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }

    Parcel *metadata = parcelForJavaObject(env, pstAudioCodecType);
    if (metadata == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "GetAudioCodecType parcel is null");
        return UNKNOWN_ERROR;
    }
    metadata->freeData();
 //   return mp->getAudioCodecType(metadata);
    return UNKNOWN_ERROR;
}

static void
android_media_TMediaPlayer_setVideoDisplayAspectRatio(JNIEnv *env, jobject thiz, int eRatio)
{
    ALOGD("android_media_TMediaPlayer_setVideoDisplayAspectRatio");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
  //  process_media_player_call( env, thiz, mp->setVideoDisplayAspectRatio(eRatio), NULL, NULL );
}


static JNINativeMethod gMethodsForTMediaPlayerClass[] = {
    {"native_init",         "()V",                              (void *)android_media_TMediaPlayer_native_init},
//    {"native_getAudioTrackInfo", "(ZLandroid/os/Parcel;)V",     (void *)android_media_TMediaPlayer_getAudioTrackInfo},
//  {"getAudioTrackStringData", "(I)Ljava/lang/String;",        (void *)android_media_TMediaPlayer_getAudioTrackStringData},
//    {"native_getSubtitleTrackInfo", "(ILandroid/os/Parcel;)V",  (void *)android_media_TMediaPlayer_getSubtitleTrackInfo},
//    {"native_getAllSubtitleTrackInfo","(Landroid/os/Parcel;)I", (void *)android_media_TMediaPlayer_getAllSubtitleTrackInfo},
 //   {"setAudioTrack",       "(I)V",                             (void *)android_media_TMediaPlayer_setAudioTrack},
//    {"setSubtitleTrack",    "(I)V",                             (void *)android_media_TMediaPlayer_setSubtitleTrack},
 //   {"setSubtitleDataSource", "(Ljava/lang/String;)V",          (void *)android_media_TMediaPlayer_setSubtitleDataSource},
 //   {"onSubtitleTrack",     "()V",                              (void *)android_media_TMediaPlayer_onSubtitleTrack},
 //   {"offSubtitleTrack",    "()V",                              (void *)android_media_TMediaPlayer_offSubtitleTrack},
 //   {"getSubtitleData",     "()Ljava/lang/String;",             (void *)android_media_TMediaPlayer_getSubtitleData},
//    {"_setSubtitleSurface", "(Landroid/view/Surface;)V",        (void *)android_media_TMediaPlayer_setSubtitleSurface},
//    {"setSubtitleSync",     "(I)I",                             (void *)android_media_TMediaPlayer_setSubtitleSync},
 //   {"native_getAudioCodecType", "(Landroid/os/Parcel;)I",      (void *)android_media_TMediaPlayer_GetAudioCodecType},
 //   {"native_getVideoInfo", "(Landroid/os/Parcel;)I",           (void *)android_media_TMediaPlayer_GetVideoInfo},
 //   {"native_setVideoDisplayAspectRatio", "(I)V",               (void *)android_media_TMediaPlayer_setVideoDisplayAspectRatio},
};

static const char* const kTMediaPlayerClassPathName = "com/tcl/media/TMediaPlayer";


static int register_android_media_TMediaPlayer(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(env,
                                                 kTMediaPlayerClassPathName, gMethodsForTMediaPlayerClass, NELEM(gMethodsForTMediaPlayerClass));
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (register_android_media_TMediaPlayer(env) < 0) {
        ALOGE("ERROR: TMediaPlayer native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    bail:
    return result;
}

