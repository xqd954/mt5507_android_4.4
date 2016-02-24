/*
 * ts_muxer.c
 *
 *  Created on: 2012-4-17
 *      Author: zhangsh
 */

#include "ts_muxer.h"
#include "MstarVideoCapApi.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>

#define LOG_TAG "TS_MUXER"

#define WRITE_BUFFER_TO_FILE
#define WRITE_BUFFER_TO_FIFO
#define FFMPEG_BIO_BUFFER_SIZE (1024 * 8)

/*
 * define muxer state
 */
typedef enum _MuxerState
{
	MUXER_STATE_INIT, MUXER_STATE_START, MUXER_STATE_STOP, MUXER_STATE_CLOSE
} MuxerState;

/*
 * Audio encoder parameters
 */
typedef struct _AudioEncoderPara
{
	int16_t *samples;
	uint8_t *audio_outbuf;
	int audio_outbuf_size;
	int audio_input_frame_size;
} AudioEncoderPara;

struct _TSMuxerPriv
{
	QueueBufferPriv *queue_buffer;

	MuxerState muxer_state;
	lthread_mutex_t muxer_mutex;
	lthread_cond_t muxer_cond;

	int aac_encoder_state;
	lthread_mutex_t aac_encoder_mutex;
	lthread_cond_t aac_encoder_cond;

	AVOutputFormat *output_format;
	AVFormatContext *output_context;
	AVStream *audio_stream;
	AVStream *video_stream;

	AudioEncoderPara audio_encoder_para;

	unsigned char bio_buffer[FFMPEG_BIO_BUFFER_SIZE];

	double audio_pts;
	double video_pts;
	int quit;
	const char *filename;

#ifdef WRITE_BUFFER_TO_FILE
	int fd;
	int fd_test;
#endif

};

typedef struct _read_para
{
	void *queue;
	lthread_mutex_t mutex;
	lthread_cond_t cond;
	lthread_t thread;
	AVFormatContext *ifmt_ctx;
	AVStream *iv_stream;
	TSMuxerPriv * ts_muxer;
} ReadPara;

typedef struct _AACEncoder
{
	void * input_queue;
	void * output_queue;
	lthread_mutex_t mutex;
	lthread_cond_t cond;
	lthread_t thread;
	TSMuxerPriv * ts_muxer;
} AACEncoder;

typedef struct _entry_head
{
	int keyframe;
	int len;
} EntryHead;

static int64_t video_base = 0;
static int64_t audio_base = 0;

static AVFrame *picture, *tmp_picture;
static uint8_t *video_outbuf;

static int open_audio(AVFormatContext *oc, AVStream *st, AudioEncoderPara *audio_encoder_para);
static void write_audio_frame(TSMuxerPriv *ts_muxer);
static void write_video_frame(TSMuxerPriv *ts_muxer);
static void close_audio(AVFormatContext *oc, AVStream *st, AudioEncoderPara *audio_encoder_para);
static void close_video(AVFormatContext *oc, AVStream *st);
static int ts_muxer_write_cb(void *opaque, uint8_t *buf, int buf_size);
static void * ts_muxing_thread(void *args);

/*
 * init ts muxer
 */
 char * output_file_name = NULL;
TSMuxerPriv * ts_muxer_init(QueueBufferPriv * queue_buffer,char* path)
{
	TSMuxerPriv * ts_muxer = NULL;

	ts_muxer = malloc(sizeof(TSMuxerPriv));
	if (!ts_muxer)
	{
		LOGD("malloc TSMuxerPriv failed \n");
		return NULL;
	}
	if (queue_buffer)
	{
		ts_muxer->queue_buffer = queue_buffer;
	}
	ts_muxer->filename = "/data/test_.ts";
	ts_muxer->quit = 0;
	video_base = 0;
    audio_base = 0;

	lthread_mutex_init(&ts_muxer->muxer_mutex);
	lthread_cond_init(&ts_muxer->muxer_cond);

	lthread_mutex_init(&ts_muxer->aac_encoder_mutex);
	lthread_cond_init(&ts_muxer->aac_encoder_cond);

	ts_muxer->muxer_state = MUXER_STATE_INIT;
	ts_muxer->aac_encoder_state = 0;
	LOGD("ts_muxer_init output_file_name = %s",output_file_name);
	output_file_name = path;

	return ts_muxer;
}

