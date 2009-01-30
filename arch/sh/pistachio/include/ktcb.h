/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_KTCB_H
#define OKL4_ARCH_SH_KTCB_H

#include <arch/thread.h>

class arch_ktcb_t
{
public:
    typedef union {
        struct {
            sh_context_t*   exception_context;
            continuation_t  exception_ipc_continuation;
            continuation_t  exception_continuation;
        } exception;

        struct {
            continuation_t  handle_timer_interrupt_continuation;
            continuation_t  irq_continuation;
            word_t          irq_number;
        } irq;

        struct {
            continuation_t  tlb_miss_continuation;
            addr_t          fault_addr;
        } fault;
    } ktcb_misc;

    sh_context_t    context;
    ktcb_misc       misc;
    word_t          exc_code;
    word_t          exc_num;

};

#define ARCH_KTCB_EXP_CTXT(ptr)     ptr->misc.exception.exception_context
#define ARCH_KTCB_EXP_IPC_CONT(ptr)     \
    ptr->misc.exception.exception_ipc_continuation
#define ARCH_KTCB_EXP_CONT(ptr)     ptr->misc.exception.exception_continuation

#define ARCH_KTCB_IRQ_TIMER_CONT(ptr)   \
    ptr->misc.irq.handle_timer_interrupt_continuation
#define ARCH_KTCB_IRQ_CONT(ptr)     ptr->misc.irq.irq_continuation
#define ARCH_KTCB_IRQ_NUM(ptr)      ptr->misc.irq.irq_number

#define ARCH_KTCB_FAULT_CONT(ptr)   ptr->misc.fault.tlb_miss_continuation
#define ARCH_KTCB_FAULT_ADDR(ptr)   ptr->misc.fault.fault_addr

#endif /* OKL4_ARCH_SH_KTCB_H */
