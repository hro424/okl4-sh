/* $Id$ */

/**
 *  @since  December 2008
 */

#ifndef OKL4_ARCH_SH_KERNEL_CONTINUATION_H
#define OKL4_ARCH_SH_KERNEL_CONTINUATION_H

#if !defined(ASSEMBLY)

#include <kernel/arch/asm.h>

#if defined(__GNUC__)

#define ACTIVATE_CONTINUATION(continuation)     \
    do {                                        \
        __asm__ __volatile__ (                  \
            "    or      %1, r15     \n"        \
            "    jmp     @%0         \n"        \
            "    nop                 \n"        \
            :                                   \
            : "r" ((word_t)(continuation)),     \
              "r" (STACK_TOP)                   \
            : "r15", "memory"                   \
        );                                      \
        while (true) ;                          \
    } while (false)

/**
 *  Calls a function with 2 arguments + a continuation so that the
 *  continuation can be retrieved by ASM_CONTINUATION
 */
NORETURN INLINE void
call_with_asm_continuation(word_t argument0, word_t argument1,
                           word_t continuation, word_t function)
{
    register word_t arg0    ASM_REG("r4") = argument0;
    register word_t arg1    ASM_REG("r5") = argument1;
    register word_t lr      ASM_REG("r14") = continuation;
    register word_t stack   ASM_REG("r1") = STACK_TOP;

    __asm__ __volatile__ (
        CHECK_ARG("r4", "%0")
        CHECK_ARG("r5", "%1")
        CHECK_ARG("r14", "%2")
        CHECK_ARG("r1", "%3")
        "    or      r1, r15     \n"
        "    jmp     @%2         \n"
        "    nop                 \n"
        :: "r" (arg0), "r" (arg1), "r" (lr), "r" (stack), "r" (function)
    );
    while (1);
}

/* call a NORETURN function with 3 arguments + reset stack */
NORETURN INLINE void
activate_function(word_t argument0, word_t argument1, word_t argument2,
                  word_t function)
{
    register word_t arg0    ASM_REG("r4") = argument0;
    register word_t arg1    ASM_REG("r5") = argument1;
    register word_t arg2    ASM_REG("r6") = argument2;
    register word_t stack   ASM_REG("r1") = STACK_TOP;

    __asm__ __volatile__ (
        CHECK_ARG("r4", "%0")
        CHECK_ARG("r5", "%1")
        CHECK_ARG("r6", "%2")
        CHECK_ARG("r1", "%3")
        "    or      r1, r15     \n"
        "    jmp     @%4         \n"
        "    nop                 \n"
        :: "r" (arg0), "r" (arg1), "r" (arg2), "r" (stack), "r" (function)
    );
    while (1);
}


#else
#error "Unknown compiler"
#endif

#endif /* ASSEMBLY */

#endif /* OKL4_ARCH_SH_KERNEL_CONTINUATION */
