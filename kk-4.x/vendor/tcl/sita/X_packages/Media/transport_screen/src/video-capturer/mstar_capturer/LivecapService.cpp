#include <sys/time.h>
#include <utils/Log.h>
#include <binder/Binder.h>
#include <binder/IServiceManager.h> 
#include <binder/IPCThreadState.h> 
#include <binder/MemoryBase.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "LivecapService.h" 

#define  LIVECAP_SERVICE_NAME "livecap.rtp"
#define UNIT_TEST 0 //1 open;0 close

#define LOG_TAG "livecap::LivecapService"
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

enum
{ 
   OUTPUT_BUF                = 0,
   LIVECAP_VIDEO_OUTPUT_BUF  = 101
};

typedef  void (*callback_func)(void *caller, void * data, size_t len);

void * caller;
callback_func callback;

void  live_play_add_h264_oneframe_data( unsigned char* data, unsigned int dataSize )
{    
}

void sendppsandspsframe( unsigned char* pbufferdata, int  bufferlen )
{
  if ( pbufferdata == NULL || bufferlen <= 0 )
  {
    return;
  }

  int totallen = bufferlen;  
  int findsynccount = 0;

  unsigned char* preframestart = NULL;
  unsigned char* nextframestart = NULL;  

  int framesyncsize = 4;
  while ( totallen >= framesyncsize )
  {
    if ( pbufferdata[0] == 0x00 && pbufferdata[1] == 0x00 && pbufferdata[2] == 0x00 && pbufferdata[3] == 0x01 )
    {
      findsynccount++;

      if ( findsynccount == 1 )
      {
        preframestart = pbufferdata;

        pbufferdata += framesyncsize;

        totallen = totallen - framesyncsize;
      }
      else if ( findsynccount == 2 )
      {
        findsynccount = 1;
        nextframestart = pbufferdata ;

        int tmpframesize = nextframestart - preframestart;
        live_play_add_h264_oneframe_data( ( unsigned char* )preframestart, tmpframesize );
        bufferlen = bufferlen - tmpframesize;

        preframestart = nextframestart;

        pbufferdata += framesyncsize;

        totallen = totallen - framesyncsize;
      }
    }
    else
    {
      pbufferdata++;
      totallen--;
    }
  }

  if ( bufferlen != 0 )
  {
    live_play_add_h264_oneframe_data( ( unsigned char* )preframestart, bufferlen );
  }
}

bool Anaylze_pps_or_sps( unsigned char* buf, unsigned int len )
{
  unsigned syncbytes = 4;
  unsigned char anaylzebuf = buf[syncbytes];
  unsigned int nal_unit_type = ( anaylzebuf )&0x1f;

  //SPS,PPS  
  if ( nal_unit_type == 7 || nal_unit_type == 8 )
  {
    return true;
  }
  else
  {
    return false;
  }
}

void livecap_service_write_video_data_buf( char* buffer, unsigned int len )
{
  unsigned char* tmpbuf = ( unsigned char* )buffer;

  bool ret = Anaylze_pps_or_sps( tmpbuf, len );
  if ( ret )
  {
    sendppsandspsframe( tmpbuf, len );
  }
  else
  {
    live_play_add_h264_oneframe_data( tmpbuf, len );
  }
}

unsigned int Test_GetTimeStampMillsec()
{
  struct timeval tv; 
  struct timezone tz; 
  gettimeofday( &tv, &tz ); 

  unsigned int timestampmillsec = tv.tv_sec*1000 + tv.tv_usec / 1000;
  return timestampmillsec;
}

FILE* g_test_h264_file = NULL;

//liuyan modify start

void Test_write_h264_frame( char* buffer, unsigned int len )
{
  INFO( "Test_write_h264_frame  len= %d\n",len);
  if ( g_test_h264_file == NULL )
  {
    g_test_h264_file = fopen( "/data/fifob.h264", "wb" );	
    if ( g_test_h264_file == NULL ){
    	INFO( "Test_write_h264_frame open /data/fifob.h264 failed...\n");
    	return;
    }
  }
  int ret = fwrite( buffer, 1, len, g_test_h264_file );
  INFO( "Test_write_h264_frame  ret= %d\n",ret);
}

/* 
void Test_write_h264_frame( char* buffer, unsigned int len )
{
  INFO( "Test_write_h264_frame  buffer= %ld\n",(long)buffer);
  INFO( "Test_write_h264_frame 00 len= %d\n",len);
  if ( g_test_h264_file == NULL )
  {
    INFO( "Test_write_h264_frame 11 len= %d\n",len);
    char *file_name = "/data/fifoa.h264";
    unlink(file_name);
    mkfifo(file_name, 0777);
    g_test_h264_file = fopen( file_name, "wb+" );
  }

  INFO( "Test_write_h264_frame 22 len= %d\n",len);
  int ret = fwrite( buffer, 1, len, g_test_h264_file );
  INFO( "Test_write_h264_frame 33 ret= %d\n",ret);
}
*/

//end

namespace android
{
  int LivecapService::instantiate()
  {
    INFO( "LivecapService instantiate" );
    int ret = defaultServiceManager()->addService( String16( LIVECAP_SERVICE_NAME ), new LivecapService() );
    INFO( "LivecapService ret = %d ", ret );
    return ret;
  }

  LivecapService::LivecapService()
  {
    INFO( "LivecapService created" );
  }

  LivecapService::~LivecapService()
  {
    INFO( "LivecapService destroyed" );
  }

  status_t LivecapService::onTransact( uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags )
  {
    INFO( "LivecapService onTransact enter...20140424\n");
    switch ( code )
    {
      case OUTPUT_BUF:
      case LIVECAP_VIDEO_OUTPUT_BUF:
        {
          const char* pBuf;
          int len;
          len = data.readInt32();

          pBuf = reinterpret_cast<const char*>( data.readInplace( len ) );
          INFO( "LivecapService onTransact len = %d\n",len );

          unsigned char* tmpbuf = ( unsigned char* )pBuf;

          #if UNIT_TEST
          Test_write_h264_frame( ( char* )pBuf, len );
          #endif 
	  
	//  INFO( "LivecapService onTransact callback before \n");
          callback(caller,(void*)pBuf,len); 
       //   INFO( "LivecapService onTransact callback after \n");

          livecap_service_write_video_data_buf( ( char* )pBuf, len );      
          return NO_ERROR;
        }
        break;
      default:
        return BBinder::onTransact( code, data, reply, flags );
    }
  }

  status_t LivecapService::setCallback(void *cb,void *cl)
  {
 //   INFO( "LivecapService::setCallback \n");
    callback = (callback_func)cb;
    caller = cl;

    return 0;
  }
}; //namespace


