/*
  Copyright (C) 2009 - 2010 SWOAG Technology <www.swoag.com>

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

  LIM utility memory module header.

  REVISION:

    1) initial creation. ----------------------------------------- 2009-08-26 MG
*/

/** @defgroup memory memory pool
 * @ingroup limutil
 * @{
 */

#ifndef LIMMEMORY_H__
#define LIMMEMORY_H__

/** calculate object alignment byte count. */
#define OBJECT_MEMORY_ALIGNED_BYTES(bytes) \
    ((((bytes) + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *))

/** opaque lim memory pool handle. */
typedef void * lim_mem_pool_t;


/** create memory pool.
 *
 * @param bytes
 *        number of bytes in pool.
 * @return
 *        memory pool handle if successful, otherwise NULL.
 */
lim_mem_pool_t lim_mem_pool_create (int bytes);


/** destroy memory pool.
 *
 * @param pool
 *        memory pool handle returned by @ref lim_mem_pool_create().
 */
void lim_mem_pool_destroy (lim_mem_pool_t pool);


/** reset memory pool.
 * invalidate all pointers allocated from pool.
 *
 * @param pool
 *        memory pool handle returned by @ref lim_mem_pool_create().
 */
void lim_mem_pool_reset (lim_mem_pool_t pool);


/** get 'bytes' memory from memory pool.
 *
 * @param pool
 *        memory pool handle returned by @ref lim_mem_pool_create().
 * @param bytes
 *        requested bytes.
 * @return
 *        pointer to memory if successful, otherwise NULL.
 */
void * lim_mem_pool_get_buf (lim_mem_pool_t pool, int bytes);


/** pet memory back to memory pool.
 * decrease memory reference counter, and put memory back to pool if
 * reference counter goes down to zero.
 *
 * @param pool
 *        memory pool handle returned by @ref lim_mem_pool_create().
 * @param ptr
 *        memory pointer.
 * @return
 *        current reference counter of ptr, negative value for error.
 */
int lim_mem_pool_put_buf (lim_mem_pool_t pool, void * ptr);


/** hold memory buffer.
 * increase memory reference counter.
 *
 * @param pool
 *        memory pool handle returned by @ref lim_mem_pool_create().
 * @param ptr
 *        memory pointer.
 * @return
 *        current reference counter of ptr, error if value is less than 2.
 */
int lim_mem_pool_hold_buf (lim_mem_pool_t pool, void * ptr);

/** @} */
#endif /* LIMMEMORY_H__ */
