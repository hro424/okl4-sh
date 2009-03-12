/**
 *  @since  December 2008
 */

#ifndef OKL4_ARCH_L4_SYSCALL_ASM_H
#define OKL4_ARCH_L4_SYSCALL_ASM_H

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
#define SYSCALL_last                0x10

#define L4_TRAP_KPUTC               0xA0
#define L4_TRAP_KGETC               0xA1
#define L4_TRAP_KGETC_NB            0xA2
#define L4_TRAP_KDEBUG              0xA3
#define L4_TRAP_GETUTCB             0xA4
#define L4_TRAP_CKBR                0xA5
#define L4_TRAP_KSET_OBJECT_NAME    0xA6
#define L4_TRAP_GETCOUNTER          0xA7
#define L4_TRAP_GETNUMTPS           0xA8
#define L4_TRAP_GETTPNAME           0xA9
#define L4_TRAP_TCCTRL              0xAA

#define L4_TRAP_PMN_READ            0xB0
#define L4_TRAP_PMN_WRITE           0xB1

#define L4_TRAP_GETTICK             0xC0
#define L4_TRAP_WBTEST              0xD0

#define SYSNUM(name)                SYSCALL_ ## name
#define SYSCALL_REG                 r8
#define SYSCALL_NUM                 0x31

#endif /* OKL4_ARCH_L4_SYSCALL_ASM_H */
