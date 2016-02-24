/*
 * ts_muxer.h
 *
 *  Created on: 2012-4-17
 *      Author: zhangsh
 */

#ifndef TS_MUXER_H_
#define TS_MUXER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "limutil.h"
#include "common.h"

#define LOGD(...);
struct _TSMuxerPriv;
typedef struct _TSMuxerPriv TSMuxerPriv;


/*
 * init ts muxer
 */
TSMuxerPriv * ts_muxer_init(QueueBufferPriv * queue_buffer,char* path);

/*
 * start ts muxer
 */
Ret ts_muxer_start(TSMuxerPriv *ts_muxer);

/*
 * pause ts muxer
 */
Ret ts_muxer_pause(TSMuxerPriv *ts_muxer);

/*
 * close ts muxer
 */
void ts_muxer_close(TSMuxerPriv *ts_muxer);

#ifdef __cplusplus
}
#endif

#endif /* TS_MUXER_H_ */
