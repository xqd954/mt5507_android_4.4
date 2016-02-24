/*
 * test.c
 *
 *  Created on: 2012-4-17
 *      Author: sujuan
 */



#include <unistd.h>
#include "limutil.h"


int flag = 1;

void sigint_handler(int sig)
{
    flag = 0;
}

int main(void)
{	

	int ret = 0;
	int status = 0;
	char *pQue;
	int xx = 0;
	limq_handle_t queue;
	AudioCapture* audio_capture;

	LIMLOG("creating queue.\n");
	queue = limq_create_ext(1024, 10, "TEST_QUEUE", NULL);
	if (NULL == queue)
	{
		LIMERR("unable to create queue object.\n");
		exit(-1);
	}	

	audio_capture = audio_capture_init(2, 44100, 16, (void *)queue);
	
	//signal(SIGINT, sigint_handler);
	status = audio_capture_start(audio_capture);
	
	LIMLOG("main after audio_capture_start");

	sleep(1);

	LIMLOG("main start get queue data");
	while(flag)
	{
		// out que
		LIMLOG("main 4444444");
		pQue = limq_dequeue_get(queue);
		LIMLOG("555555 pQue:%p", pQue);
        limq_dequeue_put(queue, pQue);

		usleep(10);
	}

	LIMLOG("main before StopPcmCapture");

	audio_capture_close(audio_capture);

	if (queue)
	{
		limq_destroy(queue);
	}
	
	return 0;
}
