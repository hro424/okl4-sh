/* $Id$ */

/**
 * @file    arch/sh/pistachio/include/asid.h
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_ASID_H
#define OKL4_ARCH_SH_ASID_H

/* SH has 8-bit ASID space */
typedef s16_t   hw_asid_t;

/**
 *  Flushes TLB and page cache for the specified ASID
 *
 *  @param asid         the address space ID of the space to purge
 */
INLINE void
flush_asid(word_t asid)
{
    //sh_cache::tlb_flush_asid(asid);
}

#endif /* OKL4_ARCH_SH_ASID_H */
