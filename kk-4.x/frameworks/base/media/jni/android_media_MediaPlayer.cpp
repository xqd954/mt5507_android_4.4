/*
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "MediaPlayer-JNI"
#include "utils/Log.h"

#include <media/mediaplayer.h>
#include <media/MediaPlayerInterface.h>
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
#include "android_runtime/Log.h"
#include "utils/Errors.h"  // for status_t
#include "utils/KeyedVector.h"
#include "utils/String8.h"
#include "android_media_Utils.h"

#include "android_os_Parcel.h"
#include "android_util_Binder.h"
#include <binder/Parcel.h>
#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

// ----------------------------------------------------------------------------

using namespace android;

// ----------------------------------------------------------------------------

struct fields_t {
    jfieldID    context;
    jfieldID    surface_texture;

    jmethodID   post_event;

    jmethodID   proxyConfigGetHost;
    jmethodID   proxyConfigGetPort;
    jmethodID   proxyConfigGetExclusionList;
};
static fields_t fields;

static Mutex sLock;

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNIMediaPlayerListener: public MediaPlayerListener
{
public:
    JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIMediaPlayerListener();
    virtual void notify(int msg, int ext1, int ext2, const Parcel *obj = NULL);
private:
    JNIMediaPlayerListener();
    jclass      mClass;     // Reference to MediaPlayer class
    jobject     mObject;    // Weak ref to MediaPlayer Java object to call on
};

JNIMediaPlayerListener::JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{

    // Hold onto the MediaPlayer class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        ALOGE("Can't find android/media/MediaPlayer");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the MediaPlayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);
}

JNIMediaPlayerListener::~JNIMediaPlayerListener()
{
    // remove global references
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}

void JNIMediaPlayerListener::notify(int msg, int ext1, int ext2, const Parcel *obj)
{
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (obj && obj->dataSize() > 0) {
        jobject jParcel = createJavaParcelObject(env);
        if (jParcel != NULL) {
            Parcel* nativeParcel = parcelForJavaObject(env, jParcel);
            nativeParcel->setData(obj->data(), obj->dataSize());
            env->CallStaticVoidMethod(mClass, fields.post_event, mObject,
                    msg, ext1, ext2, jParcel);
            env->DeleteLocalRef(jParcel);
        }
    } else {
        env->CallStaticVoidMethod(mClass, fields.post_event, mObject,
                msg, ext1, ext2, NULL);
    }
    if (env->ExceptionCheck()) {
        ALOGW("An exception occurred while notifying an event.");
        LOGW_EX(env);
        env->ExceptionClear();
    }
}

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
        player->incStrong((void*)setMediaPlayer);
    }
    if (old != 0) {
        old->decStrong((void*)setMediaPlayer);
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

static void
android_media_MediaPlayer_setDataSourceAndHeaders(
        JNIEnv *env, jobject thiz, jstring path,
        jobjectArray keys, jobjectArray values) {

    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    if (path == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }

    const char *tmp = env->GetStringUTFChars(path, NULL);
    if (tmp == NULL) {  // Out of memory
        return;
    }
    ALOGV("setDataSource: path %s", tmp);

    String8 pathStr(tmp);
    env->ReleaseStringUTFChars(path, tmp);
    tmp = NULL;

    // We build a KeyedVector out of the key and val arrays
    KeyedVector<String8, String8> headersVector;
    if (!ConvertKeyValueArraysToKeyedVector(
            env, keys, values, &headersVector)) {
        return;
    }

    status_t opStatus =
        mp->setDataSource(
                pathStr,
                headersVector.size() > 0? &headersVector : NULL);

    process_media_player_call(
            env, thiz, opStatus, "java/io/IOException",
            "setDataSource failed." );
}

static void
android_media_MediaPlayer_setDataSourceFD(JNIEnv *env, jobject thiz, jobject fileDescriptor, jlong offset, jlong length)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    if (fileDescriptor == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    ALOGV("setDataSourceFD: fd %d", fd);
    process_media_player_call( env, thiz, mp->setDataSource(fd, offset, length), "java/io/IOException", "setDataSourceFD failed." );
}

static sp<IGraphicBufferProducer>
getVideoSurfaceTexture(JNIEnv* env, jobject thiz) {
    IGraphicBufferProducer * const p = (IGraphicBufferProducer*)env->GetIntField(thiz, fields.surface_texture);
    return sp<IGraphicBufferProducer>(p);
}

static void
decVideoSurfaceRef(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        return;
    }

    sp<IGraphicBufferProducer> old_st = getVideoSurfaceTexture(env, thiz);
    if (old_st != NULL) {
        old_st->decStrong((void*)decVideoSurfaceRef);
    }
}

static void
setVideoSurface(JNIEnv *env, jobject thiz, jobject jsurface, jboolean mediaPlayerMustBeAlive)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        if (mediaPlayerMustBeAlive) {
            jniThrowException(env, "java/lang/IllegalStateException", NULL);
        }
        return;
    }

    decVideoSurfaceRef(env, thiz);

    sp<IGraphicBufferProducer> new_st;
    if (jsurface) {
        sp<Surface> surface(android_view_Surface_getSurface(env, jsurface));
        if (surface != NULL) {
            new_st = surface->getIGraphicBufferProducer();
            if (new_st == NULL) {
                jniThrowException(env, "java/lang/IllegalArgumentException",
                    "The surface does not have a binding SurfaceTexture!");
                return;
            }
            new_st->incStrong((void*)decVideoSurfaceRef);
        } else {
            jniThrowException(env, "java/lang/IllegalArgumentException",
                    "The surface has been released");
            return;
        }
    }

    env->SetIntField(thiz, fields.surface_texture, (int)new_st.get());

    // This will fail if the media player has not been initialized yet. This
    // can be the case if setDisplay() on MediaPlayer.java has been called
    // before setDataSource(). The redundant call to setVideoSurfaceTexture()
    // in prepare/prepareAsync covers for this case.
    mp->setVideoSurfaceTexture(new_st);
}

static void
android_media_MediaPlayer_setVideoSurface(JNIEnv *env, jobject thiz, jobject jsurface)
{
    setVideoSurface(env, thiz, jsurface, true /* mediaPlayerMustBeAlive */);
}

static void
android_media_MediaPlayer_prepare(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    // Handle the case where the display surface was set before the mp was
    // initialized. We try again to make it stick.
    sp<IGraphicBufferProducer> st = getVideoSurfaceTexture(env, thiz);
    mp->setVideoSurfaceTexture(st);

    process_media_player_call( env, thiz, mp->prepare(), "java/io/IOException", "Prepare failed." );
}

static void
android_media_MediaPlayer_prepareAsync(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    // Handle the case where the display surface was set before the mp was
    // initialized. We try again to make it stick.
    sp<IGraphicBufferProducer> st = getVideoSurfaceTexture(env, thiz);
    mp->setVideoSurfaceTexture(st);

    process_media_player_call( env, thiz, mp->prepareAsync(), "java/io/IOException", "Prepare Async failed." );
}

static void
android_media_MediaPlayer_start(JNIEnv *env, jobject thiz)
{
    ALOGV("start");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->start(), NULL, NULL );
}

static void
android_media_MediaPlayer_stop(JNIEnv *env, jobject thiz)
{
    ALOGV("stop");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->stop(), NULL, NULL );
}

static void
android_media_MediaPlayer_pause(JNIEnv *env, jobject thiz)
{
    ALOGV("pause");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env,
                               thiz, 
                               mp->pause(),
                               "java/io/IOException", 
                               "pause failed." );
}

static jboolean
android_media_MediaPlayer_isPlaying(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    const jboolean is_playing = mp->isPlaying();

    ALOGV("isPlaying: %d", is_playing);
    return is_playing;
}

static void
android_media_MediaPlayer_seekTo(JNIEnv *env, jobject thiz, int msec)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    ALOGI("seekTo: %d(msec)", msec);
    process_media_player_call( env, thiz, mp->seekTo(msec), NULL, NULL );
}

static int
android_media_MediaPlayer_getVideoWidth(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int w;
    if (0 != mp->getVideoWidth(&w)) {
        ALOGE("getVideoWidth failed");
        w = 0;
    }
    ALOGV("getVideoWidth: %d", w);
    return w;
}

static int
android_media_MediaPlayer_getVideoHeight(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int h;
    if (0 != mp->getVideoHeight(&h)) {
        ALOGE("getVideoHeight failed");
        h = 0;
    }
    ALOGV("getVideoHeight: %d", h);
    return h;
}


