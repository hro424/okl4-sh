/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_HWSPACE_H
#define OKL4_ARCH_SH_HWSPACE_H

#include <kernel/config.h>
#include <arch/globals.h>

template<typename T> INLINE T
virt_to_ram(T x)
{
    return (T)((u32_t)x - VIRT_ADDR_RAM + get_globals()->phys_addr_ram);
}

template<typename T> INLINE T
ram_to_virt(T x)
{
    return (T)((u32_t)x + VIRT_ADDR_RAM - get_globals()->phys_addr_ram);
}

template<typename T> INLINE T
virt_to_phys(T x)
{
    return virt_to_ram(x);
}

template<typename T> INLINE T
phys_to_virt(T x)
{
    return ram_to_virt(x);
}

#endif /* OKL4_ARCH_SH_HWSPACE_H */
