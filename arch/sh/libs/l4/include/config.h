/* $Id$ */

/**
 * @file    arch/sh/libs/l4/include/config.h
 * @since   December 9, 2008
 */

#ifndef OKL4_ARCH_SH_L4_CONFIG_H
#define OKL4_ARCH_SH_L4_CONFIG_H

#define UTCB_BITS       (8)
#define UTCB_SIZE       ((word_t)1 << UTCB_BITS)
#define UTCB_ALIGNMENT  0x100

#include <l4/arch/page.h>

#endif /* OKL4_ARCH_SH_L4_CONFIG_H */
