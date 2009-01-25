/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_TCB_H
#define OKL4_ARCH_SH_TCB_H

#include <syscalls.h>
#include <kernel/errors.h>
#include <kernel/arch/continuation.h>

extern stack_t __stack;

PURE INLINE tcb_t*
get_current_tcb()
{
    return get_globals()->current_tcb;
}

PURE INLINE clist_t*
get_current_clist()
{
    return get_globals()->current_clist;
}

INLINE void
tcb_t::enable_preempt_recover(continuation_t continuation)
{
    preemption_continuation = continuation;
}

INLINE void
tcb_t::disable_preempt_recover()
{
    preemption_continuation = 0;
}

INLINE word_t
tcb_t::set_tls(word_t *mr)
{
    get_utcb()->kernel_reserved[0] = mr[0];
    return EOK;
}

/**
 * read value of message register
 * @param index number of message register
 */
INLINE word_t
tcb_t::get_mr(word_t index)
{
    return get_utcb()->mr[index];
}

/**
 * set the value of a message register
 * @param index number of message register
 * @param value value to set
 */
INLINE void
tcb_t::set_mr(word_t index, word_t value)
{
    get_utcb()->mr[index] = value;
}

/**
 * read value of the acceptor
 */
INLINE acceptor_t
tcb_t::get_acceptor(void)
{
    return get_utcb()->acceptor;
}

/**
 * set the value of the acceptor register
 * @param value value to set
 */
INLINE void
tcb_t::set_acceptor(const acceptor_t value)
{
    get_utcb()->acceptor = value;
}

INLINE bool
tcb_t::copy_mrs(tcb_t* dest, word_t start, word_t count)
{
    return false;
}

INLINE void
tcb_t::set_exception_ipc(word_t num)
{
}

INLINE bool
tcb_t::in_exception_ipc()
{
    return false;
}



#define asm_initial_switch_to(new_sp, new_pc)   \
    __asm__ __volatile__ (                      \
        "    mov     %0, r15    \n"             \
        "    jmp     @%1        \n"             \
        "    nop                \n"             \
        :: "r" (new_sp), "r" (new_pc)           \
        : "memory")


/**
 * switch to initial thread
 * @param tcb TCB of initial thread
 *
 * Initializes context of initial thread and switches to it.  The
 * context (e.g., instruction pointer) has been generated by inserting
 * a notify procedure context on the stack.  We simply restore this
 * context.
 */

INLINE void NORETURN initial_switch_to (tcb_t * tcb)
{
    word_t*     pteh = (word_t*)REG_PTEH;
    tcb_t**     stack_top;
    space_t*    space = get_kernel_space();
    hw_asid_t   new_asid = space->get_asid()->get(space);

    sh_cache::flush();

    /* Initialize page table with ASID */
    *pteh = (new_asid & REG_PTEH_ASID_MASK) | (*pteh & REG_PTEH_VPN_MASK);
    UPDATE_REG();

    /* keep stack aligned for RVCT */
    stack_top = ((tcb_t **)(&__stack + 1)) - 2;

    /* switch */
    asm_initial_switch_to((addr_t)stack_top, tcb->cont);

    ASSERT(ALWAYS, !"We shouldn't get here!");
    while(true) {}
}

/**
 * read the user-level instruction pointer
 * @return      the user-level stack pointer
 */
INLINE addr_t tcb_t::get_user_ip()
{
    //sh_irq_context_t* context = &arch.context;
    //return (addr_t) ((context)->pc & ~1UL);
    return 0;
}


/**
 * set the user-level instruction pointer
 * @param ip    new user-level instruction pointer
 */
INLINE void tcb_t::set_user_ip(addr_t ip)
{
    //sh_irq_context_t*   context = &arch.context;
    //context->pc = (word_t)ip;
}

#endif /* OKL4_ARCH_SH_TCB_H */
