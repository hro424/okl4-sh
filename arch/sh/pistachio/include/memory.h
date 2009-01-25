/* $Id$ */

/**
 * @brief   Symbols defined by linker script
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_MEMORY_H
#define OKL4_ARCH_SH_MEMORY_H

extern "C" {
    extern char _start_rom[];
    extern char _end_rom[];
    extern char _start_ram[];
    extern char _end_ram[];
    extern char _start_init[];
    extern char _end_init[];
    extern char _end[];
}

/*
 * Wrapper macros to access linker symbols.
 */

#define START_ROM               ((addr_t) _start_rom)
#define START_RAM               ((addr_t) _start_ram)
#define END_ROM                 ((addr_t) _end_rom)
#define END_RAM                 ((addr_t) _end_ram)
#define START_INIT              ((addr_t) _start_init)
#define END_INIT                ((addr_t) _end_init)
#define END_KERNEL              ((addr_t) _end)

#endif /* OKL4_ARCH_SH_MEMORY_H */
