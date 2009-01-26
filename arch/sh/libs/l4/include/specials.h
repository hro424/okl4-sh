/* $Id$ */

/**
 * @brief   Architecture specific helper functions
 * @file    arch/sh/libs/l4/include/specials.h
 * @since   December 9, 2008
 */

#ifndef OKL4_ARCH_SH_L4_SPECIALS_H
#define OKL4_ARCH_SH_L4_SPECIALS_H

#include <compat/c.h>

L4_INLINE int __L4_Msb(L4_Word_t w) CONST;

L4_INLINE int
__L4_Msb(L4_Word_t w)
{
    int bitnum;

    if (w == 0) {
        return 32;
    }

    for (bitnum = 0, w >>= 1; w != 0; bitnum++)
        w >>= 1;

    return bitnum;
}

#define ARCH_THREAD_INIT

#endif /* OKL4_ARCH_SH_L4_SPECIALS_H */
