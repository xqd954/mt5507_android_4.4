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

  OpenMAX AL engine implementation - thread wrapper module header.

  REVISION:

    1) initial creation. ----------------------------------------- 2009-08-22 MG
*/

/** @defgroup thread thread wrapper
 * @ingroup limutil
 * @{
 */

#ifndef LIMTHREAD_H__
#define LIMTHREAD_H__

#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>

#define lthread_t pthread_t
#define lthread_mutex_t pthread_mutex_t
#define lthread_cond_t pthread_cond_t

#define LTHREAD_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define LTHREAD_COND_INITIALIZER PTHREAD_COND_INITIALIZER

typedef enum
{
    LTHREAD_MUTEX_TIMED,
    LTHREAD_MUTEX_RECURSIVE,
    LTHREAD_MUTEX_DEFAULT = LTHREAD_MUTEX_TIMED
} lthread_type_t;
int lthread_mutex_init2 (lthread_mutex_t * m, lthread_type_t type);
#define lthread_mutex_init(m) pthread_mutex_init(m, NULL)
#define lthread_mutex_destroy(m) pthread_mutex_destroy(m)
#define lthread_mutex_lock(m) pthread_mutex_lock(m)
#define lthread_mutex_unlock(m) pthread_mutex_unlock(m)
//int lthread_mutex_timedlock (
//    lthread_mutex_t * mutex,
//    int miliseconds);

#define lthread_cond_init(c) pthread_cond_init(c, NULL)
#define lthread_cond_destroy(c) pthread_cond_destroy(c)
#define lthread_cond_wait(c, m) pthread_cond_wait(c, m)
#define lthread_cond_broadcast(c) pthread_cond_broadcast(c)
#define lthread_cond_signal(c) pthread_cond_signal(c)
int lthread_cond_timedwait (
    lthread_cond_t * cond,
    lthread_mutex_t * mutex,
    int miliseconds);

#define lthread_join(t) pthread_join(t, NULL)

int lthread_create (
    lthread_t * lthread,
    void * (* func) (void *),
    void * arg,
    int detached);
#define lthread_self() pthread_self()


#define lthread_sem_t sem_t
#define lthread_sem_init(s, v) sem_init(s, 0, v)
#define lthread_sem_destroy(s) sem_destroy(s)
#define lthread_sem_up(s) sem_post(s)
#define lthread_sem_down(s) sem_wait(s)
int lthread_sem_timeddown (
    lthread_sem_t * sem,
    int miliseconds);


// thread helper macros.
#define lthread_event_timedwait(c, m, t, e, ret) do {  \
        ret = 0;                                       \
        lthread_mutex_lock(m);                         \
        while (!(e)) {                                 \
            if (lthread_cond_timedwait(c, m, t)) {     \
                ret = -1; break;                       \
            }                                          \
        }                                              \
        lthread_mutex_unlock(m);                       \
    } while(0)

#define lthread_event_broadcast(c, m, e) do {      \
        lthread_mutex_lock(m);                     \
        e;                                         \
        lthread_cond_broadcast(c);                 \
        lthread_mutex_unlock(m);                   \
    } while(0)


/** LIM run count object ID definitions. */
typedef enum {
    LIM_RUN_COUNT_DEBUG,
    LIM_RUN_COUNT_end
} LTHREAD_RUN_COUNT_ID_t;

/** interface to run 'exe' function upto 'count' times.
 * run 'exe' 'count' times within caller's thread context, through the
 * entire life of the process, when requested from within multiple threads.
 *
 * if 'count' is greater than 1, each run of 'exe' is not serialized,
 * if required to be, caller shall protect it either with the returned
 * mutex, or custom implementation.
 *
 * 'count' is relative to 'id', not 'exe', in other words, if different
 * 'exe's are requested with the same 'id', only 'count' number of the
 * 'exe's will be run and there is no guarantee of which one runs.
 *
 * for similar reason, if requested 'count' varies, there is no guarantee
 * exactly how many time 'exe' runs.
 *
 * use case #1:
 * particularly useful for the case where multiple component
 * plugins reference the same library that has some initialization code to
 * run once, yet the order of component access can not be determined
 * at compile time.
 *
 * use case #2:
 * run certain debug code up to 'count' times.
 *
 * @param exe
 *        the executable callback.
 * @param arg
 *        executable context.
 * @param id
 *        run count object ID, @ref LTHREAD_RUN_COUNT_ID_t
 * @param count
 *        maximum allowed run count.
 * @return
 *        0 success, non-zero for error.
 */
int lthread_run_upto_count (
    void (*exe)(void * arg, lthread_mutex_t * mutex, int count),
    void * arg, LTHREAD_RUN_COUNT_ID_t id, int count);

/** @} */
#endif /* LIMTHREAD_H__ */
