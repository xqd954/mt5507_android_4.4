#include <stdio.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include "LivecapService.h"
#include <utils/Log.h>

#define LOG_TAG "livecap::StartLivecapService"
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

using namespace android;

int g_flag_service = 1;
int start_livecap_service_main( )
{
    g_flag_service = 1;
    g_flag_service = 1;

    sp<ProcessState> proc( ProcessState::self() );

    sp<IServiceManager> sm = defaultServiceManager();

    INFO( "LivecapServiceThread( ServiceManager: %p)", sm.get() );

    LivecapService::instantiate();

    ProcessState::self()->startThreadPool();

    while ( ( g_flag_service ) )
    {
      usleep( 10 * 1000 );
    }
    
    INFO( "LivecapServiceThread end" );  
    return NULL;
}

void stop_livecap_service_main()
{
}

void set_callback(void * callback,void * caller)
{
    INFO( "set_callback" );  
    LivecapService::setCallback(callback,caller);
}
