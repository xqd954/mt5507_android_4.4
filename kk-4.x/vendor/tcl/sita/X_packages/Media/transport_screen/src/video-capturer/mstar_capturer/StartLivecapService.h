#ifndef ANDROID_LIVECAPSERVICE_H
#define ANDROID_LIVECAPSERVICE_H



int start_livecap_service_main( );

void stop_livecap_service_main();

void set_callback(void * callback,void * caller);

#endif

