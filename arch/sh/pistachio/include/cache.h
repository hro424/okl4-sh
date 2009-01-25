/* $Id$ */

/**
 * @file    arch/sh/pistachio/include/cache.h
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_CACHE_H
#define OKL4_ARCH_SH_CACHE_H

#define CACHE_CTL_MASKINNER     CACHE_CTL_MASKL1
#define CACHE_CTL_MASKOUTER     (CACHE_CTL_MASKALLLS & ~CACHE_CTL_MASKINNER)

#include <cpu/cache.h>

class generic_space_t;

class cache_t
{
public:
    /**
     *  Flushes all instruction cache and/or data cache.
     *
     *  @param attr     the cache attribute (i.e. I-cache, D-cache)
     *  @param offset   for resuming the iteration over MRs
     */
    static void flush_all_attribute(cacheattr_e attr, word_t* offset);

    /**
     *  Flushes the specified range of cache.
     *
     *  @param space    the space ID
     *  @param start    the start address to be flushed
     *  @param end      the end address to be flushed
     *  @param attrr    the cache attribute (i, d, or both)
     */
    static void flush_range_attribute(generic_space_t* space, addr_t start,
                                      addr_t end, cacheattr_e attr);

    /**
     *  Invalidates the cache.
     *
     *  @param start    the start address to be invalidated
     *  @param size     the size of the address space to be invalidated
     */
    static void invalidate_virtual_alias(addr_t start, word_t size);

    /**
     *  Check if flushing all the cache is cheaper than flushing ranges.
     *
     *  @param size
     */
    static bool full_flush_cheaper(word_t size);
};

INLINE void
cache_t::flush_all_attribute(cacheattr_e attr, word_t* offset)
{
    sh_cache::flush(attr);
}

INLINE void
cache_t::flush_range_attribute(generic_space_t* space, addr_t start,
                               addr_t end, cacheattr_e attr)
{
    word_t size = (word_t)end - (word_t)start;
    sh_cache::flush_range(start, size, attr);
}

INLINE void
cache_t::invalidate_virtual_alias(addr_t start, word_t size)
{
}

INLINE bool
cache_t::full_flush_cheaper(word_t size)
{
    return sh_cache::full_flush_cheaper(size);
}


//#include <soc/arch/soc.h>

INLINE void
arch_outer_cache_full_op(enum cacheattr_e attrib)
{
#ifdef CONFIG_HAS_SOC_CACHE
    soc_cache_full_op(attrib);
#endif
}

INLINE void
arch_outer_cache_range_op(addr_t pa, word_t size, enum cacheattr_e attrib)
{
#ifdef CONFIG_HAS_SOC_CACHE
    soc_cache_range_op_by_pa(pa, size, attrib);
#endif
}

INLINE void
arch_outer_drain_write_buffer(void)
{
#ifdef CONFIG_HAS_SOC_CACHE
    soc_cache_drain_write_buffer();
#endif
}

#endif /* OKL4_ARCH_SH_CACHE_H */

