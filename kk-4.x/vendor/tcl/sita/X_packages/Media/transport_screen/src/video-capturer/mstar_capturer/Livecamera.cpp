#include <camera/Camera.h>
#include <camera/ICamera.h>
#include <media/mediarecorder.h>
#include <camera/CameraParameters.h>
#include <utils/Log.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <cutils/properties.h>
#include <stdio.h>
//binder service begin
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
//binder service end

using namespace android;

#define LOG_TAG "livecap::livecamera"
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

#define VE_KEY_STRING_NAME               "mstar.SOURCETOVE"
#define VE_VALUE_STRING_OP2              "E_XC_OP2"
#define VE_VALUE_STRING_OVERLAP          "E_XC_OVERLAP"
#define  GLOBAL_DEFAUTL_RESOLVRATION_WIGTH  320
#define  GLOBAL_DEFAUTL_RESOLVRATION_HIGH     240

#define DEFAULT_NORMAL_VIDEO_BITRATES  1024000 //1M
#define DEFAULT_HIGH_VIDEO_BITRATES 2048000 //2M

typedef enum
{
    E_TRAVELING_RES_720_576,
    E_TRAVELING_RES_320_240,
    E_TRAVELING_RES_720_480,
    E_TRAVELING_RES_640_480,
    E_TRAVELING_RES_RESERVE0,
    E_TRAVELING_RES_480_320,
    E_TRAVELING_RES_RESERVE1,
    E_TRAVELING_RES_176_144,
    E_TRAVELING_RES_RESERVE2,
    E_TRAVELING_RES_720_400,
    E_TRAVELING_RES_640_368,
    E_TRAVELING_RES_320_176,
    E_TRAVELING_RES_1280_720,
    E_TRAVELING_RES_1920_1080,
    E_TRAVELING_RES_MAX,
} EN_TRAVELING_RESOLUTION;

static sp<Camera> g_livecap_camera = NULL;
static sp<MediaRecorder> g_livecap_mediarecorder = NULL;

int Camera_Listen()
{
    int Cameraid = 0;

    g_livecap_camera = Camera::connect( Cameraid , String16(), -1);
 
    if (NULL == g_livecap_camera.get() )
    {
        INFO( "livecap Camera::connect() failed \n" );
        return -1;
    
    }
    else
    {
        INFO( "livecap Camera::connect() ok \n" );  
        g_livecap_camera->lock();   
        g_livecap_camera->disconnect();
        g_livecap_camera.clear();	   
        g_livecap_camera = NULL;
    }

    Cameraid = 5;
    g_livecap_camera = Camera::connect( Cameraid , String16(), -1);
    
    if (NULL == g_livecap_camera.get() )
    {
        INFO( "livecap Camera::connect() failed \n" );
        return -1;
    }
    else
    {
        INFO( "livecap Camera::connect() ok \n" );  
        g_livecap_camera->lock();   
        g_livecap_camera->disconnect();
        g_livecap_camera.clear();	   
        g_livecap_camera = NULL;
    }

    return 0;
}

