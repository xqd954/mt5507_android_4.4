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

  OpenMAX AL memory management module.

  REVISION:

    1) initial creation. ----------------------------------------- 2009-08-26 MG
*/

#include "limlog.h"
#include "limthread.h"
#include "limmemory.h"

#undef LIMDBG
#ifndef LIMDBG
#define LIMDBG(fmt, arg...)
#endif

#define LIM_MEM_ENTRY_FREE_  0x19175081
#define LIM_MEM_ENTRY_TAKEN_ 0x18057191

#define LIM_MEM_POOL_MAGIC   0x33579bd3

typedef struct lim_mem_entry_t
{
    struct lim_mem_entry_t * prev;
    struct lim_mem_entry_t * next;
    int bytes;
    int ref_count;
    int free;
} lim_mem_entry_t;

#define MINIMUM_LIM_MEM_SLOT_ 32
#define BYTES_(b) (((b) + MINIMUM_LIM_MEM_SLOT_ - 1) & (~(MINIMUM_LIM_MEM_SLOT_ - 1)))

typedef struct lim_mem_pool_internal_t
{
    lthread_mutex_t mutex;
    void * dptr;
    int max_free_bytes; //-1: unknown
    int pool_bytes;
    struct lim_mem_pool_internal_t * next;
    int magic;
} lim_mem_pool_internal_t;

static lim_mem_pool_internal_t * pool_init (int bytes)
{
    lim_mem_pool_internal_t * hdl;
    int actual_bytes;
    lim_mem_entry_t * mem;

    hdl = (lim_mem_pool_internal_t *)malloc(sizeof(lim_mem_pool_internal_t));
    if (NULL == hdl)
    {
        LIMERR("unable to allocate memory handle\n");
        return NULL;
    }

    actual_bytes = BYTES_(bytes);
    actual_bytes += sizeof(lim_mem_entry_t);
    hdl->dptr = malloc(actual_bytes);
    if (NULL == hdl->dptr)
    {
        LIMERR("unable to allocate memory bytes: %d.\n", actual_bytes);
        free(hdl);
        return NULL;
    }

    hdl->pool_bytes = actual_bytes - sizeof(lim_mem_entry_t);
    hdl->max_free_bytes = hdl->pool_bytes;
    hdl->next = NULL;
    lthread_mutex_init(&hdl->mutex);

    mem = (lim_mem_entry_t *)hdl->dptr;
    mem->bytes = hdl->pool_bytes;
    mem->free = LIM_MEM_ENTRY_FREE_;
    mem->prev = NULL;
    mem->next = NULL;
    mem->ref_count = 0;
    LIMDBG("memory pool initialized as:\n"
           "\tmax_free_bytes : %d\n"
           "\tdptr           : %p\n",
           hdl->max_free_bytes,
           hdl->dptr);

    hdl->magic = LIM_MEM_POOL_MAGIC;
    return hdl;
}

static void * allocate_from_pool (lim_mem_pool_internal_t * hdl, int bytes)
{
    lim_mem_entry_t * mem;
    lim_mem_entry_t * next_mem;
    int actual_bytes = BYTES_(bytes);
    void * dptr = NULL;

    lthread_mutex_lock(&hdl->mutex);
    if (hdl->max_free_bytes < actual_bytes
        && -1 != hdl->max_free_bytes) goto bail;

    mem = (lim_mem_entry_t *)hdl->dptr;
    while (mem)
    {
        LIMDBG("memory entry %p info\n"
               "\tprev       : %p\n"
               "\tnext       : %p\n"
               "\tref_count  : %d\n"
               "\tbytes      : %d | %d\n"
               "\tfree       : %x | %x\n", mem,
               mem->prev, mem->next, mem->ref_count,
               mem->bytes, actual_bytes,
               mem->free, LIM_MEM_ENTRY_FREE_);

        if (mem->free == LIM_MEM_ENTRY_TAKEN_
            || (mem->free == LIM_MEM_ENTRY_FREE_ && mem->bytes < actual_bytes))
        {
            mem = mem->next;
            continue;
        } else if (mem->free != LIM_MEM_ENTRY_FREE_)
        {
            LIMERR("memory corrupted, unrecoverable error.\n");
            // no point to continue
            break;
        }

        // if this slot was the max, max becomes unknow afterwards.
        if (mem->bytes == hdl->max_free_bytes) hdl->max_free_bytes = -1;

        // memory slot is free and has the bytes needed.
        mem->free = LIM_MEM_ENTRY_TAKEN_;
        mem->ref_count = 1;
        dptr = (void *)mem + sizeof(lim_mem_entry_t);

        // split entry only if it is big enough
        ASSERT(mem->bytes >= actual_bytes);
        if (mem->bytes >
            actual_bytes + sizeof(lim_mem_entry_t) + MINIMUM_LIM_MEM_SLOT_)
        {
            next_mem = (lim_mem_entry_t *)((void *)dptr + actual_bytes);
            next_mem->prev = mem;
            next_mem->next = mem->next;
            next_mem->free = LIM_MEM_ENTRY_FREE_;
            next_mem->bytes = mem->bytes - actual_bytes - sizeof(lim_mem_entry_t);

            if (mem->next) mem->next->prev = next_mem;

            mem->bytes = actual_bytes;
            mem->next = next_mem;
        }
        break;
    }

 bail:
    lthread_mutex_unlock(&hdl->mutex);
    return dptr;
}

