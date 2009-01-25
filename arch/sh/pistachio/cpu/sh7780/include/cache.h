/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_7780_CACHE_H
#define OKL4_ARCH_SH_7780_CACHE_H

class generic_space_t;

#define CACHE_SIZE      (32 * 1024)

#define VALIDATE_REGISTER()             \
    u32_t   __dummy = 0xdeadbeef;       \
    __asm__ __volatile__ ("\t"          \
        "icbi @%0           \n\t"       \
        "synco              \n"         \
        : : "r" (__dummy) : "memory")

#define ENTER_P2()

#define ENTER_P1()

class sh_cache
{
public:
    static bool full_flush_cheaper(word_t size) {
        //TODO: How much is the best threashold on 7780?
        return (size >= CACHE_SIZE ? true : false);
    }

    /**
     * Invalidates the instruction and operand caches.
     */
    static void invalidate() {
        ENTER_P2();

        u32_t ccr = *(u32_t*)REG_CCR;
        ccr |= REG_CCR_ICI | REG_CCR_OCI;
        *(u32_t*)REG_CCR = ccr;
        VALIDATE_REGISTER();

        ENTER_P1();
    }


    /**
     * Selectively invalidates the instruction and operand caches.
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
        VALIDATE_REGISTER();

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
        VALIDATE_REGISTER();

        ENTER_P1();
    }

    static void invalidate_d_entry(addr_t vaddr, word_t size) {
        __asm__ __volatile__ (
            "   ocbi    @r1             \n"
            :::
        );
    }

    static void clean_d_entry(addr_t vaddr, word_t size) {
        __asm__ __volatile__ (
            "   ocbp    @r1             \n"
            :::
        );
    }

    static void writeback_d_entry(addr_t vaddr, word_t size) {
        __asm__ __volatile__ (
            "   ocbwb   @r1             \n"
            :::
        );
    }

    static void invalidate_i_entry(addr_t vaddr) {
        __asm__ __volatile__ (
            "   icbi    @r1             \n"
            :::
        );
    }

    static void flush() {
        invalidate();
    }

    static void flush(word_t attr) {
        invalidate(attr);
    }

    static void flush_range(addr_t vaddr, word_t size, word_t attr) {
    }
};

#endif /* OKL4_ARCH_SH_7780_CACHE_H */

