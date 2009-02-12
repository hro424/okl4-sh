/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_7780_CPU_H
#define OKL4_ARCH_SH_7780_CPU_H

class sh_cpu
{
public:
    static void cli() {
        u32_t   tmp;
        __asm__ __volatile__ (
            "    stc     sr, %0     \n"
            "    or      %1, %0     \n"
            "    ldc     %0, sr     \n"
            : "=&z" (tmp)
            : "r" (0x10000000)
            : "memory");
    }

    static void sti() {
        u32_t   tmp;
        __asm__ __volatile__ (
            "    stc     sr, %0     \n"
            "    and     %1, %0     \n"
            "    ldc     %0, sr     \n"
            : "=&z" (tmp)
            : "r" (0xefffffff)
            : "memory");
    }

    static void sleep() {
        __asm__ __volatile__ ("sleep");
    }

};

#endif /* OKL4_ARCH_SH_7780_CPU_H */