static int
android_media_MediaPlayer_getCurrentPosition(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    int msec =0; //File inavlide, duration not 0 issue
#else
    int msec;
#endif  
    process_media_player_call( env, thiz, mp->getCurrentPosition(&msec), NULL, NULL );
    ALOGV("getCurrentPosition: %d (msec)", msec);
    return msec;
}

static int
android_media_MediaPlayer_getDuration(JNIEnv *env, jobject thiz)
{
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    int msec =0; //File inavlide, duration not 0 issue
#else
    int msec;
#endif    
    process_media_player_call( env, thiz, mp->getDuration(&msec), NULL, NULL );
    ALOGV("getDuration: %d (msec)", msec);
    return msec;
}

static void
android_media_MediaPlayer_reset(JNIEnv *env, jobject thiz)
{
    ALOGV("reset");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->reset(), NULL, NULL );
}

static void
android_media_MediaPlayer_setAudioStreamType(JNIEnv *env, jobject thiz, int streamtype)
{
    ALOGV("setAudioStreamType: %d", streamtype);
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setAudioStreamType((audio_stream_type_t) streamtype) , NULL, NULL );
}

static void
android_media_MediaPlayer_setLooping(JNIEnv *env, jobject thiz, jboolean looping)
{
    ALOGV("setLooping: %d", looping);
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setLooping(looping), NULL, NULL );
}

static jboolean
android_media_MediaPlayer_isLooping(JNIEnv *env, jobject thiz)
{
    ALOGV("isLooping");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    return mp->isLooping();
}

static void
android_media_MediaPlayer_setVolume(JNIEnv *env, jobject thiz, float leftVolume, float rightVolume)
{
    ALOGV("setVolume: left %f  right %f", leftVolume, rightVolume);
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setVolume(leftVolume, rightVolume), NULL, NULL );
}

// Sends the request and reply parcels to the media player via the
// binder interface.
static jint
android_media_MediaPlayer_invoke(JNIEnv *env, jobject thiz,
                                 jobject java_request, jobject java_reply)
{
    sp<MediaPlayer> media_player = getMediaPlayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }

    Parcel *request = parcelForJavaObject(env, java_request);
    Parcel *reply = parcelForJavaObject(env, java_reply);

    // Don't use process_media_player_call which use the async loop to
    // report errors, instead returns the status.
    return media_player->invoke(*request, reply);
}

// Sends the new filter to the client.
static jint
android_media_MediaPlayer_setMetadataFilter(JNIEnv *env, jobject thiz, jobject request)
{
    sp<MediaPlayer> media_player = getMediaPlayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }

    Parcel *filter = parcelForJavaObject(env, request);

    if (filter == NULL ) {
        jniThrowException(env, "java/lang/RuntimeException", "Filter is null");
        return UNKNOWN_ERROR;
    }

    return media_player->setMetadataFilter(*filter);
}

static jboolean
android_media_MediaPlayer_getMetadata(JNIEnv *env, jobject thiz, jboolean update_only,
                                      jboolean apply_filter, jobject reply)
{
    sp<MediaPlayer> media_player = getMediaPlayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

    Parcel *metadata = parcelForJavaObject(env, reply);

    if (metadata == NULL ) {
        jniThrowException(env, "java/lang/RuntimeException", "Reply parcel is null");
        return false;
    }

    metadata->freeData();
    // On return metadata is positioned at the beginning of the
    // metadata. Note however that the parcel actually starts with the
    // return code so you should not rewind the parcel using
    // setDataPosition(0).
    return media_player->getMetadata(update_only, apply_filter, metadata) == OK;
}

// This function gets some field IDs, which in turn causes class initialization.
// It is called from a static block in MediaPlayer, which won't run until the
// first time an instance of this class is used.
static void
android_media_MediaPlayer_native_init(JNIEnv *env)
{
    jclass clazz;

    clazz = env->FindClass("android/media/MediaPlayer");
    if (clazz == NULL) {
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (fields.context == NULL) {
        return;
    }

    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        return;
    }

    fields.surface_texture = env->GetFieldID(clazz, "mNativeSurfaceTexture", "I");
    if (fields.surface_texture == NULL) {
        return;
    }

    clazz = env->FindClass("android/net/ProxyProperties");
    if (clazz == NULL) {
        return;
    }

    fields.proxyConfigGetHost =
        env->GetMethodID(clazz, "getHost", "()Ljava/lang/String;");

    fields.proxyConfigGetPort =
        env->GetMethodID(clazz, "getPort", "()I");

    fields.proxyConfigGetExclusionList =
        env->GetMethodID(clazz, "getExclusionList", "()Ljava/lang/String;");
}

static void
android_media_MediaPlayer_native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
    ALOGV("native_setup");
    sp<MediaPlayer> mp = new MediaPlayer();
    if (mp == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

    // create new listener and give it to MediaPlayer
    sp<JNIMediaPlayerListener> listener = new JNIMediaPlayerListener(env, thiz, weak_this);
    mp->setListener(listener);

    // Stow our new C++ MediaPlayer in an opaque field in the Java object.
    setMediaPlayer(env, thiz, mp);
}

static void
android_media_MediaPlayer_release(JNIEnv *env, jobject thiz)
{
    ALOGV("release");
    decVideoSurfaceRef(env, thiz);
    sp<MediaPlayer> mp = setMediaPlayer(env, thiz, 0);
    if (mp != NULL) {
        // this prevents native callbacks after the object is released
        mp->setListener(0);
        mp->disconnect();
    }
}

static void
android_media_MediaPlayer_native_finalize(JNIEnv *env, jobject thiz)
{
    ALOGV("native_finalize");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp != NULL) {
        ALOGW("MediaPlayer finalized without being released");
    }
    android_media_MediaPlayer_release(env, thiz);
}

static void android_media_MediaPlayer_set_audio_session_id(JNIEnv *env,  jobject thiz, jint sessionId) {
    ALOGV("set_session_id(): %d", sessionId);
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setAudioSessionId(sessionId), NULL, NULL );
}

static jint android_media_MediaPlayer_get_audio_session_id(JNIEnv *env,  jobject thiz) {
    ALOGV("get_session_id()");
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }

    return mp->getAudioSessionId();
}

static void
android_media_MediaPlayer_setAuxEffectSendLevel(JNIEnv *env, jobject thiz, jfloat level)
{
    ALOGV("setAuxEffectSendLevel: level %f", level);
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setAuxEffectSendLevel(level), NULL, NULL );
}

static void android_media_MediaPlayer_attachAuxEffect(JNIEnv *env,  jobject thiz, jint effectId) {
    ALOGV("attachAuxEffect(): %d", effectId);
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->attachAuxEffect(effectId), NULL, NULL );
}

static jint
android_media_MediaPlayer_pullBatteryData(JNIEnv *env, jobject thiz, jobject java_reply)
{
    sp<IBinder> binder = defaultServiceManager()->getService(String16("media.player"));
    sp<IMediaPlayerService> service = interface_cast<IMediaPlayerService>(binder);
    if (service.get() == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "cannot get MediaPlayerService");
        return UNKNOWN_ERROR;
    }

    Parcel *reply = parcelForJavaObject(env, java_reply);

    return service->pullBatteryData(reply);
}

static jint
android_media_MediaPlayer_setRetransmitEndpoint(JNIEnv *env, jobject thiz,
                                                jstring addrString, jint port) {
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return INVALID_OPERATION;
    }

    const char *cAddrString = NULL;

    if (NULL != addrString) {
        cAddrString = env->GetStringUTFChars(addrString, NULL);
        if (cAddrString == NULL) {  // Out of memory
            return NO_MEMORY;
        }
    }
    ALOGV("setRetransmitEndpoint: %s:%d",
            cAddrString ? cAddrString : "(null)", port);

    status_t ret;
    if (cAddrString && (port > 0xFFFF)) {
        ret = BAD_VALUE;
    } else {
        ret = mp->setRetransmitEndpoint(cAddrString,
                static_cast<uint16_t>(port));
    }

    if (NULL != addrString) {
        env->ReleaseStringUTFChars(addrString, cAddrString);
    }

    if (ret == INVALID_OPERATION ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    }

    return ret;
}

