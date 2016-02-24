#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>
#include "limlog.h"
#include "limqlist.h"
#include "livecapbase.h"
#include "MstarVideoCapApi.h"
#include <sys/stat.h>

#define ADD_SPS_PPS_INFO


//liuyan start
#include <utils/Log.h>
#define LOG_TAG "livecap::MstarVideoCapApi"
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

//liuyan add start
int fd = -1;
//end
static void write_h264_frame_cb(void * args, void *data, size_t datasize)
{
    INFO( "write_h264_frame_cb in datasize= %d\n",datasize);
    MstarVCapture *mstarv_capture = (MstarVCapture *) args;
  //  LOGD(" DBG: video write_h264_frame_cb  start ");

    //printf("...............liuyan write_h264_frame_cb\n");
    if (!args)
        return;
        
    //liuyan modify start  
 /*   if ( fd<0 )
    {
        fd = open( "/data/fifoc.h264", O_CREAT|O_RDWR|O_TRUNC,0666 );	
        if ( fd<0){
    	    INFO( "write_h264_frame_cb open /data/fifob.h264 failed...\n");
    	    return;
        }
    }
    int ret = write(fd,data,datasize);
    INFO( "write_h264_frame_cb  ret= %d\n",ret); */
    //end
/*
    {
     static int fd = -1;
     INFO( "write_h264_frame_cb 11 \n");
     char *file_name = "/data/fifo.h264";
     if (fd < 0)
     {
     	INFO( "write_h264_frame_cb fd < 0 \n");
        unlink(file_name);
        mkfifo(file_name,0777);
        fd = open(file_name,O_CREAT|O_RDWR|O_TRUNC,0666);
     }

     if (fd > 0)
     {
     	INFO( "write_h264_frame_cb fd > 0 \n");
        write(fd,data,datasize);
     }

    }*/
    
	
//    INFO( "write_h264_frame_cb 22 liuyan\n");
    MstarVCapture *mstarCap = (MstarVCapture*) mstarv_capture;
    void *ptr = data;
    int bytes;

#ifdef  ADD_SPS_PPS_INFO
    static unsigned char spspps_data[256];
    static int spspps_data_size = 0;
    
    unsigned int nal_unit_type;
    unsigned char * buf = (unsigned char*) data;
    
    nal_unit_type = (*(buf +4)) & 0x1f;
    //LOGD("nal_unit_type = %d ",nal_unit_type);
    /*the first save the sps pps info, the first frame include sps and pps*/
    if((nal_unit_type == 7) && (datasize < 256))
    {
        memset(spspps_data, 0, 256);
        memcpy(spspps_data, buf, datasize);
        spspps_data_size = datasize;
        LOGD(" spspps_data_size%d \n ", spspps_data_size);
        goto END;
    }
    
    /* if current frame type is I Frame, please insert spspps info by first saved*/
    if (nal_unit_type == 5)
    {
    	
        char * pQue = NULL;
        MSVideoData queuedata;
        if (mstarCap->mvcapState != MSTARVIDEO_STATE_CLOSE)
        {
      
            ltime_val_t tv;
            ltime_gettimeofday(&tv);
            queuedata.pts = ((ltime_tv2us(&tv)) / 1000);
            //queuedata.datasize = spspps_data_size ;
            queuedata.datasize = spspps_data_size + datasize-1; // add by zhangsh,20120927

    
            lthread_mutex_lock(mstarCap->msvparme->queue_mutex);
            pQue = limq_enqueue_get(mstarCap->msvparme->queue);
            lthread_mutex_unlock(mstarCap->msvparme->queue_mutex);
    
            if (pQue)
            {
            	
                memcpy(pQue,(char*) (&queuedata), sizeof(MSVideoData));
                memcpy(pQue + sizeof(MSVideoData),spspps_data,spspps_data_size);
                memcpy(pQue + sizeof(MSVideoData)+spspps_data_size, buf+1,datasize-1);// add by zhangsh,20120927

                // add sps pps info before IDR
                // sps pps start code is "00 00 00 01"
                // The realy IDR frame start code is "00 00 01" here.


                //queuedata.datasize = spspps_data_size;
                
                lthread_mutex_lock(mstarCap->msvparme->queue_mutex);
                limq_enqueue_put(mstarCap->msvparme->queue, pQue);
                lthread_mutex_unlock(mstarCap->msvparme->queue_mutex);
            }
            else
            {
                LOGD(" DBG:There is no buffer for push Sps-Pps frame\n");
            }
        }
    }    
#endif
/*add and modify end by sujuan 2012-08-01*/
    
    char * pQue = NULL;
    MSVideoData queuedata;
    if ((mstarCap->mvcapState != MSTARVIDEO_STATE_CLOSE) && (nal_unit_type != 5))
    {
    	
        ltime_val_t tv;
        ltime_gettimeofday(&tv);
        queuedata.pts = ((ltime_tv2us(&tv)) / 1000);
        queuedata.datasize = datasize ;

        lthread_mutex_lock(mstarCap->msvparme->queue_mutex);
        pQue = limq_enqueue_get(mstarCap->msvparme->queue);
        lthread_mutex_unlock(mstarCap->msvparme->queue_mutex);

        if (pQue)
        {
            
            memcpy(pQue, (char*) (&queuedata), sizeof(MSVideoData));
            memcpy(pQue + sizeof(MSVideoData), data,datasize);

            lthread_mutex_lock(mstarCap->msvparme->queue_mutex);
            limq_enqueue_put(mstarCap->msvparme->queue, pQue);
            lthread_mutex_unlock(mstarCap->msvparme->queue_mutex);
        }
        else
        {
            LOGD(" DBG: Video There is no buffer for push h264 frame \n");
        }
    }

END:    

    return;
    //LOGD(" DBG: video write_h264_frame_cb  end ");
}

