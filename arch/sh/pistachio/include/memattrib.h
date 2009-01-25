/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_MEMATTRIB_H
#define OKL4_ARCH_SH_MEMATTRIB_H

#include <l4/arch/cache_attribs.h>

enum memattrib_e {
    writethrough =          CACHE_ATTRIB_WRITETHROUGH,
    writeback =             CACHE_ATTRIB_WRITEBACK,
    uncached =              CACHE_ATTRIB_UNCACHED,

    writethrough_shared =   CACHE_ATTRIB_WRITETHROUGH | CACHE_ATTRIB_SHARED,
    writeback_shared =      CACHE_ATTRIB_WRITEBACK | CACHE_ATTRIB_SHARED,
    uncached_shared =       CACHE_ATTRIB_UNCACHED | CACHE_ATTRIB_SHARED,

#if defined(CONFIG_DEFAULT_CACHE_ATTR_WB)
    l4default =             writeback,
#else
    l4default =             writethrough,
#endif
};

#endif /* OKL4_ARCH_SH_MEMATTRIB_H */
