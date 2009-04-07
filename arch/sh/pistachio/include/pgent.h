/**
 * @brief   Page table entry
 * @file    arch/sh/pistachio/include/pgent.h
 * @since   December 9, 2008
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#ifndef OKL4_ARCH_SH_PGENT_H
#define OKL4_ARCH_SH_PGENT_H

#include <arch/hwspace.h>
#include <arch/pgtable.h>
#include <arch/syscalls.h>
#include <kmem_resource.h>

#define ARCH_READ       0
#define ARCH_WRITE      1
#define ARCH_READWRITE  2
#define ARCH_EXECUTE    3

#define PGTABLE_OFFSET      (VIRT_ADDR_PGTABLE - get_globals()->phys_addr_ram)

class mapnode_t;
class generic_space_t;

template<typename T> INLINE T
page_table_to_phys(T virt)
{
    return (T)((u32_t)virt - PGTABLE_OFFSET);
}

class pgent_t
{
public:
    union {
        l1_entry_t  l1;
        l2_entry_t  l2;
        u32_t       raw;
    };

    enum pgsize_e {
        /* Indice of HW_PGSHIFTS */
        size_4k     = 0,
        size_min    = size_4k,
        size_64k    = 1,
        size_1m     = 2,
        min_tree    = size_1m,  // minimum mid-level tree
        size_4g     = 3,
        size_max    = size_1m,
    };
    
    /**
     *  Checks if this entry is valid.
     */
    bool is_valid(generic_space_t* s, pgsize_e pgsize);

    /**
     *  Checks if this page is writable.
     */
    bool is_writable(generic_space_t* s, pgsize_e pgsize);

    /**
     *  Checks if this page is readable.
     */
    bool is_readable(generic_space_t* s, pgsize_e pgsize);

    /**
     *  Checks if this page is executable.
     */
    bool is_executable(generic_space_t* s, pgsize_e pgsize);

    /**
     *  Checks if this page belongs to the kernel space.
     */
    bool is_kernel(generic_space_t* s, pgsize_e pgsize);

    /**
     *  Checks if this entry is the root of subtree.
     */
    bool is_subtree(generic_space_t* s, pgsize_e pgsize);

    /**
     *  Obtains the physical address this entry represents.
     */
    addr_t address(generic_space_t* s, pgsize_e pgsize);

    pgent_t* subtree(generic_space_t* s, pgsize_e pgsize);

    word_t reference_bits(generic_space_t* s, pgsize_e pgsize, addr_t vaddr);

    void clear(generic_space_t* s, pgsize_e pgsize, bool kernel, addr_t vaddr);

    void clear(generic_space_t* s, pgsize_e pgsize, bool kernel);

    bool make_subtree(generic_space_t* s, pgsize_e pgsize, bool kernel,
                      kmem_resource_t* kresource);

    void remove_subtree(generic_space_t* s, pgsize_e pgsize, bool kernel,
                        kmem_resource_t* kresource);

    /**
     *  Stores the physical address to this entry.
     */
    void set_entry(generic_space_t* s, pgsize_e pgsize, addr_t paddr,
                   bool readable, bool writable, bool executable, bool kernel,
                   memattrib_e attrib);

    void set_entry_1m(generic_space_t* s, addr_t paddr, bool readable,
                      bool writable, bool executable, memattrib_e attrib);

    /**
     *  Stores the physical address to this entry.
     */
    void set_entry(generic_space_t* s, pgsize_e pgsize, addr_t paddr,
                   bool readable, bool writable, bool executable, bool kernel);

    memattrib_e get_attributes(generic_space_t* s, pgsize_e pgsize);

    u32_t ptel(pgsize_e pgsize);

    pgent_t* next(generic_space_t* s, pgsize_e pgsize, word_t num);

    void dump_misc(generic_space_t* s, pgsize_e pgsize);

private:
    void sync_large(generic_space_t* s);
    void sync_large(generic_space_t* s, word_t val);
};

INLINE bool
pgent_t::is_valid(generic_space_t* s, pgsize_e pgsize)
{
    switch (pgsize) {
        case size_1m:
            return (l1.large.valid == 1) || (l1.table.tree == 1);
        case size_64k:
            return (l2.medium.valid == 1) || (l2.medium.tree == 1);
        case size_4k:
            return (l2.small.valid == 1);
        default:
            return false;
    }
}

INLINE bool
pgent_t::is_writable(generic_space_t* s, pgsize_e pgsize)
{
    switch (pgsize) {
        case size_1m:
            return (l1.large.perm == 3);
        case size_64k:
            return (l2.medium.perm == 3);
        case size_4k:
            return (l2.small.perm == 3);
        default:
            return false;
    }
}

INLINE bool
pgent_t::is_readable(generic_space_t* s, pgsize_e pgsize)
{
    return true;
}

