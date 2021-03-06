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
    refill_tlb(ptr, space);
    return *ptr;
}

INLINE void
user_write_word(word_t* ptr, word_t value)
{
    space_t* space = get_current_space();
    refill_tlb(ptr, space);
    *ptr = value;
}

INLINE bool
user_compare_and_set_word(word_t* ptr, word_t expect, word_t value)
{
    space_t* space = get_current_space();
    refill_tlb(ptr, space);
    if (*ptr == expect) {
        *ptr = value;
        return true;
    }
    return false;
}

#endif /* OKL4_ARCH_SH_USER_ACCESS_H */
