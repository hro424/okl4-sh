/* $Id$ */

/**
 * @file    arch/sh/pistachio/include/debug.h
 * @since   December 9, 2008
 */

#ifndef OKL4_ARCH_SH_DEBUG_H
#define OKL4_ARCH_SH_DEBUG_H

#if defined(CONFIG_DEBUG)

INLINE void
spin_forever(int pos = 0)
{
    while (1) ;
}

INLINE void
spin(int pos = 0, int cpu = 0)
{
    return;
}

extern "C" void enter_kdebug(const char* s);

#endif /* CONFIG_DEBUG */

#endif /* OKL4_ARCH_SH_DEBUG_H */
