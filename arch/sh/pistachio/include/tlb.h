/**
 *  @brief  TLB utilities
 *  @since  March 2009
 *  @author Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 */

#ifndef ARCH_OKL4_TLB_H
#define ARCH_OKL4_TLB_H

#include <l4.h>
#include <arch/pgent.h>

#define UTLB_MIN        0x00
#define UTLB_MAX        0x3F
#define UTLB_UTCB       UTLB_MAX

/**
 * Fills a TLB entry with the specified virtual address.
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

/**
 * Fills the specified TLB entry with the specified virtual address.
 *
 * @param entry     the TLB entry < 0x3F
 * @param vaddr     the virtual address
 * @param space     the address space
 * @param pg        the page entry
 * @param pgsize    the size of the page
 */
void fill_tlb(int entry,
              addr_t vaddr,
              space_t* space,
              pgent_t* pg,
              pgent_t::pgsize_e pgsize);

/**
 * Check if the specified entry is in TLB.
 *
 * @param vaddr     the virtual address to be searched
 * @param space     the address space
 */
bool
lookup_tlb(addr_t vaddr, space_t* space);

void dump_utlb();

#endif /* ARCH_OKL4_TLB_H */
