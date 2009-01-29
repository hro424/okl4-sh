/* $Id$ */

/**
 * @file    libs/kernel/include/asm.h
 * @since   December 8, 2008
 */

#ifndef OKL4_ARCH_SH_LIBS_KERNEL_ASM_H
#define OKL4_ARCH_SH_LIBS_KERNEL_ASM_H

#if defined(ASSEMBLY)

#define ALIGN                   .balign
#define EXPORT                  .global
#define IMPORT                  .extern
#define LABEL(name)             name:
#define PUSH(reg)               mov.l   reg, @-r15
#define POP(reg)                mov.l   @r15+, reg

#define TRAPS_BEGIN_MARKER              \
    .section .data.traps;               \
    .balign 4096;                       \

#define BEGIN_PROC_TRAPS(name, offset)  \
    .global name;                       \
    .type   name,function;              \
    .balign offset;                     \
name:

#define END_PROC_TRAPS(name)

#define BEGIN_PROC(name)                \
    .global name;                       \
    .type   name,function;              \
    .align;                             \
name:

#define END_PROC(name)

#endif /* ASSEMBLY */

#define CHECK_ARG(a, b)                 \
    ".ifnc " a ", " b "  \n"            \
    ".err                \n"            \
    ".endif              \n"


#endif /* OKL4_ARCH_SH_LIBS_KERNEL_ASM_H */
