/*
 * Copyright (c) 2006, National ICT Australia (NICTA)
 */
/*
 * Copyright (c) 2007 Open Kernel Labs, Inc. (Copyright Holder).
 * All rights reserved.
 *
 * 1. Redistribution and use of OKL4 (Software) in source and binary
 * forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *     (a) Redistributions of source code must retain this clause 1
 *         (including paragraphs (a), (b) and (c)), clause 2 and clause 3
 *         (Licence Terms) and the above copyright notice.
 *
 *     (b) Redistributions in binary form must reproduce the above
 *         copyright notice and the Licence Terms in the documentation and/or
 *         other materials provided with the distribution.
 *
 *     (c) Redistributions in any form must be accompanied by information on
 *         how to obtain complete source code for:
 *        (i) the Software; and
 *        (ii) all accompanying software that uses (or is intended to
 *        use) the Software whether directly or indirectly.  Such source
 *        code must:
 *        (iii) either be included in the distribution or be available
 *        for no more than the cost of distribution plus a nominal fee;
 *        and
 *        (iv) be licensed by each relevant holder of copyright under
 *        either the Licence Terms (with an appropriate copyright notice)
 *        or the terms of a licence which is approved by the Open Source
 *        Initative.  For an executable file, "complete source code"
 *        means the source code for all modules it contains and includes
 *        associated build and other files reasonably required to produce
 *        the executable.
 *
 * 2. THIS SOFTWARE IS PROVIDED ``AS IS'' AND, TO THE EXTENT PERMITTED BY
 * LAW, ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED.  WHERE ANY WARRANTY IS
 * IMPLIED AND IS PREVENTED BY LAW FROM BEING DISCLAIMED THEN TO THE
 * EXTENT PERMISSIBLE BY LAW: (A) THE WARRANTY IS READ DOWN IN FAVOUR OF
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT) AND (B) ANY LIMITATIONS PERMITTED BY LAW (INCLUDING AS TO
 * THE EXTENT OF THE WARRANTY AND THE REMEDIES AVAILABLE IN THE EVENT OF
 * BREACH) ARE DEEMED PART OF THIS LICENCE IN A FORM MOST FAVOURABLE TO
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT). IN THE LICENCE TERMS, "PARTICIPANT" INCLUDES EVERY
 * PERSON WHO HAS CONTRIBUTED TO THE SOFTWARE OR WHO HAS BEEN INVOLVED IN
 * THE DISTRIBUTION OR DISSEMINATION OF THE SOFTWARE.
 *
 * 3. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY OTHER PARTICIPANT BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Description:   Functions which manipulate the ARM1136JS cache
 * Author: Carl van Schaik, June 2006
 */

#ifndef __ARCH__ARM__ARM1136JS_CACHE_H_
#define __ARCH__ARM__ARM1136JS_CACHE_H_

#include <debug.h>
#include <cpu/syscon.h>
#include <kernel/arch/asm.h>

#define           ARM1136_HARVARD
#define           ARM1136_ERRATA

#define BTB_FLUSH_SIZE 8
// Ensure page table lookups occur before flushing virtual address ranges
//#define CACHE_NEED_PGENT
// Use cache fault handling instead of pagtable lookups
#define CACHE_HANDLE_FAULT

#ifdef ARM1136_ERRATA
#define ERRATA_NOP  "   nop     \n"
#else
#define ERRATA_NOP
#endif

#ifndef ARM1136_HARVARD
#error unsupported
#endif

class generic_space_t;


#define ICACHE_SIZE         (16 * 1024)
#define ICACHE_LINE_SIZE    32
#define ICACHE_WAYS         4
#define DCACHE_SIZE          ICACHE_SIZE
#define DCACHE_LINE_SIZE     ICACHE_LINE_SIZE
#define DCACHE_WAYS         ICACHE_WAYS

#define ARM_CPU_HAS_TLBLOCK     1
#define ARM_CPU_HAS_L2_CACHE    1

class arm_cache
{
public:
    static bool full_flush_cheaper(word_t size)
    {
        return (size > 32768 ? true : false);
    }

    static inline void cache_flush(void)
    {
        cache_flush_i();
        cache_flush_d();
    }

    static inline void cache_flush(word_t attr)
    {
        if (attr & CACHE_ATTRIB_MASK_I) /* I-cache */
        {
            cache_flush_i();
        }
        if (attr & CACHE_ATTRIB_MASK_D) /* D-cache */
        {
            cache_flush_d();
        }
    }

