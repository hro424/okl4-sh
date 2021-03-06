/*********************************************************************
 *                
 * Copyright (C) 2002, 2003,  Karlsruhe University
 *                
 * File path:     kdb/arch/ia64/itanium/perf.cc
 * Description:   Itanium performance monitoring functionality
 *                
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *                
 * $Id$
 *                
 ********************************************************************/
#include <debug.h>
#include <kdb/cmd.h>
#include <kdb/kdb.h>
#include <kdb/input.h>

#include INC_ARCH(itanium_perf.h)
#include INC_GLUE(context.h)
#include INC_API(tcb.h)


bool kdb_get_perfctr (pmc_itanium_t * pmc, word_t * pmc_mask);


/**
 * Set performance counter register.
 */
DECLARE_CMD (cmd_ia64_perf_setreg, ia64_perfmon, 's', "setreg",
	     "set performance register");

CMD (cmd_ia64_perf_setreg, cg)
{
    ia64_exception_context_t * frame =
	(ia64_exception_context_t *) kdb.kdb_param;
     ia64_exception_context_t * user_frame =
	(ia64_exception_context_t *) kdb.kdb_current->get_stack_top () - 1;

    pmc_itanium_t pmc = 0;
    word_t i, reg, pmc_mask;

    if (! kdb_get_perfctr (&pmc, &pmc_mask))
	return CMD_NOQUIT;

    do {
	for (i = 1, reg = 0; (i & pmc_mask) == 0; i <<= 1, reg++)
	    ;
	reg = get_dec ("Select register", reg);
	if (reg == ABORT_MAGIC)
	    return CMD_NOQUIT;

    } while (((1UL << reg) & pmc_mask) == 0);

    pmc.ev = 0;		// no external visibility
    pmc.oi = 0;		// no overflow interrupt
    pmc.pm = 1;		// allow user access
    pmc.threshold = 0;	// sum up
    pmc.ism = 0;	// monitor IA32 and IA64 instructions

    printf("perfctr: es=%x, umask=%x, raw = %x\n", 
	   pmc.es, pmc.umask, *(word_t *) &pmc);

    switch (get_choice ("Priviledge level", "User/Kernel/All", 'a'))
    {
    case 'u': pmc.plm = pmc_t::user; break;
    case 'k': pmc.plm = pmc_t::kernel; break;
    case 'a': pmc.plm = pmc_t::both; break;
    default:
	return CMD_NOQUIT;
    }

    set_pmc (reg, pmc);
    set_pmd (reg, 0);

    frame->ipsr.pp = user_frame->ipsr.pp = 1;

    return CMD_NOQUIT;
}

