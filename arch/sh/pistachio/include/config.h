/* $Id$ */

/**
 * @file    arch/sh/pistachio/include/config.h
 * @since   December 9, 2008
 */

#ifndef OKL4_ARCH_SH_CONFIG_H
#define OKL4_ARCH_SH_CONFIG_H

#include <arch/page.h>
#include <l4/arch/config.h>
#include <kernel/arch/config.h>


#define ARCH_OBJECT_PTR(index)      (VIRT_ADDR_BASE + (index))
#define ARCH_OBJECT_INDEX(ptr)      (((word_t)(ptr)) - VIRT_ADDR_BASE)

//TODO: Make sure the reason
#define KTCB_ALIGN          16

//TODO: Make sure the reason
/* UTCB is kernel allocated */
#define CONFIG_UTCB_SIZE    0

#define UTCB_MASK           (~(UTCB_SIZE - 1))
#define UTCB_AREA_PAGESIZE  PAGE_SIZE_4K
#define UTCB_AREA_PAGEBITS  PAGE_BITS_4K
#define UTCB_AREA_PGSIZE    pgent_t::size_4k

#define IPC_NUM_MR          __L4_NUM_MRS

/* Page size for small object allocator */
//TODO: Make sure the reason
#define SMALL_OBJECT_BLOCKSIZE  PAGE_SIZE_4K

/* SH version specific config, if any */
//#include <arch/ver/config.h>

/* Tracebuffer size */
#define TBUFF_SIZE              (CONFIG_TRACEBUF_PAGES * PAGE_SIZE_4K)


/*
 * Memory layout (virtual)
 */
#define USER_AREA_START         0UL
//TODO: This is less than 0x80000000 to give a room to the UTCB area.
#define USER_AREA_SIZE          0x80000000
#define USER_AREA_END           (USER_AREA_START + USER_AREA_SIZE)

#define UTCB_AREA_START         USER_AREA_END
#define UTCB_AREA_SIZE
#define UTCB_AREA_END           (UTCB_AREA_START + UTCB_AREA_SIZE)

#define KERNEL_AREA_START       UTCB_AREA_END
#define KERNEL_AREA_SIZE        0x20000000
#define KERNEL_AREA_END         (KERNEL_AREA_START + KERNEL_AREA_SIZE)

/*
 * Memory-mapped registers (virtual)
 */

#define UPDATE_REG()        __asm__ __volatile__ ("icbi @%0" :: "r" (0x1000))

/* TRAPA */
#define REG_TRA             0xFF000020
/* Exception */
#define REG_EXPEVT          0xFF000024
/* Interrupt */
#define REG_INTEVT          0xFF000028
/* Page Table Entry (high) */
#define REG_PTEH            0xFF000000
/* Page Table Entry (low) */
#define REG_PTEL            0xFF000004
/* Base Address of the Translation Table */
#define REG_TTB             0xFF000008
/* TLB Fault Address */
#define REG_TEA             0xFF00000C
/* MMU Control */
#define REG_MMUCR           0xFF000010
/* Physical Address Space Control */
#define REG_PASCR           0xFF000070
/* No Refetch */
#define REG_IRMCR           0xFF000078
/* Cache Control */
#define REG_CCR             0xFF00001C
/* Queue Address Control */
#define REG_QACR0           0xFF000038
#define REG_QACR1           0xFF00003C
/* Internal Memory Control */
#define REG_RAMCR           0xFF000074
/* Source Address of L-Memory Transfer */
#define REG_LSA0            0xFF000050
#define REG_LSA1            0xFF000054
/* Destination Address of L-Memory Transfer */
#define REG_LDA0            0xFF000058
#define REG_LDA1            0xFF00005C

/*
 * Page Table Entry (high)
 */
#define REG_PTEH_VPN_MASK   0xFFFFFC00
#define REG_PTEH_ASID_MASK  0x0000007F

/*
 * Page Table Entry (low)
 */
#define REG_PTEL_PPN_MASK   0x1FFFFC00
#define REG_PTEL_V          0x00000100
#define REG_PTEL_SZ1        0x00000080
#define REG_PTEL_PR1        0x00000040
#define REG_PTEL_PR0        0x00000020
#define REG_PTEL_SZ0        0x00000010
#define REG_PTEL_C          0x00000008
#define REG_PTEL_D          0x00000004
#define REG_PTEL_SH         0x00000002
#define REG_PTEL_WT         0x00000001

/*
 * MMU Control Register
 */
#define REG_MMUCR_LRUI_MASK 0xFC000000
#define REG_MMUCR_URB_MASK  0x00FC0000
#define REG_MMUCR_URC_MASK  0x0000FC00
#define REG_MMUCR_SQMD      0x00000080
#define REG_MMUCR_SV        0x00000040
#define REG_MMUCR_TI        0x00000004
#define REG_MMUCR_AT        0x00000001

/*
 * Physical Address Space Control Register
 */
#define REG_PASCR_UB_MASK   0x000000FF

/*
 * No Re-fetch Register
 */
#define REG_IRMCR_R2        0x00000010
#define REG_IRMCR_R1        0x00000008
#define REG_IRMCR_LT        0x00000004
#define REG_IRMCR_MT        0x00000002
#define REG_IRMCR_MC        0x00000001

/*
 * Cache Control Register
 */
#define REG_CCR_ICI         0x00000800
#define REG_CCR_ICE         0x00000100
#define REG_CCR_OCI         0x00000008
#define REG_CCR_CB          0x00000004
#define REG_CCR_WT          0x00000002
#define REG_CCR_OCE         0x00000001

/*
 * Internal Memory Control Register
 */
#define REG_RAMCR_RMD       0x00000200
#define REG_RAMCR_RP        0x00000100
#define REG_RAMCR_IC2W      0x00000080
#define REG_RAMCR_OC2W      0x00000040


#define ARCH_MAX_SPACES     256

#endif /* OKL4_ARCH_SH_KERNEL_CONFIG_H */
