/**
 *  @brief  TLB utilities
 *  @since  March 2009
 *  @author Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 */

#ifndef ARCH_OKL4_TLB_H
#define ARCH_OKL4_TLB_H

#include <l4.h>
#include <arch/pgent.h>

/**
 * Size of UTLB
 */
#define UTLB_SIZE       0x40

/**
 * Entry for the UCB page is fixed.
 */
#define UTLB_UTCB       0x3F

/**
 * First UTLB entry
 */
#define UTLB_FIRST      0x00

/**
 * Last UTLB entry
 * NOTE: 0x3F is reserved for the UTCB reference page.
 */
#define UTLB_LAST       (UTLB_UTCB - 1)

/**
 * Initializes the UTLB LRU list.
 */
void utlb_init();

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
 * Fills TLB if the specified entry is off.  Updates the LRU list if found.
 *
 * @param vaddr     the virtual address to be searched
 * @param space     the address space
 */
void
refill_tlb(addr_t vaddr, space_t* space);

void dump_utlb();

#endif /* ARCH_OKL4_TLB_H */
