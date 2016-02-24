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

  OpenMAX AL list and queue implementation.

  REVISION:

    3) added in set support. ------------------------------------- 2011-07-05 MG
    2) buffer hold and buffer reference count support. ----------- 2010-03-06 MG
    1) initial creation. ----------------------------------------- 2009-08-29 MG
*/

#include "limlog.h"
#include "limmemory.h"
#include "limqlist.h"

#undef LIMDBG
#ifndef LIMDBG
#define LIMDBG(fmt, arg...)
#endif


#define LIMQ_HANDLE_MAGIC 0x33579bd1
#define LIMQ_OBJECT_MAGIC 0x1db97533

typedef struct
{
    uint32_t magic; //limq object magic number
    int8_t ref_count; //limq object reference counter
    int8_t hold_count; //limq object hold counter
    int8_t put_request; //limq object put request, once this is set
                        //queue pointer will be advanced as soon as
                        //reference counters go down to zero
} limq_obj_internal_t;

typedef struct
{
    list_t list;
    void * obj;
} limq_list_internal_t;

typedef struct
{
    lim_mem_pool_t pool;
    limq_compare_cb_t compare;
    limq_list_internal_t * list_entries; //internal list entries
    list_t hold_list_head; //being-held entries list head
    list_t free_list_head; //free entries list head
    list_t deq_list_head; //dequeueable entries list head
    list_t enq_list_head; //enqueueable entries list head
    //int entry_bytes; //user requested entry bytes
    int actual_entry_bytes; //actually allocated user entry bytes
    int max_entries; //maximum allowed entries in queue
    int magic; //limq handle magic number
    char name[LIMQ_NAME_MAX_BYTES];
} limq_handle_internal_t;

static int enqueue_put_release (
    limq_handle_t que,
    const void * obj,
    int put,
    int force_top)
{
    limq_handle_internal_t * hdl;
    limq_obj_internal_t * obji;
    list_t * list;
    limq_list_internal_t * qlist;
    int ret = 1;

    ASSERT(que && obj);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    qlist = (limq_list_internal_t *)hdl->enq_list_head.next;
    ASSERT(&qlist->list != &hdl->enq_list_head);

    obji = (void *)obj + hdl->actual_entry_bytes;

    LIMVERBOSE("%s queue object %p info:\n"
               "\tmagic       : %x %x\n"
               "\tref_count   : %d\n"
               "\thold_count  : %d\n"
               "\tput_request : %d\n", hdl->name, obj,
               obji->magic, LIMQ_OBJECT_MAGIC,
               obji->ref_count, obji->hold_count,
               obji->put_request);
    ASSERT(obji->magic == LIMQ_OBJECT_MAGIC
           && obji->ref_count < 0
           && obji->hold_count == 0
		   && qlist->obj == obj);

    obji->ref_count++;
    if (put && obji->ref_count == 0)
    {
        list_del_init(&qlist->list);
        list = &hdl->deq_list_head;
        if (force_top)
        {
            list_add(&qlist->list, list);
        } else
        {
            if (hdl->compare)
            {
                list = list->next;
                while (list != &hdl->deq_list_head)
                {
                    if (hdl->compare(obj, ((limq_list_internal_t *)list)->obj)
                        > 0) break;
                    else list = list->next;
                }
            }
            list_add_tail(&qlist->list, list);
        }
        ret = 0;
    }

    LIMDBG("%s list current entries after enqueue: %d\n",
           hdl->name, limq_entries(que));

    return ret;
}

