
#include <stdio.h>
#include "utils/Log.h"
#include "JNIHelp.h"
#include "string.h"
#include "screenrecorder.h"

#define printf ALOGD

static const char *classPathName = "com/tcl/screenrecorder/ScreenRecorder";
static MuxerContex *stream_muxer = NULL;


static void stream_jni_init(JNIEnv *env, jclass clazz)
{
	printf("stream_jni_init: \n");
	if(NULL == stream_muxer)
	{
		stream_muxer = screen_recorder_init();
	}
}


/*static void stream_jni_set_output_file_path(JNIEnv *env, jclass clazz,jstring path)
{
	const char *file = env->GetStringUTFChars(path,0);
	printf(" stream_jni_set_output_file_path ,  file = %s \n",file);
	if(stream_muxer)
	{
		screen_recorder_set_output_file(stream_muxer,file);
		env->ReleaseStringUTFChars(path,file);
	}
	printf("stream_jni_set_output_file_path: file = %s, stream_muxer = %p \n",file,stream_muxer);
}*/

static void stream_jni_set_output_file_fd(JNIEnv *env, jclass clazz,jobject fileDescriptor)
{
    if (fileDescriptor == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }

    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    printf("stream_jni_set_output_file_fd = %d ", fd);

    if(stream_muxer)
    {
    	screen_recorder_set_output_file_fd(stream_muxer,fd);
    }
}

static void stream_jni_set_output_format(JNIEnv *env, jclass clazz,jint output_format)
{
	printf("stream_jni_set_output_format: \n");
	if(stream_muxer)
	{
		screen_recorder_set_output_format(stream_muxer,output_format);
	}
}

static void stream_jni_set_record_mode(JNIEnv *env, jclass clazz,jint mode)
{
	printf("stream_jni_set_record_mode: %d\n", mode);
	if(stream_muxer)
	{
		screen_recorder_set_record_mode(stream_muxer,mode);
	}
}

static void stream_jni_set_video_size(JNIEnv *env, jclass clazz,jint width,jint height)
{
	printf("stream_jni_set_video_size: \n");
	if(stream_muxer)
	{
		screen_recorder_set_video_size(stream_muxer,width,height);
	}
}

static void stream_jni_set_video_frame_rate(JNIEnv *env, jclass clazz,jint frame_rate)
{
	printf("stream_jni_set_video_frame_rate: \n");
	if(stream_muxer)
	{
		screen_recorder_set_video_rate(stream_muxer,frame_rate);
	}
}

static void stream_jni_prepare(JNIEnv *env, jclass clazz)
{
	printf("stream_jni_prepare: liuyan 20140417\n");
	if(stream_muxer)
	{
		screen_recorder_prepare(stream_muxer);
	}
}

static void stream_jni_start(JNIEnv *env, jclass clazz)
{
	printf("stream_jni_start: liuyan 20140417\n");
	if(stream_muxer)
	{
		screen_recorder_start(stream_muxer);
	}
}

static void stream_jni_stop(JNIEnv *env, jclass clazz)
{
	printf("stream_jni_stop: \n");
	if(stream_muxer)
	{
		screen_recorder_stop(stream_muxer);
	}
}

static void stream_jni_reset(JNIEnv *env, jclass clazz)
{
	printf("stream_jni_reset: \n");
	if(stream_muxer)
	{
		screen_recorder_reset(stream_muxer);
	}
}

static void stream_jni_release(JNIEnv *env, jclass clazz)
{
	printf("stream_jni_release: \n");
	if(stream_muxer)
	{
		screen_recorder_release(stream_muxer);
		stream_muxer = NULL;
	}
}


static JNINativeMethod sMethods[] = {
	{"_init","()V",(void*)stream_jni_init},

	/*{"_setOutputFile","(Ljava/lang/String;)V",(void*)stream_jni_set_output_file_path},*/
	{"_setOutputFile","(Ljava/io/FileDescriptor;)V",(void*)stream_jni_set_output_file_fd},
	{"_setOutputFormat","(I)V",(void*)stream_jni_set_output_format},
	{"_setRecordMode","(I)V",(void*)stream_jni_set_record_mode},
	{"_setVideoSize","(II)V",(void*)stream_jni_set_video_size},
	{"_setVideoFrameRate","(I)V",(void*)stream_jni_set_video_frame_rate},

	{"_prepare","()V",(void*)stream_jni_prepare},
	{"_start","()V",(void*)stream_jni_start},
	{"_stop","()V",(void*)stream_jni_stop},
	{"_reset","()V",(void*)stream_jni_reset},
	{"_release","()V",(void*)stream_jni_release},

};

static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;

	clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		ALOGE("Native registration unable to find class '%s'", className);
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
	{
		ALOGE("RegisterNatives failed for '%s'", className);
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
	if (!registerNativeMethods(env, classPathName, sMethods, sizeof(sMethods) / sizeof(sMethods[0])))
	{
		return JNI_FALSE;
	}

	return JNI_TRUE;
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved)
{
	jint result = -1;
	JNIEnv* env = NULL;

	ALOGW("JNI_OnLoad");

	if (jvm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		ALOGE("ERROR: GetEnv failed");
		goto bail;
	}

	if (registerNatives(env) != JNI_TRUE)
	{
		ALOGE("ERROR: registerNatives failed");
		goto bail;
	}

	result = JNI_VERSION_1_4;


bail:
	return result;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved)
{
	JNIEnv *env;
	ALOGW("JNI_OnUnload");

	if (jvm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		return;
	}

	return;
}
