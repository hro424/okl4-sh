/* $Id$ */

/**
 * @brief   SH7780 Timer Unit (TMU)
 * @since   February 2009
 */

#ifndef OKL4_PLATFORM_SH7780_TMU_H
#define OKL4_PLATFORM_SH7780_TMU_H

#define TMU_OUTPUT_CONTROL          0xFFD80000
#define TMU_START0                  0xFFD80004
#define TMU_START3                  0xFFDC0004
#define TMU_INPUT_CAPTURE           0xFFD8002C

#define TMU_CONSTANT0               0xFFD80008
#define TMU_CONSTANT1               0xFFD80014
#define TMU_CONSTANT2               0xFFD80020
#define TMU_CONSTANT3               0xFFDC0008
#define TMU_CONSTANT4               0xFFDC0014
#define TMU_CONSTANT5               0xFFDC0020

#define TMU_COUNTER0                0xFFD8000C
#define TMU_COUNTER1                0xFFD80018
#define TMU_COUNTER2                0xFFD80024
#define TMU_COUNTER3                0xFFDC000C
#define TMU_COUNTER4                0xFFDC0018
#define TMU_COUNTER5                0xFFDC0024

#define TMU_CONTROL0                0xFFD80010
#define TMU_CONTROL1                0xFFD8001C
#define TMU_CONTROL2                0xFFD80028
#define TMU_CONTROL3                0xFFDC0010
#define TMU_CONTROL4                0xFFDC001C
#define TMU_CONTROL5                0xFFDC0028

#define TMU_START0_0                0x1
#define TMU_START0_1                0x2
#define TMU_START0_2                0x4
#define TMU_START3_3                0x1
#define TMU_START3_4                0x2
#define TMU_START3_5                0x4

#define TMU_CONTROL_ICPF            0x0200
#define TMU_CONTROL_UNF             0x0100
#define TMU_CONTROL_ICPE_DISABLE    0x0000
#define TMU_CONTROL_ICPE_ENABLE     0x0080
#define TMU_CONTROL_ICPE_ENABLE_INT 0x00C0
#define TMU_CONTROL_UNIE            0x0020
#define TMU_CONTROL_CKEG_UP         0x0000
#define TMU_CONTROL_CKEG_DOWN       0x0008
#define TMU_CONTROL_TPSC_PCK4       0x0000
#define TMU_CONTROL_TPSC_PCK16      0x0001
#define TMU_CONTROL_TPSC_PCK64      0x0002
#define TMU_CONTROL_TPSC_PCK256     0x0003
#define TMU_CONTROL_TPSC_PCK1024    0x0004
#define TMU_CONTROL_TPSC_RTC        0x0006
#define TMU_CONTROL_TPSC_TCLK       0x0007

void tmu_init(void);
void tmu_start(void);
void tmu_stop(void);

#endif /* OKL4_PLATFORM_SH7780_TMU_H */
