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


/**
 * This function cleans up after an in-kernel
 * ipc and continues through the continuation
 * passed to do_ipc
 *
 * This function does not use any other stored
 * state from the TCB
 */
CONTINUATION_FUNCTION(do_ipc_helper)
{
    tcb_t * current = get_current_tcb();

#ifdef CONFIG_IPC_FASTPATH
    current->resources.clear_kernel_ipc( current );
    current->resources.clear_except_fp( current );
#endif

    ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->do_ipc_continuation);
}

CONTINUATION_FUNCTION(restart_do_ipc)
{
    tcb_t *current = get_current_tcb();
    tcb_t *to_tcb = (tcb_t*)TCB_SYSDATA_IPC(current)->to_tid.get_raw();
    tcb_t *from_tcb = (tcb_t*)TCB_SYSDATA_IPC(current)->from_tid.get_raw();

    to_tcb = acquire_read_lock_tcb(to_tcb);
    from_tcb = acquire_read_lock_tcb(from_tcb);

    /* Call sys_ipc with a custom continuation as the return address. */
    activate_function((word_t)to_tcb, (word_t)from_tcb, 3, (word_t)ipc);
}

void
tcb_t::create_startup_stack(void (*func)())
{
    sh_context_t*   context;

    init_stack();
    notify(func);
    context = &(arch.context);
    context->sr &= ~REG_SR_MD;
}

