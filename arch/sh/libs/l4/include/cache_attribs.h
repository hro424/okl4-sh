/**
 * @brief   Definition of cache attributes
 * @since   December 2008
 * @author  Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 * @see     arch/sh/libs/l4/include/elfweaver_info.h
 */

#ifndef OKL4_ARCH_SH_LIBS_L4_CACHE_ATTRIBS_H
#define OKL4_ARCH_SH_LIBS_L4_CACHE_ATTRIBS_H

#define ARCH_CACHE_ATTRIB_CACHED_BIT    0
#define ARCH_CACHE_ATTRIB_MODE_BIT      1
#define ARCH_CACHE_ATTRIB_SHARED_BIT    2
#define ARCH_CACHE_ATTRIB_MASK          3

#define ARCH_CACHE_ATTRIB_UNCACHED      0
#define ARCH_CACHE_ATTRIB_CACHED        (1 << ARCH_CACHE_ATTRIB_CACHED_BIT)
#define ARCH_CACHE_ATTRIB_SHARED        (1 << ARCH_CACHE_ATTRIB_SHARED_BIT)
#define ARCH_CACHE_ATTRIB_WRITEBACK     ARCH_CACHE_ATTRIB_CACHED
#define ARCH_CACHE_ATTRIB_WRITETHROUGH  \
        (ARCH_CACHE_ATTRIB_CACHED | (1 << ARCH_CACHE_ATTRIB_MODE_BIT))

#define L4_DefaultMemory                ARCH_CACHE_ATTRIB_WRITEBACK
#define L4_CachedMemory                 ARCH_CACHE_ATTRIB_WRITEBACK
#define L4_UncachedMemory               ARCH_CACHE_ATTRIB_UNCACHED
#define L4_WriteBackMemory              ARCH_CACHE_ATTRIB_WRITEBACK
#define L4_WriteThroughMemory           ARCH_CACHE_ATTRIB_WRITETHROUGH
#define L4_CohearentMemory              ARCH_CACHE_ATTRIB_UNCACHED
#define L4_IOMemory                     ARCH_CACHE_ATTRIB_UNCACHED
#define L4_IOCombinedMemory             ARCH_CACHE_ATTRIB_UNCACHED
#define L4_StrongOrderedMemory          ARCH_CACHE_ATTRIB_UNCACHED

#endif /* OKL4_ARCH_SH_LIBS_L4_CACHE_ATTRIBS_H */
