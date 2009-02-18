/* $Id$ */

/**
 * @brief   General exceptions and TLB miss handling
 * @since   January 2009
 */

#include <debug.h>
#include <space.h>
#include <tcb.h>
#include <tracebuffer.h>
#include <kdb/tracepoints.h>
#include <arch/exception.h>

//DECLARE_TRACEPOINT(EXCEPTION_GENERAL);
//DECLARE_TRACEPOINT(EXCEPTION_TLB_MISS);

INLINE static void
handle_memory_fault(word_t ecode, addr_t addr, sh_context_t* context)
{
    //TODO
    /*
    addr_t              faddr;
    pgent_t*            pg;
    space_t*            space;
    tcb_t*              current;
    pgent_t::pgsize_e   pgsize;
    space_t::access_e   access;
    */

    /* Call page fault handler */

}

INLINE static void
handle_exception(word_t ecode, addr_t addr, sh_context_t* context)
{
    //TODO
    /* Call exception handler */
}

INLINE static void
handle_user_break(word_t ecode, addr_t addr, sh_context_t* context)
{
    //TODO
    /* Call kernel debugger */
}

extern "C" void
do_general_exception(word_t ecode, sh_context_t* context)
{
    addr_t faddr = (addr_t)mapped_reg_read(REG_TEA);

    switch (ecode) {
        case ECODE_TLB_FAULT_R:
        case ECODE_TLB_FAULT_W:
        case ECODE_ADDRESS_R:
        case ECODE_ADDRESS_W:
        case ECODE_INIT_WRITE:
            handle_memory_fault(ecode, faddr, context);
            break;
        case ECODE_FPU:
        case ECODE_GENERAL_INST:
        case ECODE_SLOT_INST:
        case ECODE_GENERAL_FPU:
        case ECODE_SLOT_FPU:
            handle_exception(ecode, faddr, context);
            break;
        case ECODE_USER_BREAK:
            handle_user_break(ecode, faddr, context);
            break;
        default:
            enter_kdebug("do_general_exception");
            break;
    }
}

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

extern "C" void
do_tlb_miss(word_t ecode, sh_context_t* context)
{
    addr_t              faddr;
    pgent_t*            pg;
    space_t*            space;
    tcb_t*              current;
    pgent_t::pgsize_e   pgsize;
    space_t::access_e   access;

    switch (ecode) {
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
do_syscall_exception(sh_context_t* context)
{
    //TODO
}