    static inline void cache_flush_d(void)
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            ERRATA_NOP
            "   mcr     p15, 0, %0, c7, c14, 0  \n" /* clean+invalidate data cache */
            ERRATA_NOP
            ERRATA_NOP
            "   mcr     p15, 0, %0, c7, c10, 4  \n" /* drain write buffer */
        :: "r" (zero)
        );
    }

    static inline void cache_flush_i(void)
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr     p15, 0, %0, c7, c5, 0   \n" /* invalidate instruction cache */
        :: "r" (zero)
        );
    }

    static inline void cache_drain_wb(void)
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr     p15, 0, %0, c7, c10, 4  \n" /* drain write buffer */
        :: "r" (zero)
        );
    }

    static inline void cache_flush_i_ent(addr_t vaddr, word_t log2size)
    {
        if (log2size >= 15) {   /* for >32k, whole flush is better */
            cache_flush_i();
            return;
        }
        word_t size = 1UL << (log2size > 5 ? log2size : 5);
        vaddr = addr_align(vaddr, size);
        ASSERT(DEBUG, DCACHE_LINE_SIZE == 32);

        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* clean and invalidate cache */
            __asm__ __volatile (
#ifdef ARM1136_ERRATA
                    "   orr         r2, %0, #0xc0000000         \n" /* ARM1136 erratum */
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way3 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way2 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way1 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way0 */
#else
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c5, 1       \n" /* clean/invalidate - MVA */
                    ERRATA_NOP
#endif
                    :: "r" (i)
                    : "r2"
                    );

#ifdef ARM1136_ERRATA
            for (word_t j = 0; j < (DCACHE_LINE_SIZE); j += BTB_FLUSH_SIZE)
            {
                __asm__ __volatile (
                    "   mcr         p15, 0, %0, c7, c5, 7       \n" /* Flush BTB - way/set*/
                    :: "r" (i+j)
                    );
            }
#endif
        }
#ifndef ARM1136_ERRATA
        {
            word_t zero = 0;
            __asm__ __volatile (
                    "   mcr         p15, 0, %0, c7, c5, 6       \n" /* Flush BTB */
                    :: "r" (zero)
                    );
        }
#endif
    }

    static inline void cache_flush_d_ent(addr_t vaddr, word_t log2size)
    {
        if (log2size >= 15) {   /* for >32k, whole flush is better */
            cache_flush_d();
            return;
        }
        word_t size = 1UL << (log2size > 5 ? log2size : 5);
        vaddr = addr_align(vaddr, size);

        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* clean and invalidate D cache */
            __asm__ __volatile (
                    ERRATA_NOP
#if 0
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c14, 1      \n" /* clean/invalidate data - MVA */
                    ERRATA_NOP
#else
                    "   orr         r2, %0, #0xc0000000         \n" /* ARM1136 erratum */
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way3 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way2 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way1 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way0 */
#endif
                    ERRATA_NOP
                    :: "r" (i)
                    : "r2"
                    );
        }

        cache_drain_wb();
    }

    static inline void cache_flush_ent(addr_t vaddr, word_t log2size)
    {
        if (log2size >= 15) {   /* for >32k, whole flush is better */
            cache_flush();
            return;
        }
        word_t size = 1UL << (log2size > 5 ? log2size : 5);
        vaddr = addr_align(vaddr, size);
        ASSERT(DEBUG, DCACHE_LINE_SIZE == 32);

        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* clean and invalidate cache */
            __asm__ __volatile (
                    ERRATA_NOP
#if 0
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c14, 1      \n" /* clean/invalidate data - MVA */
                    ERRATA_NOP
#else
                    "   orr         r2, %0, #0xc0000000         \n" /* ARM1136 erratum */
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way3 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way2 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way1 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c14, 2      \n" /* Invalidate D way0 */
#endif
                    ERRATA_NOP
#ifdef ARM1136_ERRATA
                    "   orr         r2, %0, #0xc0000000         \n" /* ARM1136 erratum */
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way3 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way2 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way1 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way0 */
#else
                    "   mcr         p15, 0, %0, c7, c5, 1       \n" /* clean/invalidate - MVA */
                    ERRATA_NOP
#endif
                    :: "r" (i)
                    : "r2"
                    );
#ifdef ARM1136_ERRATA
            for (word_t j = 0; j < (DCACHE_LINE_SIZE); j += BTB_FLUSH_SIZE)
            {
                __asm__ __volatile (
                    "   mcr         p15, 0, %0, c7, c5, 7       \n" /* Flush BTB - way/set*/
                    :: "r" (i+j)
                    );
            }
#endif
        }
