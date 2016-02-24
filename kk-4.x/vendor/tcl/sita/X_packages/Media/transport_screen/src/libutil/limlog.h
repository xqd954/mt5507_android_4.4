/*
  Copyright (C) 2009 - 2011 SWOAG Technology <www.swoag.com>

  This library is free software; you can redistribute it and/or modify it under
  the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation; either version 2.1 of the License, or (at your option)
  any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
  details.

  You should have received a copy of the GNU Lesser General Public License
  along with this library; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA
  02110-1301  USA

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  LIM log and debug msg support header.

  REVISION:

    2) LIM log subsystem addition. ------------------------------- 2010-04-15 MG
    1) initial creation. ----------------------------------------- 2009-08-19 MG
*/

#ifndef LIMLOG_H__
#define LIMLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/** @defgroup log debug log
 * @ingroup limutil
 * @{
 * log support is done in a non-intrusive and light way so that enableing
 * log support itelf won't affect running system much (hopefully).
 *
 * after being initialized with @ref lim_log_init(), log entry can be
 * added by calling @ref lim_log() to a circular buffer, which can
 * be retrieved at any time by @ref lim_log_read().
 *
 * log is added only when requested log level is lower than current log level
 * set by client.
 *
 * upon each log write, client supplied callback is called if present, so
 * client can catch the log and processes it if desirable.
 *
 * client can choose to completely disable internal log buffer and relies on
 * its own listener to process log entries, by calling to @ref lim_log_init()
 * with zero buffer length.
 *
 * upon fatal error, system will automatically dump log to 'lim-log.dump',
 * if internal circular buffer is enabled.
 *
 * default LIM log level: LIM_LOG_INFO.
 */

/** maximum number of bytes per log entry. */
#define LIM_LOG_MAX_ENTRY_BYTES 2048

/** maximum number of bytes allowed when printing in HEX format. */
#define LIM_LOG_HEX_PRINT_MAX   2048

/** default log circular buffer size in bytes. */
#define LIM_LOG_BUFFER_BYTES    (1024 * 1024 * 2)


/** LIM Log listener callback type definition.
 * listener is called with log mutex held, it must not block.
 *
 * @param arg
 *        client supplied callback context.
 * @param level
 *        log level
 * @param log
 *        log buffer
 * @param bytes
 *        log data bytes in buffer.
 */
typedef void (*lim_log_listener_t) (
    const void * arg,
    int level,
    const void * log,
    int bytes);


#define LIM_LOG_DISABLED -1  /** completely disable log system. */
#define LIM_LOG_FATAL     0  /** non-recoverable fatal error that must be a bug
                              * and shall be corrected, by developer.
                              */
#define LIM_LOG_ERROR     8  /** recoverable error. */
#define LIM_LOG_WARNING  16  /** things may have gone wrong but not an error. */
#define LIM_LOG_INFO     32  /** things that might interest client. */
#define LIM_LOG_VERBOSE  64  /** I like to talk just to annoy you. */
#define LIM_LOG_DEBUG   999  /** development support. */


/** initialize lim log support.
 * log system can be reinitialized at run time.
 *
 * @param bytes
 *        maximum log buffer length in bytes. If zero, internal log buffer
 *        is disabled. When internal buffer is disabled, it relies on client
 *        listener to actually interpret log entries.
 * @param level
 *        target log level.
 * @param listener
 *        client log listener if any.
 * @param arg
 *        client listener callback context.
 * @return
 *        0 if log support successfully initialized, nonzero error otherwise.
 */
int lim_log_init (
    int bytes,
    int level,
    lim_log_listener_t listener,
    void * arg);


/** write log entry.
 *
 * @param context
 *        client supplied context description, can be NULL.
 * @param level
 *        log message level.
 *
 * @return
 *        0 if log successfully written, non-zero otherwise.
 */
int lim_log (const char * context, int level, const char * fmt, ...);


/** write hex format log entry.
 * dump buffer in hex format to log entry at LIM_LOG_DEBUG level,
 * up to @ref LIM_LOG_HEX_PRINT_MAX bytes at a time.
 *
 * @param context
 *        client supplied context description, can be NULL.
 * @param data
 *        data buffer
 * @param bytes
 *        data buffer length in bytes.
 *
 * @return
 *        0 if log successfully written, non-zero otherwise.
 */
int lim_log_print_hex (const char * context, const void * data, int bytes);


/** set current log level.
 *
 * @return
 *        0 if log level successfully set, non-zero otherwise.
 */
int lim_log_set_level (int level);


/** get current log level. */
int lim_log_get_level (void);


/** set current log listener.
 * only up to one listener is supported, newly set listener will automatically
 * removed previously registered listener if any.
 *
 * @param listener
 *        client supplied listener callback.
 * @param arg
 *        client listener callback context.
 * @return
 *        0 if log listener successfully set, non-zero otherwise.
 */
int lim_log_set_listener (lim_log_listener_t listener, void * arg);


/** read log circular buffer.
 *
 * @param log
 *        client supplied buffer.
 * @param bytes
 *        input as buffer length of user supplied buffer, output as available
 *        bytes filled in by the log system.
 * @return
 *        0 if log successfully read, non-zero otherwise.
 */
int lim_log_read (void * log, int * bytes);


/** dump log to 'lim-log.dump' */
void lim_log_dump (void);

#define ANDROID_LOG
#ifndef ANDROID_LOG
#define LIMLOG(level, descriptor, fmt, arg...) do {                     \
        char context[1024];                                             \
        snprintf(context, 1024, "%s: %s--%s: %d",                       \
                 descriptor, __BASE_FILE__, __FUNCTION__, __LINE__);    \
        lim_log(context, level, fmt, ##arg); } while(0)

#define LIMHEX(buf, bytes) do {                                         \
        char context[1024];                                             \
        snprintf(context, 1024, "%s: %s--%s: %d",                       \
                 "HEXDUMP", __BASE_FILE__, __FUNCTION__, __LINE__);     \
        lim_log_print_hex(context, buf, bytes); } while(0)



#define LIMFATAL(fmt, arg...) LIMLOG(LIM_LOG_FATAL, "FATAL", fmt, ##arg)
#define LIMERR(fmt, arg...) LIMLOG(LIM_LOG_ERROR, "ERROR", fmt, ##arg)
#define LIMWARN(fmt, arg...) LIMLOG(LIM_LOG_WARNING, "WARNING", fmt, ##arg)
#define LIMDBG(fmt, arg...) LIMLOG(LIM_LOG_DEBUG, "DEBUG", fmt, ##arg)
#define LIMINFO(fmt, arg...) LIMLOG(LIM_LOG_INFO, "INFO", fmt, ##arg)
#define LIMVERBOSE(fmt, arg...) LIMLOG(LIM_LOG_VERBOSE, "VERBOSE", fmt, ##arg)
#define ASSERT(cond) do { if (!(cond)) LIMFATAL("ASSERT\n"); } while (0)
//#define ASSERT assert
#else
#include <android/log.h>
#define LOGD(fmt,arg...) do{                                                     \
    __android_log_print(3,0,"%s -> %s -> %d ",__FILE__,__func__,__LINE__);         \
    __android_log_print(3,0,fmt,##arg);                                           \
    }while(0);

#define LIMFATAL(fmt, arg...) LOGD(fmt,##arg)
#define LIMERR(fmt, arg...)   LOGD(fmt,##arg)
#define LIMWARN(fmt, arg...)
#define LIMDBG(fmt, arg...)
#define LIMLOG(fmt, arg...) LOGD(fmt,##arg)
#define LIMINFO(fmt, arg...)
#define LIMVERBOSE(fmt, arg...)
#define ASSERT(cond)

#define LOG_ERR(fmt,arg...)   LOGD(fmt,##arg);
#define LOG_WARN(fmt,arg...)  LOGD(fmt,##arg);
#define LOG_DBG(fmt,arg...)   LOGD(fmt,##arg);

#endif

/** @} */
#endif /* LIMLOG_H__ */
