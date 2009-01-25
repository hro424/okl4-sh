/* $Id$ */

/**
 * @brief   Address space implementation
 * @file    arch/sh/pistachio/include/space.h
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_SPACE_H
#define OKL4_ARCH_SH_SPACE_H

#include <asid.h>

typedef word_t  sh_pid_t;

class space_t : public generic_space_t
{
public:
    word_t space_control(word_t ctrl);
    asid_t* get_asid();
    sh_pid_t get_pid(void);
    void set_pid(sh_pid_t pid);

    /*
    bool add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                     rwx_e rwx, bool kernel,
                     memattrib_e attrib, kmem_resource_t* kresource);
    inline bool add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                            rwx_e rwx, bool kernel, kmem_resource_t* kresource)
    {
        return add_mapping(vaddr, paddr, size, rwx, kernel, l4default,
                           kresource);
    }
    */
};


PURE INLINE space_t*
get_kenel_space()
{
    return get_globals()->kernel_space;
}

#endif /* OKL4_ARCH_SH_SPACE_H */

