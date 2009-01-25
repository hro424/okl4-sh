/* $Id$ */

/**
 *  @since  December 2008
 */

/* These numbers MUST be multiple of 4 */
#define SYSCALL_ipc                 0x0
#define SYSCALL_thread_switch       0x4
#define SYSCALL_thread_control      0x8
#define SYSCALL_exchange_registers  0xc
#define SYSCALL_schedule            0x10
#define SYSCALL_map_control         0x14
#define SYSCALL_space_control       0x18
/* Unused syscall slot:             0x1c */
#define SYSCALL_cache_control       0x20
/* Unused syscall slot:             0x24 */
#define SYSCALL_lipc                0x28
#define SYSCALL_platform_control    0x2c
#define SYSCALL_space_switch        0x30
#define SYSCALL_mutex               0x34
#define SYSCALL_mutex_control       0x38
#define SYSCALL_interrupt_control   0x3c
#define SYSCALL_cap_control         0x40
#define SYSCALL_memory_copy         0x44
#define SYSCALL_last                0x44


