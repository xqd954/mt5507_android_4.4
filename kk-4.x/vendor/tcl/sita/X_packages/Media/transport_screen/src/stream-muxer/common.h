/*
 * common.h
 *
 *  Created on: 2012-4-28
 *      Author: zhangsh
 */

#ifndef COMMON_H_
#define COMMON_H_

#define TS_MUXING_STREAM_NAME "ts_muxer_tmp_file.ts"

#define AUDIO_QUEUE_ENTRY_BYTES   (1024*4)
#define AUDIO_QUEUE_ENTRY_SIZES   (8)
#define VIDEO_QUEUE_ENTRY_BYTES   (1024*256)
#define VIDEO_QUEUE_ENTRY_SIZES   (8)
#define STREAM_QUEUE_ENTRY_BYTES  (1024*8)
#define STREAM_QUEUE_ENTRY_SIZES  (128)

typedef struct _QueueBufferPriv
{
	void * audio_queue;
	void * video_queue;
	void * stream_queue;
	lthread_mutex_t audio_queue_mutex;
	lthread_cond_t audio_queue_cond;
	lthread_mutex_t video_queue_mutex;
	lthread_cond_t video_queue_cond;
	lthread_mutex_t stream_queue_mutex;
	lthread_cond_t stream_queue_cond;
} QueueBufferPriv;

typedef struct _StreamBufferPriv
{
	void *ptr;
	size_t  len;
}StrBufPriv;

#endif /* COMMON_H_ */