static void
android_media_MediaPlayer_setNextMediaPlayer(JNIEnv *env, jobject thiz, jobject java_player)
{
    ALOGV("setNextMediaPlayer");
    sp<MediaPlayer> thisplayer = getMediaPlayer(env, thiz);
    if (thisplayer == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", "This player not initialized");
        return;
    }
    sp<MediaPlayer> nextplayer = (java_player == NULL) ? NULL : getMediaPlayer(env, java_player);
    if (nextplayer == NULL && java_player != NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", "That player not initialized");
        return;
    }

    if (nextplayer == thisplayer) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Next player can't be self");
        return;
    }
    // tie the two players together
    process_media_player_call(
            env, thiz, thisplayer->setNextMediaPlayer(nextplayer),
            "java/lang/IllegalArgumentException",
            "setNextMediaPlayer failed." );
    ;
}

static void
android_media_MediaPlayer_updateProxyConfig(
        JNIEnv *env, jobject thiz, jobject proxyProps)
{
    ALOGV("updateProxyConfig");
    sp<MediaPlayer> thisplayer = getMediaPlayer(env, thiz);
    if (thisplayer == NULL) {
        return;
    }

    if (proxyProps == NULL) {
        thisplayer->updateProxyConfig(
                NULL /* host */, 0 /* port */, NULL /* exclusionList */);
    } else {
        jstring hostObj = (jstring)env->CallObjectMethod(
                proxyProps, fields.proxyConfigGetHost);

        const char *host = env->GetStringUTFChars(hostObj, NULL);

        int port = env->CallIntMethod(proxyProps, fields.proxyConfigGetPort);

        jstring exclusionListObj = (jstring)env->CallObjectMethod(
                proxyProps, fields.proxyConfigGetExclusionList);

        const char *exclusionList =
            env->GetStringUTFChars(exclusionListObj, NULL);

        if (host != NULL && exclusionListObj != NULL) {
            thisplayer->updateProxyConfig(host, port, exclusionList);
        }

        if (exclusionList != NULL) {
            env->ReleaseStringUTFChars(exclusionListObj, exclusionList);
            exclusionList = NULL;
        }

        if (host != NULL) {
            env->ReleaseStringUTFChars(hostObj, host);
            host = NULL;
        }
    }
}

/*MTK Begin*/
static int
android_media_MediaPlayer_setPlayerType(JNIEnv *env, jobject thiz, jint playertype)
{
    int ret=-1;
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        ret=-1;
        return ret;
    }
    ret=mp->setPlayerType(playertype);
    return ret;
}

static int
android_media_MediaPlayer_setVideoRect(JNIEnv *env, jobject thiz, jint left, jint top, jint right, jint bottom)
{
    int ret =-1;
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        ret =-1;
        return ret;
    }
    ret=mp->setVideoRect(left, top, right, bottom);
    return ret;
}

static jint
MediaClient_setSvctxPath(JNIEnv *env, jobject thiz,jstring p_svctx_path)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    const char* ptemp_path = NULL;
    
    ptemp_path = env->GetStringUTFChars(p_svctx_path, NULL);
    ALOGD("ptemp_path %s\n",ptemp_path);
    
    int fgRet = mp->setSvctxPath(ptemp_path);

    LOGI("MediaClient_setSvctxPath(): Leave\n");

    return fgRet;
}

static jint
MediaClient_setPlayMode(JNIEnv *env, jobject thiz,int speed)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

    int fgRet = mp->setSpeed(speed);

    LOGI("MediaClient_setPlayMode(): Leave\n");

    return fgRet;
}

static jint
MediaClient_getPlayMode(JNIEnv *env, jobject thiz)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    LOGI("MediaClient_getPlayMode(): Enter\n");
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    

    int i4Speed = 0;

    if (!mp->getSpeed(i4Speed))
    {
        LOGE("MediaClient_getPlayMode(): service get failed\n");
    }

    LOGI("MediaClient_getPlayMode(): i4Speed=%d\n", i4Speed);

    LOGI("MediaClient_getPlayMode(): Leave\n");

    return i4Speed;
}
bool ConvertKeyValueArraysToKeyedVector(
        JNIEnv *env, jobjectArray keys, jobjectArray values,
        KeyedVector<String8, String8>* keyedVector) {

    int nKeyValuePairs = 0;
    bool failed = false;
    if (keys != NULL && values != NULL) {
        nKeyValuePairs = env->GetArrayLength(keys);
        failed = (nKeyValuePairs != env->GetArrayLength(values));
    }

    if (!failed) {
        failed = ((keys != NULL && values == NULL) ||
                  (keys == NULL && values != NULL));
    }

    if (failed) {
        LOGE("keys and values arrays have different length");
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return false;
    }

    for (int i = 0; i < nKeyValuePairs; ++i) {
        // No need to check on the ArrayIndexOutOfBoundsException, since
        // it won't happen here.
        jstring key = (jstring) env->GetObjectArrayElement(keys, i);
        jstring value = (jstring) env->GetObjectArrayElement(values, i);

        const char* keyStr = env->GetStringUTFChars(key, NULL);
        if (!keyStr) {  // OutOfMemoryError
            return false;
        }

        const char* valueStr = env->GetStringUTFChars(value, NULL);
        if (!valueStr) {  // OutOfMemoryError
            env->ReleaseStringUTFChars(key, keyStr);
            return false;
        }

        keyedVector->add(String8(keyStr), String8(valueStr));

        env->ReleaseStringUTFChars(key, keyStr);
        env->ReleaseStringUTFChars(value, valueStr);
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }
    return true;
}

bool
getIntValueFromJava(JNIEnv *env, const char* pszClassPath, jobject thiz, const char* pszVar_AliasName, int & i4Ret)
{
    if (env == NULL || pszClassPath == NULL || pszVar_AliasName == NULL)
    {
        LOGE("getIntValueFromJava(): parameter error!!\n");
        return false;
    }

    LOGI("getIntValueFromJava(): pszClassPath=%s, pszVar_AliasName=%s", pszClassPath, pszVar_AliasName);

    jclass jclassTemp;
    jclassTemp = env->FindClass(pszClassPath);
    if (jclassTemp == NULL) {
        LOGE("getIntValueFromJava(): find class(%s) failed, please check\n", pszClassPath);
        return false;
    }

    jfieldID jfieldIdInt;
    jfieldIdInt = env->GetFieldID(jclassTemp, pszVar_AliasName, "I");
    if (jfieldIdInt == NULL) {
        LOGE("getIntValueFromJava(): find variable(SubtitleAttr, %s) failed, please check\n", pszVar_AliasName);
        return false;
    }
    i4Ret = (jint)env->GetIntField(thiz, jfieldIdInt);

    LOGI("getIntValueFromJava(): i4Ret=%d", i4Ret);

    return true;
}

bool
getObjValueFromJava(JNIEnv *env, const char* pszClassPath_Parent, jobject thiz_Parent, const char* pszVar_AliasName,
                const char* pszClassPathType_Son, jobject & thiz_Son)
{
    if (env == NULL || pszClassPath_Parent == NULL || pszVar_AliasName == NULL || pszClassPathType_Son == NULL)
    {
        LOGE("getObjValueFromJava(): parameter error!!\n");
        return false;
    }

    LOGI("getObjValueFromJava(): pszClassPath_Parent=%s, pszClassPathType_Son=%s, pszVar_AliasName=%s",
        pszClassPath_Parent, pszClassPathType_Son, pszVar_AliasName);

    jclass jclassTemp;
    jclassTemp = env->FindClass(pszClassPath_Parent);
    if (jclassTemp == NULL) {
        LOGE("getObjValueFromJava(): find parent class(%s) failed, please check\n", pszClassPath_Parent);
        return false;
    }

    jfieldID jfieldObjId;
    jfieldObjId = env->GetFieldID(jclassTemp, pszVar_AliasName, pszClassPathType_Son);
    if (jfieldObjId == NULL)
    {
        LOGE("getObjValueFromJava(): find variable(%s, %s) failed, please check\n", pszClassPath_Parent, pszVar_AliasName);
        return false;
    }
    thiz_Son = (jobject)env->GetObjectField(thiz_Parent, jfieldObjId);

    return true;
}