#ifndef ARM1136_ERRATA
        {
            word_t zero = 0;
            __asm__ __volatile (
                    "   mcr         p15, 0, %0, c7, c5, 6       \n" /* Flush BTB */
                    :: "r" (zero)
                    );
        }
#endif

        cache_drain_wb();
    }

    static inline void cache_flush_ent_mva(addr_t vaddr, word_t log2size)
    {
        if (log2size >= 15) {   /* for >32k, whole flush is better */
            cache_flush();
            return;
        }
        word_t size = 1UL << (log2size > 5 ? log2size : 5);
        vaddr = addr_align(vaddr, size);
#if 0
#if ((1<<5) != DCACHE_LINE_SIZE)
#error lines size not 32... fixme
#endif
#endif

        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* clean and invalidate cache */
            __asm__ __volatile (
                    ERRATA_NOP
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c14, 1      \n" /* clean/invalidate data - MVA */
                    ERRATA_NOP
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c5, 1       \n" /* clean/invalidate - MVA */
                    ERRATA_NOP
                    :: "r" (i)
                    : "r2"
                    );
        }
        {
            word_t zero = 0;
            __asm__ __volatile (
                    "   mcr         p15, 0, %0, c7, c5, 6       \n" /* Flush BTB */
                    :: "r" (zero)
                    );
        }

        cache_drain_wb();
    }

    /* These require DCACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_clean_invalidate_dlines(addr_t vaddr, word_t size)
    {
        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* clean and invalidate D cache */
            __asm__ __volatile (
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c14, 1      \n" /* clean/invalidate data - MVA */
                    ERRATA_NOP
                    :: "r" (i)
                    : "r2"
                    );
        }

        cache_drain_wb();
    }

    /* These require DCACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_clean_dlines(addr_t vaddr, word_t size)
    {
        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* clean D cache */
            __asm__ __volatile (
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c10, 1      \n" /* clean data - MVA */
                    ERRATA_NOP
                    :: "r" (i)
                    );
        }

        cache_drain_wb();
    }

    /* These require DCACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_invalidate_dlines(addr_t vaddr, word_t size)
    {
        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* invalidate D cache */
            __asm__ __volatile (
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c6, 1       \n" /* invalidate data - MVA */
                    ERRATA_NOP
                    :: "r" (i)
                    );
        }
    }

    /* These require DCACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_invalidate_ilines(addr_t vaddr, word_t size)
    {
        for (word_t i = (word_t)vaddr; i < ((word_t)vaddr + size); i += DCACHE_LINE_SIZE)
        {
            /* invalidate I cache */
            __asm__ __volatile (
#ifdef ARM1136_ERRATA
                    "   orr         r2, %0, #0xc0000000         \n" /* ARM1136 erratum */
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way3 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way2 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way1 */
                    "   sub         r2, r2, #0x40000000         \n"
                    "   mcr         p15, 0, r2, c7, c5, 2       \n" /* Invalidate I way0 */
#else
                    ERRATA_NOP
                    "   mcr         p15, 0, %0, c7, c5, 1       \n" /* clean/invalidate - MVA */
                    ERRATA_NOP
#endif
                    :: "r" (i)
#ifdef ARM1136_ERRATA
                    : "r2"
#endif
                    );
#ifdef ARM1136_ERRATA
            for (word_t j = 0; j < (DCACHE_LINE_SIZE); j += BTB_FLUSH_SIZE)
            {
                __asm__ __volatile (
                    "   mcr         p15, 0, %0, c7, c5, 7       \n" /* Flush BTB - way/set*/
                    :: "r" (i+j)
                    );
            }
#endif
        }
#ifndef ARM1136_ERRATA
        {
            word_t zero = 0;
            __asm__ __volatile (
                    "   mcr         p15, 0, %0, c7, c5, 6       \n" /* Flush BTB */
                    :: "r" (zero)
                    );
        }
