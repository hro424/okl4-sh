/* $Id$ */

#ifndef OKL4_ARCH_SH_RESOURCE_FUNCTIONS_H
#define OKL4_ARCH_SH_RESOURCE_FUNCTIONS_H

#include <arch/resources.h>
#include <tcb.h>

INLINE void
thread_resources_t::init(tcb_t* tcb)
{
    tcb->resource_bits->init();
}

INLINE void
thread_resources_t::free(tcb_t* tcb, kmem_resource_t* kresource)
{
}

INLINE bool
thread_resources_t::control(tcb_t* tcb, tc_resources_t tresources,
                            kmem_resource_t* kresource)
{
    return true;
}

#endif /* OKL4_ARCH_SH_RESOURCE_FUNCTIONS_H */
