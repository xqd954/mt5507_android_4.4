/******************************************************************************
FileName: block_cond.c
Copyright (c) 2012, org. All Rights Reserved.

Version: Ver 1.0
Author: deeve (deeve@dioux.org Dept:freedom)

Description:
		block condition operation

Note:
		none
History:
Version      Date                  Author       Modification
-------    ----------    -------    ------------
1.0              2012-3-10    deeve        Create
*******************************************************************************/
/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

#include "block_cond.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

#define DEBUG_LOG_ENABLE                       (1)  /*debug this model*/


#ifdef LOG_FOR_LOGCAT

#define LOG_TAG "libTsPlayer"
#include <cutils/log.h>

#if DEBUG_LOG_ENABLE
#define BLOCK_DBG(fmt, ...)  LOGD("[%s]line:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define BLOCK_DBG(...)
#endif

#else

#if DEBUG_LOG_ENABLE
#define BLOCK_DBG(fmt, ...)  fprintf(stderr, "[%s]line:%d "fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define BLOCK_DBG(...)
#endif

#endif


/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/
struct
{
    pthread_mutex_t  t_mutex;
    pthread_cond_t   t_cond;
    int i4_value;  /* 0 - no block  other - blocked*/
} t_block_ctx;

/******************************************************************************
 *                                 Local Functions Declaring                  *
 ******************************************************************************/

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/
int block_cond_init(int i4_init_value)
{
    int i4_ret = 0;

    i4_ret = pthread_mutex_init(&t_block_ctx.t_mutex, NULL);
    i4_ret |= pthread_cond_init(&t_block_ctx.t_cond, NULL);

    t_block_ctx.i4_value = i4_init_value;

    if (i4_ret)
    {
        BLOCK_DBG("Error return %d\n", i4_ret);
    }

    return i4_ret;
}

int block_cond_destroy(void)
{
    int i4_ret = 0;

    i4_ret = pthread_cond_destroy(&t_block_ctx.t_cond);
    i4_ret |= pthread_mutex_destroy(&t_block_ctx.t_mutex);
    t_block_ctx.i4_value = 0;

    if (i4_ret)
    {
        BLOCK_DBG("Error return %d\n", i4_ret);
    }

    return i4_ret;
}

int trigger_block_entry(int i4_waiting_value)
{
    int i4_ret = 0;

    i4_ret = pthread_mutex_lock(&t_block_ctx.t_mutex);

    t_block_ctx.i4_value |= i4_waiting_value;

    i4_ret |= pthread_cond_signal(&t_block_ctx.t_cond);

    i4_ret |= pthread_mutex_unlock(&t_block_ctx.t_mutex);

    if (i4_ret)
    {
        BLOCK_DBG("Error return %d\n", i4_ret);
    }

    return i4_ret;
}


int trigger_block_exit(int i4_waiting_value)
{
    int i4_ret = 0;

    i4_ret = pthread_mutex_lock(&t_block_ctx.t_mutex);

    t_block_ctx.i4_value &= (~i4_waiting_value);

    //i4_ret |= pthread_cond_signal(&t_block_ctx.t_cond);
    i4_ret |= pthread_cond_broadcast(&t_block_ctx.t_cond);

    i4_ret |= pthread_mutex_unlock(&t_block_ctx.t_mutex);

    if (i4_ret)
    {
        BLOCK_DBG("Error return %d\n", i4_ret);
    }

    return i4_ret;
}

int wait_block_out(int i4_waiting_value)
{
    int i4_ret = 0;

    i4_ret = pthread_mutex_lock(&t_block_ctx.t_mutex);

    if (i4_waiting_value == 0)
    {
        while (t_block_ctx.i4_value != 0)
        {
            i4_ret |= pthread_cond_wait(&t_block_ctx.t_cond, &t_block_ctx.t_mutex);
        }
    }
    else
    {
        while ((t_block_ctx.i4_value & i4_waiting_value) != 0) //等多个bit的话
        {
            i4_ret |= pthread_cond_wait(&t_block_ctx.t_cond, &t_block_ctx.t_mutex);
        }
    }

    i4_ret |= pthread_mutex_unlock(&t_block_ctx.t_mutex);

    if (i4_ret)
    {
        BLOCK_DBG("Error return %d\n", i4_ret);
    }

    return i4_ret;
}



int get_block_value(void)
{
    return t_block_ctx.i4_value;
}

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

/* EOF */

