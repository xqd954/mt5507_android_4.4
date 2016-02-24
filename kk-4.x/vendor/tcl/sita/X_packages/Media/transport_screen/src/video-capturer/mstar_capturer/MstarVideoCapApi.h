#ifndef ANDROID_MEDIACAPTURESERVICE
#define ANDROID_MEDIACAPTURESERVICE


#include "limutil.h"
#include <pthread.h>
#include <stdio.h>


enum _MstarVideo_Capture_State
{
    MSTARVIDEO_STATE_INIT,
    MSTARVIDEO_STATE_START,
    MSTARVIDEO_STATE_PAUSE,
    MSTARVIDEO_STATE_STOP,
    MSTARVIDEO_STATE_CLOSE,
    MSTARVIDEO_STATE_INVALI,
};

typedef struct _MSVideoParameter
{
    unsigned int width;
    unsigned int height;
    unsigned int framerate;
    unsigned int with_osd;
    void *queue;
    lthread_mutex_t *queue_mutex;
    lthread_cond_t *queue_cond;    
}MSVideoParameter;

typedef struct MstarVCapture
{
    MSVideoParameter *msvparme;
    enum _MstarVideo_Capture_State mvcapState;
    pthread_t thread_id; 
    lthread_mutex_t msvideo_mutex;
    lthread_cond_t  msvideo_cond;
}MstarVCapture;

typedef struct MSVideoData
{
    int datasize;
    long long pts;
    
}MSVideoData;

/**
 * Init MstarVideo capture
 */
MstarVCapture* mstarv_capture_init(MSVideoParameter *msvparame);

/**
 * listen camera if is in use/add by taoting 2012 -11- 15
 */

Ret mstar_capture_camera_listen();


/**
 * Start MstarVideo capture
 */
Ret mstarv_capture_start(MstarVCapture * mstarv_capture);

/**
 * Pause MstarVideo capture
 */
Ret mstarv_capture_pause(MstarVCapture * mstarv_capture);

/**
 * Close MstarVideo capture
 */
Ret mstarv_capture_close(MstarVCapture * mstarv_capture);

/**
 * just for test
 */

void mstar_get_queue(MstarVCapture* mstarv_capture);

#endif


