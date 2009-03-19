/**
 * @since   December 2008
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#ifndef OKL4_ARCH_SH_LIBS_L4_CACHE_ATTRIBS_H
#define OKL4_ARCH_SH_LIBS_L4_CACHE_ATTRIBS_H

#define CACHE_ATTRIB_UNCACHED           0x00
#define CACHE_ATTRIB_CACHED             0x01
#define CACHE_ATTRIB_WRITETHROUGH       0x01
#define CACHE_ATTRIB_WRITEBACK          0x03
#define CACHE_ATTRIB_SHARED             0x80

#define CACHE_ATTRIB_CACHED_BIT         0
#define CACHE_ATTRIB_MODE_BIT           1
#define CACHE_ATTRIB_SHARED_BIT         7

#define L4_DefaultMemory                CACHE_ATTRIB_WRITEBACK
#define L4_CachedMemory                 CACHE_ATTRIB_WRITEBACK
#define L4_UncachedMemory               CACHE_ATTRIB_UNCACHED
#define L4_WriteBackMemory              CACHE_ATTRIB_WRITEBACK
#define L4_WriteThroughMemory           CACHE_ATTRIB_WRITETHROUGH
#define L4_CohearentMemory              CACHE_ATTRIB_UNCACHED
#define L4_IOMemory                     CACHE_ATTRIB_UNCACHED
#define L4_IOCombinedMemory             CACHE_ATTRIB_UNCACHED
#define L4_StrongOrderedMemory          CACHE_ATTRIB_UNCACHED

#endif /* OKL4_ARCH_SH_LIBS_L4_CACHE_ATTRIBS_H */
