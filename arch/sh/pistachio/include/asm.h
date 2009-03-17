#ifndef OKL4_ARCH_SH_ASM_H
#define OKL4_ARCH_SH_ASM_H

#if defined(ASSEMBLY)

#define SWITCH_BANK_1_TO_0              \
    stc     sr, r10;                    \
    /* Make the mask to switch the bank */  \
    mov     #0x20, r11;                 \
    shll8   r11;                        \
    shll16  r11;                        \
    not     r11, r11;                   \
    /* Switch bank 1 to bank 0 */       \
    and     r11, r10;                   \
    ldc     r10, sr

#define SAVE_CONTEXT                    \
    stc.l   sgr, @-r15;                 \
    stc.l   spc, @-r15;                 \
    stc.l   ssr, @-r15;                 \
    sts.l   pr, @-r15;                  \
    sts.l   macl, @-r15;                \
    sts.l   mach, @-r15;                \
    PUSH(r14);                          \
    PUSH(r13);                          \
    PUSH(r12);                          \
    PUSH(r11);                          \
    PUSH(r10);                          \
    PUSH(r9);                           \
    PUSH(r8);                           \
    /* The current bank is 1.  */       \
    SWITCH_BANK_1_TO_0;                 \
    /* Push the rest of the registers on to the stack */    \
    PUSH(r7);                           \
    PUSH(r6);                           \
    PUSH(r5);                           \
    PUSH(r4);                           \
    PUSH(r3);                           \
    PUSH(r2);                           \
    PUSH(r1);                           \
    PUSH(r0)


#define RESTORE_CONTEXT                 \
    POP(r0);                            \
    POP(r1);                            \
    POP(r2);                            \
    POP(r3);                            \
    POP(r4);                            \
    POP(r5);                            \
    POP(r6);                            \
    POP(r7);                            \
    POP(r8);                            \
    POP(r9);                            \
    POP(r10);                           \
    POP(r11);                           \
    POP(r12);                           \
    POP(r13);                           \
    POP(r14);                           \
    lds.l   @r15+, mach;                \
    lds.l   @r15+, macl;                \
    lds.l   @r15+, pr;                  \
    ldc.l   @r15+, ssr;                 \
    ldc.l   @r15+, spc;                 \
    /*XXX: Compile error: ldc.l   @r15+, sgr; */                \
    mov.l   @r15, r15

#define EXP_VECTOR(offset, handler)     \
    .balign offset;                     \
    mov.l   1f, r1;                     \
    jmp     @r1;                        \
    nop;                                \
    .balign 4;                          \
1:  .long   handler;


#endif // ASSEMBLY

#endif /* OKL4_ARCH_SH_ASM_H */

