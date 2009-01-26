/* $Id$ */

/**
 * @file    arch/sh/pistachio/include/config.h
 * @since   December 9, 2008
 */

#ifndef OKL4_ARCH_SH_CONFIG_H
#define OKL4_ARCH_SH_CONFIG_H

#include <arch/page.h>
#include <arch/registers.h>
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


#define ARCH_MAX_SPACES     256

#endif /* OKL4_ARCH_SH_KERNEL_CONFIG_H */
