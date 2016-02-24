/*
 * stream_muxer.c
 *
 *  Created on: 2012-4-16
 *      Author: zhangsh
 */

#include "stream_muxer.h"
#include "ts_muxer.h"
#include "audio_capture.h"
#include "MstarVideoCapApi.h"
#include "common.h"

//liuyan start
#include <utils/Log.h>
#define LOG_TAG "stream-muxer::stream_muxer"
#define INFO(...) \
  do { \
      livecap_printf("\n");  \
      livecap_printf(__VA_ARGS__); \
      livecap_printf("\n"); \
      SLOGD(__VA_ARGS__); \
  } while(0)
#define livecap_printf(fmt, arg...) \
    do { \
        if (1)  printf(fmt, ##arg); \
    } while (0);
//liuyan end

struct _Stream_muxer
{
	QueueBufferPriv *queue_buffer;
	void * audio_capture;
	void * video_capture;
	void * ts_muxer;
	void * segmenter;
};

static void stream_muxer_clear(StreamMuxer * stream_muxer);

/**
 * Init stream muxer
 */
StreamMuxer * stream_muxer_init(char*path,int width,int height,int with_osd)
{	
	INFO( "stream_muxer_init ******** 20140424 ********\n");
        LOGD("stream_muxer_init IN");
	StreamMuxer *stream_muxer = NULL;
	QueueBufferPriv *queue_buf = NULL;
	limq_handle_t queue;
	Ret state = RET_OK;

	/*
	 * 1. prepare resource
	 */
	stream_muxer = malloc(sizeof(StreamMuxer));
	if (!stream_muxer)
	{
		LOG_ERR(" malloc StreamMuxer failed \n");
		return NULL;
	}
	memset(stream_muxer, 0, sizeof(StreamMuxer));
    
	queue_buf = malloc(sizeof(QueueBufferPriv));
	
	
	if (!queue_buf)
	{
		LOG_ERR(" malloc QueueBufferPriv failed \n");
		state = RET_FAIL;
		goto OUT;
	}
	memset(queue_buf, 0, sizeof(QueueBufferPriv));
	stream_muxer->queue_buffer = queue_buf;

	/* 1.1 create audio cap queue */
	queue = limq_create_ext(AUDIO_QUEUE_ENTRY_BYTES, AUDIO_QUEUE_ENTRY_SIZES,
			"audio queue", NULL);
	if (!queue)
	{
		LOG_ERR(" creat audio queue failed \n");
		state = RET_FAIL;
		goto OUT;
	}
	stream_muxer->queue_buffer->audio_queue = queue;
	LOGD(" create audio queue ,current entry is %d ", limq_entries(queue));

	lthread_mutex_init(&stream_muxer->queue_buffer->audio_queue_mutex);
	lthread_cond_init(&stream_muxer->queue_buffer->audio_queue_cond);

	/* 1.2 create video cap queue */
	queue = limq_create_ext(VIDEO_QUEUE_ENTRY_BYTES, VIDEO_QUEUE_ENTRY_SIZES,
			"video queue", NULL);
	if (!queue)
	{
		LOG_ERR(" creat video queue failed \n");
		state = RET_FAIL;
		goto OUT;
	}
	stream_muxer->queue_buffer->video_queue = queue;
	LOGD(" create video queue ,current entry is %d ", limq_entries(queue));

	lthread_mutex_init(&stream_muxer->queue_buffer->video_queue_mutex);
	lthread_cond_init(&stream_muxer->queue_buffer->video_queue_cond);

	/* 1.3 create output stream queue */
	queue = limq_create_ext(STREAM_QUEUE_ENTRY_BYTES, STREAM_QUEUE_ENTRY_SIZES,
			"output stream queue", NULL);
	if (!queue)
	{
		LOG_ERR(" create output stream queue failed \n");
		state = RET_FAIL;
		goto OUT;
	}
	stream_muxer->queue_buffer->stream_queue = queue;
	LOGD(" create video queue ,current entry is %d ", limq_entries(queue));

	lthread_mutex_init(&stream_muxer->queue_buffer->stream_queue_mutex);
	lthread_cond_init(&stream_muxer->queue_buffer->stream_queue_cond);

	/*
	 * 2. init audio cap
	 */
	AudioParameter ap;
	ap.bits = 16;
	ap.channels = 2;
	ap.rate = 48000;
	ap.queue = stream_muxer->queue_buffer->audio_queue;
	ap.queue_cond = &stream_muxer->queue_buffer->audio_queue_cond;
	ap.queue_mutex = &stream_muxer->queue_buffer->audio_queue_mutex;

	LOGD(" ap.queue_mutex = %p ",ap.queue_mutex);
	stream_muxer->audio_capture = audio_capture_init(&ap);
	if (!stream_muxer->audio_capture)
	{
		LOG_ERR(" audio_capture_init failed \n");
		state = RET_FAIL;
		goto OUT;
	}

	/*
	 * 3. init video cap
	 */
	MSVideoParameter vp; 
	vp.framerate = 20;
	vp.height = height;
	vp.width = width;
	vp.with_osd = with_osd;  
	vp.queue = stream_muxer->queue_buffer->video_queue;
	vp.queue_cond = &stream_muxer->queue_buffer->video_queue_cond;
	vp.queue_mutex = &stream_muxer->queue_buffer->video_queue_mutex;

	LOGD(" vp.queue_mutex = %p ",vp.queue_mutex);
	INFO( "stream_muxer_init init video cap vp.queue_mutex = %p",vp.queue_mutex);
	stream_muxer->video_capture = mstarv_capture_init(&vp);

	/*
	 * 4. init stream muxing
	 */
	stream_muxer->ts_muxer = ts_muxer_init(stream_muxer->queue_buffer,path);
	if (!stream_muxer->ts_muxer)
	{
		LOG_ERR(" ts_muxer_init failed \n");
		state = RET_FAIL;
		goto OUT;
	}

	/*
	 * 5. init stream segmenter
	 */
	/*SegmenterPara sp;
	sp.queue = stream_muxer->queue_buffer->stream_queue;
	sp.queue_cond = stream_muxer->queue_buffer->stream_queue_cond;
	sp.queue_mutex = stream_muxer->queue_buffer->stream_queue_mutex;

	stream_muxer->segmenter = segmenter_init(&sp);
	if (!stream_muxer->segmenter)
	{
		LOG_ERR(" segmenter_init failed \n");
		state = RET_FAIL;
		goto OUT;
	}*/

OUT:
	if (state != RET_OK)
	{
		stream_muxer_clear(stream_muxer);
	}

    LOGD("stream_muxer_init OUT");
	return stream_muxer;
}

/**
 * test camera state
 */
 
/* add by taoting 2012-11-15
		   for test camera if be used
		   if is be used by other application,must quit*/

Ret stream_muxer_camera_listen()
{
    return mstar_capture_camera_listen();
}


/**
 * Start stream muxer
 */
Ret stream_muxer_start(StreamMuxer * stream_muxer)
{
    LOGD("stream_muxer_start IN");
    	INFO( "stream_muxer_start =========\n");
	/*
	 * start video caping
	 */
	mstarv_capture_start(stream_muxer->video_capture);
	/*
	 * start audio caping
	 */
	audio_capture_start(stream_muxer->audio_capture);

	/*
	 * start stream muxing
	 */
	ts_muxer_start(stream_muxer->ts_muxer);
	/*
	 * start stream segmenter
	 */
	//segmenter_start(stream_muxer->segmenter);
	LOGD("stream_muxer_start OUT");
	return RET_OK;
}

/**
 * Pause stream muxer
 */
Ret stream_muxer_pause(StreamMuxer * stream_muxer)
{

	/*
	 * pause audio cap
	 */

	/*
	 * pause video cap
	 */

	/*
	 * pause stream muxing
	 */

	/*
	 * pause stream segmenter
	 */
	return RET_OK;
}

/**
 * close stream muxer
 */
void stream_muxer_close(StreamMuxer * stream_muxer)
{
    LOGD("stream_muxer_close IN");
	/*
	 * close video cap
	 */
	mstarv_capture_close(stream_muxer->video_capture);
	printf(" mstar video capture close end \n");

	/*
	 * close audio cap
	 */
	audio_capture_close(stream_muxer->audio_capture);
	printf(" audio capture close end \n");

	/*
	 * close stream muxing
	 */
	ts_muxer_close(stream_muxer->ts_muxer);
	printf(" ts muxer close end \n");

	/*
	 * close  stream segmenter
	 */
	//segmenter_stop(stream_muxer->segmenter);
	printf(" segmenter sotp  end \n");

	/*
	 * recycle resource
	 */
	stream_muxer_clear(stream_muxer);
	printf(" stream muxer clear end \n");
	LOGD("stream_muxer_close OUT");

}

static void stream_muxer_clear(StreamMuxer * stream_muxer)
{
    LOGD("stream_muxer_clear IN");
	if (stream_muxer)
	{
		if (stream_muxer->queue_buffer)
		{
			if (stream_muxer->queue_buffer->audio_queue)
			{
				limq_destroy(stream_muxer->queue_buffer->audio_queue);
			}
			if (stream_muxer->queue_buffer->video_queue)
			{
				limq_destroy(stream_muxer->queue_buffer->video_queue);
			}
			if (stream_muxer->queue_buffer->stream_queue)
			{
				//limq_destroy(stream_muxer->queue_buffer->stream_queue);
			}

			lthread_mutex_destroy(
					&stream_muxer->queue_buffer->audio_queue_mutex);
			lthread_cond_destroy(&stream_muxer->queue_buffer->audio_queue_cond);

			lthread_mutex_destroy(
					&stream_muxer->queue_buffer->video_queue_mutex);
			lthread_cond_destroy(&stream_muxer->queue_buffer->video_queue_cond);

			lthread_mutex_destroy(
					&stream_muxer->queue_buffer->stream_queue_mutex);
			lthread_cond_destroy(
					&stream_muxer->queue_buffer->stream_queue_cond);

			free(stream_muxer->queue_buffer);
			stream_muxer->queue_buffer = NULL;
		}

		free(stream_muxer);
		stream_muxer = NULL;
		LOGD("stream_muxer_clear OUT");
	}
}
