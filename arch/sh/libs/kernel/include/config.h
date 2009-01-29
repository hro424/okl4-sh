/* $Id$ */

/**
 * @file    arch/sh/libs/kernel/include/config.h
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_KERNEL_CONFIG_H
#define OKL4_ARCH_SH_KERNEL_CONFIG_H

#define VIRT_ADDR_BASE          (0x88000000)
#define VIRT_ADDR_PGTABLE

#define VIRT_ADDR_ROM           VIRT_ADDR_BASE
#define VIRT_ADDR_RAM           VIRT_ADDR_BASE

#define STACK_BITS              11
#define STACK_SIZE              (1 << STACK_BITS)
#define STACK_TOP               (STACK_SIZE - 4)

#endif /* OKL4_ARCH_SH_KERNEL_CONFIG_H */
