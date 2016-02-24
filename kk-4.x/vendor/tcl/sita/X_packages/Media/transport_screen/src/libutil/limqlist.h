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

  LIM queue list and set support module header.

  REVISION:

    2) added in set support. ------------------------------------- 2011-07-05 MG
    1) initial creation. ----------------------------------------- 2009-08-29 MG
*/

/** @defgroup qlist queue list set
 * @ingroup limutil
 * @{
 * this module implements list, queue and set. 'list' provides
 * doubly-linked list support, 'queue' provides FIFO support with object
 * reference count and object holding support, 'set' provides random access to
 * a given set of objects.
 */

#ifndef LIMQLIST_H__
#define LIMQLIST_H__

typedef struct list_t
{
	struct list_t * next;
    struct list_t * prev;
} list_t;

static inline void list_add_ (list_t * new, list_t * prev, list_t * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_del_ (list_t * prev, list_t * next)
{
	next->prev = prev;
	prev->next = next;
}


/** initialize list.
 *
 * @param list
 *        list head entry.
 */
static inline void list_init (list_t * list)
{
	list->next = list;
	list->prev = list;
}


/** add a new entry afer head
 *
 * @param new
 *        new entry to be added
 * @param head
 *        list head to add it after
 */
static inline void list_add (list_t * new, list_t * head)
{
	list_add_(new, head, head->next);
}


/** add a new entry before head
 *
 * @param new
 *        new entry to be added
 * @param head
 *        list head to add it before
 */
static inline void list_add_tail (list_t * new, list_t * head)
{
    list_add_(new, head->prev, head);
}


/** remove entry from list.
 *
 * @param entry
 *        the element to delete from the list.
 */
static inline void list_del (list_t * entry)
{
	list_del_(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}


/** remove entry from list and reinitialize it.
 *
 * @param entry
 *        the element to delete from the list.
 */
static inline void list_del_init (list_t * entry)
{
	list_del_(entry->prev, entry->next);
	list_init(entry);
}


/** test whether a list is empty.
 *
 * @param head
 *        list head
 * @return
 *        0 if list is not empty, nonzero if it is empty.
 */
static inline int list_empty (const list_t * head)
{
	return head->next == head;
}


/** calculate number of list entries
 *
 * @param head
 *        list head.
 * @return
 *        number of entries in list.
 */
static inline int list_entries (const list_t * head)
{
    int entries = 0;
    list_t * list = head->next;
    while (list != head)
    {
        entries++;
        list = list->next;
    }
    return entries;
}

/** lim queue handle.
 * queue implementation with fix-sized entry, queue is not mutex protected.
 * queue can be created by calling @ref limq_create_ext(), and destroyed or
 * reset by @ref limq_destroy() @ref limq_reset().
 *
 * object is internally allocated and can be fetched either through
 * @ref limq_dequeue_get() or @ref limq_enqueue_get() depending on queue
 * operation.
 *
 * on enqueing, object reference counter goes up at @ref limq_enqueue_get(),
 * and goes down by @ref limq_enqueue_put() or @ref limq_enqueue_release().
 *
 * on dequeing, object reference counter goes up at @ref limq_dequeue_get(),
 * and goes down by @ref limq_dequeue_put() or @ref limq_dequeue_release().
 * Object only gets released when reference counter goes down to zero.
 *
 * @ref limq_dequeue_hold() can hold object returned by @ref limq_dequeue_get(),
 * which can later on be released/dereferenced by @ref limq_dequeue_put() or
 * @ref limq_dequeue_release().
 *
 * @ref limq_dequeue_get() must be paired with either @ref limq_dequeue_put(),
 * or @ref limq_dequeue_release(), and
 * @ref limq_dequeue_hold() must also be paired with
 * either @ref limq_dequeue_put(), or @ref limq_dequeue_release(), and
 * @ref limq_enqueue_get() must be paired with @ref limq_enqueue_put() or
 * @ref limq_enqueue_release().
 *
 * whether queue pointer is advanced or not depends on the call to release
 * the object, once @ref limq_dequeue_put() is called, queue pointer will be
 * advanced as soon as object reference counter goes down to zero. In other
 * words, if object is referenced (or held) by multiple clients, object will be
 * released and queue pointer be advanced if any one of the clients calls
 * @ref limq_dequeue_put() API on that object, when object reference counter
 * goes down to zero.
 */
typedef void * limq_handle_t;

/** limq compare callback type.
 *
 * @param obj1
 *        user data objet
 * @param obj2
 *        user data objet
 * @return
 *        -1 if obj1 is less than obj2
 *         0 if obj1 equals to obj2
 *         1 if obj1 is greater than obj2
 *         the greatest object always gets dequeued first.
 */
typedef int (*limq_compare_cb_t) (const void * obj1, const void * obj2);

/** maximum allowed LIMQ name length in bytes. */
#define LIMQ_NAME_MAX_BYTES 64

/** create lim queue handle.
 *
 * @param entry_bytes
 *        bytes of each entry
 * @param max_entries
 *        maximum allowed queued entries
 * @param name
 *        queue name for debug tracking purpose, can be NULL.
 * @return
 *        queue handle if successful, NULL otherwise.
 */
//limq_handle_t limq_create (int entry_bytes, int max_entries, const char * name);
#define limq_create(entry_bytes, max_entries, name) \
    limq_create_ext(entry_bytes, max_entries, name, NULL)

/** create ordered lim queue handle.
 *
 * @param entry_bytes
 *        bytes of each entry
 * @param max_entries
 *        maximum allowed queued entries
 * @param name
 *        queue name for debug tracking purpose, can be NULL.
 * @param compare
 *        callback compare function, @ref limq_compare_cb_t
 * @return
 *        queue handle if successful, NULL otherwise.
 */
limq_handle_t limq_create_ext (
    int entry_bytes,
    int max_entries,
    const char * name,
    limq_compare_cb_t compare);


/** reset lim queue.
 *
 * @param que
 *        queue handle returned by @ref limq_create();
 */
void limq_reset (limq_handle_t que);


/** destroy lim queue.
 *
 * @param que
 *        queue handle returned by @ref limq_create();
 */
void limq_destroy (limq_handle_t que);


/** get next queue entry that can be written to.
 *
 * @param que
 *        queue handle.
 * @return
 *        queue entry if successful, NULL otherwise.
 */
void * limq_enqueue_get (limq_handle_t que);


/** release writable queue entry without advance queue pointer
 * next call to @ref limq_enqueue_get() would return the same
 * queue object.
 *
 * @param que
 *        queue handle.
 * @param obj
 *        data object pointer returned by @ref limq_enqueue_get().
 * @return
 *        0 if successful and object no longer referenced and
 *          queue pointer is advanced.
 *        1 if successful and object is either still referenced
 *          or queue pointer remains not being advanced.
 *       -1 otherwise.
 */
int limq_enqueue_release (limq_handle_t que, const void * obj);


/** put back writable queue entry and advance queue pointer
 * next call to @ref limq_enqueue_get() would return next
 * writable object in the queue.
 *
 * @param que
 *        queue handle.
 * @param obj
 *        data object pointer returned by @ref limq_enqueue_get().
 * @return
 *        0 if successful and queue pointer advanced
 *        1 if successful but object is still referenced
 *       -1 otherwise.
 */
//int limq_enqueue_put (limq_handle_t que, const void * obj);
#define limq_enqueue_put(que, obj) limq_enqueue_put_ext(que, obj, 0)

/** put back writable queue entry
 *
 * @param que
 *        queue handle.
 * @param obj
 *        data object pointer returned by @ref limq_enqueue_get().
 * @param force_top
 *        if set, entry is forced to be queued to top of the queue.
 * @return
 *        0 if successful and object no longer referenced and
 *          queue pointer is advanced.
 *        1 if successful but object is still referenced.
 *       -1 otherwise.
 */
int limq_enqueue_put_ext (limq_handle_t que, const void * obj, int force_top);


/** get next queue entry that can be read from.
 * get readable entry from queue without advancing queue pointer. Entry must
 * be released either by @ref limq_dequeue_put() or @ref
 * limq_dequeue_release().
 *
 * @param que
 *        queue handle.
 * @return
 *        queue_entry if successful, NULL otherwise.
 */
void * limq_dequeue_get (limq_handle_t que);


/** hold readable queue entry
 * hold readable entry, object must be returned later by
 * @ref limq_dequeue_put() or @ref limq_dequeue_release().
 *
 * @param que
 *        queue handle.
 * @param obj
 *        data object pointer returned by @ref limq_dequeue_get().
 * @return
 *        0 if successful, -1 otherwise.
 */
int limq_dequeue_hold (limq_handle_t que, const void * obj);


/** release readable queue entry
 * dereference and release queue object, if object is previously held by
 * @ref limq_dequeue_hold(), then also advance queue pointer when hold count
 * goes down to zero.
 *
 * @param que
 *        queue handle.
 * @param obj
 *        data object pointer returned by @ref limq_dequeue_get().
 * @return
 *        0 if successful and object no longer referenced and
 *          queue pointer is advanced.
 *        1 if successful but object is either still referenced
 *          or queue pointer remains not being advanced.
 *       -1 otherwise.
 */
int limq_dequeue_release (limq_handle_t que, const void * obj);


/** put back readable queue entry
 * release readable queue entry and advance queue pointer if queue object
 * is no longer referenced.
 *
 * @param que
 *        queue handle.
 * @param obj
 *        data object pointer returned by @ref limq_dequeue_get().
 * @return
 *        0 if successful and object no longer referenced and
 *          queue pointer is advanced.
 *        1 if successful but object is still referenced.
 *       -1 otherwise.
 */
int limq_dequeue_put (limq_handle_t que, const void * obj);


/** query current queue entry number.
 * include entries that either are dequeable or on hold.
 *
 * @param que
 *        queue handle.
 * @return
 *        current number of queue entries.
 */
int limq_entries (limq_handle_t que);


/** query queue object reference count.
 *
 */
int limq_obj_ref_count (limq_handle_t que, const void * obj);


/** lim set handle.
 * set implementation with fix-sized entry, set is not mutex protected. set
 * internally maintains object order in an FIFO manner, where actual object
 * access order depends on client usage.
 *
 * set can be created by calling @ref limset_create(), and destroyed by
 * @ref limset_destroy().
 *
 * object is internally allocated and can be iterated through
 * @ref limset_next() and ref limset_entries(). Object can then be consumed by
 * @ref limset_consume() or simply skipped by doing nothing with it.
 *
 * on writing, empty object slot can be fetched by @ref limset_new(), and
 * commit back by @ref limset_commit()
 */
typedef void * limset_handle_t;


/** maximum allowed LIMSET name length in bytes. */
#define LIMSET_NAME_MAX_BYTES 64


/** create lim set handle.
 *
 * @param entry_bytes
 *        bytes of each entry
 * @param max_entries
 *        maximum allowed set entries
 * @param name
 *        set name for debug tracking purpose, can be NULL.
 * @return
 *        set handle if successful, NULL otherwise.
 */
limset_handle_t limset_create (
    int entry_bytes,
    int max_entries,
    const char * name);


/** destroy lim set.
 *
 * @param set
 *        set handle returned by @ref limset_create();
 */
void limset_destroy (limset_handle_t set);


/** get next set entry that can be read from.
 *
 * @param set
 *        set handle.
 * @param obj
 *        current object, fetch next entry from head if NULL.
 * @return
 *        set entry object if successful, NULL otherwise.
 */
void * limset_next (limset_handle_t set, void * obj);


/** consume and release readable set entry
 *
 * @param set
 *        set handle.
 * @param obj
 *        data object pointer returned by @ref limset_next().
 * @return
 *        0 if successful, -1 otherwise.
 */
int limset_consume (limset_handle_t set, const void * obj);


/** get next set entry that can be written to.
 *
 * @param set
 *        set handle.
 * @return
 *        set entry object if successful, NULL otherwise.
 */
void * limset_new (limset_handle_t set);


/** commit/write object back to set.
 *
 * @param set
 *        set handle.
 * @param obj
 *        data object pointer returned by @ref limset_new().
 * @return
 *        0 if successful, -1 otherwise.
 */
int limset_commit (limset_handle_t set, const void * obj);


/** query current readable set entry number.
 *
 * @param set
 *        set handle.
 * @return
 *        current number of readable set entries.
 */
int limset_entries (limset_handle_t set);


/** @} */
#endif /* LIMQLIST_H__ */
