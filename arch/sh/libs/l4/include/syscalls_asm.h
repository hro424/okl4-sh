/* $Id$ */

/**
 *  @since  December 2008
 */

#define SYSCALL_ipc                 0x01
#define SYSCALL_thread_switch       0x02
#define SYSCALL_thread_control      0x03
#define SYSCALL_exchange_registers  0x04
#define SYSCALL_schedule            0x05
#define SYSCALL_map_control         0x06
#define SYSCALL_space_control       0x07
#define SYSCALL_cache_control       0x08
#define SYSCALL_lipc                0x09
#define SYSCALL_platform_control    0x0A
#define SYSCALL_space_switch        0x0B
#define SYSCALL_mutex               0x0C
#define SYSCALL_mutex_control       0x0D
#define SYSCALL_interrupt_control   0x0E
#define SYSCALL_cap_control         0x0F
#define SYSCALL_memory_copy         0x10

#define SYSNUM(name)                SYSCALL_ ## name
#define SYSCALL_REG                 r8
#define SYSCALL_NUM                 0x31
