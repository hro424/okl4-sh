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

DECLRE_TRACEPOINT(SH_PAGE_FAULT);

extern "C" void
do_tlb_miss(word_t expevt, sh_context_t* context)
{
}


