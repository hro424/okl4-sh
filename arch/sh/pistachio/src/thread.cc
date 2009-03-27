/**
 * @since   February 2009
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#include <schedule.h>
#include <tcb.h>
#include <arch/thread.h>

#if defined(__GNUC__)
#define asm_switch_to(cont, dest)                           \
    do {                                                    \
        __asm__ __volatile__ (                              \
            "    mov     %3, r0          \n"                \
            "    add     %4, r0          \n"                \
            "    mov.l   %2, @r0         \n"                \
            "    or      %1, r15         \n"                \
            "    jmp     @%0             \n"                \
            "    nop                     \n"                \
            :                                               \
            : "r" (cont),                                   \
              "r" (STACK_TOP),                              \
              "r" (dest),                                   \
              "r" (get_globals()),                          \
              "i" (offsetof(globals_t, current_tcb))        \
            : "memory"                                      \
        );                                                  \
        while (true);                                       \
    } while (false)
#else
#error "Unknown compiler"
#endif

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
    /* Update the global schedule variable. */
    set_active_schedule(schedule);

    /* Restore any resources held by the current thread. */
    if (EXPECT_FALSE(dest->resource_bits)) {
        dest->resources.load(dest);
    }

    /* Setup the destination's address space. */
    space_t* space = dest->get_space();
    if (EXPECT_FALSE(space == NULL)) {
        space = get_kernel_space();
    }
    space->activate(dest);

    /* Perform the context switch for real. */
    asm_switch_to(dest->cont, (word_t)dest);
    /* NOT REACHED */
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

