/*
  Copyright (C) 2010 SWOAG Technology <www.swoag.com>

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

  LIM time helper module header.

  REVISION:

    1) initial creation. ----------------------------------------- 2010-03-02 MG
*/

/** @defgroup time time helper
 * @ingroup limutil
 * @{
 */

#ifndef LIMTIME_H__
#define LIMTIME_H__

#include <sys/time.h>
#include <time.h>

typedef struct timeval ltime_val_t;
#define ltime_gettimeofday(tv) gettimeofday(tv, NULL)

/** convert milisecond to struct timespec value.
 *
 */
void ltime_ms2timespec (int ms, struct timespec * ts);


/** convert time value to microsecond.
 *
 */
//int64_t ltime_tv2us (ltime_val_t * tv);
#define ltime_tv2us(tv) ((int64_t)((tv)->tv_sec) * 1000000 + (tv)->tv_usec)


/** convert time value to millisecond.
 *
 */
#define ltime_tv2ms(tv) (ltime_tv2us(tv) / 1000)

/** @} */
#endif /* LIMTIME_H__ */
