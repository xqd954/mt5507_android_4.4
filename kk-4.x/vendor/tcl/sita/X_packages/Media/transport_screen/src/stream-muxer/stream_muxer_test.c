/*
 * stream_muxer_test.c
 *
 *  Created on: 2012-4-18
 *      Author: zhangsh
 */


#include <utils/Log.h>
#include "stream_muxer.h"
#include "limutil.h"

//liuyan start
#include <utils/Log.h>
#define LOG_TAG "stream-muxer::stream_muxer_test"
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

int main (int argc, char *argv[])
{
	INFO( "main liuyan@20140424==============\n");
	StreamMuxer *steam_muxer = NULL;

	/*
	 * Init stream muxer
	 */
	steam_muxer = stream_muxer_init("/data/screencap.ts",640,368,1); //liuyan 0:video; 1:video with osd
	printf(" init stream muxer liuyan\n");
	/*
	 * Start stream muxer
	 */
	stream_muxer_start(steam_muxer);
	printf(" start stream muxer \n");


	while(1)
	{
		if (getchar() == 'q')
		{
			stream_muxer_close(steam_muxer);
			printf(" close stream muxer \n");
			break;
		}
		else
		{
			sleep(1);
		}

	}

	printf(" The stream program end normally ! \n\n\n");
	return 0;
}
