/* $Id$ */

/**
 * @brief   SH7780 Timer Unit (TMU)
 * @since   February 2009
 */

#ifndef OKL4_PLATFORM_SH7780_TMU_H
#define OKL4_PLATFORM_SH7780_TMU_H

#define TMU_TOCR                    0xFFD80000
#define TMU_TSTR0                    0xFFD80004
#define TMU_TSTR1                    0xFFDC0004
#define TMU_TCPR2                   0xFFD8002C

#define TMU_TCOR0                    0xFFD80008
#define TMU_TCOR1                    0xFFD80014
#define TMU_TCOR2                    0xFFD80020
#define TMU_TCOR3                    0xFFDC0008
#define TMU_TCOR4                    0xFFDC0014
#define TMU_TCOR5                    0xFFDC0020

#define TMU_TCNT0                    0xFFD8000C
#define TMU_TCNT1                    0xFFD80018
#define TMU_TCNT2                    0xFFD80024
#define TMU_TCNT3                    0xFFDC000C
#define TMU_TCNT4                    0xFFDC0018
#define TMU_TCNT5                    0xFFDC0024

#define TMU_TCR0                     0xFFD80010
#define TMU_TCR1                     0xFFD8001C
#define TMU_TCR2                     0xFFD80028
#define TMU_TCR3                     0xFFDC0010
#define TMU_TCR4                     0xFFDC001C
#define TMU_TCR5                     0xFFDC0028

#define TMU_TSTR0_0                  0x1
#define TMU_TSTR0_1                  0x2
#define TMU_TSTR0_2                  0x4
#define TMU_TSTR1_3                  0x1
#define TMU_TSTR1_4                  0x2
#define TMU_TSTR1_5                  0x4

#define TMU_TCR_ICPF                 0x0200
#define TMU_TCR_UNF                  0x0100
#define TMU_TCR_ICPE_DISABLE         0x0000
#define TMU_TCR_ICPE_ENABLE          0x0080
#define TMU_TCR_ICPE_ENABLE_INT      0x00C0
#define TMU_TCR_UNIE                 0x0020
#define TMU_TCR_CKEG_UP              0x0000
#define TMU_TCR_CKEG_DOWN            0x0008
#define TMU_TCR_TPSC_PCK4            0x0000
#define TMU_TCR_TPSC_PCK16           0x0001
#define TMU_TCR_TPSC_PCK64           0x0002
#define TMU_TCR_TPSC_PCK256          0x0003
#define TMU_TCR_TPSC_PCK1024         0x0004
#define TMU_TCR_TPSC_RTC             0x0006
#define TMU_TCR_TPSC_TCLK            0x0007

void tmu_init(void);
void tmu_start(void);
void tmu_stop(void);
void tmu_clear(void);

#endif /* OKL4_PLATFORM_SH7780_TMU_H */