/*
 * start ts muxer
 */
Ret ts_muxer_start(TSMuxerPriv *ts_muxer)
{
	lthread_t thread;
	lthread_create(&thread, ts_muxing_thread, ts_muxer, 1);

	ts_muxer->muxer_state = MUXER_STATE_START;

	return RET_OK;
}

/*
 * pause ts muxer
 */
Ret ts_muxer_pause(TSMuxerPriv *ts_muxer)
{

	return RET_OK;
}

/*
 * close ts muxer
 */
void ts_muxer_close(TSMuxerPriv *ts_muxer)
{
	/*
	 * send quit message
	 */
	ts_muxer->quit = 1;

	/*
	 * wait ts muxer thread quit
	 */
	lthread_mutex_lock(&ts_muxer->muxer_mutex);
	if (ts_muxer->muxer_state != MUXER_STATE_STOP)
	{
		lthread_cond_wait(&ts_muxer->muxer_cond, &ts_muxer->muxer_mutex);
	}lthread_mutex_unlock(&ts_muxer->muxer_mutex);

	ts_muxer->muxer_state = MUXER_STATE_CLOSE;

	if (ts_muxer)
	{
		lthread_mutex_destroy(&ts_muxer->muxer_mutex);
		lthread_cond_destroy(&ts_muxer->muxer_cond);

		lthread_mutex_destroy(&ts_muxer->aac_encoder_mutex);
		lthread_cond_destroy(&ts_muxer->aac_encoder_cond);

		free(ts_muxer);
		ts_muxer = NULL;
	}
	printf(" %s->%s->%d : TS muxer close \n", __FILE__, __func__, __LINE__);
}

static int ts_muxer_write_cb(void *opaque, uint8_t *buf, int buf_size)
{

	TSMuxerPriv * ts_muxer = (TSMuxerPriv *) opaque;
	QueueBufferPriv * queue = ts_muxer->queue_buffer;

	void * stream_entry = NULL;
	void *buf_write = buf;

	int bytes = 0;

#if 1
#ifdef WRITE_BUFFER_TO_FILE
	if (ts_muxer->fd > 0)
	{
	    LOGD("ts_muxer_write_cb BEFORE");
		bytes = write(ts_muxer->fd, buf, buf_size);
		LOGD("ts_muxer_write_cb AFTER");
	}
#endif
#endif

#if 0
	/*
	 * write to queue
	 */
	while( buf_size >0 )
	{
		lthread_mutex_lock(&ts_muxer->queue_buffer->stream_queue_mutex);
		stream_entry = limq_enqueue_get(ts_muxer->queue_buffer->stream_queue);
		lthread_mutex_unlock(&ts_muxer->queue_buffer->stream_queue_mutex);

		if (stream_entry)
		{
			StrBufPriv str_buf;
			int head_len = sizeof(StrBufPriv);
			int body_len = STREAM_QUEUE_ENTRY_BYTES - head_len;

			// first write entry body
			if (buf_size >= body_len )
			{
				memcpy(stream_entry+head_len,buf_write,body_len);
				buf_size -= body_len;
				buf_write += body_len;
				str_buf.len = body_len;

			}
			else
			{
				memcpy(stream_entry+head_len,buf_write,buf_size);
				buf_write += buf_size;
				str_buf.len = buf_size;
				buf_size -= buf_size;
			}
			// then write entry header
			str_buf.ptr = stream_entry+head_len;
			memcpy(stream_entry,&str_buf,head_len);

			lthread_mutex_lock(&ts_muxer->queue_buffer->stream_queue_mutex);
			limq_enqueue_put(ts_muxer->queue_buffer->stream_queue,
					stream_entry);
			lthread_mutex_unlock(&ts_muxer->queue_buffer->stream_queue_mutex);

			lthread_mutex_lock(&ts_muxer->queue_buffer->stream_queue_mutex);
			LOGD(" current stream queue entry is %d \n",limq_entries(ts_muxer->queue_buffer->stream_queue));
			lthread_mutex_unlock(&ts_muxer->queue_buffer->stream_queue_mutex);
		}
	}
#endif

	return bytes;
}