bool
getByteValueFromJava(JNIEnv *env, const char* pszClassPath, jobject thiz, const char* pszVar_AliasName, char & chRet)
{
    if (env == NULL || pszClassPath == NULL || pszVar_AliasName == NULL)
    {
        LOGE("getByteValueFromJava(): parameter error!!\n");
        return false;
    }

    LOGI("getByteValueFromJava(): pszClassPath=%s, pszVar_AliasName=%s", pszClassPath, pszVar_AliasName);

    jclass jclassTemp;
    jclassTemp = env->FindClass(pszClassPath);
    if (jclassTemp == NULL) {
        LOGE("getByteValueFromJava(): find class(%s) failed, please check\n", pszClassPath);
        return false;
    }

    jfieldID jfieldIdByte;
    jfieldIdByte = env->GetFieldID(jclassTemp, pszVar_AliasName, "B");
    if (jfieldIdByte == NULL) {
        LOGE("getByteValueFromJava(): find variable(SubtitleAttr, %s) failed, please check\n", pszVar_AliasName);
        return false;
    }

    chRet = (jbyte)env->GetByteField(thiz, jfieldIdByte);

    LOGI("getByteValueFromJava(): chRet=%d", chRet);

    return true;
}

bool
getBoolValueFromJava(JNIEnv *env, const char* pszClassPath, jobject thiz, const char* pszVar_AliasName, bool & fgRet)
{
    if (env == NULL || pszClassPath == NULL || pszVar_AliasName == NULL)
    {
        LOGE("getBoolValueFromJava(): parameter error!!\n");
        return false;
    }

    LOGI("getBoolValueFromJava(): pszClassPath=%s, pszVar_AliasName=%s", pszClassPath, pszVar_AliasName);

    jclass jclassTemp;
    jclassTemp = env->FindClass(pszClassPath);
    if (jclassTemp == NULL) {
        LOGE("getBoolValueFromJava(): find class(%s) failed, please check\n", pszClassPath);
        return false;
    }

    jfieldID jfieldIdBool;
    jfieldIdBool = env->GetFieldID(jclassTemp, pszVar_AliasName, "Z");
    if (jfieldIdBool == NULL) {
        LOGE("getBoolValueFromJava(): find variable(SubtitleAttr, %s) failed, please check\n", pszVar_AliasName);
        return false;
    }

    fgRet = (jboolean)env->GetByteField(thiz, jfieldIdBool);

    LOGI("getBoolValueFromJava(): fgRet=%d", fgRet);

    return true;
}
static const char* const kJavaSubtitle_ClassPathName              = "android/media/SubtitleAttr";

static const char* const kJavaSubtitle_TimeOffset_ClassPathName   = "android/media/SubtitleAttr$Subtitle_TimeOffset";
static const char* const kJavaSubtitle_TimeOffset_Class_FieldType = "Landroid/media/SubtitleAttr$Subtitle_TimeOffset;";

static const char* const kJavaSubtitle_FontInfo_ClassPathName     = "android/media/SubtitleAttr$Subtitle_FontInfo";
static const char* const kJavaSubtitle_FontInfo_Class_FieldType   = "Landroid/media/SubtitleAttr$Subtitle_FontInfo;";

static const char* const kJavaSubtitle_Color_ClassPathName   = "android/media/SubtitleAttr$Subtitle_Color";
static const char* const kJavaSubtitle_Color_Class_FieldType = "Landroid/media/SubtitleAttr$Subtitle_Color;";

static const char* const kJavaSubtitle_DisplayRect_ClassPathName   = "android/media/SubtitleAttr$Subtitle_DisplayRect";
static const char* const kJavaSubtitle_DisplayRect_Class_FieldType = "Landroid/media/SubtitleAttr$Subtitle_DisplayRect;";

static const char* const kJavaSubtitle_DisplayOffset_ClassPathName   = "android/media/SubtitleAttr$Subtitle_DisplayOffset";
static const char* const kJavaSubtitle_DisplayOffset_Class_FieldType = "Landroid/media/SubtitleAttr$Subtitle_DisplayOffset;";

static const char* const kJava_ThumbnailInfo_ClassPathName   = "android/media/ThumbnailInfo";
static const char* const kJava_ThumbnailInfo_Class_FieldType = "Landroid/media/ThumbnailInfo;";

static const char* const kJavaPcmMediaInfo_ClassPathName   = "android/media/PcmMediaInfo";
static const char* const kJavaPcmMediaInfo_AudioPcmInfo_ClassPathName   = "android/media/PcmMediaInfo$AudioPcmInfo";
static const char* const kJavaPcmMediaInfo_AudioPcmInfo_Class_FieldType = "Landroid/media/PcmMediaInfo$AudioPcmInfo;";

static const char* const kJavaMediaPlayer_ClassPathName           = "android/media/MediaPlayer";
#ifndef CUSTOMER_LENGTH_MEDIUM
static const int CUSTOMER_LENGTH_MEDIUM = 128;
#endif

