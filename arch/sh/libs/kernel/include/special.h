/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_KERNEL_SPECIAL_H
#define OKL4_ARCH_SH_KERNEL_SPECIAL_H

#include <kernel/generic/lib.h>

CONST INLINE word_t
msb(word_t w)
{
    return msb_binary(w);
}

#endif /* OKL4_ARCH_SH_KERNEL_SPECIAL_H */

