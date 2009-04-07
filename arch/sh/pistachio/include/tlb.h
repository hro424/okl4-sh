/**
 *  @brief  TLB utilities
 *  @since  March 2009
 *  @author Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#ifndef ARCH_OKL4_TLB_H
#define ARCH_OKL4_TLB_H

#include <l4.h>
#include <arch/pgent.h>

/**
 * Fills the specified TLB entry.
 * NOTE: The current replacement policy is round-robin.
 *
 * @param vaddr     the virtual address
 * @param space     the address space
 * @param pg        the page entry
 * @param pgsize    the size of the page
 */
void fill_tlb(addr_t vaddr,
              space_t* space,
              pgent_t* pg,
              pgent_t::pgsize_e pgsize);

void fill_tlb(int entry,
              addr_t vaddr,
              space_t* space,
              pgent_t* pg,
              pgent_t::pgsize_e pgsize);

void dump_utlb();

#endif /* ARCH_OKL4_TLB_H */