static int
MediaClient_setSubtitleAttr(JNIEnv *env, jobject thiz, jobject jObjSubtitleAttr)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    LOGI("setSubtitleAttr(): Enter\n");

    int ret= -1;

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        ret= -1;
        return ret;
    }
    

    int i4Mask;
    if (! getIntValueFromJava(env, kJavaSubtitle_ClassPathName, jObjSubtitleAttr, "mMask", i4Mask))
    {
        LOGE("setSubtitleAttr(): deal with %s: mMask, error happen\n", kJavaSubtitle_ClassPathName);
        ret= -1;
        return ret;
    }


    if(i4Mask & FLAG_SUBTITLE_TIME_OFFSET)
    {
        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_TIME_OFFSET, occur");

        jobject  jobjectTimeOffset;
        if (! getObjValueFromJava(env, kJavaSubtitle_ClassPathName, jObjSubtitleAttr, "mTimeOffset",
            kJavaSubtitle_TimeOffset_Class_FieldType, jobjectTimeOffset))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TIME_OFFSET, mTimeOffset, error happen\n", kJavaSubtitle_ClassPathName);
            ret= -1;
            return ret;
        }

        int i4TimeOffsetType = 0;
        if (! getIntValueFromJava(env, kJavaSubtitle_TimeOffset_ClassPathName, jobjectTimeOffset, "mTimeOffsetType", i4TimeOffsetType))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TIME_OFFSET, mTimeOffsetType, error happen\n", kJavaSubtitle_TimeOffset_ClassPathName);
            ret= -1;
            return ret;
        }

        int i4TimeOffsetValue = 0;
        if (! getIntValueFromJava(env, kJavaSubtitle_TimeOffset_ClassPathName, jobjectTimeOffset, "mOffsetValue", i4TimeOffsetValue))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TIME_OFFSET, mOffsetValue, error happen\n", kJavaSubtitle_TimeOffset_ClassPathName);
            ret= -1;
            return ret;
        }

        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_TIME_OFFSET, i4TimeOffsetType=%d, i4TimeOffsetValue=%d", i4TimeOffsetType, i4TimeOffsetValue);

        Subtitle_TimeOffset rTimeOffset;
        rTimeOffset.mTimeOffsetType = i4TimeOffsetType;
        rTimeOffset.mOffsetValue = i4TimeOffsetValue;

        ret=(int)ms->setSubtitleAttr((int)SUBTL_ATTR_TYPE_TIME_OFFSET, (void*)&rTimeOffset);
    }

    if(i4Mask & FLAG_SUBTITLE_FONT_INFO)
    {
        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_FONT_INFO, occur");

        jobject  jobjectFontInfo;
        if (! getObjValueFromJava(env, kJavaSubtitle_ClassPathName, jObjSubtitleAttr, "mFontInfo",
            kJavaSubtitle_FontInfo_Class_FieldType, jobjectFontInfo))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_FONT_INFO, mFontInfo, error happen\n", kJavaSubtitle_ClassPathName);
            ret= -1;
            return ret;
        }

        int i4FontSizeType = 0;
        if (! getIntValueFromJava(env, kJavaSubtitle_FontInfo_ClassPathName, jobjectFontInfo, "mFontSizeType", i4FontSizeType))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_FONT_INFO, mFontSizeType, error happen\n", kJavaSubtitle_FontInfo_ClassPathName);
            ret= -1;
            return ret;
        }

        char chCustomSize = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_FontInfo_ClassPathName, jobjectFontInfo, "mCustomSize", chCustomSize))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_FONT_INFO, mCustomSize, error happen\n", kJavaSubtitle_FontInfo_ClassPathName);
            ret= -1;
            return ret;
        }

        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_FONT_INFO, i4FontSizeType=%d, chCustomSize=%d", i4FontSizeType, chCustomSize);

        Subtitle_FontInfo rFontInfo;
        rFontInfo.mFontSizeType = i4FontSizeType;
        rFontInfo.mCustomSize = chCustomSize;

        ret=(int)ms->setSubtitleAttr((int)SUBTL_ATTR_TYPE_FONT_INFO, (void*)&rFontInfo);
    }

    if(i4Mask & FLAG_SUBTITLE_BG_COLOR)
    {
        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_BG_COLOR, occur");

        jobject  jobjectBgClr;
        if (! getObjValueFromJava(env, kJavaSubtitle_ClassPathName, jObjSubtitleAttr, "mBgColor",
            kJavaSubtitle_Color_Class_FieldType, jobjectBgClr))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_BG_COLOR, mBgColor, error happen\n", kJavaSubtitle_ClassPathName);
            ret= -1;
            return ret;
        }

        char chA = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectBgClr, "mA", chA))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_BG_COLOR, mA, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chR = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectBgClr, "mR", chR))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_BG_COLOR, mR, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chG = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectBgClr, "mG", chG))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_BG_COLOR, mG, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chB = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectBgClr, "mB", chB))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_BG_COLOR, mB, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_BG_COLOR, chA=%d, chR=%d, chG=%d, chB=%d", chA, chR, chG, chB);

        Subtitle_Color rBgClr;
        rBgClr.mA = chA;
        rBgClr.mR = chR;
        rBgClr.mG = chG;
        rBgClr.mB = chB;

        ret=(int)ms->setSubtitleAttr((int)SUBTL_ATTR_TYPE_BG_COLOR, (void*)&rBgClr);
    }

    if(i4Mask & FLAG_SUBTITLE_TEXT_COLOR)
    {
        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_TEXT_COLOR, occur");

        jobject  jobjectTxtClr;
        if (! getObjValueFromJava(env, kJavaSubtitle_ClassPathName, jObjSubtitleAttr, "mTextColor",
            kJavaSubtitle_Color_Class_FieldType, jobjectTxtClr))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TEXT_COLOR, mTextColor, error happen\n", kJavaSubtitle_ClassPathName);
            ret= -1;
            return ret;
        }

        char chA = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mA", chA))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TEXT_COLOR, mA, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chR = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mR", chR))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TEXT_COLOR, mR, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chG = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mG", chG))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TEXT_COLOR, mG, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chB = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mB", chB))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_TEXT_COLOR, mB, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_TEXT_COLOR, chA=%d, chR=%d, chG=%d, chB=%d", chA, chR, chG, chB);

        Subtitle_Color rTxtClr;
        rTxtClr.mA = chA;
        rTxtClr.mR = chR;
        rTxtClr.mG = chG;
        rTxtClr.mB = chB;

        ret=(int)ms->setSubtitleAttr((int)SUBTL_ATTR_TYPE_TEXT_COLOR, (void*)&rTxtClr);
    }

    //
    if(i4Mask & FLAG_SUBTITLE_EDGE_COLOR)
    {
        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_EDGE_COLOR, occur");

        jobject  jobjectTxtClr;
        if (! getObjValueFromJava(env, kJavaSubtitle_ClassPathName, jObjSubtitleAttr, "mEdgColor",
            kJavaSubtitle_Color_Class_FieldType, jobjectTxtClr))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_EDGE_COLOR, mEdgColor, error happen\n", kJavaSubtitle_ClassPathName);
            ret= -1;
            return ret;
        }

        char chA = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mA", chA))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_EDGE_COLOR, mA, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chR = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mR", chR))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_EDGE_COLOR, mR, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chG = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mG", chG))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_EDGE_COLOR, mG, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        char chB = 0;
        if (! getByteValueFromJava(env, kJavaSubtitle_Color_ClassPathName, jobjectTxtClr, "mB", chB))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_EDGE_COLOR, mB, error happen\n", kJavaSubtitle_Color_ClassPathName);
            ret= -1;
            return ret;
        }

        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_EDGE_COLOR, chA=%d, chR=%d, chG=%d, chB=%d", chA, chR, chG, chB);

        Subtitle_Color rTxtClr;
        rTxtClr.mA = chA;
        rTxtClr.mR = chR;
        rTxtClr.mG = chG;
        rTxtClr.mB = chB;

        ret=(int)ms->setSubtitleAttr((int)SUBTL_ATTR_TYPE_EDGE_COLOR, (void*)&rTxtClr);
    }
    //

    if(i4Mask & FLAG_SUBTITLE_DISPLAY_OFFSET)
    {
        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_DISPLAY_OFFSET, occur");

        jobject  jobjectDisplayOffset;
        if (! getObjValueFromJava(env, kJavaSubtitle_ClassPathName, jObjSubtitleAttr, "mDisplayOffset",
            kJavaSubtitle_DisplayOffset_Class_FieldType, jobjectDisplayOffset))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_DISPLAY_OFFSET, mDisplayOffset, error happen\n", kJavaSubtitle_ClassPathName);
            ret= -1;
            return ret;
        }

        int i4X = 0;
        if (! getIntValueFromJava(env, kJavaSubtitle_DisplayOffset_ClassPathName, jobjectDisplayOffset, "mX", i4X))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_DISPLAY_OFFSET, mX, error happen\n", kJavaSubtitle_FontInfo_ClassPathName);
            ret= -1;
            return ret;
        }

        int i4Y = 0;
        if (! getIntValueFromJava(env, kJavaSubtitle_DisplayOffset_ClassPathName, jobjectDisplayOffset, "mY", i4Y))
        {
            LOGE("setSubtitleAttr(): deal with %s: FLAG_SUBTITLE_DISPLAY_OFFSET, mY, error happen\n", kJavaSubtitle_FontInfo_ClassPathName);
            ret= -1;
            return ret;
        }

        LOGI("setSubtitleAttr(): config FLAG_SUBTITLE_DISPLAY_OFFSET, i4X=%d, i4Y=%d", i4X, i4Y);

        Subtitle_DisplayOffset rDisplayOffset;
        rDisplayOffset.mX = i4X;
        rDisplayOffset.mY = i4Y;

        ret=(int)ms->setSubtitleAttr((int)SUBTL_ATTR_TYPE_DISPLAY_OFFSET, (void*)&rDisplayOffset);

    }
    
    ms->flushSubtitleAttr();
    
    LOGI("setSubtitleAttr(): Leave\n");
    
    if(1==ret)//true
    {
        ret = 0;
    }
    else if(0==ret)//false
    {
        ret = -1;
    }
    
    return ret;
}

static jboolean
MediaClient_setAudioTrack(JNIEnv *env, jobject thiz,int track)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    bool fgRet = mp->setAudioTrack(track);

    LOGI("MediaClient_setAudioTrack(): Leave\n");

    return fgRet;
}

static jint
MediaClient_getBufferingTime(JNIEnv *env, jobject thiz)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    LOGI("MediaClient_getBufferingTime(): Enter\n");
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    

    int OutPercent = 0;

    if (!mp->getBufferPercent(OutPercent))
    {
        LOGE("MediaClient_getBufferingTime(): service get failed\n");
    }

    LOGI("MediaClient_getBufferingTime(): OutPercent=%d\n", OutPercent);

    LOGI("MediaClient_getBufferingTime(): Leave\n");

    return OutPercent;
}

static jboolean
MediaClient_onSubtitleTrack(JNIEnv *env, jobject thiz)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    bool fgRet = mp->onSubtitleTrack();

    LOGI("MediaClient_onSubtitleTrack(): Leave\n");

    return fgRet;
}

static jboolean
MediaClient_offSubtitleTrack(JNIEnv *env, jobject thiz)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    bool fgRet = mp->offSubtitleTrack();

    LOGI("MediaClient_offSubtitleTrack(): Leave\n");

    return fgRet;
}
static jboolean
MediaClient_setSubtitleDataSource(JNIEnv *env, jobject thiz, jstring url)
{
    LOGI("MediaClient_setSubtitleDataSource(): Enter\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_setSubtitleDataSource(): service not exist, error\n");
    }

    const char* pszURL = NULL;
    pszURL = env->GetStringUTFChars(url, NULL);
    if (!pszURL)
    {
        LOGE("MediaClient_setSubtitleDataSource(): url=null, return directly, error!!!!\n");
    }

    LOGI("MediaClient_setSubtitleDataSource(): url=%s\n", pszURL);
    bool fgRet = true;
    
    fgRet=ms->setSubtitleDataSource(pszURL);

    env->ReleaseStringUTFChars(url, pszURL);

    LOGI("MediaClient_setSubtitleDataSource(): Leave\n");
    
    return fgRet;
}

