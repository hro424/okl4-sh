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

#define IRQS                8
#define USE_SIMPLESOC       1

void soc_mask_irq(word_t irq);
void soc_unmask_irq(word_t irq);

#define INT_CONTROL0            0xFFD00000
#define INT_CONTROL1            0xFFD0001C
#define INT_PRIORITY            0xFFD00010
#define INT_REQUEST             0xFFD00024
#define INT_MASK0               0xFFD00044
#define INT_MASK1               0xFFD00048
#define INT_MASK2               0xFFD40080
#define INT_MASK0_CLR           0xFFD00064
#define INT_MASK1_CLR           0xFFD00068
#define INT_MASK2_CLR           0xFFD40084
#define INT_NMI_CONTROL         0xFFD00000
#define INT_USER_MASK           0xFFD30000
#define INT2_PRIORITY0          0xFFD40000
#define INT2_PRIORITY1          0xFFD40004
#define INT2_PRIORITY2          0xFFD40008
#define INT2_PRIORITY3          0xFFD4000C
#define INT2_PRIORITY4          0xFFD40010
#define INT2_PRIORITY5          0xFFD40014
#define INT2_PRIORITY6          0xFFD40018
#define INT2_PRIORITY7          0xFFD4001C
#define INT2_REQUEST            0xFFD40030
#define INT2_REQUEST_MASK       0xFFD40034
#define INT2_MASK               0xFFD40038
#define INT2_MASK_CLR           0xFFD4003C
#define INT2_MODREQ0            0xFFD40040
#define INT2_MODREQ1            0xFFD40044
#define INT2_MODREQ2            0xFFD40048
#define INT2_MODREQ3            0xFFD4004C
#define INT2_MODREQ4            0xFFD40050
#define INT2_MODREQ5            0xFFD40054
#define INT2_MODREQ6            0xFFD40058
#define INT2_MODREQ7            0xFFD4005C
#define INT2_GPIO               0xFFD40090

#define INT_CONTROL0_NMIL       0x80000000
#define INT_CONTROL0_MAI        0x40000000
#define INT_CONTROL0_NMIB       0x02000000
#define INT_CONTROL0_NMIE       0x01000000
#define INT_CONTROL0_IRLM0      0x00800000
#define INT_CONTROL0_IRLM1      0x00400000
#define INT_CONTROL0_LSH        0x00200000


#define INTEVT_TMU0             0x580
#define INTEVT_TMU1             0x5A0
#define INTEVT_TMU2             0x5C0
#define INTEVT_TMU2_INPUT       0x5E0
#define INTEVT_SCIF0_ERR        0x700
#define INTEVT_SCIF0_RX         0x720
#define INTEVT_SCIF0_BRK        0x740
#define INTEVT_SCIF0_TX         0x760
#define INTEVT_CMT              0x900
#define INTEVT_SCIF1_ERR        0xB80
#define INTEVT_SCIF1_RX         0xBA0
#define INTEVT_SCIF1_BRK        0xBC0
#define INTEVT_SCIF1_TX         0xBE0
#define INTEVT_TMU3             0xE00
#define INTEVT_TMU4             0xE20
#define INTEVT_TMU5             0xE40

#endif /* OKL4_PLATFORM_SH7780_INTERRUPT_H */
