/*
 * stream_muxer.h
 *
 *  Created on: 2012-4-16
 *      Author: zhangsh
 */

#ifndef STREAM_MUXER_H_
#define STREAM_MUXER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _Ret
{
	RET_INVALID_PARAMS,
	RET_OK,
	RET_FAIL
}Ret;

struct _Stream_muxer;
typedef struct _Stream_muxer StreamMuxer;


/**
 * Init stream muxer
 */
StreamMuxer * stream_muxer_init(char* path,int width,int height,int with_osd);

/**
 *test camera srate
 */

Ret stream_muxer_camera_listen();


/**
 * Start stream muxer
 */
Ret stream_muxer_start(StreamMuxer * stream_muxer);

/**
 * Pause stream muxer
 */
Ret stream_muxer_pause(StreamMuxer * stream_muxer);

/**
 * close stream muxer
 */
void stream_muxer_close(StreamMuxer * stream_muxer);


#ifdef __cplusplus
}
#endif


#endif /* STREAM_MUXER_H_ */
