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

  LIM log test code.

  REVISION:

    1) initial creation. ----------------------------------------- 2010-04-15 MG
*/

#include "stdio.h"
#include "stdlib.h"
#include "limlog.h"
#include "limutil.h"

static void my_log (void * arg, int level, void * log, int bytes)
{
    switch (level)
    {
    case LIM_LOG_FATAL :
    case LIM_LOG_ERROR :
    case LIM_LOG_WARNING :
        printf("%s", (char *)log);

    default :
        break;
    }
}

int main (int argc, char ** argv)
{
    int ii;

    lim_log_init(1024 * 4, LIM_LOG_VERBOSE, my_log, NULL);

    LIMINFO("test begins.\n");
    LIMDBG("entry 11\n");
    for (ii = 0; ii < 100; ii++)
    {
        LIMDBG("running entry %d\n", ii);
    }

    for (ii = 0; ii < 100; ii++)
    {
        LIMDBG("repeating this message.\n");
    }

    LIMWARN("about to disable internal log buffer.\n");
    //reinitialize to disable internal log buffer.
    //lim_log_init(0, LIM_LOG_VERBOSE, my_log, NULL);

    LIMFATAL("this is a fatal error.\n");
    printf("good bye.\n");
    return 0;
}
