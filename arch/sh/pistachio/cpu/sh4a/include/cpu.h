/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_7780_CPU_H
#define OKL4_ARCH_SH_7780_CPU_H

/* Jump to the uncached area */
#define ENTER_P2()                              \
    do {                                        \
        unsigned long dummy;                    \
        __asm__ __volatile__ (                  \
            "    mova    1f, %0         \n"     \
            "    add     %1, %0         \n"     \
            "    jmp     @%0            \n"     \
            "    nop                    \n"     \
            ".balign 4                  \n"     \
            "1:                         \n"     \
            : "=&z" (dummy)                     \
            : "r" (P2_START - P1_START)         \
        );                                      \
    } while (0)

/* Jump to the cached area */
#define ENTER_P1()                              \
    do {                                        \
        unsigned long dummy;                    \
        __asm__ __volatile__ (                  \
            "    mov.l   1f, %0         \n"     \
            "    jmp     @%0            \n"     \
            "    nop                    \n"     \
            ".balign 4                  \n"     \
            "1: .long 2f                \n"     \
            "2:                         \n"     \
            : "=&r" (dummy)                     \
        );                                      \
    } while (0)


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