int Start_VE_Camera(int width,int height,int with_osd)
{
  INFO("Start_VE_Camera\n");
  unsigned int mwigth  = width;
  unsigned int mheigh    = height;
  int framerate = 20;
  int ret = 0;
  int Cameraid = 5;
  
  g_livecap_camera = Camera::connect( Cameraid , String16(), -1);

  if ( g_livecap_camera == NULL )
  {
    INFO( "livecap Camera::connect() fail " );
    return 1;
  }

  CameraParameters tmpcamerparame( g_livecap_camera->getParameters() );

  if ( mwigth == 640 && mheigh == 368 )
  {
    tmpcamerparame.set( "traveling-res", "10" );
  }
  else
  {
    INFO( "livecap not support resolution wigth=%d,high=%d\n", mwigth, mheigh ); 
    return 1;
  }

//modify by liuyan for:can set mode  start

  if(!with_osd)
  {
    tmpcamerparame.set(tmpcamerparame.KEY_TRAVELING_MODE,E_TRAVELING_ALL_VIDEO);  
    INFO("livecap  tranveling---E_TRAVELING_ALL_VIDEO................\n");
  }
  else
  {
    tmpcamerparame.set(tmpcamerparame.KEY_TRAVELING_MODE,E_TRAVELING_ALL_VIDEO_WITH_OSD);
    INFO("livecap  tranveling---E_TRAVELING_ALL_VIDEO_WITH_OSD...............\n");
    
  }

/*  if(!with_osd)
  {
  //  tmpcamerparame.set(tmpcamerparame.KEY_TRAVELING_MODE,E_TRAVELING_ALL_VIDEO);  
    tmpcamerparame.set(tmpcamerparame.KEY_TRAVELING_MODE,E_TRAVELING_ALL_VIDEO_WITH_OSD); //liuyan test
    INFO("livecap  tranveling---E_TRAVELING_ALL_VIDEO\n");
  }
  else
  {
    INFO("livecap  tranveling---E_TRAVELING_ALL_VIDEO_WITH_OSD\n");
    return 1;
  }*/
//end
  tmpcamerparame.set( tmpcamerparame.KEY_TRAVELING_FRAMERATE, 20 );


  g_livecap_camera->setParameters( tmpcamerparame.flatten()); 

  g_livecap_camera->unlock();
  g_livecap_mediarecorder = new MediaRecorder();
  
  ret = g_livecap_mediarecorder->setCamera( g_livecap_camera->remote(), g_livecap_camera->getRecordingProxy() );
  if ( ret != NO_ERROR )
  {
    INFO( "livecap mediacorder setCamera error" );
    return 1;
  }

  ret = g_livecap_mediarecorder->setVideoSource( VIDEO_SOURCE_CAMERA );
  if ( ret != NO_ERROR )
  {
    INFO( "livecap mediacorder setVideoSource error" );
    return 1;
  }

  ret = g_livecap_mediarecorder->setOutputFormat( 10 );

  if ( ret != NO_ERROR )
  {
    INFO( "livecap mediacorder setOutputFormat error" );
    return 1;
  }

  ret = g_livecap_mediarecorder->setVideoEncoder( VIDEO_ENCODER_H264 );
  if ( ret != NO_ERROR )
  {
    INFO( "livecap mediacorder setVideoEncoder error" );
    return 1;
  }

  g_livecap_mediarecorder->setVideoSize( mwigth, mheigh );
  g_livecap_mediarecorder->setVideoFrameRate( 20 );
  
  String8 cameraid;
  if(5 == Cameraid)
  {
    cameraid = "video-param-camera-id=5";
  }
  g_livecap_mediarecorder->setParameters(cameraid);

  char strVieobitratval[100];
  unsigned int videobitate = DEFAULT_HIGH_VIDEO_BITRATES;
  snprintf( strVieobitratval, 100, "video-param-encoding-bitrate=%u", videobitate );
  String8 strVideobitrate(strVieobitratval);
  g_livecap_mediarecorder->setParameters(strVideobitrate);

  ret = g_livecap_mediarecorder->prepare();
  if ( ret != NO_ERROR )
  {
    INFO( "livecap mediacorder prepare error" );
    return 1;
  }

  ret = g_livecap_mediarecorder->start();
  if ( ret != NO_ERROR )
  {
    INFO( "livecap mediacorder  start error" );
    return 1;
  }
  
  property_set("CameraService.onuse", "1");
  return 0;
}

static void sig_alrm(int sifno)
{
  return;  
}

int Close_VE_Camera()
{
  INFO("------Close_VE_Camera begin\n" ); 

  property_set("CameraService.onuse", "0");

  if(signal(SIGALRM,sig_alrm) == SIG_ERR)
  {
      INFO("@@@@@@signal ERROR@@@@@@\n");
  }

  alarm(1);

  if ( g_livecap_mediarecorder != NULL )
  {
    g_livecap_mediarecorder->stop();        
    g_livecap_mediarecorder->release();
    g_livecap_mediarecorder.clear();       
    g_livecap_mediarecorder = NULL;
  }
  
  alarm(0);

  if ( g_livecap_camera != NULL )
  {
    g_livecap_camera->lock();   
    g_livecap_camera->disconnect();
    g_livecap_camera.clear();       
    g_livecap_camera = NULL;
  }
  
  INFO("****Close_VE_Camera end\n" ); 
 
  return 0;
}
