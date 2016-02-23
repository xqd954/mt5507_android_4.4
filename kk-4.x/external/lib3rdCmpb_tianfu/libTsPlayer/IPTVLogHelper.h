#ifndef _ANDROID_LOG_HELPER_H_
#define _ANDROID_LOG_HELPER_H_

#include <android/log.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "IPTV_NATIVE"

#ifdef  LOGV
#undef  LOGV
#endif

#ifdef  LOGD
#undef  LOGD
#endif

#ifdef  LOGI
#undef  LOGI
#endif

#ifdef  LOGW
#undef  LOGW
#endif

#ifdef  LOGE
#undef  LOGE
#endif
#if 1
#define LOGV(...)	__android_log_print( ANDROID_LOG_VERBOSE,	LOG_TAG, __VA_ARGS__ )

#define LOGD(...)	__android_log_print( ANDROID_LOG_DEBUG,		LOG_TAG, __VA_ARGS__ )

#define LOGI(...)	__android_log_print( ANDROID_LOG_INFO,		LOG_TAG, __VA_ARGS__ )

#define LOGW(...)	__android_log_print( ANDROID_LOG_WARN,		LOG_TAG, __VA_ARGS__ )

#define LOGE(...)	__android_log_print( ANDROID_LOG_ERROR,		LOG_TAG, __VA_ARGS__ )
#else
#define LOGV(...)	 

#define LOGD(...)	 

#define LOGI(...)	 

#define LOGW(...)	 

#define LOGE(...)	 
#endif
#endif
