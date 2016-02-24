#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <stdio.h> 
#include "livecapbase.h"
#include <utils/Log.h>

#define LOG_TAG "livecap::livecapmainstart"
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

static bool g_proc_status = true;

int main( int argc, char* argv[] )
{

  real_start_livecap_service_thread();

  usleep( 10 * 1000 );

  main_capture_video_open(640,368,0);

  while ( g_proc_status )
  {
    usleep( 10 * 1000 );
  }

  INFO( "\n livecap all services exit ! \n" );
}


