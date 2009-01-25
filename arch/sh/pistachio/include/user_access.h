/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_USER_ACCESS_H
#define OKL4_ARCH_SH_USER_ACCESS_H

INLINE word_t
user_read_word(word_t* ptr)
{
    return *ptr;
}

INLINE void
user_write_word(word_t* ptr, word_t value)
{
    *ptr = value;
}

INLINE bool
user_compare_and_set_word(word_t* ptr, word_t expect, word_t value)
{
    if (*ptr == expect) {
        *ptr = value;
        return true;
    }
    return false;
}

#endif /* OKL4_ARCH_SH_USER_ACCESS_H */