static jboolean
MediaClient_setSubtitleDataSourceEx(JNIEnv *env, jobject thiz, jstring urlSub, jstring urlIdx)
{
    LOGI("MediaClient_setSubtitleDataSourceEx(): Enter\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_setSubtitleDataSourceEx(): service not exist, error\n");
    }

    const char* pszUrlSub = NULL;
    pszUrlSub = env->GetStringUTFChars(urlSub, NULL);
    if (!pszUrlSub)
    {
        LOGE("MediaClient_setSubtitleDataSourceEx(): pszUrlSub=null, return directly, error!!!!\n");
    }
    LOGI("MediaClient_setSubtitleDataSourceEx(): pszUrlSub=%s\n", pszUrlSub);

    const char* pszUrlIdx = NULL;
    pszUrlIdx = env->GetStringUTFChars(urlIdx, NULL);
    if (!pszUrlIdx)
    {
        LOGE("MediaClient_setSubtitleDataSourceEx(): pszUrlIdx=null, return directly, error!!!!\n");
    }
    LOGI("MediaClient_setSubtitleDataSourceEx(): pszUrlIdx=%s\n", pszUrlIdx);
    
    bool fgRet = true;
    
    fgRet =ms->setSubtitleDataSourceEx(pszUrlSub, pszUrlIdx);

    env->ReleaseStringUTFChars(urlSub, pszUrlSub);
    env->ReleaseStringUTFChars(urlIdx, pszUrlIdx);

    LOGI("MediaClient_setSubtitleDataSourceEx(): Leave\n");
    return fgRet;
}

