/**
 * @brief   SH4A cache management
 * @since   December 2008
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#ifndef OKL4_ARCH_SH_7780_CACHE_H
#define OKL4_ARCH_SH_7780_CACHE_H

#include <cpu/cpu.h>

class generic_space_t;

#define CACHE_LINE_SIZE     (32)
#define CACHE_WAYS          (4)
#define CACHE_ENTRIES       (256)
#define CACHE_WAY_SIZE      CACHE_LINE_SIZE * CACHE_ENTRIES
#define CACHE_SIZE          CACHE_WAY_SIZE * CACHE_WAYS

#define ROUND_UP(var, x)    (((var) + x - 1) / x * x)

class sh_cache
{
public:
    static bool full_flush_cheaper(word_t size) {
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
     * Invalidates the instruction cache entry.
     *
     * @param vaddr     the address to be invalidated
     * @param log2size  the range of invaliation in log2
     */
    static void invalidate_i(addr_t vaddr, word_t log2size) {
        /* Whole invalidation is better for >32k */
        if (log2size >= 15) {
            invalidate_i();
            return;
        }

        word_t size = 1 << (log2size > 5 ? log2size : 5);
        invalidate_i_range(vaddr, size);
    }

    static void invalidate_i_range(addr_t vaddr, word_t size) {
        __asm__ __volatile__ (
            "    mov     %0, r1         \n"
            "    mov     %1, r0         \n"
            "    add     r1, r0         \n"
            "1:                         \n"
            "    icbi    @r1            \n"
            "    add     %2, r1         \n"
            "    cmp/eq  r0, r1         \n"
            "    bf      1b             \n"
            : "+r" (vaddr)
            : "r" (size), "i" (CACHE_LINE_SIZE)
            : "r0", "r1", "memory"
        );
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
     * Invalidates the specified address.  The block is not written back.
     *
     * @param vaddr     the address to be invalidated
     * @param log2size  the range of invalidation in log2
     */
    static void invalidate_d(addr_t vaddr, word_t log2size) {
        /* Whole invalidation is better for >32k */
        if (log2size >= 15) {
            invalidate_d();
            return;
        }

        word_t size = 1 << (log2size > 5 ? log2size : 5);
        invalidate_d_range(vaddr, size);
    }

    static void invalidate_d_range(addr_t vaddr, word_t size) {
        __asm__ __volatile__ (
            "    mov     %0, r1         \n"
            "    mov     %1, r0         \n"
            "    add     r1, r0         \n"
            "1:                         \n"
            "    ocbi    @r1            \n"
            "    add     %2, r1         \n"
            "    cmp/eq  r0, r1         \n"
            "    bf      1b             \n"
            : "+r" (vaddr)
            : "r" (size), "i" (CACHE_LINE_SIZE)
            : "r0", "r1", "memory"
        );
    }

    /**
     * Flush all caches.
     */
    static void flush() {
        invalidate_i();
        flush_d();
    }

    /**
     * Selectively flushes the instruction and/or operand caches.
     *
     * @param attr      the cache attribute
     */
    static void flush(word_t attr) {
        if (attr & CACHE_ATTRIB_MASK_I) {
            invalidate_i();
        }
        if (attr & CACHE_ATTRIB_MASK_D) {
            flush_d();
        }
    }

    static void flush_range(addr_t start, word_t size, word_t attr) {
        if (attr & CACHE_ATTRIB_MASK_I) {
            if (size > CACHE_SIZE) {
                invalidate_i();
            }
            else {
                invalidate_i_range(start, ROUND_UP(size, CACHE_LINE_SIZE));
            }
        }
        if (attr & CACHE_ATTRIB_MASK_D) {
            if (size > CACHE_SIZE) {
                flush_d();
            }
            else {
                flush_d_range(start, ROUND_UP(size, CACHE_LINE_SIZE));
            }
        }
    }

    /**
     * Writes back and invalidates the operand cache.
     */
    static void flush_d() {
        word_t  addr0, addr1, addr2, addr3, end;

        addr0 = VIRT_ADDR_BASE;
        addr1 = addr0 + CACHE_WAY_SIZE;
        addr2 = addr1 + CACHE_WAY_SIZE;
        addr3 = addr2 + CACHE_WAY_SIZE;
        end = VIRT_ADDR_BASE + CACHE_WAY_SIZE;

        /*
         * NOTE: Use the 32KiB range at VIRT_ADDR_BASE, because
         * (1)  TLB miss is not thrown because P1 is a non-TLB area.
         * (2)  movca.l writes r0 to a cache block, not directly to memory,
         *      if write-back cache is enabled.
         * (3)  ocbi immediately invalidates the cache block, so that the
         *      cache block is not written back to memory.
         */
        /*
         * NOTE: flush 4 entries at the same time, because SH-4A's cache is 
         * 4-way set associative.
         */
        do {
            sh_cpu::cli();
            __asm__ __volatile__ (
                "    movca.l     r0, @%0    \n"
                "    movca.l     r0, @%1    \n"
                "    movca.l     r0, @%2    \n"
                "    movca.l     r0, @%3    \n"
                "    ocbi        @%0        \n"
                "    ocbi        @%1        \n"
                "    ocbi        @%2        \n"
                "    ocbi        @%3        \n"
                :
                : "r" (addr0), "r" (addr1), "r" (addr2), "r" (addr3)
            );
            addr0 += CACHE_LINE_SIZE;
            addr1 += CACHE_LINE_SIZE;
            addr2 += CACHE_LINE_SIZE;
            addr3 += CACHE_LINE_SIZE;
            sh_cpu::sti();
        } while (addr0 < end);
    }


    static void flush_d_range(addr_t vaddr, word_t size) {
        __asm__ __volatile__ (
            "    mov     %0, r1         \n"
            "    mov     %1, r0         \n"
            "    add     r1, r0         \n"
            "1:                         \n"
            "    ocbp    @r1            \n"
            "    add     %2, r1         \n"
            "    cmp/eq  r0, r1         \n"
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
    static void flush_d(addr_t vaddr, word_t log2size) {
        if (log2size >= 15) {
            flush_d();
            return;
        }

        word_t  size = 1 << (log2size > 5 ? log2size : 5);

        flush_d_range(vaddr, size);
    }
    
    /**
     * Writes back the specified address.
     *
     * @param vaddr     the address to be written back
     * @param log2size  the range of write-back in log2
     */
    static void writeback_d(addr_t vaddr, word_t log2size) {
        word_t  size = 1 << (log2size > 5 ? log2size : 5);

        __asm__ __volatile__ (
            "    mov     %0, r1         \n"
            "    mov     %1, r0         \n"
            "    add     r1, r0         \n"
            "1:                         \n"
            "    ocbwb   @r1            \n"
            "    add     %2, r1         \n"
            "    cmp/eq  r0, r1         \n"
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

        addr = REG_UTLB_ADDRESS | REG_UTLB_ASSOC;
        data = asid | ((word_t)vaddr & 0xFFFFFC00);
        ENTER_P2();
        mapped_reg_write(addr, data);
        UPDATE_REG();
        ENTER_P1();
    }
};

#endif /* OKL4_ARCH_SH_7780_CACHE_H */

