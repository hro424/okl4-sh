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

/* SH provides its own implementation of space->arch_free */
#define HAVE_ARCH_FREE_SPACE


PURE INLINE space_t*
get_kenel_space()
{
    return get_globals()->kernel_space;
}


class space_t : public generic_space_t
{
public:
    enum rwx_e {
        read_only       = 0x1,
        read_write      = 0x3,
        read_execute    = 0x5,
        read_write_ex   = 0x7,
    };

    hw_asid_t   asid;

    /**
     * Base physical address of the page table of this space.
     */
    word_t      pgbase;

    word_t space_control(word_t ctrl);
    asid_t* get_asid();
    void set_asid(asid_t* asid);
    sh_pid_t get_pid(void);
    void set_pid(sh_pid_t pid);

    bool add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                     rwx_e rwx, bool kernel,
                     memattrib_e attrib, kmem_resource_t* kresource);
    inline bool add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                            rwx_e rwx, bool kernel, kmem_resource_t* kresource)
    {
        return add_mapping(vaddr, paddr, size, rwx, kernel, l4default,
                           kresource);
    }

};

INLINE asid_t*
space_t::get_asid()
{
    return (asid_t*)&this->asid;
}

INLINE word_t
space_t::space_control(word_t ctrl)
{
    //TODO
    return 0;
}



INLINE bool
generic_space_t::does_tlbflush_pay(word_t log2size)
{
    return log2size > 12;
}

INLINE pgent_t*
generic_space_t::pgent(word_t num, word_t cpu)
{
    return (&pdir[num]);
}

INLINE bool
generic_space_t::is_user_area(addr_t addr)
{
    return ((word_t)addr < USER_AREA_END);
}

#endif /* OKL4_ARCH_SH_SPACE_H */

