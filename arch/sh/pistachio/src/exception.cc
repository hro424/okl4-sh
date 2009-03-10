/* $Id$ */

/**
 * @brief   General exceptions and TLB miss handling
 * @since   January 2009
 */

#include <debug.h>
#include <schedule.h>
#include <space.h>
#include <tcb.h>
#include <tracebuffer.h>
#include <kdb/tracepoints.h>
#include <arch/exception.h>

//TODO
//DECLARE_TRACEPOINT(EXCEPTION_GENERAL);
//DECLARE_TRACEPOINT(EXCEPTION_TLB_MISS);

NORETURN INLINE void
halt_user_thread(continuation_t continuation)
{
    tcb_t * current = get_current_tcb();
    scheduler_t * scheduler = get_current_scheduler();

    scheduler->deactivate_sched(current, thread_state_t::halted,
                                current, continuation,
                                scheduler_t::sched_default);
}


INLINE static void
send_exception_ipc(word_t ecode, addr_t addr, sh_context_t* context)
{
    //TODO
    /* Call exception handler */
}

INLINE static void
handle_invalid_exception(word_t ecode)
{
}

INLINE static void
handle_user_break(word_t ecode, addr_t addr, sh_context_t* context)
{
    //TODO
    /* Call kernel debugger */
}

/**
 * Handles the general exceptions apart from TLB exceptions.
 * Invoked by the trap handler.
 *
 * @param ecode     the exception code
 * @param context   the context where this exception was raised.
 */
extern "C" void
handle_general_exception(word_t ecode, sh_context_t* context)
{
    addr_t faddr = (addr_t)mapped_reg_read(REG_TEA);

    switch (ecode) {
        case ECODE_FPU:
            //handle_cpu_exception(ecode, faddr, context);
            break;
        case ECODE_USER_BREAK:
            handle_user_break(ecode, faddr, context);
            break;
        case ECODE_ADDRESS_R:
        case ECODE_ADDRESS_W:
        case ECODE_GENERAL_INST:
        case ECODE_SLOT_INST:
        case ECODE_GENERAL_FPU:
        case ECODE_SLOT_FPU:
        default:
            enter_kdebug("do_general_exception");
            break;
    }
}

/**
 * Fills the specified TLB entry.
 *
 * @param vaddr     the virtual address
 * @param space     the address space
 * @param pg        the page entry
 * @param pgsize    the size of the page
 */
static void
fill_tlb(addr_t vaddr, space_t* space, pgent_t* pg, pgent_t::pgsize_e pgsize)
{
    static u8_t entry = 0;
    word_t      reg;

    reg  = mapped_reg_read(REG_MMUCR);
    reg &= ~(REG_MMUCR_URC_MASK);
    // TODO: mask it with URB value
    reg |= (entry << 10) & REG_MMUCR_URC_MASK;
    mapped_reg_write(REG_MMUCR, reg);
    entry++;    // overflow -> go back to 0

    mapped_reg_write(REG_PTEH,
                 (word_t)vaddr & REG_PTEH_VPN_MASK |
                 (word_t)space->get_asid()->get(space) & REG_PTEH_ASID_MASK);
    mapped_reg_write(REG_PTEL, pg->raw & REG_PTEL_MASK);

    __asm__ __volatile__ ("ldtlb");

    UPDATE_REG();
}

/**
 * Handles TLB miss and memory access violation.  Invoked by the trap handler.
 *
 * @param ecode     the exception code
 * @param context   the context where this exception was raised
 */
extern "C" void
handle_tlb_exception(word_t ecode, sh_context_t* context)
{
    addr_t              faddr;
    pgent_t*            pg;
    space_t*            space;
    tcb_t*              current;
    pgent_t::pgsize_e   pgsize;
    space_t::access_e   access;

    switch (ecode) {
        //TODO
        case ECODE_TLB_FAULT_R:
        case ECODE_TLB_FAULT_W:
        case ECODE_INIT_WRITE:
            break;

        case ECODE_TLB_MISS_W:
            access = space_t::write;
            break;
        case ECODE_TLB_MISS_R:
            access = space_t::read;
            break;
        default:
            enter_kdebug("do_tlb_miss");
    }
       
    faddr = (addr_t)mapped_reg_read(REG_TEA);
    current = get_current_tcb();
    space = current->get_space();
    if (space == NULL) {
        space = get_kernel_space();
    }

    if (space->lookup_mapping(faddr, &pg, &pgsize)) {
        if (((access == space_t::write) && pg->is_writable(space, pgsize)) ||
            ((access == space_t::read) && pg->is_readable(space, pgsize))) {
            fill_tlb(faddr, space, pg, pgsize);
            return;
        }
    }

    /* Need to raise a page fault */

    bool kernel;
    if (context->sr & REG_SR_MD) {
        kernel = true;
    }
    else {
        kernel = false;
    }

    space->handle_pagefault(faddr, (addr_t)context->pc, access, kernel,
                            ASM_CONTINUATION);
}

extern "C" void
handle_syscall_exception(sh_context_t* context)
{
    //TODO
}