#endif
    }

    static void cache_flush_range_attr(generic_space_t *space,
            addr_t vaddr, word_t size, word_t attr);

    static inline void cache_flush_debug(void)
    {
        printf("About to cache flush... ");
        cache_flush();
        printf("done.\n");
    }

    static inline void tlb_flush(void)
    {
        /* Flush I&D TLB */
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr    p15, 0, %0, c8, c7, 0    \n"
            :: "r" (zero));
    }

    static inline void tlb_flush_asid(u8_t asid)
    {
        /* Flush I&D TLB */
        word_t val = asid;
        __asm__ __volatile__ (
            "   mcr    p15, 0, %0, c8, c7, 2    \n"
            :: "r" (val));
    }

    static inline void tlb_flush_ent(u8_t asid, addr_t vaddr, word_t log2size)
    {
        vaddr = addr_align(vaddr, PAGE_SIZE_4K);
        word_t a = (word_t)vaddr;

        a = (a & (~0x3ffUL)) | asid;

        for (; a < (word_t)vaddr + (1UL << log2size); a += PAGE_SIZE_4K)
        {
#ifdef ARM1136_HARVARD
            __asm__ __volatile__ (
                "    mcr     p15, 0, %0, c8, c6, 1    \n"       /* Invalidate D TLB entry */
                ERRATA_NOP
                ERRATA_NOP
                "    mcr     p15, 0, %0, c8, c5, 1    \n"       /* Invalidate I TLB entry */
                ERRATA_NOP
                ERRATA_NOP
            :: "r" (a));
#else
            __asm__ __volatile__ (
                "    mcr     p15, 0, %0, c8, c7, 1    \n"       /* Invalidate TLB entry */
            :: "r" (a));
#endif
        }
    }

    static inline void tlb_flush_debug(void)
    {
        printf("About to TLB flush... ");
        tlb_flush();
        printf("done.\n");
    }

    static inline void cache_enable()
    {
UNIMPLEMENTED();
    }

    static inline void cache_invalidate_d_line(word_t target)
    {
UNIMPLEMENTED();
#if 0
        __asm__ __volatile__ (
            "mcr p15, 0, %0, c7, c6, 1 \n"
            :: "r" (target));
        CPWAIT;
#endif
    }

    static inline void cache_invalidate_i()
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "mcr p15, 0, %0, c7, c5, 0 \n"
            :: "r" (zero)
        );
    }

    static inline void cache_invalidate_d()
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "mcr p15, 0, %0, c7, c6, 0 \n"
            :: "r" (zero)
        );
    }

    static inline void cache_clean(word_t target)
    {
UNIMPLEMENTED();
#if 0
        __asm__ __volatile__ (
            "mcr p15, 0, %0, c7, c10, 1 \n"
        :: "r" (target));
        CPWAIT;
#endif
    }

    static inline void cache_drain_write_buffer(void)
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr     p15, 0, %0, c7, c10, 4  \n" /* drain write buffer */
            :: "r" (zero)
        );
    }

    static inline void lock_tlb_addr(addr_t vaddr)
    {
        __asm__ __volatile__ (
            "   mrc     p15, 0, r0, c10, c0, 0          ;"  /* Get lockdown register    */
            "   orr     r0, r0, #1                      ;"  /* Set preserve bit         */
            "   mcr     p15, 0, r0, c10, c0, 0          ;"  /* Write lockdown register  */
            "   mcr     p15, 0, " _(vaddr) ", c8, c7, 1 ;"  /* - Invalidate TLB entry   */
            "   ldr     r0, [" _(vaddr) "]              ;"  /* - Touch the entry        */
            "   mrc     p15, 0, r0, c10, c0, 0          ;"  /* Get lockdown register    */
            "   bic     r0, r0, #1                      ;"  /* Clear preserve bit       */
            "   mcr     p15, 0, r0, c10, c0, 0          ;"  /* Write lockdown register  */
#if defined(__GNUC__)
            :: [vaddr] "r" (vaddr)
            : "r0"
#endif
        );
    }

    /* Lock address range from vstart to vstart+size-1 in cache way (way) [0..3]
     * Must be called from uncached code */
    static inline void lock_icache_range(addr_t vstart, word_t size, word_t way)
    {
        UNIMPLEMENTED();
    }

    static inline void unlock_icache()
    {
        word_t unlock = 0x0000fff0;

        __asm__ __volatile__ (
            "   mcr     p15, 0, " _(unlock) ", c9, c0, 1    "/* Set the value of c9 (i-cache lockdown register) */
#if defined(__GNUC__)
            :: [unlock] "r" (unlock)
#endif
        );
    }

    static inline void prefetch_icache(addr_t vaddr)
    {
        vaddr = addr_align(vaddr, ICACHE_LINE_SIZE);
        __asm__ __volatile__ (
            "   mcr     p15, 0, %0, c7, c13, 1  \n"     /* Prefetch Instruction Cache Line */
            :: "r" ((word_t)vaddr)
        );
    }
};

#endif /* __ARCH__ARM__ARM1136JS_CACHE_H_ */