static int open_file(char *filename)
{
	int fd;
#ifdef WRITE_BUFFER_TO_FILE
#ifdef WRITE_BUFFER_TO_FIFO
	//unlink(filename);
	//mkfifo(filename, 0777);
	LOGD("ts mux open_file called filename = %s",filename);
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0)
	{
		LIMERR("unable to create debug file.\n");
		fd = -2;
	}
#else
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 0777);
	if (fd < 0)
	{
		LIMERR("unable to create debug file.\n");
		fd = -2;
	}
#endif
#endif

	return fd;
}

void * aac_encoder_thread(void *args)
{
	AACEncoder *aac_encoder = (AACEncoder *) args;

	void *input_queue = aac_encoder->ts_muxer->queue_buffer->audio_queue;
	void *output_queue = aac_encoder->output_queue;

	lthread_cond_t *output_cond = &aac_encoder->cond;
	lthread_mutex_t *output_mutex = &aac_encoder->mutex;

	lthread_cond_t *input_cond = &aac_encoder->ts_muxer->queue_buffer->audio_queue_cond;
	lthread_mutex_t *input_mutex = &aac_encoder->ts_muxer->queue_buffer->audio_queue_mutex;

	void *input_entry = NULL;
	void *output_entry = NULL;
	AVCodecContext *codec_context = aac_encoder->ts_muxer->audio_stream->codec;
	TSMuxerPriv *ts_muxer = aac_encoder->ts_muxer;
	int size = 0;

	while (!aac_encoder->ts_muxer->quit)
	{

		lthread_mutex_lock(input_mutex);
		input_entry = limq_dequeue_get(input_queue);
		lthread_mutex_unlock(input_mutex);

		if (input_entry)
		{
			// encoder
			size = avcodec_encode_audio(codec_context, ts_muxer->audio_encoder_para.audio_outbuf,
					ts_muxer->audio_encoder_para.audio_outbuf_size, input_entry);

			// put encoder data to output queue
			while (!aac_encoder->ts_muxer->quit)
			{

				lthread_mutex_lock(output_mutex);
				output_entry = limq_enqueue_get(output_queue);
				lthread_mutex_unlock(output_mutex);

				if (output_entry)
				{

					memcpy(output_entry, &size, sizeof(int));
					memcpy(output_entry + sizeof(int), ts_muxer->audio_encoder_para.audio_outbuf, size);

					lthread_mutex_lock(output_mutex);
					limq_enqueue_put(output_queue, output_entry);
					lthread_mutex_unlock(output_mutex);

					break;
				}
				else
				{
					usleep(100);
				}
			}

			lthread_mutex_lock(input_mutex);
			limq_dequeue_put(input_queue, input_entry);
			lthread_mutex_unlock(input_mutex);
		}
		else
		{
			usleep(100);
		}
	}

	lthread_mutex_lock(&ts_muxer->aac_encoder_mutex);
	lthread_cond_broadcast(&ts_muxer->aac_encoder_cond);
	ts_muxer->aac_encoder_state = 1;
	lthread_mutex_unlock(&ts_muxer->aac_encoder_mutex);

	printf(" %s->%s->%d : AAC encoder thread quit \n", __FILE__, __func__, __LINE__);

	return NULL;
}

void * read_frame_thread(void *args)
{
	ReadPara *read_para = (ReadPara *) args;

	void *queue = read_para->queue;
	lthread_cond_t *cond = &read_para->cond;
	lthread_mutex_t *mutex = &read_para->mutex;
	AVFormatContext *fmt_ctx = read_para->ifmt_ctx;
	int quit = read_para->ts_muxer->quit;
	void *entry = NULL;

	while (!quit)
	{

		lthread_mutex_lock(mutex);
		entry = limq_enqueue_get(queue);
		lthread_mutex_unlock(mutex);
		if (entry)
		{
			AVPacket packet;
			EntryHead head;
			int ret;

			head.keyframe = 0;
			head.len = 0;

			ret = av_read_frame(fmt_ctx, &packet);
			if (ret < 0)
			{
				LOGD(" av read frame error or end of file \n");
				continue;
			}
			if (packet.flags & AV_PKT_FLAG_KEY)
			{
				head.keyframe = 1;
			}
			head.len = packet.size;
			//write(read_para->ts_muxer->fd_test, packet.data, packet.size);

			memcpy(entry, &head, sizeof(EntryHead));
			memcpy(entry + sizeof(EntryHead), packet.data, packet.size);

			lthread_mutex_lock(mutex);
			limq_enqueue_put(queue, entry);
			lthread_mutex_unlock(mutex);

			av_free_packet(&packet);
		}
	}

	lthread_mutex_lock(mutex);
	lthread_cond_broadcast(cond);
	lthread_mutex_unlock(mutex);

	return NULL;
}

