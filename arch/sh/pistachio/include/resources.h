/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_RESOURCES_H
#define OKL4_ARCH_SH_RESOURCES_H

class thread_resources_t : public generic_thread_resources_t
{
public:
    void init(tcb_t* tcb);
    void free(tcb_t* tcb, kmem_resource_t* kresource);
    bool control(tcb_t* tcb, tc_resources_t tresources,
                 kmem_resource_t* kresource);
};

#endif /* OKL4_ARCH_SH_RESOURCES_H */
