/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_GLOBALS_H
#define OKL4_ARCH_SH_GLOBALS_H

#define SH_GLOBAL_BASE          VIRT_ADDR_RAM

#if !defined(ASSEMBLY)

class clist_t;
class pgent_t;
class space_t;
class tcb_t;

typedef struct globals
{
    tcb_t*      current_tcb;
    clist_t*    current_clist;
    space_t*    kernel_space;
    tcb_t*      current_schedule;
#if defined(CONFIG_TRACEPOINTS)
    bool        tracepoints_enabled;
#endif
    word_t      phys_addr_ram;
} globals_t;

INLINE PURE globals_t*
get_globals()
{
    return (globals_t*)SH_GLOBAL_BASE;
}

#endif

#endif /* OKL4_ARCH_SH_GLOBALS_H */
