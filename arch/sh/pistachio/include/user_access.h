/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_USER_ACCESS_H
#define OKL4_ARCH_SH_USER_ACCESS_H

#include <arch/tlb.h>

INLINE word_t
user_read_word(word_t* ptr)
{
    space_t* space = get_current_space();

    // Ensure that the address is on TLB.
    if (!lookup_tlb(ptr, space)) {
        pgent_t*            pg;
        pgent_t::pgsize_e   pgsize;

        if (!space->lookup_mapping(ptr, &pg, &pgsize)) {
            panic("USER READ: page not found\n");
        }
        if (!pg->is_readable(space, pgsize)) {
            panic("USER READ: permission denied\n");
        }

        fill_tlb(ptr, space, pg, pgsize);
    }

    return *ptr;
}

INLINE void
user_write_word(word_t* ptr, word_t value)
{
    space_t* space = get_current_space();

    // Ensure that the address is on TLB.
    if (!lookup_tlb(ptr, space)) {
        pgent_t*            pg;
        pgent_t::pgsize_e   pgsize;

        if (!space->lookup_mapping(ptr, &pg, &pgsize)) {
            panic("USER WRITE: page not found\n");
        }
        if (!pg->is_writable(space, pgsize)) {
            panic("USER WRITE: permission denied\n");
        }

        fill_tlb(ptr, space, pg, pgsize);
    }

    *ptr = value;
}

INLINE bool
user_compare_and_set_word(word_t* ptr, word_t expect, word_t value)
{
    space_t* space = get_current_space();

    if (!lookup_tlb(ptr, space)) {
        pgent_t*            pg;
        pgent_t::pgsize_e   pgsize;

        if (!space->lookup_mapping(ptr, &pg, &pgsize)) {
            panic("USER COMPARE AND SET: page not found\n");
        }
        if (!pg->is_writable(space, pgsize)) {
            panic("USER COMPARE AND SET: permission denied\n");
        }

        fill_tlb(ptr, space, pg, pgsize);
    }

    if (*ptr == expect) {
        *ptr = value;
        return true;
    }
    return false;
}

#endif /* OKL4_ARCH_SH_USER_ACCESS_H */
