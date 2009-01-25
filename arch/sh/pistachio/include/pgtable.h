/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_PGTABLE_H
#define OKL4_ARCH_SH_PGTABLE_H

#include <cache.h>
#include <arch/memattrib.h>

/*
 * The implementation manages physical memory pages with two levels of page
 * tables like IA32 MMU does.
 */

class l1_entry_t
{
public:
    union {
        u32_t   raw;

        /* Structure of 1MiB page entries */
        struct {
            BITFIELD11(word_t,
                /* Write-through/Copy-back */
                wt              : 1,
                /* Dirty */
                dirty           : 1,
                /* User/Supervisor & Read/Write */
                perm            : 2,
                /* Cache enabled */
                cache           : 1,
                /* Shared */
                shared          : 1,
                /* Size of the entry */
                size            : 2,
                /* Executable */
                x               : 1,
                /* Present */
                present         : 1,
                /* Accessed */
                accessed        : 1,

                reserved        : 10,
                /* Base address of a level-2 table or a 1M page */
                base_address    : 12
            );
        } large;
    };

    void clear() { raw = 0; }

    memattrib_e attributes() {
        return (memattrib_e)((large.shared << 5) | (large.cache << 4)
                             | large.wt);
    }

    addr_t address_large() { return (addr_t)(large.base_address << 20); }
};

class l2_entry_t
{
public:
    union {
        u32_t   raw;

        /* Structure of 64KiB page entries */
        struct {
            BITFIELD11(word_t,
                /* Write-through/Copy-back */
                wt              : 1,
                /* Dirty */
                dirty           : 1,
                /* User/Supervisor & Read/Write */
                perm            : 2,
                /* Cache enabled */
                cache           : 1,
                /* Shared */
                shared          : 1,
                /* Size of the entry */
                size            : 2,
                /* Executable */
                x               : 1,
                /* Present */
                present         : 1,
                /* Accessed */
                accessed        : 1,

                reserved        : 6,
                /* Base address of a level-2 table or a 1M page */
                base_address    : 16
            );
        } medium;

        /* Structure of 4KiB page entries */
        struct {
            BITFIELD11(word_t,
                /* Write-through(1)/Copy-back(0) */
                wt              : 1,
                /* Dirty */
                dirty           : 1,
                /* User/Supervisor & Read/Write */
                perm            : 2,
                /* Cache enabled */
                cache           : 1,
                /* Shared */
                shared          : 1,
                /* Size of the entry */
                size            : 2,
                /* Executable */
                x               : 1,
                /* Present */
                present         : 1,
                /* Accessed */
                accessed        : 1,

                reserved        : 2,
                /* Base address of a level-2 table or a 1M page */
                base_address    : 20
            );
        } small;
    };

    void clear() { raw = 0; }

    memattrib_e attributes() {
        return (memattrib_e)((small.shared << 5) | (small.cache << 4)
                             | small.wt);
    }

    addr_t address_medium() { return (addr_t)(medium.base_address << 16); }

    addr_t address_small() { return (addr_t)(small.base_address << 12); }
};

#define SH_L1_BITS          SH_SECTION_BITS
#define SH_L1_SIZE          (1UL << SH_SECTION_BITS)
#define SH_L2_BITS          (32 - SH_SECTION_BITS - PAGE_BITS_4K)
#define SH_L2_SIZE          (1UL << SH_L2_BITS)

#define PG_TOP_SIZE         SH_L1_SIZE

#endif /* OKL4_ARCH_SH_PGTABLE_H */

