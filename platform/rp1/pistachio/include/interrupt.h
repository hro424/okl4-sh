#ifndef OKL4_PLATFORM_SH7780_INTERRUPT_H
#define OKL4_PLATFORM_SH7780_INTERRUPT_H

#include <soc/interface.h>
#include <soc/arch/soc.h>

/*
 * NOTE: SH2007 external interrupts
 * IRQ0     LAN0
 * IRQ1     LAN1
 * IRQ2     Compact Flash
 * IRQ3     IDE
 * IRQ4     PC104:SIRQ3
 * IRQ5     PC104:SIRQ4
 * IRQ6     PC104:SIRQ5
 * IRQ7     PC104:SIRQ7
 */

/**
 * Memory mapped registers
 */
#define INTC_ICR0               0xFFD00000
#define INTC_ICR1               0xFFD0001C
#define INTC_INTPRI             0xFFD00010
#define INTC_INTREQ             0xFFD00024
#define INTC_INTMSK0            0xFFD00044
#define INTC_INTMSK1            0xFFD00048
#define INTC_INTMSK2            0xFFD40080
#define INTC_INTMSKCLR0         0xFFD00064
#define INTC_INTMSKCLR1         0xFFD00068
#define INTC_INTMSKCLR2         0xFFD40084
#define INTC_NMIFCR             0xFFD00000
#define INTC_USERIMASK          0xFFD30000
#define INTC_INT2PRI0           0xFFD40000
#define INTC_INT2PRI1           0xFFD40004
#define INTC_INT2PRI2           0xFFD40008
#define INTC_INT2PRI3           0xFFD4000C
#define INTC_INT2PRI4           0xFFD40010
#define INTC_INT2PRI5           0xFFD40014
#define INTC_INT2PRI6           0xFFD40018
#define INTC_INT2PRI7           0xFFD4001C
#define INTC_INT2A0             0xFFD40030
#define INTC_INT2A1             0xFFD40034
#define INTC_INT2MSKR           0xFFD40038
#define INTC_INT2MSKCR          0xFFD4003C
#define INTC_INT2B0             0xFFD40040
#define INTC_INT2B1             0xFFD40044
#define INTC_INT2B2             0xFFD40048
#define INTC_INT2B3             0xFFD4004C
#define INTC_INT2B4             0xFFD40050
#define INTC_INT2B5             0xFFD40054
#define INTC_INT2B6             0xFFD40058
#define INTC_INT2B7             0xFFD4005C
#define INTC_INT2GPIC           0xFFD40090

/**
 * Masks for ICR0
 */
#define INTC_ICR0_NMIL          0x80000000
#define INTC_ICR0_MAI           0x40000000
#define INTC_ICR0_NMIB          0x02000000
#define INTC_ICR0_NMIE          0x01000000
#define INTC_ICR0_IRLM0         0x00800000
#define INTC_ICR0_IRLM1         0x00400000
#define INTC_ICR0_LSH           0x00200000

/**
 * Event numbers for internal use
 */
typedef enum {
    IRQ0 = 0,
    IRQ1,
    IRQ2,
    IRQ3,
    IRQ4,
    IRQ5,
    IRQ6,
    IRQ7,
    RTC_ATI,
    RTC_PRI,
    RTC_CUI,
    WDT,
    TMU0,
    TMU1,
    TMU2,
    TMU2_TICPI,
    HUDI,
    DMAC0_DMINT0,
    DMAC0_DMINT1,
    DMAC0_DMINT2,
    DMAC0_DMINT3,
    DMAC0_DMAE,
    SCIF0_ERI,
    SCIF0_RXI,
    SCIF0_BRI,
    SCIF0_TXI,
    DMAC0_DMINT4,
    DMAC0_DMINT5,
    DMAC1_DMINT6,
    DMAC1_DMINT7,
    CMT,
    HAC,
    PCISERR,
    PCIINTA,
    PCIINTB,
    PCIINTC,
    PCIINTD,
    PCIERR,
    PCIPWD3,
    PCIPWD2,
    PCIPWD1,
    PCIPWD0,
    SCIF1_ERI,
    SCIF1_RXI,
    SCIF1_BRI,
    SCIF1_TXI,
    SIOF,
    HSPI,
    MMCIF_FSTAT,
    MMCIF_TRAN,
    MMCIF_ERR,
    MMCIF_FRDY,
    DMAC1_DMINT8,
    DMAC1_DMINT9,
    DMAC1_DMINT10,
    DMAC1_DMINT11,
    TMU3,
    TMU4,
    TMU5,
    SSI,
    FLSTE,
    FLTEND,
    FLTRQ0,
    FLTRQ1,
    GPIO0,
    GPIO1,
    GPIO2,
    GPIO3,
    IRQS,
    INTEVT_MIN = 0x200,
    INTEVT_MAX = 0xFE0
} intevt_e;

/**
 * Interrupt masks
 */
typedef enum {
    MASK_IRQ0 =         0x80000000,
    MASK_IRQ1 =         0x40000000,
    MASK_IRQ2 =         0x20000000,
    MASK_IRQ3 =         0x10000000,
    MASK_IRQ4 =         0x08000000,
    MASK_IRQ5 =         0x04000000,
    MASK_IRQ6 =         0x02000000,
    MASK_IRQ7 =         0x01000000,
    MASK_TMU012 =       0x00000001,
    MASK_TMU345 =       0x00000002,
    MASK_RTC =          0x00000004,
    MASK_SCIF0 =        0x00000008,
    MASK_SCIF1 =        0x00000010,
    MASK_WDT =          0x00000020,
    MASK_HUDI =         0x00000080,
    MASK_DMAC0 =        0x00000100,
    MASK_DMAC1 =        0x00000200,
    MASK_CMT =          0x00001000,
    MASK_HAC =          0x00002000,
    MASK_PCIC0 =        0x00004000,
    MASK_PCIC1 =        0x00008000,
    MASK_PCIC2 =        0x00010000,
    MASK_PCIC3 =        0x00020000,
    MASK_PCIC4 =        0x00040000,
    MASK_PCIC5 =        0x00080000,
    MASK_SIOF =         0x00100000,
    MASK_HSPI =         0x00200000,
    MASK_MMCIF =        0x00400000,
    MASK_SSI =          0x00800000,
    MASK_FLCTL =        0x01000000,
    MASK_GPIO =         0x02000000
} intc_mask_e;

typedef enum {
    INTC_PRIORITY_MASK0 =       0x00,
    INTC_PRIORITY_MASK1 =       0x01,
    INTC_PRIORITY_LOWEST =      0x02,
    INTC_PRIORITY_HIGHEST =     0x1F,
} intc_priority_e;

/**
 * Initializes the SH7780 interrupt controller.
 */
void init_intc(void);

/**
 * Masks the specified interrupt.
 *
 * @param index     the interlly used index of the interrupt.
 */
void mask_intevt(word_t index);

/**
 * Removes the mask of the specified interrupt.
 *
 * @param index     the internally used index of the interrupt.
 */
void unmask_intevt(word_t index);

/**
 * Assigns the priority of the specified interrupt.
 * NOTE: Priority 0 and 1 masks the interrupt.
 *
 * @param index     the internally used index of the interrupt.
 * @param prio      the priority to be set.
 */
void set_intprio(word_t index, u8_t prio);

/**
 * Gets the current priority of the specified interrupt.
 *
 * @param index     the internally used index of the interrupt.
 */
u8_t get_intprio(word_t index);

#endif /* OKL4_PLATFORM_SH7780_INTERRUPT_H */
