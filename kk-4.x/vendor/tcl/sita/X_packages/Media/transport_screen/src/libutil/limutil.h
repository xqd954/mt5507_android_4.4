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

  LIM utility library header.

  REVISION:

    1) initial creation. ----------------------------------------- 2010-03-02 MG
*/

/** @mainpage
 *
 * As an open source implementation of Khronos OpenMAX application and
 * integration layer, LIM project comes with the following parts,
 *
 * @subpage limoa "LIM openmax application layer"
 *
 * OpenMAX Application Layer implementation, where underlying system is built
 * with OpenMAX IL components, currently uses LIM IL components, but
 * should work with IL components from any other vendor as well.
 *
 * @subpage limoi_core "LIM openmax integration layer core"
 *
 * OpenMAX Integration Layer API implementation and component loader, currently
 * it loads LIM IL components, but should work with IL components from any other
 * vendor as well.
 *
 * @subpage limoi_base "LIM openmax integration layer base"
 *
 * LIM OpenMAX IL component base class, where all LIM IL components are built
 * on top of.
 *
 * @subpage limoi_components "LIM openmax integration layer components"
 *
 * LIM OpenMAX IL components.
 *
 * @subpage limoi_plugins "LIM openmax integration layer plugins"
 *
 * LIM OpenMAX IL core resource manager and loader plugins
 *
 * @subpage limutil "LIM utility library"
 *
 * utility library that has memory pool, thread wrapper and various debug
 * log support.
 *
 * <br> <B>how each part works with/relates to each other</B>
 *    - limoa, limoi-core, limoi-base, limoi-components and limoi-plugins
 *      all depend on limutil.
 *    - limoa uses limoi-core (for IL APIs)
 *    - limoi-core has no other dependency
 *    - limoi-base uses OMX headers and does not depend on limoi-core
 *    - limoi-components depend on limoi-base
 *    - limoi-plugins extend limoi-core component loading capability, and
 *      provide resource manager implementations.
 *
 * @defgroup limoa LIM openmax application layer
 * @defgroup limoi_core LIM integration layer core
 * @defgroup limoi_base LIM integration layer base
 * @defgroup limoi_plugins LIM integration layer core plugins
 * @defgroup limoi_components LIM integration layer components
 * @defgroup limutil LIM utility library
 *
 */
#ifndef LIMUTIL_H__
#define LIMUTIL_H__

#include "limlog.h"
#include "limtime.h"
#include "limthread.h"
#include "limqlist.h"
#include "limmemory.h"
#include "typedef.h"

#define ALOGI(...) LOGI(...)
#define ALOGD(...) LOGD(...)
#define ALOGE(...) LOGE(...)
#endif /* LIMUTIL_H__ */
