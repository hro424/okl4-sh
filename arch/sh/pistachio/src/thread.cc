/* $Id$ */

/**
 * @since   February 2009
 */

#include <schedule.h>
#include <tcb.h>
#include <arch/thread.h>

void
switch_from(tcb_t* current, continuation_t continuation)
{
    ASSERT(ALWAYS, current->ready_list.next == NULL);

    if (EXPECT_FALSE(current->resource_bits)) {
        current->resources.save(current);
    }

    current->cont = continuation;
}

void
switch_to(tcb_t* dest, tcb_t* schedule)
{
    ASSERT(ALWAYS, dest->ready_list.next == NULL);
    //space_t*    dest_space = dest->get_space();

    /* Update the global schedule variable. */
    set_active_schedule(schedule);

    /* Restore any resources held by the current thread. */
    if (EXPECT_FALSE(dest->resource_bits)) {
        dest->resources.load(dest);
    }

    /* Perform the context switch for real. */

    while (1);
}