lim_mem_pool_t lim_mem_pool_create (int bytes)
{
    return (lim_mem_pool_t)pool_init(bytes);
}

void * lim_mem_pool_get_buf (lim_mem_pool_t pool, int bytes)
{
    lim_mem_pool_internal_t * hdl;
    lim_mem_pool_internal_t * cur_hdl;
    lim_mem_pool_internal_t * next_hdl;
    void * dptr;
    int pool_bytes;

    if (NULL == pool) return NULL;

    hdl = (lim_mem_pool_internal_t *)pool;
    ASSERT(hdl->magic == LIM_MEM_POOL_MAGIC);

    pool_bytes = hdl->pool_bytes;
    do
    {
        dptr = allocate_from_pool(hdl, bytes);
        if (dptr) return dptr;

        cur_hdl = hdl;
        lthread_mutex_lock(&cur_hdl->mutex);
        hdl = hdl->next;
        if (hdl) lthread_mutex_unlock(&cur_hdl->mutex);
        else break;
    } while (1);

    LIMDBG("existing memory pool depleted, allocating more.\n");
    next_hdl = pool_init((pool_bytes > bytes)? pool_bytes : bytes);
    if (NULL == next_hdl)
    {
        lthread_mutex_unlock(&cur_hdl->mutex);
        return NULL;
    }

    cur_hdl->next = next_hdl;
    lthread_mutex_unlock(&cur_hdl->mutex);

    return allocate_from_pool(next_hdl, bytes);
}

int lim_mem_pool_put_buf (lim_mem_pool_t pool, void * ptr)
{
    lim_mem_entry_t * mem;
    lim_mem_entry_t * next_mem;
    lim_mem_entry_t * prev_mem;
    lim_mem_pool_internal_t * hdl;
    lim_mem_pool_internal_t * cur_hdl;
    int ref_count = -1;

    if (NULL == pool || NULL == ptr) return -1;

    hdl = (lim_mem_pool_internal_t *)pool;
    ASSERT(hdl->magic == LIM_MEM_POOL_MAGIC);

    do
    {
        cur_hdl = hdl;
        lthread_mutex_lock(&cur_hdl->mutex);

        if (ptr > hdl->dptr && ptr < hdl->dptr + hdl->pool_bytes) break;

        hdl = hdl->next;
        if (!hdl) break;

        lthread_mutex_unlock(&cur_hdl->mutex);
    } while(1);

    if (NULL == hdl)
    {
        LIMWARN("unable to find requested memory pool.\n");
        goto bail;
    }

    mem = (lim_mem_entry_t *)hdl->dptr;
    while (mem)
    {
        if (mem->free == LIM_MEM_ENTRY_TAKEN_
            && ((void *)mem + sizeof(lim_mem_entry_t)) == ptr)
        {
            ref_count = --mem->ref_count;
            if (ref_count) goto bail;

            mem->free = LIM_MEM_ENTRY_FREE_;
            break;
        }
        mem = mem->next;
    }

    if (NULL == mem)
    {
        LIMWARN("unable to find requested memory.\n");
        goto bail;
    }

    next_mem = mem->next;
    prev_mem = mem->prev;

    if (next_mem)
    {
        if (next_mem->free == LIM_MEM_ENTRY_FREE_)
        {
            mem->bytes += next_mem->bytes + sizeof(lim_mem_entry_t);
            if (next_mem->next) next_mem->next->prev = mem;
            mem->next = next_mem->next;
        }
    }

    if (prev_mem)
    {
        if (prev_mem->free == LIM_MEM_ENTRY_FREE_)
        {
            prev_mem->bytes += mem->bytes + sizeof(lim_mem_entry_t);
            prev_mem->next = mem->next;
            if (mem->next) mem->next->prev = prev_mem;
        }
    }

    // reset max free bytes
    mem = (lim_mem_entry_t *)hdl->dptr;
    while (mem)
    {
        if (mem->free == LIM_MEM_ENTRY_FREE_
            && hdl->max_free_bytes < mem->bytes)
        {
            hdl->max_free_bytes = mem->bytes;
        }
        mem = mem->next;
    }
    LIMDBG("current max_free_bytes : %d\n", hdl->max_free_bytes);

 bail:
    lthread_mutex_unlock(&cur_hdl->mutex);
    return ref_count;
}