void * ts_muxing_thread(void *args)
{
	TSMuxerPriv *ts_muxer = (TSMuxerPriv *) args;
	QueueBufferPriv * queue_buffer = ts_muxer->queue_buffer;
	int state = RET_OK;
	int i;
	int ret;

	AVFormatContext *ifmt_ctx = NULL;
	AVInputFormat *ifmt = NULL;
	AVStream *iv_stream = NULL;

	AVFormatContext *ofmt_ctx = NULL;
	AVOutputFormat *ofmt = NULL;
	AVStream *ov_stream = NULL;
	AVStream *oa_stream = NULL;

	char * input_file_name = "/data/fifo.h264";
	

	av_register_all();

	/*
	 * open the output file ,if needed
	 */
#ifdef WRITE_BUFFER_TO_FILE
	LOGD("TS MUX open_file output_file_name = %s",output_file_name);
	ts_muxer->fd = open_file(output_file_name);
#else
	if (!(ts_muxer->output_format->flags & AVFMT_NOFILE))
	{
		if (avio_open(&ts_muxer->output_context->pb, ts_muxer->filename,
						AVIO_FLAG_WRITE) < 0)
		{
			LOGD("Could not open '%s'\n", ts_muxer->filename);
			state = RET_FAIL;
			goto EXIT;
		}
	}
#endif

	/*
	 *  init output context
	 */
	LOGD(" open output file %s", output_file_name);
	ofmt = av_guess_format("mpegts", NULL, NULL);
	if (!ofmt)
	{
		LOGD(" av guess format mpegts failed \n");
	}

	ret = avformat_alloc_output_context2(&ofmt_ctx, ofmt, NULL, NULL);
	if (ret < 0)
	{
		LOGD("avformat alloc output context failed \n");
	}

	ofmt_ctx->pb = avio_alloc_context(ts_muxer->bio_buffer, FFMPEG_BIO_BUFFER_SIZE, 1, (void*) ts_muxer, NULL,
			ts_muxer_write_cb, NULL);
	//ofmt_ctx->max_delay = (int) (-0.3* AV_TIME_BASE); // delay time,copy ffmpeg
	ofmt_ctx->max_delay  = 0;

	/*
	 *  add video stream
	 */
	ov_stream = av_new_stream(ofmt_ctx, 0);
	if (ov_stream)
	{
		AVCodecContext *c;
		AVCodecContext *ic;
		AVStream *st;

		AVCodec *codec;
		c = ov_stream->codec;

		//ic = iv_stream->codec;
		//find the video encoder
		codec = avcodec_find_encoder(CODEC_ID_H264);
		if (!codec)
		{
			LOGD("codec not found\n");
		}
		avcodec_get_context_defaults3(c, codec);

		c->codec_id = CODEC_ID_H264;

		/* put sample parameters */
		c->bit_rate = 400000;
		/* resolution must be a multiple of two */
		c->width = 640;
		c->height = 480;
		/* time base: this is the fundamental unit of time (in seconds) in terms
		 of which frame timestamps are represented. for fixed-fps content,
		 timebase should be 1/framerate and timestamp increments should be
		 identically 1. */
		c->time_base.den = 20;
		c->time_base.num = 1;
		c->gop_size = 12; /* emit one intra frame every twelve frames at most */
		c->pix_fmt = PIX_FMT_YUV420P;
		if (c->codec_id == CODEC_ID_MPEG2VIDEO)
		{
			/* just for testing, we also add B frames */
			c->max_b_frames = 2;
		}
		if (c->codec_id == CODEC_ID_MPEG1VIDEO)
		{
			/* Needed to avoid using macroblocks in which some coeffs overflow.
			 This does not happen with normal video, it just happens here as
			 the motion of the chroma plane does not match the luma plane. */
			c->mb_decision = 2;
		}
	}
	else
	{
		LOGD(" avforamt new stream failed \n");
	}

	/*
	 * add audio stream
	 */
	LOGD(" add audio stream ");
	oa_stream = av_new_stream(ofmt_ctx, 1);
	if (oa_stream)
	{
		AVCodecContext *c;
		AVCodec *codec;

		c = oa_stream->codec;
		c->codec_id = CODEC_ID_AAC;
		c->codec_type = AVMEDIA_TYPE_AUDIO;

		c->sample_fmt = AV_SAMPLE_FMT_S16;
		c->bit_rate = 64000;
		c->sample_rate = 48000;
		c->channels = 2;
		// some formats want stream headers to be separate
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			c->flags |= CODEC_FLAG_GLOBAL_HEADER;

		if (open_audio(ofmt_ctx, oa_stream, &ts_muxer->audio_encoder_para) != 0)
		{
			LOGD(" open audio stream failed \n");
			state = RET_FAIL;
			goto EXIT;
		}
		oa_stream->codec->codec_id = CODEC_ID_AAC;

	}
	else
	{
		LOGD(" avforamt new stream failed \n");
	}

	LOGD(" ouput file info: ");
	av_dump_format(ofmt_ctx, 0, output_file_name, 1);
	ts_muxer->output_format = ofmt;
	ts_muxer->output_context = ofmt_ctx;
	ts_muxer->video_stream = ov_stream;
	ts_muxer->audio_stream = oa_stream;

	/*
	 * AAC Encoder thread init
	 */
	AACEncoder aac_encoder;
	lthread_mutex_init(&aac_encoder.mutex);
	lthread_cond_init(&aac_encoder.cond);

	aac_encoder.input_queue = ts_muxer->queue_buffer->audio_queue;
	aac_encoder.output_queue = limq_create_ext(1024 * 4, 8, "AAC Encoder Thread", NULL);
	aac_encoder.ts_muxer = ts_muxer;

	lthread_create(&aac_encoder.thread, aac_encoder_thread, (void*) &aac_encoder, 1);
	/*
	 * write the stream header, if any
	 */
	LOGD(" av write header ");
	av_write_header(ts_muxer->output_context);

	int write_video = 0;
	int64_t base = 0;
	double video_pts = 0.0;
	double audio_pts = 0.0;


	/*
	 * write audio and video ts packet
	 */
	while (!ts_muxer->quit)
	{
		int audio_entry_count = 0;
		int video_entry_count = 0;

		/* compute current audio and video time */
		if (ts_muxer->audio_stream)
		{
			ts_muxer->audio_pts = audio_base * av_q2d(ts_muxer->audio_stream->time_base);
		}
		else
		{
			ts_muxer->audio_pts = 0.0;
		}

		if (ts_muxer->video_stream)
		{
			ts_muxer->video_pts = video_base * av_q2d(ts_muxer->video_stream->time_base);
		}
		else
		{
			ts_muxer->video_pts = 0.0;
		}

		lthread_mutex_lock(&aac_encoder.mutex);
		audio_entry_count = limq_entries(aac_encoder.output_queue);
		lthread_mutex_unlock(&aac_encoder.mutex);

		lthread_mutex_lock(&ts_muxer->queue_buffer->video_queue_mutex);
		video_entry_count = limq_entries(ts_muxer->queue_buffer->video_queue);
		lthread_mutex_unlock( &ts_muxer->queue_buffer->video_queue_mutex);

		if ( audio_entry_count > 0 || video_entry_count > 0)
		{
			LOGD(" audio_entry = %p , count = %d ", aac_encoder.output_queue, audio_entry_count);
			LOGD(" video_entry = %p , count = %d ", ts_muxer->queue_buffer->video_queue, video_entry_count);
		}

		/* write audio and video frames*/
		if ((audio_entry_count > 0) && ((ts_muxer->audio_pts < ts_muxer->video_pts) || (video_entry_count == 0)))
		{
			void * audio_entry;
			int *size = NULL;
			AVPacket pkt;
			AVFormatContext *oc = ts_muxer->output_context;
			AVStream *st = ts_muxer->audio_stream;

			av_init_packet(&pkt);

			lthread_mutex_lock(&aac_encoder.mutex);
			audio_entry = limq_dequeue_get(aac_encoder.output_queue);
			lthread_mutex_unlock(&aac_encoder.mutex);

			if (audio_entry)
			{
				LOGD( " PTS_DBG: -------------------Audio--------------------------");
				LOGD(" PTS_DBG:ts_muxer->video_pts = %f", ts_muxer->video_pts);
				LOGD(" PTS_DBG:ts_muxer->audio_pts = %f", ts_muxer->audio_pts);

				size = (int *) audio_entry;

				pkt.size = *size;
				pkt.flags |= AV_PKT_FLAG_KEY;
				pkt.stream_index = st->index;
				pkt.data = audio_entry + sizeof(int);
				pkt.pts = pkt.dts = audio_base;
				audio_base += 1920;

				if (av_interleaved_write_frame(oc, &pkt) != 0)
				{
					LOGD("Error: while writing audio frame ");
				}

				lthread_mutex_lock(&aac_encoder.mutex);
				limq_dequeue_put(aac_encoder.output_queue, audio_entry);
				lthread_mutex_unlock(&aac_encoder.mutex);
			}

		}
		else if (video_entry_count > 0)
		{
			void * video_entry = NULL;
			MSVideoData *vd = NULL;

			lthread_mutex_lock(&ts_muxer->queue_buffer->video_queue_mutex);
			video_entry = limq_dequeue_get(ts_muxer->queue_buffer->video_queue);
			lthread_mutex_unlock( &ts_muxer->queue_buffer->video_queue_mutex);

			if (video_entry)
			{
				AVPacket packet;
				av_init_packet(&packet);
				int tmp_base = video_base;

				LOGD("\n\n");
				LOGD( " PTS_DBG: --------------Video-------------------------------");
				LOGD(" PTS_DBG:ts_muxer->video_pts = %f", ts_muxer->video_pts);
				LOGD(" PTS_DBG:ts_muxer->audio_pts = %f", ts_muxer->audio_pts);
				if (ts_muxer->audio_pts >= ts_muxer->video_pts + 0.001)
				{
					video_base = ts_muxer->audio_pts / av_q2d(ts_muxer->video_stream->time_base);
					LOGD(" PTS_DBG: modify video_base");
				}
				else if (ts_muxer->video_pts >= ts_muxer->audio_pts + 0.1)
				{
					video_base += 2250;
				}
				else
				{
					video_base += 4500;
				}
				LOGD(" PTS_DBG:ts_muxer->video_pts = %f", video_base * av_q2d(ts_muxer->video_stream->time_base));


				//while( ((video_base - tmp_base) * av_q2d(ts_muxer->video_stream->time_base) * 1000) > 100 )
				//{
					//video_base -= 0.01/ av_q2d(ts_muxer->video_stream->time_base);
				//}

				vd = (MSVideoData *) video_entry;
				packet.stream_index = ov_stream->index;
				packet.size = vd->datasize;
				/*modify by taoting for make audio add video Synchronization begin 20120807*/
				packet.pts = packet.dts = video_base;
				#if 0
				if(video_base - 1.2/av_q2d(ts_muxer->video_stream->time_base) >0)
				{
				    packet.pts = packet.dts = video_base - 1.2/av_q2d(ts_muxer->video_stream->time_base);
				}
				else
				{
				     packet.pts = packet.dts = video_base;
				}
				#endif
				/*modify by taoting for make audio add video Synchronization end 20120807*/
				packet.data = video_entry + sizeof(MSVideoData);

				ret = av_interleaved_write_frame(ofmt_ctx, &packet);
				if (ret != 0)
				{
					LOGD("Error: while writing video frame \n");
				}

				lthread_mutex_lock(&ts_muxer->queue_buffer->video_queue_mutex);
				limq_dequeue_put(ts_muxer->queue_buffer->video_queue, video_entry);
				lthread_mutex_unlock( &ts_muxer->queue_buffer->video_queue_mutex);
			}
		}
		usleep(100);
	}

	EXIT:
	/* write the trailer, if any.  the trailer must be written
	 * before you close the CodecContexts open when you wrote the
	 * header; otherwise write_trailer may try to use memory that
	 * was freed on av_codec_close()
	 */
	av_write_trailer(ts_muxer->output_context);

	/* close each codec */
	if (ts_muxer->video_stream)
	{
		close_video(ts_muxer->output_context, ts_muxer->video_stream);
	}

	if (ts_muxer->audio_stream)
	{
		close_audio(ts_muxer->output_context, ts_muxer->audio_stream, &ts_muxer->audio_encoder_para);
	}

	/* free the streams */
	for (i = 0; i < ts_muxer->output_context->nb_streams; i++)
	{
		av_freep(&ts_muxer->output_context->streams[i]->codec);
		av_freep(&ts_muxer->output_context->streams[i]);
	}

#ifdef WRITE_BUFFER_TO_FILE

#else
	if (!(ts_muxer->output_format->flags & AVFMT_NOFILE))
	{
		/* close the output file */
		avio_close(ts_muxer->output_context->pb);
	}
#endif

#ifdef WRITE_BUFFER_TO_FILE
	if (ts_muxer->fd > 0)
	{
		close(ts_muxer->fd);
	}

	av_free(ts_muxer->output_context->pb);
#endif

	/* free the stream */
	av_free(ts_muxer->output_context);
	/*
	 * wait aac encode thread quit
	 */
	lthread_mutex_lock(&ts_muxer->aac_encoder_mutex);
	if (ts_muxer->aac_encoder_state == 0)
	{
		lthread_cond_wait(&ts_muxer->aac_encoder_cond, &ts_muxer->aac_encoder_mutex);
	}lthread_mutex_unlock(&ts_muxer->aac_encoder_mutex);

	lthread_mutex_lock(&ts_muxer->muxer_mutex);
	lthread_cond_broadcast(&ts_muxer->muxer_cond);
	ts_muxer->muxer_state = MUXER_STATE_STOP;
	lthread_mutex_unlock(&ts_muxer->muxer_mutex);

	printf(" %s->%s->%d : TS muxing thread  quit \n", __FILE__, __func__, __LINE__);

	return NULL;
}