INLINE bool
pgent_t::is_executable(generic_space_t* s, pgsize_e pgsize)
{
    switch (pgsize) {
        case size_1m:
            return (l1.large.x == 1);
        case size_64k:
            return (l2.medium.x == 1);
        case size_4k:
            return (l2.small.x == 1);
        default:
            return false;
    }
}

INLINE bool
pgent_t::is_kernel(generic_space_t* s, pgsize_e pgsize)
{
    return pgsize == size_1m ?  l1.large.perm <= 1 : l2.small.perm <= 1;
    switch (pgsize) {
        case size_1m:
            return (l1.large.perm <= 1);
        case size_64k:
            return (l2.medium.perm <= 1);
        case size_4k:
            return (l2.small.perm <= 1);
        default:
            return false;
    }
}

INLINE bool
pgent_t::is_subtree(generic_space_t* s, pgsize_e pgsize)
{
    switch (pgsize) {
        case size_1m:
            return (l1.large.tree == 1);
        case size_64k:
            return (l2.medium.tree == 1);
        case size_4k:
        default:
            return false;
    }
}

INLINE addr_t
pgent_t::address(generic_space_t* s, pgsize_e pgsize)
{
    switch (pgsize) {
        case size_1m:
            return l1.address_large();
        case size_64k:
            return l2.address_medium();
        case size_4k:
        default:
            return l2.address_small();
    }
}

INLINE word_t
pgent_t::reference_bits(generic_space_t* s, pgsize_e pgsize, addr_t vaddr)
{
    return 7;
}

INLINE void
pgent_t::clear(generic_space_t* s, pgsize_e pgsize, bool kernel, addr_t vaddr)
{
    clear(s, pgsize, kernel);
}

INLINE void
pgent_t::clear(generic_space_t* s, pgsize_e pgsize, bool kernel)
{
    switch (pgsize) {
        case size_64k:
            raw = 0;
            sync_large(s, 0);
            break;
        case size_4k:
            int tree = l2.small.tree;
            raw = 0;
            l2.small.tree = tree;
            break;
        case size_1m:
        default:
            raw = 0;
            break;
    }
}

INLINE bool
pgent_t::make_subtree(generic_space_t* s, pgsize_e pgsize, bool kernel,
                      kmem_resource_t* kresource)
{
    if (pgsize == size_1m) {
        // Allocate an L2 table
        addr_t base = kresource->alloc(kmem_group_pgtab, SH_L2_SIZE, true);
        if (base == NULL) {
            return false;
        }
        l1.raw = 0;
        l1.table.tree = 1;
        l1.table.base_address = (word_t)virt_to_phys(base) >> SH_L2_BITS;
        sh_cache::flush_d(base, SH_L2_BITS);
    }
    else if (pgsize == size_64k) {
        l2.medium.tree = 1;
        sync_large(s);
    }
    return true;
}

INLINE void
pgent_t::remove_subtree(generic_space_t* s, pgsize_e pgsize, bool kernel,
                        kmem_resource_t* kresource)
{
    if (pgsize == size_1m) {
        // Release the L2 table
        kresource->free(kmem_group_pgtab,
                phys_to_virt((addr_t)(l1.table.base_address << SH_L2_BITS)),
                SH_L2_SIZE);
        l1.table.tree = 0;
    }
    else {
        l2.medium.tree = 0;
    }
    clear(s, pgsize, kernel);
}

INLINE pgent_t*
pgent_t::subtree(generic_space_t* s, pgsize_e pgsize)
{
    if (pgsize == size_1m) {
        return (pgent_t*)phys_to_virt(l1.address_table());
    }
    return this;
}

INLINE void
pgent_t::set_entry(generic_space_t* s, pgsize_e pgsize, addr_t paddr,
                   bool readable, bool writable, bool executable,
                   bool kernel, memattrib_e attrib)
{
    word_t perm;

    if (kernel) {
        perm = writable ? 1 : 0;
    }
    else {
        perm = 2 | (writable ? 1 : 0);
    }

    if (EXPECT_TRUE(pgsize == size_4k)) {
        l2_entry_t l2_entry;

        l2_entry.raw = 0;
        l2_entry.small.tree = l2.small.tree;
        l2_entry.small.valid = 1;
        l2_entry.small.x = executable;
        l2_entry.small.size0 = 1;
        l2_entry.small.perm = perm;
        l2_entry.small.shared =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_SHARED_BIT) & 0x1;
        l2_entry.small.wt =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_MODE_BIT) & 0x1;
        l2_entry.small.cache =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_CACHED_BIT) & 0x1;
        l2_entry.small.base_address = (word_t)paddr >> PAGE_BITS_4K;

        l2.raw = l2_entry.raw;
    }
    else if (pgsize == size_1m) {
        l1_entry_t l1_entry;

        l1_entry.raw = 0;
        l1_entry.large.size0 = 1;
        l1_entry.large.size1 = 1;
        l1_entry.large.valid = 1;
        l1_entry.large.x = executable;
        l1_entry.large.perm = perm;
        l1_entry.large.shared =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_SHARED_BIT) & 0x1;
        l1_entry.large.wt =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_MODE_BIT) & 0x1;
        l1_entry.large.cache =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_CACHED_BIT) & 0x1;
        l1_entry.large.base_address = (word_t)paddr >> PAGE_BITS_1M;

        l1.raw = l1_entry.raw;
    }
    // size_64k
    else {
        l2_entry_t l2_entry;

        l2_entry.raw = 0;
        l2_entry.medium.size1 = 1;
        l2_entry.medium.valid = 1;
        l2_entry.medium.x = executable;
        l2_entry.medium.perm = perm;
        l2_entry.medium.shared =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_SHARED_BIT) & 0x1;
        l2_entry.medium.wt =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_MODE_BIT) & 0x1;
        l2_entry.medium.cache =
            ((word_t)attrib >> ARCH_CACHE_ATTRIB_CACHED_BIT) & 0x1;
        l2_entry.medium.base_address = (word_t)paddr >> PAGE_BITS_64K;

        l2.raw = l2_entry.raw;
        sync_large(s);
    }
}

