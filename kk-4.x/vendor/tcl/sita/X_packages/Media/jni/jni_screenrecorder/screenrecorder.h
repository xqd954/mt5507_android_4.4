/*
 * stream.h
 *
 *  Created on: 2013-5-24
 *      Author: zhangsh
 */

#ifndef STREAM_H_
#define STREAM_H_


struct _MuxerContex;
typedef struct _MuxerContex MuxerContex;

#ifdef __cplusplus
extern "C" {
#endif

MuxerContex * screen_recorder_init();

void screen_recorder_set_output_file(MuxerContex *mc,const char * path);

void screen_recorder_set_output_file_fd(MuxerContex *mc,int fd);

void screen_recorder_set_output_format(MuxerContex *mc,int format);

void screen_recorder_set_record_mode(MuxerContex *mc,int mode);

void screen_recorder_set_video_size(MuxerContex *mc,int width, int height);

void screen_recorder_set_video_rate(MuxerContex *mc,int rate);

void screen_recorder_prepare(MuxerContex *mc);

void screen_recorder_start(MuxerContex *mc);

void screen_recorder_stop(MuxerContex *mc);

void screen_recorder_reset(MuxerContex *mc);

void screen_recorder_release(MuxerContex *mc);


#ifdef __cplusplus
}
#endif

#endif /* STREAM_H_ */
