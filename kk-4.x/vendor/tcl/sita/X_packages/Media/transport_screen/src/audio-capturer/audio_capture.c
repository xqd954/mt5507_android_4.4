/*
 * audio_capture.c
 *
 *  Created on: 2012-4-17
 *      Author: sujuan
 */

#include "audio_capture.h"
#include <tinyalsa/asoundlib.h>

#define LOG_TAG "screen_share_audio_caputure"

typedef enum _AudioCaptureState
{
	AUDIO_STATE_INIT,
	AUDIO_STATE_START,
	AUDIO_STATE_STOP,
	AUDIO_STATE_PAUSE,
	AUDIO_STATE_CLOSE,

} AudioCaptureState;

struct _AudioCapture
{
	AudioParameter *audioparameter;

	AudioCaptureState audio_state;
	lthread_mutex_t audio_mutex;
	lthread_cond_t audio_cond;

	int quit;

	unsigned int device;
};

static void* audio_capture_thread(void * args);

/**
 *
 * init audio capture
 **/
AudioCapture* audio_capture_init(AudioParameter *audioparameter)
{
	AudioCapture *audio_capture = NULL;
	AudioParameter *audio_parameter = NULL;

	LOGD("audio_capture_init queue:%p", audioparameter->queue);

	audio_capture = malloc(sizeof(AudioCapture));
	if (!audio_capture)
	{
		LOGD("audio_capture_init malloc AudioCapture failed");
		return NULL;
	}

	if (audioparameter)
	{
		audio_parameter = malloc(sizeof(AudioParameter));
		if (!audio_parameter)
		{
			LOGD("audio_capture_init malloc AudioParameter failed");
			return NULL;
		}

		memcpy(audio_parameter, audioparameter, sizeof(AudioParameter));

		audio_capture->audioparameter = audio_parameter;
	}

	audio_capture->device = 1;

	lthread_mutex_init(&audio_capture->audio_mutex);
	lthread_cond_init(&audio_capture->audio_cond);

	audio_capture->audio_state = AUDIO_STATE_INIT;
	audio_capture->quit = 0;

	LOGD(
			"audio_capture_init bits:%d, channels:%d, rate:%d, queue:%p",
			audio_capture->audioparameter->bits, audio_capture->audioparameter->channels, audio_capture->audioparameter->rate, audio_capture->audioparameter->queue);

	return audio_capture;
}

/**
 *
 * start audio capture
 **/

Ret audio_capture_start(AudioCapture* audio_capture)
{
	Ret state = RET_OK;

	LOGD("audio_capture_start in");
	LOGD(
			"audio_capture_start bits:%d, channels:%d, rate:%d, queue:%p",
			audio_capture->audioparameter->bits, audio_capture->audioparameter->channels, audio_capture->audioparameter->rate, audio_capture->audioparameter->queue);

	audio_capture->audio_state = AUDIO_STATE_START;

	lthread_t thread;

	lthread_create(&thread, audio_capture_thread, audio_capture, 1);

	return state;
}

/**
 *
 * pause audio capture
 **/
Ret audio_capture_pause(AudioCapture* audio_capture)
{
	Ret state = RET_OK;

	LOGD("audio_capture_pause");

	audio_capture->audio_state = AUDIO_STATE_PAUSE;

	return state;
}

/**
 *
 * close audio capture
 **/
Ret audio_capture_close(AudioCapture* audio_capture)
{
	Ret state = RET_OK;

	LOGD("audio_capture_close");

	audio_capture->quit = 1;

	lthread_mutex_lock(&audio_capture->audio_mutex);
	if (audio_capture->audio_state != AUDIO_STATE_STOP)
	{
		lthread_cond_wait(&audio_capture->audio_cond,
				&audio_capture->audio_mutex);
	}

	lthread_mutex_unlock(&audio_capture->audio_mutex);


	audio_capture->audio_state = AUDIO_STATE_CLOSE;

	if (audio_capture)
	{
		lthread_mutex_destroy(&audio_capture->audio_mutex);
		lthread_cond_destroy(&audio_capture->audio_cond);

		if (audio_capture->audioparameter)
		{
			free(audio_capture->audioparameter);
			audio_capture->audioparameter = NULL;
		}

		free(audio_capture);
		audio_capture = NULL;
	}

	printf(" %s->%s->%d : Audio capture close \n",__FILE__,__func__,__LINE__);

	return state;
}

