#ifndef  __LIVE_BASE_H__
#define __LIVE_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

int main_capture_video_camera_listen();

int main_capture_video_open(int width,int height,int with_osd);

int main_capture_video_close();

int main_capture_audio_open();

int main_capture_audio_close();

void main_capture_set_callback(void *callback, void* caller);

void real_start_livecap_service_thread();
void real_close_livecap_service_thread();



#ifdef __cplusplus
}
#endif

#endif


