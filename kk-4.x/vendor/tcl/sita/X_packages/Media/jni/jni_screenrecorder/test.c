

#include "screenrecorder.h"
#include <stdio.h>

int main()
{

	MuxerContex  *stream;

	//while(1)
	{
		stream = NULL;

		printf(" ------------------1-------------- \n");
		stream = screen_recorder_init();

		printf(" ------------------2-------------- \n");
		screen_recorder_set_output_file(stream, "/data/zhang.ts");

		printf(" ------------------3-------------- \n");
		screen_recorder_set_output_format(stream, 0);

		printf(" ------------------4-------------- \n");
		screen_recorder_set_video_rate(stream, 20);

		printf(" ------------------5-------------- \n");
		screen_recorder_set_video_size(stream, 640, 368);

		printf(" ------------------6-------------- \n");
		screen_recorder_set_record_mode(stream, 2);

		printf(" ------------------7-------------- \n");
		screen_recorder_prepare(stream);

		printf(" ------------------8-------------- \n");
		screen_recorder_start(stream);

		sleep(7);

		printf(" ------------------9-------------- \n");
		screen_recorder_stop(stream);

		printf(" ------------------10-------------- \n");
		screen_recorder_reset(stream);

		printf(" ------------------11-------------- \n");
		screen_recorder_release(stream);

		printf(" ------------------12-------------- \n");
		//sleep(3);

	}

	return 0;
}
