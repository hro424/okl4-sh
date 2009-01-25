/* $Id$ */

/**
 * @file    arch/sh/libs/l4/include/page.h
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_L4_PAGE_H
#define OKL4_ARCH_SH_L4_PAGE_H

/*
 * Supported page sizes:
 *  1k, 4k, 64k, 1M
 *
 * Note:    Do not use 1k pages to avoid cache synonims when MMU is
 *          enable.
 */

#define PAGE_BITS_4K                (12)
#define PAGE_BITS_64K               (16)
#define PAGE_BITS_1M                (20)

#define PAGE_SIZE_4K                (1UL << PAGE_BITS_4K)
#define PAGE_SIZE_64K               (1UL << PAGE_BITS_64K)
#define PAGE_SIZE_1M                (1UL << PAGE_BITS_1M)

#define PAGE_MASK_4K                (~(PAGE_SIZE_4K - 1))
#define PAGE_MASK_64K               (~(PAGE_SIZE_64K - 1))
#define PAGE_MASK_1M                (~(PAGE_SIZE_1M - 1))

#define USER_HW_VALID_PGSIZES       ((1 << 12) | (1 << 16) | (1 << 20))
#define __L4_MIN_PAGE_BITS          12
#define __L4_VALID_HW_PAGE_PERMS    ((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7))

// deprecated
#define USER_HW_PAGE_PERMS          3

#endif /* OKL4_ARCH_SH_L4_PAGE_H */
