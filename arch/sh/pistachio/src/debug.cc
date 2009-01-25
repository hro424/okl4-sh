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
    "    add     #-84, r15          \n"
    "    mov.l   r0, @r15           \n"
    "    mov.l   r1, @(4, r15)      \n"
    "    mov.l   r2, @(8, r15)      \n"
    "    mov.l   r3, @(12, r15)     \n"
    "    mov.l   r4, @(16, r15)     \n"
    "    mov.l   r5, @(20, r15)     \n"
    "    mov.l   r6, @(24, r15)     \n"
    "    mov.l   r7, @(28, r15)     \n"
    "    mov.l   r8, @(32, r15)     \n"
    "    mov.l   r9, @(36, r15)     \n"
    "    mov.l   r10, @(40, r15)    \n"
    "    mov.l   r11, @(44, r15)    \n"
    "    mov.l   r12, @(48, r15)    \n"
    "    mov.l   r13, @(52, r15)    \n"
    "    mov.l   r14, @(56, r15)    \n"
    /* mov.l R0, @(disp, Rn) cannot access over 60-byte displacement.
     * Need to shift the stack pointer.
     */
    "    mov     r15, r1            \n"
    "    add     #60, r1            \n"
    "    sts     macl, r2           \n"
    "    mov.l   r2, @(0, r1)       \n"
    "    sts     mach, r2           \n"
    "    mov.l   r2, @(4, r1)       \n"
    "    mov.l   r15, @(8, r1)      \n"
    "    sts     pr, r2             \n"
    "    mov.l   r2, @(12, r1)      \n"
    "    stc     sr, r2             \n"
    "    mov.l   r2, @(20, r1)      \n"
    "    mov     r15, r1            \n"
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
        "    mov     %0, r0             \n"
        "    lds     r0, pr             \n"
        "    mov     %1, r5             \n"
        "    add     #60, r5            \n"
        "    mov.l   @(20, r5), r6      \n"
        "    ldc     r6, sr             \n"
        "    mov.l   @(4, r5), r6       \n"
        "    lds     r6, mach           \n"
        "    mov.l   @(0, r5), r6       \n"
        "    lds     r6, macl           \n"
        "    mov.l   @(56, r15), r14    \n"
        "    mov.l   @(52, r15), r13    \n"
        "    mov.l   @(48, r15), r12    \n"
        "    mov.l   @(44, r15), r11    \n"
        "    mov.l   @(40, r15), r10    \n"
        "    mov.l   @(36, r15), r9     \n"
        "    mov.l   @(32, r15), r8     \n"
        "    mov.l   @(28, r15), r7     \n"
        "    mov.l   @(24, r15), r6     \n"
        "    mov.l   @(20, r15), r5     \n"
        "    mov.l   @(16, r15), r4     \n"
        "    mov.l   @(12, r15), r3     \n"
        "    mov.l   @(8, r15), r2      \n"
        "    mov.l   @(4, r15), r1      \n"
        "    mov.l   @(0, r15), r0      \n"
        "    add     #84, r15           \n"
        "    rts                        \n"
        "    nop                        \n"
        :: "r" (context->pr), "r" (context)
        );

    while (1);
}

#endif /* CONFIG_DEBUG */

