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

/**
 * Trap handler
 */
#define L4_TRAP_KPUTC               0xa0
#define L4_TRAP_KGETC               0xa4
#define L4_TRAP_KGETC_NB            0xa8
#define L4_TRAP_KDEBUG              0xac
#define L4_TRAP_GETUTCB             0xb0
#define L4_TRAP_CKBR                0xe4
#define L4_TRAP_KIP					0xb4
#define L4_TRAP_KSET_THRD_NAME		0xb8
#define L4_TRAP_KSET_OBJECT_NAME    0xb8
#define L4_TRAP_GETCOUNTER          0xbc
#define L4_TRAP_GETNUMTPS           0xc0
#define L4_TRAP_GETTPNAME           0xc4
#define L4_TRAP_TCCTRL              0xc8

#define L4_TRAP_PMU_RESET           0xcc
#define L4_TRAP_PMU_STOP            0xd0
#define L4_TRAP_PMU_READ            0xd4
#define L4_TRAP_PMU_CONFIG          0xd8

#define L4_TRAP_GETTICK             0xe0
#define L4_TRAP_WBTEST              0xec

#define SYSCALL_TRAPANUM			0x31
#define SYSCALL_TRAPNUM_IPC			0x30


#define SYSNUM(name)                SYSCALL_ ## name
#define SYSCALL_REG                 r8
#define SYSCALL_NUM                 0x31
