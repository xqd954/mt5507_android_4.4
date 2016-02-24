/*
 * ts_muxer_jni.c
 *
 *  Created on: 2013-5-24
 *      Author: zhangsh
 */


#include <stdio.h>

#include "stream_muxer.h"
#include "screenrecorder.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "utils/Log.h"

#define printf ALOGD

struct _MuxerContex
{
	char path[256];
	int fd;

	int output_format;
	int width;
	int height;
	int rate;
	int record_mode;


	void *stream_muxer;
};



MuxerContex * screen_recorder_init()
{
	MuxerContex *mc = malloc(sizeof(MuxerContex));
	if(mc)
	{
		memset(mc,0,sizeof(MuxerContex));
		
		mc->fd = -1;
		mc->height = 640;
		mc->width = 368;
		mc->output_format = 0;
		mc->rate = 20;
		mc->record_mode = 0;

		memset(mc->path,0,sizeof(mc->path));
	}

	return mc;
}

void screen_recorder_set_output_file(MuxerContex *mc,const char * path)
{
	if(mc && path)
	{
		printf(" screen_recorder_set_output_file \n");

		memset(mc->path,0,256);
		strcpy(mc->path,path);

		printf(" mc->path = %s \n",mc->path);


		//mc->fd = open(mc->path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	}
}

void screen_recorder_set_output_file_fd(MuxerContex *mc,int fd)
{
	if(mc)
	{
		printf(" screen_recorder_set_output_file_fd \n");

                mc->fd = dup(fd);

                char s[256]; 
	        char path[256];
	
	        memset(s,0,256);
	        memset(path,0,256);
	
	        snprintf(s, 255, "/proc/%d/fd/%d", getpid(), mc->fd);
	        readlink(s, path, 255);
	        printf(" file_path = %s \n",path);
                memset(mc->path,0,256);
                strcpy(mc->path,path);
  
                printf(" mc->path = %s \n",mc->path);
	}
}

void screen_recorder_set_output_format(MuxerContex *mc,int format)
{
	if(mc)
	{
		printf(" screen_recorder_set_output_format \n");

		mc->output_format = format;

		printf(" mc->output_format = %d \n",mc->output_format);
	}
}

void screen_recorder_set_record_mode(MuxerContex *mc,int mode)
{
	if(mc)
	{
		printf(" screen_recorder_set_record_mode %d\n", mode);

		mc->record_mode = mode;

		if((mc->record_mode < 0 ) || (mc->record_mode > 2)) {
	            mc->record_mode = 0;
		} else if ((mc->record_mode == 0 ) || (mc->record_mode == 1)) {
                    mc->record_mode = 0;
                } else if (mc->record_mode == 2 ) {
                    mc->record_mode = 1;
                }

		printf(" mc->record_mode = %d \n",mc->record_mode);
	}
}

void screen_recorder_set_video_size(MuxerContex *mc,int width, int height)
{
    if(mc)
	{
		printf(" screen_recorder_set_video_size \n");

		mc->width = 640;
		mc->height = 368;

		/*if( mc->width <= 0 )
		{
			mc->width = 640;
		}

		if( mc->height <= 0 )
		{
			mc->height = 368;
		}*/

		printf(" mc->width = %d, mc->height = %d \n",mc->width,mc->height);
	}
}

void screen_recorder_set_video_rate(MuxerContex *mc,int rate)
{
	if(mc)
	{
		printf(" screen_recorder_set_video_rate \n");

		mc->rate = 20;

		/*if( (mc->rate <= 0) || (mc->rate > 30) )
		{
			mc->rate = 25;
		}*/

		printf(" mc->rate = %d \n",mc->rate);
	}
}

void screen_recorder_prepare(MuxerContex *mc)
{
	printf(" screen_recorder_prepare liuyan \n");
}

void screen_recorder_start(MuxerContex *mc)
{
	void *stream_muxer = NULL;
	if (mc)
	{
		printf(" stream_muxer_init liuyan \n");

		//stream_muxer = stream_muxer_init();
                printf(" stream_muxer_init mc->path = %s ,%d ,%d ,%d\n", mc->path, mc->width, mc->height, mc->record_mode);
                stream_muxer = stream_muxer_init(mc->path,mc->width,mc->height,mc->record_mode); 

		mc->stream_muxer = stream_muxer;
	}

	if(mc && mc->stream_muxer)
	{
                Ret ret = 0;
                ret = stream_muxer_camera_listen();
                if(ret == RET_OK )
	        {
                    printf(" stream_muxer_start \n");
	            stream_muxer_start(mc->stream_muxer);
	        }
		printf(" stream_muxer_start \n");
	}
}

void screen_recorder_stop(MuxerContex *mc)
{
	if(mc && mc->stream_muxer)
	{
		printf(" screen_recorder_stop \n");
		stream_muxer_close(mc->stream_muxer);
		mc->stream_muxer = NULL;
	}
}
void screen_recorder_reset(MuxerContex *mc)
{
	printf(" screen_recorder_reset \n");
	screen_recorder_stop(mc);
        //printf(" screen_recorder_reset 2\n");
	mc->fd = -1;
	mc->height = 640;
	mc->width = 368;
	mc->output_format = 0;
	mc->rate = 20;
	mc->record_mode = 0;
        //printf(" screen_recorder_reset 3\n");
	memset(mc->path,0,sizeof(mc->path));
        //printf(" screen_recorder_reset 4\n");

}

void screen_recorder_release(MuxerContex *mc)
{
	printf(" screen_recorder_release \n");
	screen_recorder_reset(mc);

	if(mc)
	{
		free(mc);
		mc = NULL;
	}
}
