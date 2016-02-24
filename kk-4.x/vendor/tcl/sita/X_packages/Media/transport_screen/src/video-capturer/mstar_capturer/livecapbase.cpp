#include "livecapbase.h"
#include "thread_mutex.h"
#include  "StartLivecapService.h"
#include  "Livecamera.h"

//liuyan start
#include <utils/Log.h>
#define LOG_TAG "mstar_capturer::livecapbase"
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

static thread_t g_start_livecap_service = 0;
static bool g_flag_start_livecap_service = false;

int main_capture_video_camera_listen()
{
  return Camera_Listen();
}

int main_capture_video_open(int width,int height,int with_osd )
{
  Start_VE_Camera(width,height,with_osd);
  return 0;
}

int main_capture_video_close()
{
  Close_VE_Camera();
  return 0;
}

int main_capture_audio_open()
{
  return 0;
}

int main_capture_audio_close()
{
  return 0;
}

void* start_livecap_service_thread(void * data)
{
  start_livecap_service_main();
  return NULL;
}

void* stop_livecap_service_thread()
{
  stop_livecap_service_main();
  return NULL;
}

void main_capture_set_callback(void *callback, void* caller)
{
  //  INFO( "main_capture_set_callback liuyan@20140417\n");
    set_callback(callback,caller);
}

void real_start_livecap_service_thread()
{
 // INFO( "real_start_livecap_service_thread liuyan@20140417\n");
  thread_t g_start_livecap_service = 0; 
  void *data;
  thread_create( &( g_start_livecap_service ), start_livecap_service_thread, data);
}

void real_close_livecap_service_thread( )
{
  stop_livecap_service_thread();
}
