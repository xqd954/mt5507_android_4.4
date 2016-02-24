#include "utils/Log.h"
//#include <android/log.h>
#define LOG "Zxia-Debug"
//#define BLKID_DEBUG
#ifdef BLKID_DEBUG
	#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG, __VA_ARGS__)
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG, __VA_ARGS__)
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG, __VA_ARGS__)
#else
	#define LOGD(...)
	#define LOGI(...)
	#define LOGE(...)
#endif