static int dequeue_put_release (
    limq_handle_t que,
    const void * obj,
    int put)
{
    limq_handle_internal_t * hdl;
    limq_obj_internal_t * obji;
    limq_list_internal_t * qlist;
    list_t * list;
    int from_hold_list = 0;
    int ret = 1;

    ASSERT(que && obj);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    //search dequeue list first, it is entirely possible that
    //requested object is IN dequeue list and NOT on head, since
    //client may have requested to queue onto head before
    //dequeue_put gets called.
    qlist = NULL;
    list = hdl->deq_list_head.next;
    while (list != &hdl->deq_list_head)
    {
        if (((limq_list_internal_t *)list)->obj == obj)
        {
            qlist = (limq_list_internal_t *)list;
            break;
        }
        else list = list->next;
    }

    if (NULL == qlist)
    {
        //search hold list if this is not on dequeue list head.
        list = hdl->hold_list_head.next;
        while (list != &hdl->hold_list_head)
        {
            if (((limq_list_internal_t *)list)->obj == obj)
            {
                qlist = (limq_list_internal_t *)list;
                from_hold_list = 1;
                break;
            } else list = list->next;
        }
    }

    obji = (void *)obj + hdl->actual_entry_bytes;
    LIMVERBOSE("%s queue object %p info:\n"
               "\tmagic       : %x %x\n"
               "\tref_count   : %d\n"
               "\thold_count  : %d\n"
               "\tput_request : %d\n"
               "\tqlist       : %p\n", hdl->name, obj,
               obji->magic, LIMQ_OBJECT_MAGIC,
               obji->ref_count, obji->hold_count,
               obji->put_request, qlist);
    ASSERT(obji->magic == LIMQ_OBJECT_MAGIC
           && (obji->ref_count > 0 || obji->hold_count > 0)
		   && qlist);

    if (put) obji->put_request = 1;
    if (obji->ref_count) obji->ref_count--;
    else
    {
        obji->hold_count--;

        //if hold count drops down to zero here, and it was NOT
        //from hold list, the object must have been in both
        //deq_list and hold_list, locate it in the hold_list
        //here and put it back to free_list.
        //
        //the following logic will then,
        // - remove it from deq_list and put it back to enq_list
        //   if requested to 'put' back.
        // - do nothing and leave item in deq_list if requested
        //   to just 'release' item.
        if (obji->hold_count == 0 && !from_hold_list)
        {
            //find item from hold list and deference hold list
            list_t * list = hdl->hold_list_head.next;
            limq_list_internal_t * qlist2 = NULL;

            LIMDBG("%s search in hold list for obj: %p\n", hdl->name, obj);
            while (list != &hdl->hold_list_head)
            {
                if (((limq_list_internal_t *)list)->obj == obj)
                {
                    qlist2 = (limq_list_internal_t *)list;
                    break;
                } else list = list->next;
            }

            ASSERT(qlist2);
            list_del_init(&qlist2->list);
            list_add_tail(&qlist2->list, &hdl->free_list_head);
        }
    }

    if (obji->ref_count == 0 && obji->hold_count == 0)
    {
        if (obji->put_request)
        {
            obji->put_request = 0;
            list_del_init(&qlist->list);
            list_add_tail(&qlist->list, &hdl->enq_list_head);
            ret = 0;
        }
    }
    else if (obji->ref_count == 0 && obji->hold_count)
    {
        if (put && !from_hold_list)
        {
            qlist->obj = NULL;
            list_del_init(&qlist->list);
            list_add_tail(&qlist->list, &hdl->free_list_head);
        }
    } else
    {
        LIMDBG("%s object ref_count: %d hold_count: %d\n",
               hdl->name, obji->ref_count, obji->hold_count);
    }

    LIMDBG("%s list current entries after dequeue: %d\n",
           hdl->name, limq_entries(que));

    return ret;
}

