/* $Id$ */

/**
 * @since   December 2008
 */

#include <debug.h>
#include <arch/syscalls.h>
#include <arch/thread.h>

#if defined(CONFIG_DEBUG)

__asm__ (
    "    .global     enter_kdebug   \n"
    "enter_kdebug:                  \n"
    "    mov.l   r15, @-r15;        \n" 
    "    stc.l   spc, @-r15;        \n"
    "    stc.l   ssr, @-r15;        \n"
    "    sts.l   pr, @-r15;         \n"
    "    sts.l   macl, @-r15;       \n"
    "    sts.l   mach, @-r15;       \n"
    "    mov.l   r14, @-r15         \n"
    "    mov.l   r13, @-r15         \n"
    "    mov.l   r12, @-r15         \n"
    "    mov.l   r11, @-r15         \n"
    "    mov.l   r10, @-r15         \n"
    "    mov.l   r9, @-r15          \n"
    "    mov.l   r8, @-r15          \n"
    "    mov.l   r7, @-r15          \n"
    "    mov.l   r6, @-r15          \n"
    "    mov.l   r5, @-r15          \n"
    "    mov.l   r4, @-r15          \n"
    "    mov.l   r3, @-r15          \n"
    "    mov.l   r2, @-r15          \n"
    "    mov.l   r1, @-r15          \n"
    "    mov.l   r0, @-r15          \n"
    "    mov     r15, r5            \n"
    "    stc     ssr, r6            \n"
    "    bra     enter_kdebug_c     \n"
    "    nop                        \n"
);

extern "C" void
enter_kdebug_c(char* msg, sh_context_t* context, word_t stat)
{
    context->pc = (u32_t)__return_address();
    context->sr = (u32_t)stat;
    
    printf(TXT_BRIGHT "--- KD# %s ---\n" TXT_NORMAL, msg);

    if (EXPECT_FALSE(kdebug_entries.kdebug_entry != NULL)) {
        kdebug_entries.kdebug_entry(context);
    }
    else {
        while (1);
    }

    __asm__ __volatile__ (
        "    mov     %0, r15            \n"
        "    mov.l   @r15+, r0          \n"
        "    mov.l   @r15+, r1          \n"
        "    mov.l   @r15+, r2          \n"
        "    mov.l   @r15+, r3          \n"
        "    mov.l   @r15+, r4          \n"
        "    mov.l   @r15+, r5          \n"
        "    mov.l   @r15+, r6          \n"
        "    mov.l   @r15+, r7          \n"
        "    mov.l   @r15+, r8          \n"
        "    mov.l   @r15+, r9          \n"
        "    mov.l   @r15+, r10         \n"
        "    mov.l   @r15+, r11         \n"
        "    mov.l   @r15+, r12         \n"
        "    mov.l   @r15+, r13         \n"
        "    mov.l   @r15+, r14         \n"
        "    lds.l   @r15+, mach        \n"
        "    lds.l   @r15+, macl        \n"
        "    lds.l   @r15+, pr          \n"
        "    ldc.l   @r15+, ssr         \n"
        "    ldc.l   @r15+, spc         \n"
        "    mov.l   @r15, r15          \n"
        "    rts                        \n"
        "    nop                        \n"
        :: "r" (context)
        );

    while (1);
}

#endif /* CONFIG_DEBUG */

