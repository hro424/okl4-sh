#ifndef OKL4_ARCH_SH_REGISTERS_H
#define OKL4_ARCH_SH_REGISTERS_H

/**
 *  @since  January 2009
 */

#include <kernel/arch/registers.h>

/*
 * MASKS FOR CPU REGISTERS
 */

/*
 * Status Register
 */

#define REG_SR_MD           (1 << 30)
#define REG_SR_RB           (1 << 29)
#define REG_SR_BL           (1 << 28)
#define REG_SR_FD           (1 << 15)
#define REG_SR_M            (1 << 9)
#define REG_SR_Q            (1 << 8)
#define REG_SR_IMASK        0x000000F0
#define REG_SR_S            (1 << 1)
#define REG_SR_T            (1 << 0)

/*
 * FPU Register
 */
#define REG_FPSCR_FR
#define REG_FPSCR_SZ
#define REG_FPSCR_PR
#define REG_FPSCR_DN
#define REG_FPSCR_CAUSE
#define REG_FPSCR_ENABLE
#define REG_FPSCR_FLAG
#define REG_FPSCR_RM

/*
 * MEMORY-MAPPED REGISTERS (VIRTUAL ADDRESS)
 */

#define UPDATE_REG()        __asm__ __volatile__ ("icbi @%0" :: "r" (0x80000000))

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

/* Version */
#define REG_PVR             0xFF000030
#define REG_PRR             0xFF000044

/* Frequency Control */
#define REG_CPG_FRQCR       0xFFC80000
#define REG_CPG_PLLCR       0xFFC80024
#define REG_CPG_MSTPCR      0xFFC80030

/*
 * MASKS FOR MEMORY-MAPPED REGISTERS
 */

/*
 * Page Table Entry (high)
 */
#define REG_PTEH_VPN_MASK   0xFFFFFC00
#define REG_PTEH_ASID_MASK  0x000000FF

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
#define REG_PTEL_MASK       0x1FFFFDFF

/*
 * MMU Control Register
 */
#define REG_MMUCR_LRUI_MASK 0xFC000000
#define REG_MMUCR_URB_MASK  0x00FC0000
#define REG_MMUCR_URC_MASK  0x0000FC00
#define REG_MMUCR_SQMD      0x00000200
#define REG_MMUCR_SV        0x00000100
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

/*
 * Memory Mapped TLBs
 */
#define REG_ITLB_ADDRESS    0xF2000000
#define REG_ITLB_DATA       0xF3000000
#define REG_UTLB_ADDRESS    0xF6000000
#define REG_UTLB_DATA       0xF7000000

#define REG_UTLB_ASSOC      0x80


/*
 * User Break Controller
 */
#define REG_UBC_CBR0                0xFF200000
#define REG_UBC_CRR0                0xFF200004
#define REG_UBC_CAR0                0xFF200008
#define REG_UBC_CAMR0               0xFF20000C
#define REG_UBC_CBR1                0xFF200020
#define REG_UBC_CRR1                0xFF200024
#define REG_UBC_CAR1                0xFF200028
#define REG_UBC_CAMR1               0xFF20002C
#define REG_UBC_CDR1                0xFF200030
#define REG_UBC_CDMR1               0xFF200034
#define REG_UBC_CETR1               0xFF200038
#define REG_UBC_CCMFR               0xFF200600
#define REG_UBC_CBCR                0xFF200620

#define REG_UBC_CBR_MFE             0x80000000
#define REG_UBC_CBR_AIE             0x40000000
#define REG_UBC_CBR_MFI_MF0         0x00000000
#define REG_UBC_CBR_MFI_MR1         0x01000000
#define REG_UBC_CBR_AIV_MASK        0x00FF0000
#define REG_UBC_CBR_DBE             0x00008000
#define REG_UBC_CBR_SZ_ALL          0x00000000
#define REG_UBC_CBR_SZ_B            0x00001000
#define REG_UBC_CBR_SZ_W            0x00002000
#define REG_UBC_CBR_SZ_L            0x00003000
#define REG_UBC_CBR_SZ_Q            0x00004000
#define REG_UBC_CBR_ETBE            0x00000800
#define REG_UBC_CBR_CD              0x00000000
#define REG_UBC_CBR_ID_ID           0x00000000
#define REG_UBC_CBR_ID_I            0x00000010
#define REG_UBC_CBR_ID_D            0x00000020
#define REG_UBC_CBR_RW_RW           0x00000000
#define REG_UBC_CBR_RW_R            0x00000002
#define REG_UBC_CBR_RW_W            0x00000004
#define REG_UBC_CBR_CE              0x00000001

#define REG_UBC_CRR_PCB             0x00000002
#define REG_UBC_CRR_BIE             0x00000001

#endif /* OKL4_ARCH_SH_REGISTERS_H */