limq_handle_t limq_create_ext (
    int entry_bytes,
    int max_entries,
    const char * name,
    limq_compare_cb_t compare)
{
    limq_handle_internal_t * hdl;
    int total_entry_bytes;
    int actual_entry_bytes;
    int actual_obj_bytes;
    int ii;
    limq_list_internal_t * qlist;
    void * ptr;

    hdl = (limq_handle_internal_t *)malloc(sizeof(limq_handle_internal_t));
    if (NULL == hdl)
    {
        LIMERR("unable to allocated queue handle.\n");
        return NULL;
    }

    actual_entry_bytes = OBJECT_MEMORY_ALIGNED_BYTES(entry_bytes);
    actual_obj_bytes = OBJECT_MEMORY_ALIGNED_BYTES(sizeof(limq_obj_internal_t));
    total_entry_bytes = max_entries * (actual_entry_bytes + actual_obj_bytes);

    hdl->pool = lim_mem_pool_create(total_entry_bytes);
    if (NULL == hdl->pool)
    {
        LIMERR("unable to allocated queue memory pool.\n");
        free(hdl);
        return NULL;
    }

    //construct circular buffer list.
    hdl->list_entries = (limq_list_internal_t *)calloc
        (1, sizeof(limq_list_internal_t) * max_entries * 2);
    if (NULL == hdl->list_entries)
    {
        lim_mem_pool_destroy(hdl->pool);
        free(hdl);
        LIMERR("unable to allocated queue sort list entries.\n");
        return NULL;
    }

    list_init(&hdl->free_list_head);
    list_init(&hdl->deq_list_head);
    list_init(&hdl->enq_list_head);
    list_init(&hdl->hold_list_head);

    qlist = hdl->list_entries;
    ptr = lim_mem_pool_get_buf(hdl->pool, total_entry_bytes);
    for (ii = 0; ii < max_entries; ii++)
    {
        limq_obj_internal_t * obji = ptr + actual_entry_bytes;
        obji->magic = LIMQ_OBJECT_MAGIC;
        obji->ref_count = 0;
        obji->hold_count = 0;
        obji->put_request = 0;
        qlist->obj = ptr;
        list_init(&qlist->list);
        list_add(&qlist->list, &hdl->enq_list_head);
        qlist++;
        ptr += actual_entry_bytes + actual_obj_bytes;
    }

    //construct free buffer list.
    for (ii = 0; ii < max_entries; ii++)
    {
        qlist->obj = NULL;
        list_init(&qlist->list);
        list_add(&qlist->list, &hdl->free_list_head);
        qlist++;
    }

    //finalizing queue controls.
    hdl->compare = compare;
    //hdl->entry_bytes = entry_bytes;
    hdl->actual_entry_bytes = actual_entry_bytes;
    hdl->max_entries = max_entries;
    hdl->magic = LIMQ_HANDLE_MAGIC;
    if (name)
    {
        strncpy(hdl->name, name, LIMQ_NAME_MAX_BYTES - 1);
        hdl->name[LIMQ_NAME_MAX_BYTES - 1] = 0;
    } else
    {
        strcpy(hdl->name, "unknown");
    }
    return (limq_handle_t)hdl;
}

void limq_reset (limq_handle_t que)
{
    limq_handle_internal_t * hdl;
    list_t * list;
    list_t * next;

    ASSERT(que);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    list = hdl->deq_list_head.next;
    while (list != &hdl->deq_list_head)
    {
        limq_obj_internal_t * obji;
        limq_list_internal_t * qlist;

        qlist = (limq_list_internal_t *)list;
        obji = qlist->obj + hdl->actual_entry_bytes;
        obji->ref_count = 0;
        obji->hold_count = 0;
        obji->put_request = 0;

        next = list->next;
        list_del_init(list);
        list_add(list, &hdl->enq_list_head);
        list = next;
    }

    list = hdl->hold_list_head.next;
    while (list != &hdl->hold_list_head)
    {
        limq_obj_internal_t * obji;
        limq_list_internal_t * qlist;

        qlist = (limq_list_internal_t *)list;
        obji = qlist->obj + hdl->actual_entry_bytes;

        next = list->next;
        list_del_init(list);
        if (obji->hold_count)
        {
            obji->hold_count = 0;
            list_add(list, &hdl->enq_list_head);
        } else
        {
            qlist->obj = NULL;
            list_add(list, &hdl->free_list_head);
        }
        obji->ref_count = 0;
        obji->put_request = 0;

        list = next;
    }

    list = hdl->enq_list_head.next;
    while (list != &hdl->enq_list_head)
    {
        limq_obj_internal_t * obji;
        limq_list_internal_t * qlist;

        qlist = (limq_list_internal_t *)list;
        obji = qlist->obj + hdl->actual_entry_bytes;

        obji->magic = LIMQ_OBJECT_MAGIC;
        obji->ref_count = 0;

        list = list->next;
    }
    ASSERT(list_entries(&hdl->enq_list_head) == hdl->max_entries);
    ASSERT(list_entries(&hdl->free_list_head) == hdl->max_entries);
}

