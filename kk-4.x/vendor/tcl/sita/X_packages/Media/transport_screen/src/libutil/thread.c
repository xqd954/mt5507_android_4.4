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

  OpenMAX AL thread wrapper implementation.

  REVISION:

    1) initial creation. ----------------------------------------- 2009-08-20 MG
*/

#define _GNU_SOURCE
#include "limlog.h"
#include "limthread.h"
#include "limtime.h"


int lthread_create (
    lthread_t * thread,
    void * (* func) (void *),
    void * arg,
    int detached)
{
    int ret;

    ret = pthread_create(thread, NULL, func, arg);
    if (!ret && detached)
    {
        ret = pthread_detach(*thread);
    }
    return ret;
}

//int lthread_mutex_timedlock (lthread_mutex_t * mutex, int miliseconds)
//{
//    struct timespec ts;
//
//    ltime_ms2timespec(miliseconds, &ts);
//    return pthread_mutex_timedlock(mutex, &ts);
//}

int lthread_mutex_init2 (lthread_mutex_t * mutex, lthread_type_t type)
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    switch (type)
    {
    case LTHREAD_MUTEX_RECURSIVE :
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        break;

    case LTHREAD_MUTEX_DEFAULT :
    default:
        break;
    }
    return pthread_mutex_init(mutex, &attr);
}

int lthread_cond_timedwait (
    lthread_cond_t * cond,
    lthread_mutex_t * mutex,
    int miliseconds)
{
    struct timespec ts;

    if (-1 == miliseconds) return pthread_cond_wait(cond, mutex);

    ltime_ms2timespec(miliseconds, &ts);
    return pthread_cond_timedwait(cond, mutex, &ts);
}

int lthread_sem_timeddown (
    lthread_sem_t * sem,
    int miliseconds)
{
    struct timespec ts;

    ltime_ms2timespec(miliseconds, &ts);
    return sem_timedwait(sem, &ts);
}

typedef struct
{
    int count;
    lthread_mutex_t mutex;
} run_count_reg_t;

static run_count_reg_t run_count_regs[LIM_RUN_COUNT_end] = {
    {0, LTHREAD_MUTEX_INITIALIZER}  /* _DEBUG */
};

int lthread_run_upto_count (
    void (*exe)(void * arg, lthread_mutex_t * mutex, int count),
    void * arg, LTHREAD_RUN_COUNT_ID_t id, int count)
{
    if (id >= LIM_RUN_COUNT_end || count <= 0)
    {
        LIMERR("invalid run-count-request-id: %d or count: %d\n", id, count);
        return -1;
    }

    lthread_mutex_lock(&run_count_regs[id].mutex);
    if (run_count_regs[id].count < count)
    {
        run_count_regs[id].count++;
        count = run_count_regs[id].count;
        lthread_mutex_unlock(&run_count_regs[id].mutex);
        exe(arg, &run_count_regs[id].mutex, count);
        goto bail;
    }
    lthread_mutex_unlock(&run_count_regs[id].mutex);

 bail:
    return 0;
}