static int open_audio(AVFormatContext *oc, AVStream *st, AudioEncoderPara *audio_encoder_para)
{
	AVCodecContext *c;
	AVCodec *codec;

	c = st->codec;

	/* find the audio encoder */
	codec = avcodec_find_encoder(c->codec_id);
	if (!codec)
	{
		LOGD("codec not found\n");
		return -1;
	}

	/* open it */
	if (avcodec_open(c, codec) < 0)
	{
		LOGD("could not open codec\n");
		return -1;
	}

	/* init signal generator */
	//t = 0;
	//tincr = 2 * M_PI * 110.0 / c->sample_rate;
	/* increment frequency by 110 Hz per second */
	//tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;
	audio_encoder_para->audio_outbuf_size = 10000;
	audio_encoder_para->audio_outbuf = av_malloc(audio_encoder_para->audio_outbuf_size);

	/* ugly hack for PCM codecs (will be removed ASAP with new PCM
	 support to compute the input frame size in samples */

	st->codec->codec_id = CODEC_ID_PCM_S16LE;

	if (c->frame_size <= 1)
	{
		audio_encoder_para->audio_input_frame_size = audio_encoder_para->audio_outbuf_size / c->channels;
		switch (st->codec->codec_id)
		{
		case CODEC_ID_PCM_S16LE:
		case CODEC_ID_PCM_S16BE:
		case CODEC_ID_PCM_U16LE:
		case CODEC_ID_PCM_U16BE:
			audio_encoder_para->audio_input_frame_size >>= 1;
			break;
		default:
			break;
		}
	}
	else
	{
		audio_encoder_para->audio_input_frame_size = c->frame_size;
	}
	audio_encoder_para->samples = av_malloc(audio_encoder_para->audio_input_frame_size * 2 * c->channels);

	return 0;
}