void limq_destroy (limq_handle_t que)
{
    limq_handle_internal_t * hdl;

    ASSERT(que);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    free(hdl->list_entries);

    hdl->magic = ~LIMQ_HANDLE_MAGIC;
    if (hdl->pool) lim_mem_pool_destroy(hdl->pool);
    free(que);
}

void * limq_enqueue_get (limq_handle_t que)
{
    limq_handle_internal_t * hdl;
    limq_obj_internal_t * obji;
    limq_list_internal_t * qlist;

    ASSERT(que);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    if (hdl->enq_list_head.next == &hdl->enq_list_head) return NULL;

    qlist = (limq_list_internal_t *)hdl->enq_list_head.next;

    obji = qlist->obj + hdl->actual_entry_bytes;

    LIMVERBOSE("%s queue object info:\n"
               "\tmagic       : %x %x\n"
               "\tref_count   : %d\n"
               "\thold_count  : %d\n"
               "\tput_request : %d\n", hdl->name,
               obji->magic, LIMQ_OBJECT_MAGIC,
               obji->ref_count, obji->hold_count,
               obji->put_request);
    ASSERT(obji->magic == LIMQ_OBJECT_MAGIC
           && obji->ref_count <= 0
           && obji->hold_count == 0);
    obji->ref_count--;

    return  qlist->obj;
}

int limq_enqueue_release (limq_handle_t que, const void * obj)
{
    return enqueue_put_release(que, obj, 0, 0);
}

int limq_enqueue_put_ext (limq_handle_t que, const void * obj, int force_top)
{
    return enqueue_put_release(que, obj, 1, force_top);
}

void * limq_dequeue_get (limq_handle_t que)
{
    limq_handle_internal_t * hdl;
    limq_obj_internal_t * obji;
    limq_list_internal_t * qlist;

    ASSERT(que);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    if (hdl->deq_list_head.next == &hdl->deq_list_head) return NULL;

    qlist = (limq_list_internal_t *)hdl->deq_list_head.next;

    obji = qlist->obj + hdl->actual_entry_bytes;

    LIMVERBOSE("%s queue object %p info:\n"
               "\tmagic       : %x %x\n"
               "\tref_count   : %d\n"
               "\thold_count  : %d\n"
               "\tput_request : %d\n", hdl->name, qlist->obj,
               obji->magic, LIMQ_OBJECT_MAGIC,
               obji->ref_count, obji->hold_count,
               obji->put_request);
    ASSERT(obji->magic == LIMQ_OBJECT_MAGIC && obji->ref_count >= 0);
    obji->ref_count++;
    if (obji->ref_count == 1) obji->put_request = 0;

    return  qlist->obj;
}

