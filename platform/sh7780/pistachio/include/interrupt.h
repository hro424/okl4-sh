#ifndef OKL4_PLATFORM_SH7780_INTERRUPT_H
#define OKL4_PLATFORM_SH7780_INTERRUPT_H

#include <soc/interface.h>
#include <soc/arch/soc.h>

/*
 * SH2007
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

typedef enum {
    INTEVT_MIN =        0x200,
    IRQ0 =              0x240,
    IRQ1 =              0x280,
    IRQ2 =              0x2C0,
    IRQ3 =              0x300,
    IRQ4 =              0x340,
    IRQ5 =              0x380,
    IRQ6 =              0x3C0,
    IRQ7 =              0x200,
    RTC_ATI =           0x480,
    RTC_PRI =           0x4A0,
    RTC_CUI =           0x4C0,
    WDT =               0x560,
    TMU0 =              0x580,
    TMU1 =              0x5A0,
    TMU2 =              0x5C0,
    TMU2_TICPI =        0x5E0,
    HUDI =              0x600,
    DMAC0_DMINT0 =      0x640,
    DMAC0_DMINT1 =      0x660,
    DMAC0_DMINT2 =      0x680,
    DMAC0_DMINT3 =      0x6A0,
    DMAC0_DMAE =        0x6C0,
    SCIF0_ERI =         0x700,
    SCIF0_RXI =         0x720,
    SCIF0_BRI =         0x740,
    SCIF0_TXI =         0x760,
    DMAC0_DMINT4 =      0x780,
    DMAC0_DMINT5 =      0x7A0,
    DMAC0_DMINT6 =      0x7C0,
    DMAC0_DMINT7 =      0x7E0,
    CMT =               0x900,
    HAC =               0x980,
    PCISERR =           0xA00,
    PCIINTA =           0xA20,
    PCIINTB =           0xA40,
    PCIINTC =           0xA60,
    PCIINTD =           0xA80,
    PCIERR =            0xAA0,
    PCIPWD3 =           0xAC0,
    PCIPWD2 =           0xAE0,
    PCIPWD1 =           0xB00,
    PCIPWD0 =           0xB20,
    SCIF1_ERI =         0xB80,
    SCIF1_RXI =         0xBA0,
    SCIF1_BRI =         0xBC0,
    SCIF1_TXI =         0xBE0,
    SIOF =              0xC00,
    HSPI =              0xC80,
    MMCIF_FSTAT =       0xD00,
    MMCIF_TRAN =        0xD20,
    MMCIF_ERR =         0xD40,
    MMCIF_FRDY =        0xD60,
    DMAC1_DMINT8 =      0xD80,
    DMAC1_DMINT9 =      0xDA0,
    DMAC1_DMINT10 =     0xDC0,
    DMAC1_DMINT11 =     0xDE0,
    TMU3 =              0xE00,
    TMU4 =              0xE20,
    TMU5 =              0xE40,
    SSI =               0xE80,
    FLCTL_FLSTE =       0xF00,
    FLCTL_FLTEND =      0xF20,
    FLCTL_FLTRQ0 =      0xF40,
    FLCTL_FLTRQ1 =      0xF60,
    GPIO0 =             0xF80,
    GPIO1 =             0xFA0,
    GPIO2 =             0xFC0,
    GPIO3 =             0xFE0,
    INTEVT_MAX =        0xFE0
} intevt_e;

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

#define IRQS            (68)

void init_intc(void);

#endif /* OKL4_PLATFORM_SH7780_INTERRUPT_H */
