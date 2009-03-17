/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_HWSPACE_H
#define OKL4_ARCH_SH_HWSPACE_H

#include <kernel/config.h>
#include <arch/globals.h>

/**
 * Converts the virtual address to a physical address.
 * Usable only between 0x80000000 and 0xBFFFFFFF.
 *
 * @param vaddr     the virtual address to be converted 
 */
template<typename T> INLINE T
virt_to_phys(T x)
{
    return (T)((u32_t)x & 0x1FFFFFFF);
}

/**
 * Converts the physical address to a virtual address
 *
 * @param paddr     the physical address to be converted
 */
template<typename T> INLINE T
phys_to_virt(T x)
{
    return (T)((u32_t)x - get_globals()->phys_addr_ram + VIRT_ADDR_RAM + IPL_OFFSET);
}

template<typename T> INLINE T
ram_to_virt(T x)
{
    return phys_to_virt(x);
}

#endif /* OKL4_ARCH_SH_HWSPACE_H */