int limq_dequeue_hold (limq_handle_t que, const void * obj)
{
    limq_handle_internal_t * hdl;
    limq_obj_internal_t * obji;
    limq_list_internal_t * qlist;
    list_t * list;

    ASSERT(que && obj);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    obji = (void *)obj + hdl->actual_entry_bytes;

    LIMVERBOSE("%s queue object %p info:\n"
               "\tmagic       : %x %x\n"
               "\tref_count   : %d\n"
               "\thold_count  : %d\n"
               "\tput_request : %d\n", hdl->name, obj,
               obji->magic, LIMQ_OBJECT_MAGIC,
               obji->ref_count, obji->hold_count,
               obji->put_request);
    ASSERT(obji->magic == LIMQ_OBJECT_MAGIC
           && (obji->ref_count > 0 || obji->hold_count > 0));

    obji->hold_count++;
    LIMDBG("%s object on-hold: %d and referenced: %d %p.\n",
           hdl->name, obji->hold_count, obji->ref_count, obj);

    if (obji->hold_count > 1) return 0;

    //the very first hold, clone from free list to hold list.
    list = hdl->free_list_head.next;
    ASSERT(list != &hdl->free_list_head);
    list_del_init(list);
    qlist = (limq_list_internal_t *)list;
    qlist->obj = (void *)obj;
    list_add_tail(list, &hdl->hold_list_head);

    LIMDBG("%s list total entries after hold: %d\n",
           hdl->name, limq_entries(que));
    return 0;
}

int limq_dequeue_release (limq_handle_t que, const void * obj)
{
    return dequeue_put_release(que, obj, 0);
}

int limq_dequeue_put (limq_handle_t que, const void * obj)
{
    return dequeue_put_release(que, obj, 1);
}

int limq_entries (limq_handle_t que)
{
    limq_handle_internal_t * hdl;
    int entries = 0;
    list_t * list;

    ASSERT(que);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    list = hdl->hold_list_head.next;
    while (list != &hdl->hold_list_head)
    {
        entries++;
        list = list->next;
    }
    LIMDBG("%s dequeue list hold entries: %d\n", hdl->name, entries);

    list = hdl->deq_list_head.next;
    while (list != &hdl->deq_list_head)
    {
        limq_obj_internal_t * obji;
        limq_list_internal_t * qlist;

        qlist = (limq_list_internal_t *)list;
        obji = qlist->obj + hdl->actual_entry_bytes;

        if (obji->hold_count == 0) entries++;

        list = list->next;
    }

    LIMDBG("%s dequeue list total entries: %d\n", hdl->name, entries);
    return entries;
}

int limq_obj_ref_count (limq_handle_t que, const void * obj)
{
    limq_handle_internal_t * hdl;
    limq_obj_internal_t * obji;

    ASSERT(que && obj);

    hdl = (limq_handle_internal_t *)que;
    ASSERT(hdl->magic == LIMQ_HANDLE_MAGIC);

    obji = (void *)obj + hdl->actual_entry_bytes;
    ASSERT(obji->magic == LIMQ_OBJECT_MAGIC);

    return obji->ref_count + obji->hold_count;
}

//------------------------------------------------------------------------------
typedef struct
{
    list_t list;
    void * obj;
} limset_list_internal_t;

typedef struct
{
    lim_mem_pool_t pool;
    limset_list_internal_t * list_entries; //internal list entries
    list_t read_list_head; //readable entries list head
    list_t write_list_head; //writable entries list head
    int max_entries; //maximum allowed entries in queue
    char name[LIMSET_NAME_MAX_BYTES];
    int entries;
} limset_handle_internal_t;

