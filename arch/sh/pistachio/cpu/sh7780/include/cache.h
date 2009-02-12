/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_7780_CACHE_H
#define OKL4_ARCH_SH_7780_CACHE_H

class generic_space_t;

#define CACHE_LINE_SIZE     (32)
#define CACHE_WAY           (4)
#define CACHE_SIZE          (CACHE_LINE_SIZE * CACHE_WAY * 256)

/* Jump to the uncached area */
#define ENTER_P2()                              \
    do {                                        \
        unsigned long dummy;                    \
        __asm__ __volatile__ (                  \
            "    mova    1f, %0         \n"     \
            "    add     %1, %0         \n"     \
            "    jmp     @%0            \n"     \
            "    nop                    \n"     \
            ".blign 4                   \n"     \
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


class sh_cache
{
public:
    static bool full_flush_cheaper(word_t size) {
        //TODO: How much is the best threashold on 7780?
        return (size >= CACHE_SIZE ? true : false);
    }

    /**
     * Invalidates the instruction and operand caches.  Cache blocks are
     * discarded.
     */
    static void invalidate() {
        ENTER_P2();

        u32_t ccr = *(u32_t*)REG_CCR;
        ccr |= REG_CCR_ICI | REG_CCR_OCI;
        *(u32_t*)REG_CCR = ccr;
        UPDATE_REG();

        ENTER_P1();
    }


    /**
     * Selectively invalidates the instruction and operand caches.  Cache
     * blocks are discarded
     *
     * @param attr      the cache attribute
     */
    static void invalidate(word_t attr) {
        if (attr & CACHE_ATTRIB_MASK_I) {
            invalidate_i();
        }
        if (attr & CACHE_ATTRIB_MASK_D) {
            invalidate_d();
        }
    }


    /**
     * Invalidates the entire operand cache.  Cache data is not written back.
     */
    static void invalidate_d() {
        ENTER_P2();

        u32_t ccr = *(u32_t*)REG_CCR;
        ccr |= REG_CCR_OCI;
        *(u32_t*)REG_CCR = ccr;
        UPDATE_REG();

        ENTER_P1();
    }


    /**
     * Invalidates the entire instruction cache.
     */
    static void invalidate_i() {
        ENTER_P2();

        u32_t ccr = *(u32_t*)REG_CCR;
        ccr |= REG_CCR_ICI;
        *(u32_t*)REG_CCR = ccr;
        UPDATE_REG();

        ENTER_P1();
    }

    /**
     * Invalidates the specified address.  The block is not written back.
     *
     * @param vaddr     the address to be invalidated
     * @param log2size  the range of invalidation in log2
     */
    static void invalidate_d_entry(addr_t vaddr, word_t log2size) {
        /* Whole invalidation is better for >32k */
        if (log2size >= 15) {
            invalidate_d();
            return;
        }

        word_t size = 1 << (log2size > 5 ? log2size : 5);

        __asm__ __volatile__ (
            "    mov.l   %0, r1         \n"
            "    mov.l   %1, r0         \n"
            "    add     r1, r0         \n"
            "1b:                        \n"
            "    ocbi    @r1            \n"
            "    add     %2, r1         \n"
            "    cmp.eq  r0, r1         \n"
            "    bf      1b             \n"
            : "+r" (vaddr)
            : "r" (size), "i" (CACHE_LINE_SIZE)
            : "r0", "r1", "memory"
        );
    }

    /**
     * Invalidates the specified address.  The block is written back.
     *
     * @param vaddr     the address to be invalidated
     * @param log2size  the range of invalidation in log2
     */
    static void flush_d_entry(addr_t vaddr, word_t log2size) {
        word_t  size = 1 << (log2size > 5 ? log2size : 5);

        __asm__ __volatile__ (
            "    mov.l   %0, r1         \n"
            "    mov.l   %1, r0         \n"
            "    add     r1, r0         \n"
            "1b:                        \n"
            "    ocbp    @r1            \n"
            "    add     %2, r1         \n"
            "    cmp.eq  r0, r1         \n"
            "    bf      1b             \n"
            : "+r" (vaddr)
            : "r" (size), "i" (CACHE_LINE_SIZE)
            : "r0", "r1", "memory"
        );
    }

    /**
     * Writes back the specified address.
     *
     * @param vaddr     the address to be written back
     * @param log2size  the range of write-back in log2
     */
    static void writeback_d_entry(addr_t vaddr, word_t log2size) {
        word_t  size = 1 << (log2size > 5 ? log2size : 5);

        __asm__ __volatile__ (
            "    mov.l   %0, r1         \n"
            "    mov.l   %1, r0         \n"
            "    add     r1, r0         \n"
            "1b:                        \n"
            "    ocbwb   @r1            \n"
            "    add     %2, r1         \n"
            "    cmp.eq  r0, r1         \n"
            "    bf      1b             \n"
            : "+r" (vaddr)
            : "r" (size), "i" (CACHE_LINE_SIZE)
            : "r0", "r1", "memory"
        );
    }

    /**
     * Invalidates the instruction cache entry.
     *
     * @param vaddr     the address to be invalidated
     * @param log2size  the range of invaliation in log2
     */
    static void invalidate_i_entry(addr_t vaddr, word_t log2size) {
        word_t  size;

        /* Whole invalidation is better for >32k */
        if (log2size >= 15) {
            invalidate_i();
            return;
        }

        size = 1 << (log2size > 5 ? log2size : 5);
        __asm__ __volatile__ (
            "    mov.l   %0, r1         \n"
            "    mov.l   %1, r0         \n"
            "    add     r1, r0         \n"
            "1b:                        \n"
            "    icbi    @r1            \n"
            "    add     %2, r1         \n"
            "    cmp.eq  r0, r1         \n"
            "    bf      1b             \n"
            : "+r" (vaddr)
            : "r" (size), "i" (CACHE_LINE_SIZE)
            : "r0", "r1", "memory"
        );
    }

    /**
     * Invalidates the TLB entry.
     *
     * @param asid      the ASID
     * @param vaddr     the address to be invalidated
     * @param log2size  the range of invalidation in log2
     */
    static void invalidate_tlb_entry(u8_t asid, addr_t vaddr) {
        word_t  addr;
        word_t  data;

        addr = REG_UTLB_ARRAY | REG_UTLB_ASSOC;
        data = asid | (vaddr & 0xFFFFFC00);
        ENTER_P2();
        mapped_reg_write(addr, data);
        UPDATE_REG();
        ENTER_P1();
    }
};

#endif /* OKL4_ARCH_SH_7780_CACHE_H */

