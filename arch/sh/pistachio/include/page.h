/* $Id$ */

/**
 * @file    arch/sh/pistachio/include/page.h
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_PAGE_H
#define OKL4_ARCH_SH_PAGE_H

/*
 * Supported page sizes:
 * 4KiB
 * 64KiB
 * 1MiB ("sections")
 */
#define SH_SECTION_BITS             12
#define SH_SECTION_SIZE             (1UL << (32 - SH_SECTION_BITS))
#define SH_NUM_SECTIONS             (1UL << SH_SECTION_BITS)

#define HW_PGSHIFTS                 {12, 16, 20, 32}

#define HW_VALID_PGSIZES            ((1 << 12) | (1 << 16) | (1 << 20))

#define PAGE_BITS_4K                (12)
#define PAGE_BITS_64K               (16)
#define PAGE_BITS_1M                (20)

#define PAGE_SIZE_4K                (1UL << PAGE_BITS_4K)
#define PAGE_SIZE_64K               (1UL << PAGE_BITS_64K)
#define PAGE_SIZE_1M                (1UL << PAGE_BITS_1M)

#define PAGE_MASK_4K                (~(PAGE_SIZE_4K - 1))
#define PAGE_MASK_64K               (~(PAGE_SIZE_64K - 1))
#define PAGE_MASK_1M                (~(PAGE_SIZE_1M - 1))

#endif /* OKL4_ARCH_SH_PAGE_H */
