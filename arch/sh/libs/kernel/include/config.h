/* $Id$ */

/**
 * @file    arch/sh/libs/kernel/include/config.h
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_KERNEL_CONFIG_H
#define OKL4_ARCH_SH_KERNEL_CONFIG_H

/* Should move to platform specific config */

/* SH4A physical memory layout */
#define AREA0_START             (0x00000000)
#define AREA1_START             (0x04000000)
#define AREA2_START             (0x08000000)
#define AREA3_START             (0x0C000000)
#define AREA4_START             (0x10000000)
#define AREA5_START             (0x14000000)
#define AREA6_START             (0x18000000)

/* SH2007 physical memory layout */
#define ROM_START               AREA0_START
#define RAM_START               AREA2_START

/* SH4A virtual memory layout */
#define P1_START                (0x80000000)
#define P2_START                (0xA0000000)

/* SH-IPL uses 0x08000000 - 0x08210000 */
#define IPL_OFFSET              (0x00210000)

#define VIRT_ADDR_BASE          (P1_START + RAM_START)
// Make the kernel page table write-through
//#define VIRT_ADDR_PGTABLE       (P2_START + RAM_START)
#define VIRT_ADDR_PGTABLE       VIRT_ADDR_BASE

#define VIRT_ADDR_ROM           P1_START
#define VIRT_ADDR_RAM           VIRT_ADDR_BASE

#define PHYS_KERNEL_BASE        (RAM_START + IPL_OFFSET)

#define STACK_BITS              (11)
#define STACK_SIZE              (1 << STACK_BITS)
#define STACK_TOP               (STACK_SIZE - 4)

#endif /* OKL4_ARCH_SH_KERNEL_CONFIG_H */