void write_audio_frame(TSMuxerPriv *ts_muxer)
{
	AVCodecContext *c;
	AVPacket pkt;
	void * audio_entry;
	AVFormatContext *oc = ts_muxer->output_context;
	AVStream *st = ts_muxer->audio_stream;

	/*if (ts_muxer->audio_pts < (ts_muxer->video_pts - 1.0))
	 {
	 audio_base = ts_muxer->video_stream->pts.val;
	 }*/

	c = st->codec;
	av_init_packet(&pkt);

	/*
	 * Get PCM data
	 */
	//LOGD(" get pcm data \n");
	lthread_mutex_lock(&ts_muxer->queue_buffer->audio_queue_mutex);
	audio_entry = limq_dequeue_get(ts_muxer->queue_buffer->audio_queue);
	lthread_mutex_unlock(&ts_muxer->queue_buffer->audio_queue_mutex);
	/*
	 * Encode pcm data with libfaac
	 */
	if (audio_entry)
	{
		pkt.size = avcodec_encode_audio(c, ts_muxer->audio_encoder_para.audio_outbuf,
				ts_muxer->audio_encoder_para.audio_outbuf_size, audio_entry);
		/*
		 * Add other info
		 */
		pkt.flags |= AV_PKT_FLAG_KEY;
		pkt.stream_index = st->index;
		pkt.data = ts_muxer->audio_encoder_para.audio_outbuf;
		pkt.pts = pkt.dts = audio_base;
		audio_base += 1920;
		// write the compressed frame in the media file
		if (av_interleaved_write_frame(oc, &pkt) != 0)
		{
			LOGD(stderr, "Error while writing audio frame ");
		}

		/*{
		 static int fd = -1;
		 char *filename = "/data/test.aac";

		 if (fd < 0)
		 {
		 fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0666);
		 }
		 if (fd > 0)
		 {
		 write(fd, pkt.data, pkt.size);
		 }
		 }*/

		lthread_mutex_lock(&ts_muxer->queue_buffer->audio_queue_mutex);
		limq_dequeue_put(ts_muxer->queue_buffer->audio_queue, audio_entry);
		lthread_mutex_unlock(&ts_muxer->queue_buffer->audio_queue_mutex);
	}

}