/**
 * Init MstarVideo capture
 */
MstarVCapture* mstarv_capture_init(MSVideoParameter *msvparame)
{
//    LOGD("mstarv_capture_init in");
    INFO("mstarv_capture_init in liuyan.... \n");
    MstarVCapture *mvideocap = (MstarVCapture*) malloc(sizeof(MstarVCapture));
    mvideocap->msvparme = (MSVideoParameter*) malloc(sizeof(MSVideoParameter));
    lthread_mutex_init(&mvideocap->msvideo_mutex);
    lthread_cond_init(&mvideocap->msvideo_cond);

    if (msvparame != NULL)
    {
        mvideocap->msvparme->width = msvparame->width;
        mvideocap->msvparme->height = msvparame->height;
        mvideocap->msvparme->framerate = msvparame->framerate;
        mvideocap->msvparme->with_osd = msvparame->with_osd;
        mvideocap->msvparme->queue = msvparame->queue;
        mvideocap->msvparme->queue_cond = msvparame->queue_cond;
        mvideocap->msvparme->queue_mutex = msvparame->queue_mutex;
   /*     LOGD("mstarv_capture_init %d %d %d %p %x %x",
                mvideocap->msvparme->width, mvideocap->msvparme->height, mvideocap->msvparme->framerate, mvideocap->msvparme->queue, mvideocap->msvparme->queue_cond, mvideocap->msvparme->queue_mutex);
        LOGD("mstarv_capture_init out");*/
       // INFO("mstarv_capture_init out width = %d ",mvideocap->msvparme->width,"height = %d ",mvideocap->msvparme->height,"framerate = %d ",mvideocap->msvparme->framerate);
        INFO("mstarv_capture_init out width = %d ",mvideocap->msvparme->width);
        INFO("mstarv_capture_init out height = %d ",mvideocap->msvparme->height);
        INFO("mstarv_capture_init out framerate = %d ",mvideocap->msvparme->framerate);
        mvideocap->mvcapState = MSTARVIDEO_STATE_INIT;
        INFO("mstarv_capture_init out end");
        return mvideocap;
    }

    mvideocap->mvcapState = MSTARVIDEO_STATE_INVALI;

//    LOGD(" init end ");
    INFO("mstarv_capture_init init end \n");
    return mvideocap;
}

/**
 * listen camera if is in use
 */

/* add by taoting 2012-11-15
           for test camera if be used
           if is be used by other application,must quit*/

Ret mstar_capture_camera_listen()
{
    int ret;
    ret = main_capture_video_camera_listen();

    if(ret == -1)
    {
        printf("mstar_capture_camera_listen return NULL");
        return RET_FAIL;
    }
    return RET_OK;
}

/**
 * Start MstarVideo capture
 */
Ret mstarv_capture_start(MstarVCapture * mstarv_capture)
{
    INFO("mstarv_capture_start in 20140423 \n");
    Ret state = RET_OK;

    if (mstarv_capture)
    {
        if (mstarv_capture->mvcapState != MSTARVIDEO_STATE_INIT)
        {
            state = RET_FAIL;
            LOGD("mstarv_capture_start state error");
            goto bail;
        }
    }

    real_start_livecap_service_thread();
    usleep(100 * 1000);
    
    INFO("mstarv_capture_start main_capture_set_callback before \n");
    main_capture_set_callback(write_h264_frame_cb, (void*) mstarv_capture);
    INFO("mstarv_capture_start main_capture_set_callback after \n");
  //  LOGD("mstarv_capture->msvparme->width = %d",mstarv_capture->msvparme->width);
  //  LOGD("mstarv_capture->msvparme->height = %d",mstarv_capture->msvparme->height);
    main_capture_video_open(mstarv_capture->msvparme->width,mstarv_capture->msvparme->height,mstarv_capture->msvparme->with_osd);
    INFO("mstarv_capture_start 11 \n");
    if (mstarv_capture)
    {
    	INFO("mstarv_capture_start 22 \n");
        mstarv_capture->mvcapState = MSTARVIDEO_STATE_START;
    }
    INFO("mstarv_capture_start 33 \n");
    bail: return state;
}

/**
 * Pause MstarVideo capture
 */
Ret mstarv_capture_pause(MstarVCapture * mstarv_capture)
{
    Ret state = RET_OK;
    mstarv_capture->mvcapState = MSTARVIDEO_STATE_PAUSE;
    return state;
}

/**
 * Close MstarVideo capture
 */
Ret mstarv_capture_close(MstarVCapture * mstarv_capture)
{
    INFO("mstarv_capture_close in \n");
    Ret state = RET_OK;

    if (mstarv_capture)
    {
        if (mstarv_capture->mvcapState == MSTARVIDEO_STATE_INVALI)
        {
            state = RET_FAIL;
            LOGD("mstarv_capture_start state error");
            goto bail;
        }

        lthread_mutex_lock(&mstarv_capture->msvideo_mutex);
        mstarv_capture->mvcapState = MSTARVIDEO_STATE_CLOSE;
        lthread_mutex_unlock(&mstarv_capture->msvideo_mutex);
    }
    real_close_livecap_service_thread();   
    main_capture_video_close();

    if (mstarv_capture != NULL)
    {
        if (mstarv_capture->msvparme != NULL)
        {
            free(mstarv_capture->msvparme);
            mstarv_capture->msvparme = NULL;
        }
        lthread_mutex_destroy(&mstarv_capture->msvideo_mutex);
        lthread_cond_destroy(&mstarv_capture->msvideo_cond);
        free(mstarv_capture);
        mstarv_capture = NULL;
    }
    bail: return state;
}

