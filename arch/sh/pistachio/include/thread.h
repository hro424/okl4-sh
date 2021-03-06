
/*
 * Thread switch and interrupt stack frames
 */

#ifndef OKL4_ARCH_SH_KERNEL_THREAD_H
#define OKL4_ARCH_SH_KERNEL_THREAD_H

#define SH_USER_FLAGS_MASK  0x100083F3

#if !defined(ASSEMBLY)
class sh_irq_context_t
{
public:
    u32_t   r0;
    u32_t   r1;
    u32_t   r2;
    u32_t   r3;
    u32_t   r4;
    u32_t   r5;
    u32_t   r6;
    u32_t   r7;
    u32_t   r8;
    u32_t   r9;
    u32_t   r10;
    u32_t   r11;
    u32_t   r12;
    u32_t   r13;
    u32_t   r14;
    u32_t   mach;
    u32_t   macl;
    u32_t   pr;
    u32_t   sr;
    u32_t   pc;
    u32_t   sp;
};
#endif

#define SIZEOF_CONTEXT          84
#define OFS_CONTEXT_MACH        60
#define OFS_CONTEXT_PR          68

#endif /* OKL4_ARCH_SH_KERNEL_THREAD_H */
