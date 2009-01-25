/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_L4_ELFWEAVER_INFO_H
#define OKL4_ARCH_SH_L4_ELFWEAVER_INFO_H

/*
 * See tools/pyelf/weaver/machine.py:183 and 290, and
 * tools/pyelf/weaver/kernel_micro_elf.py,
 */
/*                                      Mask,   Comparator */
#define L4_Info_Uncached            {0xffff,        0x00}
#define L4_Info_Cached              {0x0000,        0x0f}
#define L4_Info_IOMemory            {0x0000,        0x0f}
#define L4_Info_IOMemoryShared      {0x0000,        0x0f}
#define L4_Info_WriteThrough        {0xffff,        0x01}
#define L4_Info_WriteBack           {0xffff,        0x02}
#define L4_Info_Shared              {0x0080,        0x80}
#define L4_Info_NonShared           {0x0080,        0x00}
#define L4_Info_Custom              {0x0000,        0x0f}
#define L4_Info_Strong              {0x0000,        0x0f}
#define L4_Info_Buffered            {0x0000,        0x0f}

#endif /* OKL4_ARCH_SH_L4_ELFWEAVER_INFO_H */
