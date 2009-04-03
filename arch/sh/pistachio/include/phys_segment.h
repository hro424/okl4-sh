/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_PHYS_SEGMENT_H
#define OKL4_ARCH_SH_PHYS_SEGMENT_H

//TODO: No reason to choose the flags for each attribute
enum allowed_attrib_e {
    allowed_uncached        = 0x01,
    allowed_writeback       = 0x02,
    allowed_writethrough    = 0x04
};

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
        case writethrough:
        case writethrough_shared:
            res = allowed_writethrough;
            break;
        case writeback:
        case writeback_shared:
            res = allowed_writeback;
            break;
        case uncached:
        case uncached_shared:
            res = allowed_uncached;
            break;
        default:
            res = (~0UL);
            break;
    }

    return res;
}
 
#endif /* OKL4_ARCH_SH_PHYS_SEGMENT_H */