int lim_mem_pool_hold_buf (lim_mem_pool_t pool, void * ptr)
{
    lim_mem_entry_t * mem;
    lim_mem_pool_internal_t * hdl;
    lim_mem_pool_internal_t * cur_hdl;
    int ref_count = -1;

    if (NULL == pool || NULL == ptr) return -1;

    hdl = (lim_mem_pool_internal_t *)pool;
    ASSERT(hdl->magic == LIM_MEM_POOL_MAGIC);

    do
    {
        cur_hdl = hdl;
        lthread_mutex_lock(&cur_hdl->mutex);

        if (ptr > hdl->dptr && ptr < hdl->dptr + hdl->pool_bytes) break;

        hdl = hdl->next;
        if (!hdl) break;

        lthread_mutex_unlock(&cur_hdl->mutex);
    } while(1);

    if (NULL == hdl)
    {
        LIMWARN("unable to find requested memory pool.\n");
        goto bail;
    }

    mem = (lim_mem_entry_t *)hdl->dptr;
    while (mem)
    {
        if (mem->free == LIM_MEM_ENTRY_TAKEN_
            && ((void *)mem + sizeof(lim_mem_entry_t)) == ptr)
        {
            ref_count = ++mem->ref_count;
            goto bail;
        }
        mem = mem->next;
    }
    LIMWARN("unable to find requested memory.\n");

 bail:
    lthread_mutex_unlock(&cur_hdl->mutex);
    return ref_count;
}

void lim_mem_pool_reset (lim_mem_pool_t pool)
{
    lim_mem_entry_t * mem;
    lim_mem_pool_internal_t * next_hdl;
    lim_mem_pool_internal_t * hdl;

    hdl = ((lim_mem_pool_internal_t *)pool)->next;
    while (hdl)
    {
        LIMDBG("reset memory pool: %p\n", hdl);
        ASSERT(hdl->magic == LIM_MEM_POOL_MAGIC);
        lthread_mutex_lock(&hdl->mutex);
        if (hdl->dptr) free(hdl->dptr);
        next_hdl = hdl->next;
        lthread_mutex_unlock(&hdl->mutex);
        if (lthread_mutex_destroy(&hdl->mutex))
        {
            LIMERR("unable to destroy memory pool mutex.\n");
        }
        free(hdl);
        hdl = next_hdl;
    }

    hdl = (lim_mem_pool_internal_t *)pool;
    ASSERT(hdl->magic == LIM_MEM_POOL_MAGIC);

    lthread_mutex_lock(&hdl->mutex);
    mem = (lim_mem_entry_t *)hdl->dptr;
    mem->bytes = hdl->pool_bytes;
    mem->free = LIM_MEM_ENTRY_FREE_;
    mem->prev = NULL;
    mem->next = NULL;
    mem->ref_count = 0;
    hdl->next = NULL;
    lthread_mutex_unlock(&hdl->mutex);
}

void lim_mem_pool_destroy (lim_mem_pool_t pool)
{
    lim_mem_pool_internal_t * next_hdl;
    lim_mem_pool_internal_t * hdl = (lim_mem_pool_internal_t *)pool;

    while (hdl)
    {
        LIMDBG("destroy memory pool: %p\n", hdl);
        ASSERT(hdl->magic == LIM_MEM_POOL_MAGIC);
        lthread_mutex_lock(&hdl->mutex);
        if (hdl->dptr) free(hdl->dptr);
        next_hdl = hdl->next;
        lthread_mutex_unlock(&hdl->mutex);
        if (lthread_mutex_destroy(&hdl->mutex))
        {
            LIMERR("unable to destroy memory pool mutex.\n");
        }
        free(hdl);
        hdl = next_hdl;
    }
}
