
#ifndef OKL4_ARCH_SH_KERNEL_EXCEPTION_H
#define OKL4_ARCH_SH_KERNEL_EXCEPTION_H

#define PC(x)   ((x) & ~1UL)

/*
 * Generic exception message format
 * All exceptions not handled by the kernel or other exception
 * messages.
 */

#define EXCEPT_IPC_GEN_MR_IP        1
#define EXCEPT_IPC_GEN_MR_SP        2
#define EXCEPT_IPC_GEN_MR_FLAGS     3
#define EXCEPT_IPC_GEN_MR_EXCEPTNO  4
#define EXCEPT_IPC_GEN_MR_ERRORCODE 5
#define EXCEPT_IPC_GEN_MR_NUM       5

/*
 * System call exception
 */
#define EXCEPT_IPC_SYS_MR_R0        1
#define EXCEPT_IPC_SYS_MR_R1        2
#define EXCEPT_IPC_SYS_MR_R2        3
#define EXCEPT_IPC_SYS_MR_R3        4
#define EXCEPT_IPC_SYS_MR_R4        5
#define EXCEPT_IPC_SYS_MR_R5        6
#define EXCEPT_IPC_SYS_MR_R6        7
#define EXCEPT_IPC_SYS_MR_R7        8
#define EXCEPT_IPC_SYS_MR_PC        9
#define EXCEPT_IPC_SYS_MR_SP        10
#define EXCEPT_IPC_SYS_MR_PR        11
#define EXCEPT_IPC_SYS_MR_SYSCALL   12
#define EXCEPT_IPC_SYS_MR_FLAGS     13
#define EXCEPT_IPC_SYS_MR_NUM       13

/*
 * SH4A exception code
 */

#define ECODE_POWER_ON_RESET        0x000
#define ECODE_H_UDI_RESET           0x000
#define ECODE_MANUAL_RESET          0x020
#define ECODE_TLB_MISS_R            0x040
#define ECODE_TLB_MISS_W            0x060
#define ECODE_INIT_WRITE            0x080
#define ECODE_TLB_FAULT_R           0x0A0
#define ECODE_TLB_FAULT_W           0x0C0
#define ECODE_ADDRESS_R             0x0E0
#define ECODE_ADDRESS_W             0x100
#define ECODE_FPU                   0x120
#define ECODE_TLB_MULTI_HIT         0x140
#define ECODE_TRAPA                 0x160
#define ECODE_GENERAL_INST          0x180
#define ECODE_SLOT_INST             0x1A0
#define ECODE_NMI                   0x1C0
#define ECODE_USER_BREAK            0x1E0
#define ECODE_GENERAL_FPU           0x800
#define ECODE_SLOT_FPU              0x820

#endif /* OKL4_ARCH_SH_KERNEL_EXCEPTION_H */
