/* $Id$ */

/**
 * @brief   TLB miss handling
 * @since   January 2009
 */

#include <debug.h>
#include <space.h>
#include <tcb.h>
#include <tracebuffer.h>
#include <kdb/tracepoints.h>
#include <arch/excpetion.h>

DECLARE_TRACEPOINT(EXCEPTION_GENERAL);
DECLARE_TRACEPOINT(EXCEPTION_TLB_MISS);

INLINE static void
handle_memory_fault(sh_context_t* context)
{
    /* Call page fault handler */
}

INLINE static void
handle_exception(sh_context_t* context)
{
    /* Call exception handler */
}


extern "C" void
do_general_exception(word_t ecode, sh_context_t* context)
{
    context->exc_num = ecode;
    context->exc_code = read_mapped_reg(TEA);

    switch (ecode) {
        case ECODE_TLB_FAULT_R:
        case ECODE_TLB_FAULT_W:
        case ECODE_ADDRESS_R:
        case ECODE_ADDRESS_W:
        case ECODE_INIT_WRITE:
            handle_memory_fault(context);
            break;
        case ECODE_FPU:
        case ECODE_GENERAL_INST:
        case ECODE_SLOT_INST:
        case ECODE_GENERAL_FPU:
        case ECODE_SLOT_FPU:
            handle_exception(context);
            break;
        case ECODE_USER_BREAK:
            handle_user_break(context);
            break;
        default:
            enter_kdebug("do_general_exception");
            break;
    }
}

extern "C" void
do_tlb_miss(word_t ecode, sh_context_t* context)
{
    addr_t              faddr;
    pgent_t*            pg;
    space_t*            space;
    tcb_t*              current;
    hw_asid_t           asid;
    pgent_t::pgsize_e   pgsize;
    space_t::access_e   access;
        
    faddr = *(addr_t*)REG_TEA;
    current = get_current_tcb();
    space = current->get_space();
    if (space == NULL) {
        space = get_kernel_space();
    }

    asid = space->get_asid()->get();

    access = (ecode == ECODE_TLB_MISS_W) ? space_t::write : space_t::read;

    if (space->lookup_mapping(faddr, &pg, &pgsize)) {
        if (((access == space_t::write) && pg->is_writable(space, pgsize)) ||
            ((access == space_t::read) && pg->is_readable(space, pgsize))) {
            ASSERT(asid ==
                   (hw_asid_t)((*(word_t*)REG_PTEH) & REG_PTEH_ASID_MASK));
            pg->pgent.apply_to_mmu(faddr, space->get_asid()->get, true);
            return;
        }
    }

    kernel = context->sr & REG_SR_MD ? true : false;
    if (!kernel) {
        current->arch.misc.fault.fault_access = access;
        current->arch.misc.fault.fault_addr = faddr;
        current->arch.misc.fault.tlbmiss_continuation = ASM_CONTINUATION;
    }

    space->handle_pagefault(faddr, (addr_t)context->pc, access, kernel,
                            (continuation_t)finish_tlb_miss);
}