static jboolean
MediaClient_ifMP3Media(JNIEnv *env, jobject thiz)
{
    LOGI("MediaClient_ifMP3Media(): occur\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_ifMP3Media(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

    bool fgIsMP3 = false;
    bool fgRet = ms->ifMP3Media(fgIsMP3);

    if (! fgRet)
    {
        LOGE("MediaClient_ifMP3Media(): service get failed\n");
    }

    LOGI("MediaClient_ifMP3Media(): fgIsMP3=%d\n", fgIsMP3);

    return fgIsMP3;
}
static jboolean
MediaClient_seekToPosition(JNIEnv *env, jobject thiz, jlong position)
{
    LOGI("MediaClient_seekToPosition(): occur\n");

    LOGI("MediaClient_seekToPosition(): seek position(byte) = %lld", position);

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_seekToPosition(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

    bool fgSetByteSeek = ms->seekToPosition(position);
    if (! fgSetByteSeek)
    {
        LOGE("MediaClient_seekToPosition(): service get failed\n");
    }
    return fgSetByteSeek;
}

static jboolean
MediaClient_setPlayerRole(JNIEnv *env, jobject thiz, jint playerRole)
{
    LOGI("MediaClient_setPlayerRole(): occur\n");

    LOGI("MediaClient_setPlayerRole(): player's role is = %d", playerRole);

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_setPlayerRole(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

   int eType = playerRole;

   bool fgRet= ms->setPlayerRole(eType);
   return fgRet;
}

static jboolean
MediaClient_setABRepeat(JNIEnv *env, jobject thiz, jint type)
{
    LOGI("MediaClient_setABRepeat(): Enter\n");
    
    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGI("MediaClient_setABRepeat(): service not exist, error\n");
        return false;
    }

    bool fgRet= ms->setABRepeat((int)type);
    if (!fgRet)
    {
        LOGI("MediaClient_setABRepeat(): service get failed\n");
    }

    LOGI("MediaClient_setABRepeat(): Leave\n");
    return fgRet;
}

static jboolean
MediaClient_setTS(JNIEnv *env, jobject thiz, jint index)
{
    LOGI("MediaClient_setTS: occur\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_setTS(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    if (!ms->setTS((int)index))
    {
        LOGE("MediaClient_setTS(): service get failed\n");
        return false;
    }

    LOGI("MediaClient_setTS()end");
    return true;
}

static jboolean
MediaClient_canDoTrick(JNIEnv *env, jobject thiz,jint speed)
{
    LOGI("MediaClient_canDoTrick: occur\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_canDoTrick(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

    return ( ms->canDoTrick(speed) );
}

static jboolean
MediaClient_canDoSeek(JNIEnv *env, jobject thiz,jint speed)
{
    LOGI("MediaClient_canDoSeek: occur\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_canDoSeek(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

    return ( ms->canDoSeek(speed) );
}

static void
MediaClient_step(JNIEnv *env, jobject thiz, jint amount)
{
    LOGI("MediaClient_step(): Enter\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_step(): service not exist, error\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return ;
    }
    bool fgRet = ms->step(amount);
    if (!fgRet)
    {
        LOGE("MediaClient_step(): service get failed\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    }

    LOGI("MediaClient_step(): Leave\n");
    return ;
}

static int MediaClient_shortLength(char const* str, int maxLength)
{
    int length=0;
    for(int i=0; i<maxLength; i++)
    {
        if(str[i] == 0) break;
        length++;
    }
    //LOGI("MediaClient_getAudioTrackInfo(): length:%d\n", length);
    return length;
}

static jint
MediaClient_getAllAudioTrackInfo(JNIEnv *env, jobject thiz, jboolean typeIsAudio)
{
    LOGI("MediaClient_getAllAudioTrackInfo(): Enter\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_getAllAudioTrackInfo(): service not exist, error\n");
        return -1;
    }
    int nCount = 0;
    
    if (! ms->getAudioTrackCount(nCount))
    {
        LOGE("MediaClient_getAllAudioTrackInfo(): getAudioTrackCount(), service get failed\n");
        return -1;
    }
    
    LOGI("MediaClient_getAllAudioTrackInfo(): count = %d\n", nCount);

    if (nCount == 0)
    {
        LOGI("MediaClient_getAllAudioTrackInfo(): count = 0, no need to fetch subtitle info\n");
        return 0;
    }
    
    jclass clazz;
    clazz = env->FindClass(kJavaMediaPlayer_ClassPathName);
    if (clazz == NULL) {
        LOGE("MediaClient_getAllAudioTrackInfo(): find class(%s) failed, please check\n", kJavaMediaPlayer_ClassPathName);
        return -1;
    }

    jmethodID setAllAudioTrackInfoJMethodId = env->GetMethodID(clazz, "setAllAudioTrackInfo",
    "(IIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    if (setAllAudioTrackInfoJMethodId == NULL)
    {
        LOGE("MediaClient_getAllAudioTrackInfo(): get method id, setAudioTrackInfo, failed\n");
        return -1;
    }
    
    for(int i4Index = 0; i4Index < nCount; ++ i4Index)
    {
        int AudioTrackNum;
        int BitRate;
        int TotalPlayTime;
        int CurrentPlayTime;
        int SampleRate;
        int CodecID;
        int ChannelNumber;
        String8 
             Year,
             Title,
             Album,
             Artist,
             Genre,
             Director,
             Copyright,
             Language;
        
        bool fgRet = ms->getAudioTrackInfo(typeIsAudio,
                                    i4Index, 
                                    AudioTrackNum,
                                    BitRate,
                                    TotalPlayTime,
                                    CurrentPlayTime,
                                    SampleRate,
                                    CodecID,
                                    ChannelNumber,
                                    Year,
                                    Title,
                                    Album,
                                    Artist,
                                    Genre,
                                    Director,
                                    Copyright,
                                    Language);
        if (!fgRet)
        {
            LOGE("MediaClient_getAllAudioTrackInfo(): service get failed\n");
            return false;
        }
        
        jstring jstrTitle = env->NewString((jchar*)Title.string(),
                            MediaClient_shortLength(Title.string(), CUSTOMER_LENGTH_MEDIUM));
        jstring jstrYear = env->NewString((jchar*)Year.string(),
                           MediaClient_shortLength(Year.string(), CUSTOMER_LENGTH_MEDIUM));
        jstring jstrAlbum = env->NewString((jchar*)Album.string(), 
                            MediaClient_shortLength(Album.string(), CUSTOMER_LENGTH_MEDIUM));
        jstring jstrArtist = env->NewString((jchar*)Artist.string(), 
                             MediaClient_shortLength(Artist.string(), CUSTOMER_LENGTH_MEDIUM));
        
        jstring jstrGenre = env->NewString((jchar*)Genre.string(), 
                            MediaClient_shortLength(Genre.string(), CUSTOMER_LENGTH_MEDIUM));
        jstring jstrDirector = env->NewString((jchar*)Director.string(), 
                               MediaClient_shortLength(Director.string(), CUSTOMER_LENGTH_MEDIUM));
        jstring jstrCopyright = env->NewString((jchar*)Copyright.string(), 
                                MediaClient_shortLength(Copyright.string(), CUSTOMER_LENGTH_MEDIUM));
        
        jstring jstrLanguage = env->NewString((jchar*)Language.string(), 
                                MediaClient_shortLength(Language.string(), CUSTOMER_LENGTH_MEDIUM));
        
        LOGI("MediaClient_getAllAudioTrackInfo(): Year %s\n",Year.string());
        LOGI("MediaClient_getAllAudioTrackInfo(): Language %s\n",Language.string());
        
        env->CallVoidMethod(thiz, setAllAudioTrackInfoJMethodId, nCount, i4Index,
                            AudioTrackNum, 
                            BitRate,
                            TotalPlayTime,
                            CurrentPlayTime,
                            SampleRate,
                            CodecID,
                            jstrYear,
                            jstrTitle,
                            jstrAlbum, 
                            jstrArtist,
                            ChannelNumber,
                            jstrGenre,
                            jstrDirector, 
                            jstrCopyright,
                            jstrLanguage);
    }


    LOGI("MediaClient_getAllAudioTrackInfo(): Leave\n");

    return true;
}

static jint
MediaClient_getAllSubtitleTrackInfo(JNIEnv *env, jobject thiz)
{
    LOGI("MediaClient_getAllSubtitleTrackInfo(): Enter\n");

    sp<MediaPlayer> ms = getMediaPlayer(env, thiz);
    if (ms == NULL ) {
        LOGE("MediaClient_getAllSubtitleTrackInfo(): service not exist, error\n");
        return -1;
    }

    int nSubtitleCount = 0;

    if (! ms->getSubtitleTrackCount(nSubtitleCount))
    {
        LOGE("MediaClient_getAllSubtitleTrackInfo(): getSubtitleTrackCount(), service get failed\n");
        return -1;
    }

    LOGI("MediaClient_getAllSubtitleTrackInfo(): subtitle count = %d\n", nSubtitleCount);

    if (nSubtitleCount == 0)
    {
        LOGI("MediaClient_getAllSubtitleTrackInfo(): subtitle count = 0, no need to fetch subtitle info\n");
        return 0;
    }

    //begin get each subtitle info
    jclass clazz;
    clazz = env->FindClass(kJavaMediaPlayer_ClassPathName);
    if (clazz == NULL) {
        LOGE("MediaClient_getAllSubtitleTrackInfo(): find class(%s) failed, please check\n", kJavaMediaPlayer_ClassPathName);
        return -1;
    }

    jmethodID setAllSubtitleTrackInfoJMethodId = env->GetMethodID(clazz,
                                                   "setAllSubtitleTrackInfo",
                                                   "(IIIILjava/lang/String;Ljava/lang/String;)V");

    if (setAllSubtitleTrackInfoJMethodId == NULL)
    {
        LOGE("MediaClient_getAllSubtitleTrackInfo(): get method id, getAllSubtitleTrackInfo, failed\n");
        return -1;
    }

    for(int i4Index = 0; i4Index < nSubtitleCount; ++ i4Index)
    {
        int SubtitleType,SubtitleCodeType;
        String8 SubtitleLanguage;
        String8 SubtitleTitle;
                                  
        bool fgRet = ms->getSubtitleTrackInfo(i4Index,
                                       SubtitleType,
                                       SubtitleCodeType,
                                       SubtitleLanguage,
                                       SubtitleTitle);
        if (!fgRet)
        {
            LOGE("MediaClient_getAllSubtitleTrackInfo(): service get failed\n");
            return -1;
        }

        jstring jstrLanguage = env->NewString((jchar*)SubtitleLanguage.string(), 
                                MediaClient_shortLength(SubtitleLanguage.string(), CUSTOMER_LENGTH_MEDIUM));

        jstring jstrSubtitle = env->NewString((jchar*)SubtitleTitle.string(), 
                                MediaClient_shortLength(SubtitleTitle.string(), CUSTOMER_LENGTH_MEDIUM));
        env->CallVoidMethod(thiz, 
                            setAllSubtitleTrackInfoJMethodId, 
                            nSubtitleCount, 
                            i4Index,
                            SubtitleType,
                            SubtitleCodeType,
                            jstrLanguage,
                            jstrSubtitle);
    }

    LOGI("MediaClient_getAllSubtitleTrackInfo(): Leave\n");

    return nSubtitleCount;
}

static jboolean
MediaClient_setSubtitleTrack(JNIEnv *env, jobject thiz,int track)
{
    ALOGD("%s %s %d",__FUNCTION__,__FILE__,__LINE__);
    
    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    bool fgRet = mp->setSubtitleTrack(track);

    LOGI("MediaClient_setAudioTrack(): Leave\n");

    return fgRet;
}
static jint
MediaClient_getCurrentBytePosition(JNIEnv *env, jobject thiz)
{
    //LOGI("MediaClient_getCurrentPosition(): Enter\n");

    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        LOGE("MediaClient_getCurrentPosition(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int mposition;

    process_media_player_call( env, thiz, mp->getCurrentBytePosition(&mposition), NULL, NULL );

    //LOGV("MediaClient_getCurrentPosition(): %d (msec)", msec);
    //LOGI("MediaClient_getCurrentPosition(): Leave\n");
    
    return (jint)mposition;
}

static jdouble
MediaClient_getDownloadRate(JNIEnv *env, jobject thiz)
{
    LOGI("MediaClient_getDownloadRate(): Enter\n");

    sp<MediaPlayer> mp = getMediaPlayer(env, thiz);
    if (mp == NULL ) {
        LOGE("MediaClient_getDownloadRate(): service not exist, throw exception\n");
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    double mposition;

    process_media_player_call( env, thiz, mp->getDownloadRate(&mposition), NULL, NULL );

    LOGV("MediaClient_getDownloadRate(): %lf (msec)", mposition);
    LOGI("MediaClient_getDownloadRate(): Leave\n");
    
    return (jdouble)mposition;
}
/*MTK END*/

// ----------------------------------------------------------------------------

static JNINativeMethod gMethods[] = {
    {
        "_setDataSource",
        "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V",
        (void *)android_media_MediaPlayer_setDataSourceAndHeaders
    },

    {"_setDataSource",       "(Ljava/io/FileDescriptor;JJ)V",    (void *)android_media_MediaPlayer_setDataSourceFD},
    {"_setVideoSurface",    "(Landroid/view/Surface;)V",        (void *)android_media_MediaPlayer_setVideoSurface},
    {"prepare",             "()V",                              (void *)android_media_MediaPlayer_prepare},
    {"prepareAsync",        "()V",                              (void *)android_media_MediaPlayer_prepareAsync},
    {"_start",              "()V",                              (void *)android_media_MediaPlayer_start},
    {"_stop",               "()V",                              (void *)android_media_MediaPlayer_stop},
    {"getVideoWidth",       "()I",                              (void *)android_media_MediaPlayer_getVideoWidth},
    {"getVideoHeight",      "()I",                              (void *)android_media_MediaPlayer_getVideoHeight},
    {"seekTo",              "(I)V",                             (void *)android_media_MediaPlayer_seekTo},
    {"_pause",              "()V",                              (void *)android_media_MediaPlayer_pause},
    {"isPlaying",           "()Z",                              (void *)android_media_MediaPlayer_isPlaying},
    {"getCurrentPosition",  "()I",                              (void *)android_media_MediaPlayer_getCurrentPosition},
    {"getDuration",         "()I",                              (void *)android_media_MediaPlayer_getDuration},
    {"_release",            "()V",                              (void *)android_media_MediaPlayer_release},
    {"_reset",              "()V",                              (void *)android_media_MediaPlayer_reset},
    {"setAudioStreamType",  "(I)V",                             (void *)android_media_MediaPlayer_setAudioStreamType},
    {"setLooping",          "(Z)V",                             (void *)android_media_MediaPlayer_setLooping},
    {"isLooping",           "()Z",                              (void *)android_media_MediaPlayer_isLooping},
    {"setVolume",           "(FF)V",                            (void *)android_media_MediaPlayer_setVolume},
    {"native_invoke",       "(Landroid/os/Parcel;Landroid/os/Parcel;)I",(void *)android_media_MediaPlayer_invoke},
    {"native_setMetadataFilter", "(Landroid/os/Parcel;)I",      (void *)android_media_MediaPlayer_setMetadataFilter},
    {"native_getMetadata", "(ZZLandroid/os/Parcel;)Z",          (void *)android_media_MediaPlayer_getMetadata},
    {"native_init",         "()V",                              (void *)android_media_MediaPlayer_native_init},
    {"native_setup",        "(Ljava/lang/Object;)V",            (void *)android_media_MediaPlayer_native_setup},
    {"native_finalize",     "()V",                              (void *)android_media_MediaPlayer_native_finalize},
    {"getAudioSessionId",   "()I",                              (void *)android_media_MediaPlayer_get_audio_session_id},
    {"setAudioSessionId",   "(I)V",                             (void *)android_media_MediaPlayer_set_audio_session_id},
    {"setAuxEffectSendLevel", "(F)V",                           (void *)android_media_MediaPlayer_setAuxEffectSendLevel},
    {"attachAuxEffect",     "(I)V",                             (void *)android_media_MediaPlayer_attachAuxEffect},
    {"native_pullBatteryData", "(Landroid/os/Parcel;)I",        (void *)android_media_MediaPlayer_pullBatteryData},
    {"native_setRetransmitEndpoint", "(Ljava/lang/String;I)I",  (void *)android_media_MediaPlayer_setRetransmitEndpoint},
    {"setNextMediaPlayer",  "(Landroid/media/MediaPlayer;)V",   (void *)android_media_MediaPlayer_setNextMediaPlayer},
    {"updateProxyConfig", "(Landroid/net/ProxyProperties;)V", (void *)android_media_MediaPlayer_updateProxyConfig},

    /*MTK Begin*/
	{
		"native_setPlayerType",
		"(I)I",
		(void *)android_media_MediaPlayer_setPlayerType
    },
	{
		"native_setVideoRect",
		"(IIII)I",
		(void *)android_media_MediaPlayer_setVideoRect
	},
    {
         "native_setSvctxPath",
         "(Ljava/lang/String;)V",
         (void *)MediaClient_setSvctxPath
    },
    {
        "native_setPlayMode",
        "(I)I",
        (void *)MediaClient_setPlayMode
    },
    {
        "native_getPlayMode",
        "()I",
        (void *)MediaClient_getPlayMode
    },
    {
        "native_setSubtitleAttr",
        "(Landroid/media/SubtitleAttr;)I",
        (void *)MediaClient_setSubtitleAttr
    },
    {
        "native_setAudioTrack",
        "(I)Z",
        (void *)MediaClient_setAudioTrack
    },
    {
        "native_getBufferPercent",
        "()I",
        (void *)MediaClient_getBufferingTime
    },
    {
        "native_onSubtitleTrack",
        "()Z",
        (void *)MediaClient_onSubtitleTrack
    },
    
    {
        "native_offSubtitleTrack",
         "()Z",
        (void *)MediaClient_offSubtitleTrack
    },
    {
        "native_setSubtitleDataSource",
        "(Ljava/lang/String;)Z",
        (void *)MediaClient_setSubtitleDataSource
    },/*15*/
    {
        "native_setSubtitleDataSourceEx",
        "(Ljava/lang/String;Ljava/lang/String;)Z",
        (void *)MediaClient_setSubtitleDataSourceEx
    },
    {
        "native_ifMP3Media",
        "()Z",
        (void *)MediaClient_ifMP3Media
    },
     {
         "native_seekToPosition",
         "(J)Z",
         (void *)MediaClient_seekToPosition
     },
     {
         "native_setPlayerRole",
         "(I)Z",
         (void *)MediaClient_setPlayerRole
     },
     {
      "native_setABRepeat",
      "(I)Z",
     (void *)MediaClient_setABRepeat
     },
     {
         "native_setTS",
          "(I)Z",
         (void *)MediaClient_setTS
     },
     {
         "native_canDoTrick",
         "(I)Z",
         (void *)MediaClient_canDoTrick
     },
     {
         "native_canDoSeek",
         "(I)Z",
         (void *)MediaClient_canDoSeek
     },
     {
         "native_step",
         "(I)V",
         (void *)MediaClient_step
     },
     {
         "native_getAllAudioTrackInfo",
          "(Z)I",
         (void *)MediaClient_getAllAudioTrackInfo
     },
     {
         "native_getAllSubtitleTrackInfo",
         "()I",
         (void *)MediaClient_getAllSubtitleTrackInfo
     },
     {
         "native_setSubtitleTrack",
         "(I)Z",
         (void *)MediaClient_setSubtitleTrack
     },
     {
         "native_getCurrentBytePosition",
         "()I",
         (void *)MediaClient_getCurrentBytePosition
     },
     {
         "native_getDownloadRate",
         "()D",
         (void *)MediaClient_getDownloadRate
     },
   /*MTK END*/
};

static const char* const kClassPathName = "android/media/MediaPlayer";

// This function only registers the native methods
static int register_android_media_MediaPlayer(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(env,
                "android/media/MediaPlayer", gMethods, NELEM(gMethods));
}

extern int register_android_media_ImageReader(JNIEnv *env);
extern int register_android_media_Crypto(JNIEnv *env);
extern int register_android_media_Drm(JNIEnv *env);
extern int register_android_media_MediaCodec(JNIEnv *env);
extern int register_android_media_MediaExtractor(JNIEnv *env);
extern int register_android_media_MediaCodecList(JNIEnv *env);
extern int register_android_media_MediaMetadataRetriever(JNIEnv *env);
extern int register_android_media_MediaMuxer(JNIEnv *env);
extern int register_android_media_MediaRecorder(JNIEnv *env);
extern int register_android_media_MediaScanner(JNIEnv *env);
extern int register_android_media_ResampleInputStream(JNIEnv *env);
extern int register_android_media_MediaProfiles(JNIEnv *env);
extern int register_android_media_AmrInputStream(JNIEnv *env);
extern int register_android_mtp_MtpDatabase(JNIEnv *env);
extern int register_android_mtp_MtpDevice(JNIEnv *env);
extern int register_android_mtp_MtpServer(JNIEnv *env);

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (register_android_media_ImageReader(env) < 0) {
        ALOGE("ERROR: ImageReader native registration failed");
        goto bail;
    }

    if (register_android_media_MediaPlayer(env) < 0) {
        ALOGE("ERROR: MediaPlayer native registration failed\n");
        goto bail;
    }

    if (register_android_media_MediaRecorder(env) < 0) {
        ALOGE("ERROR: MediaRecorder native registration failed\n");
        goto bail;
    }

    if (register_android_media_MediaScanner(env) < 0) {
        ALOGE("ERROR: MediaScanner native registration failed\n");
        goto bail;
    }

    if (register_android_media_MediaMetadataRetriever(env) < 0) {
        ALOGE("ERROR: MediaMetadataRetriever native registration failed\n");
        goto bail;
    }

    if (register_android_media_AmrInputStream(env) < 0) {
        ALOGE("ERROR: AmrInputStream native registration failed\n");
        goto bail;
    }

    if (register_android_media_ResampleInputStream(env) < 0) {
        ALOGE("ERROR: ResampleInputStream native registration failed\n");
        goto bail;
    }

    if (register_android_media_MediaProfiles(env) < 0) {
        ALOGE("ERROR: MediaProfiles native registration failed");
        goto bail;
    }

    if (register_android_mtp_MtpDatabase(env) < 0) {
        ALOGE("ERROR: MtpDatabase native registration failed");
        goto bail;
    }

    if (register_android_mtp_MtpDevice(env) < 0) {
        ALOGE("ERROR: MtpDevice native registration failed");
        goto bail;
    }

    if (register_android_mtp_MtpServer(env) < 0) {
        ALOGE("ERROR: MtpServer native registration failed");
        goto bail;
    }

    if (register_android_media_MediaCodec(env) < 0) {
        ALOGE("ERROR: MediaCodec native registration failed");
        goto bail;
    }

    if (register_android_media_MediaExtractor(env) < 0) {
        ALOGE("ERROR: MediaCodec native registration failed");
        goto bail;
    }

    if (register_android_media_MediaMuxer(env) < 0) {
        ALOGE("ERROR: MediaMuxer native registration failed");
        goto bail;
    }

    if (register_android_media_MediaCodecList(env) < 0) {
        ALOGE("ERROR: MediaCodec native registration failed");
        goto bail;
    }

    if (register_android_media_Crypto(env) < 0) {
        ALOGE("ERROR: MediaCodec native registration failed");
        goto bail;
    }

    if (register_android_media_Drm(env) < 0) {
        ALOGE("ERROR: MediaDrm native registration failed");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}

// KTHXBYE
