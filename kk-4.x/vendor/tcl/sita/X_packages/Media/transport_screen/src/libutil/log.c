/*
  Copyright (C) 2010 - 2011 SWOAG Technology <www.swoag.com>

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

  LIM log support routines.

  REVISION:

    1) initial creation. ----------------------------------------- 2010-04-15 MG
*/

#define _GNU_SOURCE
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include "limlog.h"
#include "limtime.h"
#include "limthread.h"

static lthread_mutex_t log_mutex = LTHREAD_MUTEX_INITIALIZER;
static int log_level = LIM_LOG_INFO;
static lim_log_listener_t log_listener = NULL;
static void * log_listener_arg = NULL;
static unsigned int log_timebase;
static int log_head;
static int log_bytes;
static int log_buf_length;
static void * log_buf = NULL;
static int log_initialized = 0;

static void log_add (int level, const void * buf, int bytes)
{
    int len;

    if (log_listener)
    {
        //include the NULL terminator to listener.
        log_listener(log_listener_arg, level, buf, bytes + 1);
    }

    if (log_buf)
    {
        len = log_buf_length - log_head;
        if (len >= bytes)
        {
            memcpy(log_buf + log_head, buf, bytes);
            log_head += bytes;
        } else
        {
            if (len) memcpy(log_buf + log_head, buf, len);
            memcpy(log_buf, buf + len, bytes - len);
            log_head = bytes - len;
        }
        log_bytes += bytes;
        log_bytes = (log_bytes > log_buf_length)? log_buf_length : log_bytes;
    }
}

static void lim_log_destroy (void)
{
    lthread_mutex_lock(&log_mutex);
    if (log_buf)
    {
        //printf("freeing log buffer.\n");
        free(log_buf);
        log_buf = NULL;
    }
    lthread_mutex_unlock(&log_mutex);
}

static int log_init (
    int bytes,
    int level,
    lim_log_listener_t listener,
    void * arg)
{
    ltime_val_t tv;

    //at least allows 2 full log entries.
    bytes = (bytes > LIM_LOG_MAX_ENTRY_BYTES)?
        bytes : LIM_LOG_MAX_ENTRY_BYTES * 2;

    if (log_buf)
    {
        free(log_buf);
        log_buf = NULL;
    }
    else atexit(lim_log_destroy);

    ltime_gettimeofday(&tv);
    log_timebase = ltime_tv2us(&tv);
    log_buf = malloc(bytes);
    if (NULL == log_buf)
    {
        printf("ERROR: %s--%s: %d\n"
               "\tunable to allocate memory for log support.\n",
               __BASE_FILE__, __FUNCTION__, __LINE__);
    }

    log_listener = listener;
    log_listener_arg = arg;
    log_level = level;
    log_head = 0;
    log_bytes = 0;
    log_buf_length = bytes;
    log_initialized = 1 ;
    return 0;
}

static void signal_action_handler (int signum)
{
    printf("ERROR: signal %d caught.\n", signum);

    if (log_buf && log_bytes)
    {
        //this may further raise signals, but who cares. Great if we can
        //recover some log, too bad if not.
        lim_log_dump();
        printf("see lim-log.dump for details.\n");
    }
    exit(-1);
}

int lim_log_init (
    int bytes,
    int level,
    lim_log_listener_t listener,
    void * arg)
{
    int ret = 0;
    struct sigaction sig_action;

    //catch signals and dump log upon.
    memset(&sig_action, 0, sizeof(sig_action));
    sig_action.sa_handler = signal_action_handler;
    sigaction(SIGFPE, &sig_action, NULL); //floating point execption
    sigaction(SIGSEGV, &sig_action, NULL); //segmentation fault
    sigaction(SIGILL, &sig_action, NULL); //illegal instruction
    sigaction(SIGSTKFLT, &sig_action, NULL); //stack fault
    sigaction(SIGINT, &sig_action, NULL); //interrupt

    lthread_mutex_lock(&log_mutex);

    //disable internal log buffer.
    if (bytes == 0)
    {
        if (log_buf)
        {
            free(log_buf);
            log_buf = NULL;
        }
        log_level = level;
        log_listener = listener;
        log_listener_arg = arg;
        log_initialized = 1;
    } else
    {
        ret = log_init(bytes, level, listener, arg);
    }

    lthread_mutex_unlock(&log_mutex);

    return ret;
}

int lim_log (const char * context, int level, const char * fmt, ...)
{
    static int repeat = -1;
    static char history[LIM_LOG_MAX_ENTRY_BYTES];
    static unsigned int last_us;
    char buf[LIM_LOG_MAX_ENTRY_BYTES];
    int len;
    ltime_val_t tv;
    unsigned int us;
    va_list args;

    //this is not rocket science, ignore lock for minimum overhead.
    if (level > log_level) return 0;

    lthread_mutex_lock(&log_mutex);
    if (!log_initialized)
        log_init(LIM_LOG_BUFFER_BYTES, log_level,
                 log_listener, log_listener_arg);
    //if (level > log_level) goto bail;
    lthread_mutex_unlock(&log_mutex);

    ltime_gettimeofday(&tv);
    us = ltime_tv2us(&tv) - log_timebase;

    len = LIM_LOG_MAX_ENTRY_BYTES;
    len -= snprintf(buf, len, "%010u %s\n\t", us, (context)? context : "");

    va_start(args, fmt);
    len -= vsnprintf(buf + LIM_LOG_MAX_ENTRY_BYTES - len, len, fmt, args);
    va_end(args);

    lthread_mutex_lock(&log_mutex);
    if (strcmp(history, buf + 10))
    {
        if (repeat > 0)
        {
            char buf2[80];
            int len2;

            len2 = sprintf(buf2, "%010u previous message repeated %d times.\n",
                           last_us, repeat + 1);
            log_add(level, buf2, len2);
        }
        log_add(level, buf, LIM_LOG_MAX_ENTRY_BYTES - len);

        strcpy(history, buf + 10);
        repeat = 0;
    } else
    {
        repeat++;
        last_us = us;
    }
    lthread_mutex_unlock(&log_mutex);

    if (LIM_LOG_FATAL == level)
    {
        lim_log_dump();
        exit(-1);
    }

    return 0;
}

