/*********************************************************************
 *                
 * Copyright (C) 2002-2003,  University of New South Wales
 *                
 * File path:     glue/v4-mips64/memcontrol.cc
 * Description:   Temporary memory_control implementation
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

#include INC_API(config.h)
#include INC_API(tcb.h)
#include INC_API(thread.h)
#include INC_API(fpage.h)
#include INC_GLUE(syscalls.h)
#include INC_API(syscalls.h)
#include INC_PLAT(cache.h)

#include <kdb/tracepoints.h>

DECLARE_TRACEPOINT(SYSCALL_MEMORY_CONTROL);

enum attribute_e {
    a_l4default		= 0,
    a_uncached		= 1,
    a_write_back	= 2,
    a_write_through	= 3,
    a_write_through_noalloc = 4,
#ifdef CONFIG_SMP
    a_coherent		= 5,
#endif
    a_flush_i		= 30,
    a_flush_d		= 31,
};


#include INC_ARCH(pgent.h)
#include INC_API(space.h)
#include INC_GLUE(space.h)
#include <linear_ptab.h>

static inline addr_t address (fpage_t fp, word_t size)
{
    return (addr_t) (fp.raw & ~((1UL << size) - 1));
}

/**
 * @param fpage		fpage to change
 * @param attrib	new fpage attributes
 *
 * @returns 
 */
word_t attrib_fpage (tcb_t *current, fpage_t fpage, attribute_e attrib)
{
    pgent_t::pgsize_e size, pgsize;
    pgent_t * pg;
    addr_t vaddr;
    word_t num, modify = 1;

    pgent_t *r_pg[pgent_t::size_max];
    word_t r_num[pgent_t::size_max];
    space_t *space = current->get_space();

    num = fpage.get_size_log2 ();
    vaddr = address (fpage, num);

    if (num < hw_pgshifts[0])
    {
	current->set_error_code(9);  /* Invalid fpage */
	return 1;
    }

    /*
     * Some architectures may not support a complete virtual address
     * space.  Enforce attrib to only cover the supported space.
     */

    if (num > hw_pgshifts[pgent_t::size_max+1])
	num = hw_pgshifts[pgent_t::size_max+1];

    /*
     * Find pagesize to use, and number of pages to map.
     */

    for (pgsize = pgent_t::size_max; hw_pgshifts[pgsize] > num; pgsize--) {}

    num = 1UL << (num - hw_pgshifts[pgsize]);
    size = pgent_t::size_max;
    pg = space->pgent (page_table_index (size, vaddr));

    while (num)
    {
	translation_t::memattrib_e new_att = translation_t::l4default;

	if (! space->is_user_area (vaddr))
	    /* Do not mess with kernel area. */
	    break;

	if (size > pgsize)
	{
	    /* We are operating on too large page sizes. */
	    if (! pg->is_valid (space, size))
		break;
	    else if (pg->is_subtree (space, size))
	    {
		size--;
		pg = pg->subtree (space, size+1)->next
		    (space, size, page_table_index (size, vaddr));
		continue;
	    }
	    else
	    {
		/* page is too large */
		current->set_error_code(9);  /* Invalid fpage */
		return 1;
	    }
	}

	if (! pg->is_valid (space, size))
	    goto Next_entry;

	if (pg->is_subtree (space, size))
	{
	    /* We have to modify each single page in the subtree. */
	    size--;
	    r_pg[size] = pg;
	    r_num[size] = num - 1;

	    pg = pg->subtree (space, size+1);
	    num = page_table_size (size);
	    continue;
	}

	if (space->is_mappable (vaddr))
	{
	    switch (attrib)
	    {
	    case a_l4default: new_att = translation_t::l4default; break;
	    case a_uncached: new_att = translation_t::uncached; break;
	    case a_write_back: new_att = translation_t::write_back; break;
	    case a_write_through: new_att = translation_t::write_through; break;
	    case a_write_through_noalloc: new_att = translation_t::write_through_noalloc; break;
#ifdef CONFIG_SMP
	    case a_coherent: new_att = translation_t::coherent; break;
#endif
	    case a_flush_d:
		cache_t::flush_cache_page((unsigned long)vaddr, size);
		modify = 0; break;
	    case a_flush_i:
		cache_t::flush_icache_page((unsigned long)vaddr, size);
		modify = 0; break;
	    default:
		/* invalid attribute */
		current->set_error_code(EINVALID_PARAM);
		return 1;
	    }
	    if (modify)
	    {
		space->flush_tlbent (space, vaddr, page_shift (size));
		pg->translation() -> set_attrib (new_att);
	    }
	}

    Next_entry:

	pg = pg->next (space, size, 1);
	vaddr = addr_offset (vaddr, page_size (size));
	num--;
    }

    return 0;
}

SYS_MEMORY_CONTROL (word_t control, word_t attribute0, word_t attribute1,
		    word_t attribute2, word_t attribute3)
{
    tcb_t * current = get_current_tcb();
    space_t *space = current->get_space();
    word_t fp_idx, att;

    TRACEPOINT (SYSCALL_MEMORY_CONTROL, 
		printf ("SYS_MEMORY_CONTROL: control=%lx, attribute0=%lx, "
			"attribute1=%lx, attribute2=%lx, attribute3=%lx\n",
    			control, attribute0, attribute1, attribute2,
    			attribute3));

    // invalid request - thread not privileged
    if (!is_privileged_space(get_current_space()))
    {
	current->set_error_code(ENO_PRIVILEGE);
	return_memory_control(0);
    }

    // flush cpu cache
    if (control == -2UL)
    {
	cache_t::flush_cache_all();
	return_memory_control(1);
    }

    if (control >= (1<<6))
    {
        current->set_error_code(EINVALID_PARAM);
	return_memory_control(0);
    }

    for (fp_idx = 0; fp_idx <= control; fp_idx++)
    {
	fpage_t fpage;
	addr_t addr;
	pgent_t * pg;
	pgent_t::pgsize_e pgsize;

	fpage.raw = current->get_mr(fp_idx);

	/* nil pages act as a no-op */
	if (fpage.is_nil_fpage() )
	    continue;

	switch(fpage.raw & 0x3)
	{
	    case 0: att = attribute0; break;
	    case 1: att = attribute1; break;
	    case 2: att = attribute2; break;
	    default: att = attribute3; break;
	}

	addr = address (fpage, fpage.get_size_log2 ());
	// Check if mapping exist in page table
	if (!space->lookup_mapping (addr, &pg, &pgsize))
	{
	    if (!is_sigma0_space(current->get_space()))
	    {
		current->set_error_code(ENO_PRIVILEGE);
		return_memory_control(0);
	    }

	    space->map_sigma0(addr);
	}

	if (attrib_fpage(current, fpage, (attribute_e)att))
	    return_memory_control(0);
    }

    return_memory_control(1);
}
