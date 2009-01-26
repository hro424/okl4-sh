/* $Id$ */

/**
 * @file    arch/sh/libs/kernel/include/types.h
 * @since   December 9, 2008
 */

#ifndef OKL4_ARCH_SH_KERNEL_TYPES_H
#define OKL4_ARCH_SH_KERNEL_TYPES_H

#if !defined(ASSEMBLY)
typedef unsigned long long      u64_t;
typedef unsigned long           u32_t;
typedef unsigned short          u16_t;
typedef unsigned char           u8_t;

typedef signed long long        s64_t;
typedef signed long             s32_t;
typedef signed short            s16_t;
typedef signed char             s8_t;

#define GLOBAL_DEC(type, x)
#define GLOBAL_DEF(x)
#define GLOBAL(x)               (get_globals()->x)
#endif

#endif /* OKL4_ARCH_SH_KERNEL_TYPES_H */
