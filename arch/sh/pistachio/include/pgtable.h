/**
 * @since   December 2008
 * @author  Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 */

#ifndef OKL4_ARCH_SH_PGTABLE_H
#define OKL4_ARCH_SH_PGTABLE_H

#include <cache.h>
#include <arch/memattrib.h>

/*
 * The implementation manages physical memory pages with two levels of page
 * tables like IA32 MMU does.
 */

//NOTE: +2 is the size of a page table entry in log2.
#define SH_L1_BITS          (SH_SECTION_BITS + 2)

/**
 * Size of L1 table
 */
#define SH_L1_SIZE          (1UL << SH_L1_BITS)

#define SH_L2_BITS          (32 - SH_SECTION_BITS - PAGE_BITS_4K + 2)

/**
 * Size of L2 table
 */
#define SH_L2_SIZE          (1UL << SH_L2_BITS)

#define PG_TOP_SIZE         SH_L1_SIZE


class l1_entry_t
{
public:
    union {
        u32_t   raw;

        /* Structure of 1MiB page entries */
        struct {
            BITFIELD12(word_t,
                /* Write-through/Copy-back */
                wt              : 1,
                /* Shared */
                shared          : 1,
                /* Dirty */
                dirty           : 1,
                /* Cache enabled */
                cache           : 1,
                /* Size of the entry */
                size0           : 1,
                /* User/Supervisor & Read/Write */
                perm            : 2,
                /* Size of the entry */
                size1           : 1,
                /* Present */
                valid           : 1,
                /* Subtree */
                tree            : 1,
                /* Executable */
                x               : 1,

                reserved        : 9,
                /* Base address of a level-2 table or a 1M page */
                base_address    : 12
            );
        } large;

        struct {
            BITFIELD4(word_t,
                reserved        : 8,
                valid           : 1,
                tree            : 1,
                base_address    : 22
            );
        } table;
    };

    memattrib_e attributes() {
        return (memattrib_e)((large.shared << 5) | (large.cache << 4)
                             | large.wt);
    }

    addr_t address_large() {
        return (addr_t)(large.base_address << (32 - SH_SECTION_BITS));
    }

    addr_t address_table() {
        return (addr_t)(table.base_address << SH_L2_BITS);
    }

    word_t ptel_large() { return raw & (REG_PTEL_MASK & ~0x000FFC00); }
};

class l2_entry_t
{
public:
    union {
        u32_t   raw;

        /* Structure of 64KiB page entries */
        struct {
            BITFIELD12(word_t,
                /* Write-through/Copy-back */
                wt              : 1,
                /* Shared */
                shared          : 1,
                /* Dirty */
                dirty           : 1,
                /* Cache enabled */
                cache           : 1,
                /* Size of the entry (bit 0) */
                size0           : 1,
                /* User/Supervisor & Read/Write */
                perm            : 2,
                /* Size of the entry (bit 1) */
                size1           : 1,
                /* Present */
                valid           : 1,
                /* Executable */
                x               : 1,
                /* Subtree */
                tree            : 1,

                reserved        : 5,
                /* Base address of a level-2 table or a 1M page */
                base_address    : 16
            );
        } medium;

        /* Structure of 4KiB page entries */
        struct {
            BITFIELD12(word_t,
                /* Write-through(1)/Copy-back(0) */
                wt              : 1,
                /* Shared */
                shared          : 1,
                /* Dirty */
                dirty           : 1,
                /* Cache enabled */
                cache           : 1,
                /* Size of the entry */
                size0           : 1,
                /* User/Supervisor & Read/Write */
                perm            : 2,
                /* Size of the entry */
                size1           : 1,
                /* Present */
                valid           : 1,
                /* Executable */
                x               : 1,
                /* Subtree */
                tree            : 1,

                reserved        : 1,
                /* Base address of a level-2 table or a 1M page */
                base_address    : 20
            );
        } small;
    };

    memattrib_e attributes() {
        return (memattrib_e)((small.shared << 5) | (small.cache << 4)
                             | small.wt);
    }

    addr_t address_medium() {
        return (addr_t)(medium.base_address << PAGE_BITS_64K);
    }

    addr_t address_small() {
        return (addr_t)(small.base_address << PAGE_BITS_4K);
    }

    word_t ptel_medium() { return raw & (REG_PTEL_MASK & ~0x0000FC00); }

    word_t ptel_small() { return raw & (REG_PTEL_MASK & ~0x00000C00); }
};

#endif /* OKL4_ARCH_SH_PGTABLE_H */

