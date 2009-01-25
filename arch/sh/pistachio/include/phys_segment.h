/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_PHYS_SEGMENT_H
#define OKL4_ARCH_SH_PHYS_SEGMENT_H

/**
 * Convert a memory attribute into a set of flags that can then be checked
 * against a segment attribute permissions. Any flags set which are not set in
 * the segment indicates the requested attributes are invalid.
 */
INLINE word_t
to_physattrib(memattrib_e attr)
{
    word_t res = 0UL;

    switch (attr) {
        default:
            break;
    }

    return res;
}
 
#endif /* OKL4_ARCH_SH_PHYS_SEGMENT_H */