INLINE void
pgent_t::set_entry_1m(generic_space_t* s, addr_t paddr, bool readable,
                      bool writable, bool executable, memattrib_e attrib)
{
    l1_entry_t  l1_entry;
    word_t      perm;

    /* Assume kernel mapping */
    perm = writable ? 0 : 1;

    l1_entry.raw = 0;
    l1_entry.large.valid = 1;
    l1_entry.large.x = executable;
    l1_entry.large.perm = perm;
    l1_entry.large.shared =
        ((word_t)attrib >> ARCH_CACHE_ATTRIB_SHARED_BIT) & 0x1;
    l1_entry.large.wt =
        ((word_t)attrib >> ARCH_CACHE_ATTRIB_MODE_BIT) & 0x1;
    l1_entry.large.cache =
        ((word_t)attrib >> ARCH_CACHE_ATTRIB_CACHED_BIT) & 0x1;
    l1_entry.large.base_address = (word_t)paddr >> PAGE_BITS_1M;

    l1.raw = l1_entry.raw;
}

INLINE void
pgent_t::set_entry(generic_space_t* s, pgsize_e pgsize, addr_t paddr,
                   bool readable, bool writable, bool executable,
                   bool kernel)
{
    set_entry(s, pgsize, paddr, readable, writable, executable,
              kernel, l4default);
}

INLINE memattrib_e
pgent_t::get_attributes(generic_space_t* s, pgsize_e pgsize)
{
    word_t ret = 0;

    switch (pgsize) {
        case size_1m:
            ret = (l1.large.shared << ARCH_CACHE_ATTRIB_SHARED_BIT) |
                    (l1.large.wt << ARCH_CACHE_ATTRIB_MODE_BIT) |
                    (l1.large.cache << ARCH_CACHE_ATTRIB_CACHED_BIT);
            break;
        case size_64k:
            ret = (l2.medium.shared << ARCH_CACHE_ATTRIB_SHARED_BIT) |
                    (l2.medium.wt << ARCH_CACHE_ATTRIB_MODE_BIT) |
                    (l2.medium.cache << ARCH_CACHE_ATTRIB_CACHED_BIT);
            break;
        case size_4k:
        default:
            ret = (l2.small.shared << ARCH_CACHE_ATTRIB_SHARED_BIT) |
                    (l2.small.wt << ARCH_CACHE_ATTRIB_MODE_BIT) |
                    (l2.small.cache << ARCH_CACHE_ATTRIB_CACHED_BIT);
            break;
    }
    return (memattrib_e)ret;
}

INLINE u32_t
pgent_t::ptel(pgsize_e pgsize)
{
    switch (pgsize) {
        case size_1m:
            return l1.ptel_large();
        case size_64k:
            return l2.ptel_medium();
        case size_4k:
        default:
            return l2.ptel_small();
    }
}

INLINE pgent_t*
pgent_t::next(generic_space_t* s, pgsize_e pgsize, word_t num)
{
    if (pgsize == size_64k) {
        return this + (num * 16);
    }
    return this + num;
}

INLINE void
pgent_t::dump_misc(generic_space_t* s, pgsize_e pgsize)
{
    UNIMPLEMENTED();
}

INLINE void
pgent_t::sync_large(generic_space_t* s)
{
    for (word_t i = 1; i < 16; i++) {
        ((u32_t*)this)[i] = raw;
    }
}

INLINE void
pgent_t::sync_large(generic_space_t* s, word_t val)
{
    for (word_t i = 1; i < 16; i++) {
        ((u32_t*)this)[i] = val;
    }
}

#endif /* OKL4_ARCH_SH_PGENT_H */
