
/*
 * Thread switch and interrupt stack frames
 */

#ifndef OKL4_ARCH_SH_KERNEL_THREAD_H
#define OKL4_ARCH_SH_KERNEL_THREAD_H

#if !defined(ASSEMBLY)
class sh_context_t
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
#define OFFSET_CONTEXT_MACH     60

#endif /* OKL4_ARCH_SH_KERNEL_THREAD_H */