//67 = 16(chars/line) * 4(chars/byte) + TAB + \n + preserve
#define _MAX_HEX_BUF_LEN_ \
    (LIM_LOG_MAX_ENTRY_BYTES + (LIM_LOG_HEX_PRINT_MAX / 16 + 3) * 67)
int lim_log_print_hex (const char * context, const void * data, int bytes)
{
    char buf[_MAX_HEX_BUF_LEN_];
    ltime_val_t tv;
    unsigned int us;
    const unsigned char * pd;
    const unsigned char * pd2;
    int len;

    //buffer is only dumped at or below LIM_LOG_DEBUG level.
    if (LIM_LOG_DEBUG > log_level) return 0;

    ltime_gettimeofday(&tv);
    us = ltime_tv2us(&tv) - log_timebase;
    len = sprintf(buf, "%010u %s\n\t%d bytes at address: %p\n",
                  us, (context)? context : "", bytes, data);

    lthread_mutex_lock(&log_mutex);
    if (!log_initialized)
        log_init(LIM_LOG_BUFFER_BYTES, log_level,
                 log_listener, log_listener_arg);
    lthread_mutex_unlock(&log_mutex);

    len += sprintf(buf + len,
                   "\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F ");
    len += sprintf(buf + len, "0123456789ABCDEF\n");

    len += sprintf(buf + len,
                   "\t------------------------------------------------");
    len += sprintf(buf + len, "----------------\n");

    bytes = (bytes > LIM_LOG_HEX_PRINT_MAX)? LIM_LOG_HEX_PRINT_MAX : bytes;
    pd = (const unsigned char *)data;
    while (bytes >= 16)
    {
        int ii = 16;
        buf[len++] = '\t';
        pd2 = pd;
        while (ii--) len += sprintf(buf + len, "%02x ", *pd++);

        ii = 16;
        while (ii--)
        {
            buf[len++] = isgraph(*pd2)? *pd2 : '.';
            pd2++;
        }
        buf[len++] = '\n';
        bytes -= 16;
    }

    if (bytes)
    {
        int ii = bytes;
        buf[len++] = '\t';
        pd2 = pd;
        while (ii--) len += sprintf(buf + len, "%02x ", *pd++);
        ii = 16 - bytes;
        while (ii--) len += sprintf(buf + len, "xx ");

        ii = bytes;
        while (ii--)
        {
            buf[len++] = isgraph(*pd2)? *pd2 : '.';
            pd2++;
        }
        ii = 16 - bytes;
        while (ii--) buf[len++] = '.';
        buf[len++] = '\n';
    }
    buf[len++] = 0;

    lthread_mutex_lock(&log_mutex);
    log_add(LIM_LOG_DEBUG, buf, len);
    lthread_mutex_unlock(&log_mutex);

    return 0;
}


int lim_log_set_level (int level)
{
    lthread_mutex_lock(&log_mutex);

    log_level = level;

    lthread_mutex_unlock(&log_mutex);

    return 0;
}

int lim_log_get_level (void)
{
    int level;

    lthread_mutex_lock(&log_mutex);

    level = log_level;

    lthread_mutex_unlock(&log_mutex);

    return level;
}

int lim_log_set_listener (lim_log_listener_t listener, void * arg)
{
    lthread_mutex_lock(&log_mutex);

    log_listener = listener;
    log_listener_arg = arg;

    lthread_mutex_unlock(&log_mutex);

    return 0;
}

int lim_log_read (void * log, int * bytes)
{
    int len;

    lthread_mutex_lock(&log_mutex);
    if (log_buf && log_bytes && log && *bytes)
    {
        len = (*bytes > log_bytes)? log_bytes : *bytes;
        if (log_bytes < log_buf_length)
        {
            memcpy(log, log_buf, len);
        } else
        {
            memcpy(log, log_buf + log_head, log_buf_length - log_head);
            memcpy(log + log_buf_length - log_head, log_buf, log_head);
        }

        *bytes = len + 1;
        *((uint8_t *)log + len) = 0;
    } else
    {
        *((uint8_t *)log) = 0;
        *bytes = 0;
    }
    lthread_mutex_unlock(&log_mutex);

    return 0;
}

void lim_log_dump (void)
{
    int fd;
    int bytes;

    lthread_mutex_lock(&log_mutex);

    if (!log_buf || !log_bytes) goto bail;

    fd = open("lim-log.dump", O_WRONLY | O_CREAT | O_TRUNC,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        printf("ERROR: unable to create log dump file.\n");
    } else
    {
        if (log_bytes < log_buf_length)
        {
            bytes = write(fd, log_buf, log_bytes);
        } else
        {
            bytes = write(fd, log_buf + log_head, log_buf_length - log_head);
            bytes += write(fd, log_buf, log_head);
        }
        if (bytes != log_bytes)
        {
            printf("ERROR: unable to dump log.\n");
        }
        close(fd);
    }

 bail :
    lthread_mutex_unlock(&log_mutex);
}