static void write_video_frame(TSMuxerPriv *ts_muxer)
{
	int out_size, ret;
	AVCodecContext *c;
	static struct SwsContext *img_convert_ctx;
	AVFormatContext *oc = ts_muxer->output_context;
	AVStream *st = ts_muxer->video_stream;
	void *video_entry = NULL;
	MSVideoData *vd;
	AVPacket pkt;

	static long long int pts = 0;

	c = st->codec;

	/*
	 *
	 */
	lthread_mutex_lock(&ts_muxer->queue_buffer->video_queue_mutex);
	video_entry = limq_dequeue_get(ts_muxer->queue_buffer->video_queue);
	lthread_mutex_unlock(&ts_muxer->queue_buffer->video_queue_mutex);

	/*
	 *
	 */
	if (video_entry)
	{
		int nal_ref_idc = 0;
		int nal_unit_type = 0;
		uint8_t *src = NULL;
		static long long int pts = 0;

		LOGD(" video_entry = %p \n", video_entry);

		vd = (MSVideoData *) video_entry;
		av_init_packet(&pkt);

		src = vd;
		nal_ref_idc = src[0] >> 5;
		nal_unit_type = src[0] & 0x1F;
		if (5 == nal_unit_type)
		{
			LOGD(" this is key frame ");
			pkt.flags |= AV_PKT_FLAG_KEY;
		}
		pkt.stream_index = st->index;
		pkt.pts = pts++;	//vd->pts;//一定要设置PTS
		pkt.size = vd->datasize;
		pkt.data = video_entry + sizeof(MSVideoData);

#if 1
		ret = av_interleaved_write_frame(oc, &pkt);
		if (ret != 0)
		{
			LOGD(" write video frame failed ");
		}
#endif

#if 0
#ifdef WRITE_BUFFER_TO_FILE
		ltime_val_t tv;
		ltime_gettimeofday(&tv);
		static int count = 0;
		LOGD(" count = %d ,current time = %lld ",count++,ltime_tv2ms(&tv));

		if (ts_muxer->fd > 0)
		{
			write(ts_muxer->fd, pkt.data, pkt.size);
		}
#endif
#endif

		lthread_mutex_lock(&ts_muxer->queue_buffer->video_queue_mutex);
		limq_dequeue_put(ts_muxer->queue_buffer->video_queue, video_entry);
		lthread_mutex_unlock(&ts_muxer->queue_buffer->video_queue_mutex);
	}

}

static void close_audio(AVFormatContext *oc, AVStream *st, AudioEncoderPara *audio_encoder_para)
{
	avcodec_close(st->codec);

	av_free(audio_encoder_para->samples);
	av_free(audio_encoder_para->audio_outbuf);
}

static void close_video(AVFormatContext *oc, AVStream *st)
{
	avcodec_close(st->codec);
}

