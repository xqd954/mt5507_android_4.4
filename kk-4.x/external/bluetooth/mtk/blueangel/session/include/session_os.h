#ifndef __SESSION_OS_H__
#define __SESSION_OS_H__

//#include "bt_common.h"

#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
//#include "utils/Log.h"
//#include <cutils/xlog.h>

#include "session_api.h"

#define session_log(fmt, ...) XLOGI(fmt,## __VA_ARGS__)
#define session_log_err(fmt, ...) XLOGE(fmt,## __VA_ARGS__)

typedef pthread_mutex_t Session_Mutex;
typedef pthread_t Session_Thread;
typedef void *(*Session_Routine)(void *);



Session_Thread session_thread_create(Session_Routine, void * arg);
SESSION_RESULT session_thread_wait(Session_Thread *thread, int timeout);
SESSION_RESULT session_mutex_create(Session_Mutex **mutex);

SESSION_RESULT session_mutex_destroy(Session_Mutex *mutex);
SESSION_RESULT session_mutex_lock(Session_Mutex *mutex);
SESSION_RESULT session_mutex_unlock(Session_Mutex *mutex);

#endif