limset_handle_t limset_create (
    int entry_bytes,
    int max_entries,
    const char * name)
{
    limset_handle_internal_t * hdl;
    int total_entry_bytes;
    int actual_entry_bytes;
    int ii;
    limset_list_internal_t * slist;
    void * ptr;

    hdl = malloc(sizeof(limset_handle_internal_t));
    if (NULL == hdl)
    {
        LIMERR("unable to allocated set handle.\n");
        return NULL;
    }

    actual_entry_bytes = OBJECT_MEMORY_ALIGNED_BYTES(entry_bytes);
    total_entry_bytes = max_entries * actual_entry_bytes;

    hdl->pool = lim_mem_pool_create(total_entry_bytes);
    if (NULL == hdl->pool)
    {
        LIMERR("unable to allocated set memory pool.\n");
        free(hdl);
        return NULL;
    }

    //construct circular buffer list.
    hdl->list_entries = calloc
        (1, sizeof(limset_list_internal_t) * max_entries);
    if (NULL == hdl->list_entries)
    {
        lim_mem_pool_destroy(hdl->pool);
        free(hdl);
        LIMERR("unable to allocated set list entries.\n");
        return NULL;
    }

    list_init(&hdl->read_list_head);
    list_init(&hdl->write_list_head);

    slist = hdl->list_entries;
    ptr = lim_mem_pool_get_buf(hdl->pool, total_entry_bytes);
    for (ii = 0; ii < max_entries; ii++)
    {
        slist->obj = ptr;
        list_init(&slist->list);
        list_add(&slist->list, &hdl->write_list_head);
        slist++;
        ptr += actual_entry_bytes;
    }

    //finalizing set controls.
    hdl->max_entries = max_entries;
    hdl->entries = 0;
    if (name)
    {
        strncpy(hdl->name, name, LIMSET_NAME_MAX_BYTES - 1);
        hdl->name[LIMSET_NAME_MAX_BYTES - 1] = 0;
    } else
    {
        strcpy(hdl->name, "unknown");
    }
    return (limset_handle_t)hdl;
}

void limset_destroy (limset_handle_t set)
{
    limset_handle_internal_t * hdl;

    ASSERT(set);

    hdl = (limset_handle_internal_t *)set;
    free(hdl->list_entries);
    if (hdl->pool) lim_mem_pool_destroy(hdl->pool);
    free(set);
}

void * limset_next (limset_handle_t set, void * obj)
{
    limset_handle_internal_t * hdl;
    list_t * list;

    ASSERT(set);

    hdl = (limset_handle_internal_t *)set;

    if (!hdl->entries) return NULL;

    list = hdl->read_list_head.next;
    if (obj)
    {
        while (obj != ((limset_list_internal_t *)list)->obj)
        {
            list = list->next;
            if (list == &hdl->read_list_head)
            {
                LIMERR("%s unable to find object: %p\n", hdl->name, obj);
                return NULL;
            }
        }

        list = list->next;
        if (list == &hdl->read_list_head) return NULL;
    }

    return  ((limset_list_internal_t *)list)->obj;
}

int limset_consume (limset_handle_t set, const void * obj)
{
    limset_handle_internal_t * hdl;
    list_t * list;
    int ii;

    ASSERT(set);

    hdl = (limset_handle_internal_t *)set;

    ii = hdl->entries;
    list = hdl->read_list_head.next;
    while (((limset_list_internal_t *)list)->obj != obj && ii--) list = list->next;

    if (((limset_list_internal_t *)list)->obj != obj)
    {
        LIMERR("%s invalid object: %p\n", hdl->name, obj);
        return -1;
    }

    list_del_init(list);
    list_add(list, &hdl->write_list_head);

    hdl->entries--;

    return 0;
}

void * limset_new (limset_handle_t set)
{
    limset_handle_internal_t * hdl;
    limset_list_internal_t * slist;

    ASSERT(set);

    hdl = (limset_handle_internal_t *)set;

    if (hdl->entries == hdl->max_entries) return NULL;

    slist = (limset_list_internal_t *)hdl->write_list_head.next;

    return  slist->obj;
}

int limset_commit (limset_handle_t set, const void * obj)
{
    limset_handle_internal_t * hdl;
    limset_list_internal_t * slist;

    ASSERT(set);

    hdl = (limset_handle_internal_t *)set;

    slist = (limset_list_internal_t *)hdl->write_list_head.next;
    if (slist->obj != obj)
    {
        LIMERR("%s unable to commit object: %p\n", hdl->name, obj);
        return -1;
    }

    list_del_init(&slist->list);
    list_add_tail(&slist->list, &hdl->read_list_head);

    hdl->entries++;

    return 0;
}

int limset_entries (limset_handle_t set)
{
    ASSERT(set);
    return ((limset_handle_internal_t *)set)->entries;
}
