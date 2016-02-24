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

  LIM qlist test code.

  REVISION:

    1) initial creation. ----------------------------------------- 2010-03-05 MG
*/

#include "stdio.h"
#include "stdlib.h"
#include "limlog.h"
#include "limutil.h"

#define TT_ENTRIES 4

typedef struct {
    int xx;
    void * pp;
} test_t;

static int test_compare(const void * obj1, const void * obj2)
{
    test_t * t1 = (void *)obj1;
    test_t * t2 = (void *)obj2;

    LIMDBG("comparing obj1: %p with obj2: %p\n"
           "\tobj1->xx: %d\n"
           "\tobj2->xx: %d\n",
           obj1, obj2, t1->xx, t2->xx);

    if (t1->xx > t2->xx) return 1;
    else if (t1->xx < t2->xx) return -1;

    return 0;
}

static void my_log_print (const void * arg, int level,
                          const void * log, int bytes)
{
    printf("%s", (char *)log);
}

int main (int argc, char ** argv)
{
    limq_handle_t que;
    limset_handle_t set;
    test_t * tt;
    test_t * tts[TT_ENTRIES];
    int ii;

    //lim_log_set_level(LIM_LOG_DEBUG);
    lim_log_set_level(LIM_LOG_INFO);
    lim_log_set_listener(my_log_print, NULL);

    printf("creating queue.\n");
    que = limq_create_ext(sizeof(test_t), TT_ENTRIES, "TEST_QUEUE", test_compare);
    if (NULL == que)
    {
        LIMERR("unable to create queue object.\n");
        exit(-1);
    }

    //basics
    printf("BASICS: enquing.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tt = limq_enqueue_get(que);
        tt->xx = ii + 1;
        tt->pp = (void *)0xdead0000 + ii + 1;
        limq_enqueue_put(que, tt);
    }
    printf("queue entries: %d\n", limq_entries(que));

    printf("BASICS: dequing.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tt = limq_dequeue_get(que);
        printf("queue object->xx: %d | %p\n", tt->xx, tt->pp);
        limq_dequeue_put(que, tt);
    }
    printf("queue entries: %d\n", limq_entries(que));

    //buffer holding
    printf("BUFFER HOLDING: enquing.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tt = limq_enqueue_get(que);
        tt->xx = ii + 1;
        tt->pp = (void *)0xdead0000 + ii + 1;
        limq_enqueue_put(que, tt);
    }

    printf("BUFFER HOLDING: dequing.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tts[ii] = limq_dequeue_get(que);
        limq_dequeue_hold(que, tts[ii]);
        limq_dequeue_put(que, tts[ii]);
    }
    printf("queue entries: %d\n", limq_entries(que));

    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        limq_dequeue_put(que, tts[ii]);
    }
    printf("queue entries: %d\n", limq_entries(que));

    //random ordered buffer get/hold
    printf("RANDOM BUFFER GET/HOLD: enquing.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tt = limq_enqueue_get(que);
        tt->xx = ii + 1;
        tt->pp = (void *)0xdead0000 + ii + 1;
        limq_enqueue_put(que, tt);
    }

    printf("queue entries: %d\n", limq_entries(que));
    tts[0] = limq_dequeue_get(que);
    tts[1] = limq_dequeue_get(que);
    tts[2] = limq_dequeue_get(que);
    limq_dequeue_put(que, tts[1]);
    limq_dequeue_hold(que, tts[0]);
    limq_dequeue_put(que, tts[2]);
    printf("queue entries: %d\n", limq_entries(que));
    limq_dequeue_put(que, tts[0]);
    printf("queue entries: %d\n", limq_entries(que));

    //error detecting
    printf("ERROR DETECTING: enquing.\n");
    limq_reset(que);
    for (ii = 0; ii < TT_ENTRIES - 2; ii++)
    {
        tt = limq_enqueue_get(que);
        tt->xx = ii + 1;
        tt->pp = (void *)0xdead0000 + ii + 1;
        limq_enqueue_put(que, tt);
    }
    printf("queue entries: %d\n", limq_entries(que));

    //unevened ge/put
#if 0
    printf("ERROR DETECTING: unevened get/put.\n");
    tts[0] = limq_dequeue_get(que);
    limq_dequeue_put(que, tts[0]);
    limq_dequeue_put(que, tts[0]);
    printf("queue entries: %d\n", limq_entries(que));
#endif

    //hold entry that is no longer available or bogus.
#if 0
    printf("ERROR DETECTING: hold bogus entry.\n");
    limq_dequeue_hold(que, tts[0]);
    limq_dequeue_hold(que, tts[3]);
#endif

    //enque bogus object
#if 0
    printf("ERROR DETECTING: enqueue bogus entry.\n");
    limq_enqueue_put(que, tts[3]);
    tt = limq_enqueue_get(que);
    tt->xx = 9;
    limq_enqueue_put(que, tt);
    limq_enqueue_put(que, tt);
#endif

    //bogus dequeue release/put
#if 0
    printf("ERROR DETECTING: bogus dequeue release/put.\n");
    tt = limq_enqueue_get(que);
    limq_dequeue_put(que, tt);

    tt = limq_dequeue_get(que);
    limq_enqueue_put(que, tt);

    tt = limq_enqueue_get(que);
    limq_dequeue_release(que, tt);

    tt = limq_dequeue_get(que);
    limq_enqueue_release(que, tt);
#endif

    printf("reseting queue.\n");
    limq_reset(que);

    printf("destroying queue.\n");
    limq_destroy(que);
    printf("good bye.\n");


    printf("\n\ncreating set.\n");
    set = limset_create(sizeof(test_t), TT_ENTRIES, "TEST_SET");
    if (NULL == set)
    {
        LIMERR("unable to create set object.\n");
        exit(-1);
    }

    //basics
    printf("BASICS: writing to set.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tt = limset_new(set);
        tt->xx = ii + 1;
        tt->pp = (void *)0xdead0000 + ii + 1;
        limset_commit(set, tt);
    }
    printf("set entries: %d\n", limset_entries(set));

    printf("BASICS: reading from set.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tt = limset_next(set, NULL);
        printf("set object->xx: %d | %p\n", tt->xx, tt->pp);
        limset_consume(set, tt);
    }
    printf("set entries: %d\n", limset_entries(set));

    //random ordered buffer get
    printf("RANDOM BUFFER GET: writing.\n");
    for (ii = 0; ii < TT_ENTRIES; ii++)
    {
        tt = limset_new(set);
        tt->xx = ii + 1;
        tt->pp = (void *)0xdead0000 + ii + 1;
        limset_commit(set, tt);
    }

    printf("set entries: %d\n", limset_entries(set));
    tts[0] = limset_next(set, NULL);
    tts[1] = limset_next(set, tts[0]);
    tts[2] = limset_next(set, tts[1]);
    limset_consume(set, tts[1]);
    printf("set entries: %d\n", limset_entries(set));
    limset_consume(set, tts[2]);
    printf("set entries: %d\n", limset_entries(set));
    limset_consume(set, tts[0]);
    printf("set entries: %d\n", limset_entries(set));

    //error detecting
#if 0
    printf("ITERATE from non-existing object.\n");
    tts[2] = limset_next(set, tts[1]);

    //commit bogus object
    printf("ERROR DETECTING: writing bogus entry.\n");
    tt = limset_new(set);
    tt->xx = 9;
    limset_commit(set, tt);
    limset_commit(set, tt);
#endif

    printf("destroying set.\n");
    limset_destroy(set);
    printf("good bye.\n");

    return 0;
}