/**
 *
 * audio capture thread
 **/
static void * audio_capture_thread(void * args)
{
	AudioCapture *audio_capture = (AudioCapture*) args;
	struct pcm_config config;
	struct pcm *pcm;
	char *buffer, *audio_entry;
	int ret = -1;
	unsigned int pcm_data_size;

	LOGD("audio_capture_thread 1");
	LOGD(
			"audio_capture_thread bits:%d, channels:%d, rate:%d, queue:%p",
			audio_capture->audioparameter->bits, audio_capture->audioparameter->channels, audio_capture->audioparameter->rate, audio_capture->audioparameter->queue);

	config.channels = audio_capture->audioparameter->channels;
	config.rate = audio_capture->audioparameter->rate;
	config.period_size = 1024;
	config.period_count = 4 /*4*/;

	if (audio_capture->audioparameter->bits == 32)
	{
		config.format = PCM_FORMAT_S32_LE;
	}
	else if (audio_capture->audioparameter->bits == 16)
	{
		config.format = PCM_FORMAT_S16_LE;
	}

	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;

	pcm = pcm_open(0, audio_capture->device, PCM_IN, &config);
	if (!pcm || !pcm_is_ready(pcm))
	{
		fprintf(stderr, "Unable to open PCM device (%s)\n", pcm_get_error(pcm));
		goto EXIT;
	}

	pcm_data_size = pcm_get_buffer_size(pcm);

#if 1
	buffer = malloc(pcm_data_size);
	if (!buffer)
	{
		fprintf(stderr, "Unable to allocate %d bytes\n", pcm_data_size);
		free(buffer);
		goto EXIT;
	}
#endif

	ltime_val_t time1;
	ltime_val_t time2;

	while (!audio_capture->quit && !pcm_read(pcm, buffer, pcm_data_size))
	{
		lthread_mutex_lock(audio_capture->audioparameter->queue_mutex);
		audio_entry = limq_enqueue_get(audio_capture->audioparameter->queue);
		lthread_mutex_unlock(audio_capture->audioparameter->queue_mutex);

		if (audio_entry)
		{
			memcpy(audio_entry,buffer,pcm_data_size);
			//release queue
			lthread_mutex_lock(audio_capture->audioparameter->queue_mutex);
			limq_enqueue_put(audio_capture->audioparameter->queue, audio_entry);
			lthread_mutex_unlock(audio_capture->audioparameter->queue_mutex);
		}
		else
		{
			LOGD("DBG: Audio ,There is no buffer for push PCM data ");
		}

	}

	/*while (!audio_capture->quit)
	{
		lthread_mutex_lock(&audio_capture->audioparameter->queue_mutex);
		audio_entry = limq_enqueue_get(audio_capture->audioparameter->queue);
		lthread_mutex_unlock(&audio_capture->audioparameter->queue_mutex);


		if (audio_entry)
		{
			ret = pcm_read(pcm, audio_entry, pcm_data_size);
			//release queue
			lthread_mutex_lock(&audio_capture->audioparameter->queue_mutex);
			limq_enqueue_put(audio_capture->audioparameter->queue, audio_entry);
			lthread_mutex_unlock(&audio_capture->audioparameter->queue_mutex);
		}

	}*/


EXIT:

#if 1
	if (buffer)
	{
		free(buffer);
	}
#endif

	if (pcm)
	{
		pcm_close(pcm);
	}

	lthread_mutex_lock(&audio_capture->audio_mutex);
	lthread_cond_broadcast(&audio_capture->audio_cond);
	lthread_mutex_unlock(&audio_capture->audio_mutex);

	audio_capture->audio_state = AUDIO_STATE_STOP;

	printf(" %s->%s->%d : Audio capture thread quit \n",__FILE__,__func__,__LINE__);
	return NULL;
}

