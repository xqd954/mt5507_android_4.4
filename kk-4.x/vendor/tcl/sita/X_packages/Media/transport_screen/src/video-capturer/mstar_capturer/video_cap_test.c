/*
 * video_cap_test.c
 *
 *  Created on: 2012-5-22
 *      Author: zhangsh
 */

#include "livecapbase.h"
#include "MstarVideoCapApi.h"
#include "limutil.h"

//liuyan start
#include <utils/Log.h>
#define LOG_TAG "livecap::video_cap_test"
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


MSVideoParameter video_para;
void *video_queue;
lthread_cond_t cond;
lthread_mutex_t mutex;

void * consume_queue_buffer_thread(void *args)
{
	void *queue = NULL;

	while(1)
	{

		lthread_mutex_lock(&mutex);
		//LOGD(" DBG: current_video_entry = %d ",limq_entries(video_queue));
		lthread_mutex_unlock(&mutex);

		lthread_mutex_lock(&mutex);
		queue = limq_dequeue_get(video_queue);
		lthread_mutex_unlock(&mutex);

		if (queue)
		{
			lthread_mutex_lock(&mutex);
			limq_dequeue_put(video_queue,queue);
			lthread_mutex_unlock(&mutex);

		//	LOGD(" DBG: get_video_queue_buffer,and put out ");
		}

		usleep(10*1000);
	}

	return NULL;
}


void mstar_video_cap_test()
{
	MstarVCapture *mstar_video;
	lthread_type_t thread;

	video_queue = limq_create_ext(256*1024,8,NULL,NULL);
//	LOGD("liuyan  DBG: mstar_video_cap_test = %d ",video_queue);
	INFO("mstar_video_cap_test video_queue = %d ",video_queue);
	lthread_mutex_init(&mutex);
	lthread_cond_init(&cond);

	video_para.framerate = 20;
	video_para.height = 368; //liuyan test 480
	video_para.width = 640;
	video_para.queue = video_queue;
	video_para.queue_cond = &cond;
	video_para.queue_mutex = &mutex;

//	printf(" init mstar video cap \n");
	INFO("mstar_video_cap_test init mstar video cap \n");
	mstar_video = mstarv_capture_init(&video_para);

	lthread_create(&thread,consume_queue_buffer_thread,NULL,1);

//	printf(" start mstar video cap \n");
	INFO("mstar_video_cap_test start mstar video cap \n");
	mstarv_capture_start( mstar_video);

//	printf(" start mstar video cap2 \n");
	INFO("mstar_video_cap_test start mstar video cap2 \n");
	while (1)
	{
		if (getchar() == 'q')
		{
			break;
		}
		usleep(10 * 1000);
	}

//	printf("close  mstar video cap ");
	INFO("mstar_video_cap_test close  mstar video cap \n");
	mstarv_capture_close(mstar_video);

//	printf(" end \n");
	INFO("mstar_video_cap_test close  end \n");
}



/*void video_cap_test()
{
	printf(" start service thread \n");
	real_start_livecap_service_thread();
	usleep(100 * 1000);

	printf(" open video capture \n");
	main_capture_video_open();

	printf(" capture...");
	while (1)
	{
		if (getchar() == 'q')
		{
			break;
		}
		usleep(10 * 1000);
	}

	printf(" receive quit message \n");

	main_capture_video_close();
	real_close_livecap_service_thread();

	printf(" end \n");
}*/

int main(int argc, char *argv[])
{
	mstar_video_cap_test();
	return 0;
}
