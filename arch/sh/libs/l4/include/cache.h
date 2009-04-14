#ifndef __ARCH__L4__CACHE_H__
#define __ARCH__L4__CACHE_H__

#define L4_CacheCtl_MaskInner       L4_CacheCtl_MaskL1
#define L4_CacheCtl_MaskOuter       (L4_CacheCtl_MaskAllLs & ~L4_CacheCtl_MaskInner)

#endif
