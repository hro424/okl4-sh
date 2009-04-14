/**
 * @brief   SH7780 Timer Unit
 * @since   February 2009
 * @author  Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <kernel/arch/registers.h>
#include <timer.h>
#include <tmu.h>

#define TMU_TSTR        TMU_TSTR0
#define TMU_TSTR_N      TMU_TSTR0_2
#define TMU_TCOR        TMU_TCOR2
#define TMU_TCNT        TMU_TCNT2
#define TMU_TCR         TMU_TCR2

#define PCK             50000000

void
tmu_init(void)
{
    u32_t   count;
    u16_t   reg;

    tmu_stop();

    count = PCK / 4 / (1000000 / TIMER_TICK_LENGTH);

    reg = TMU_TCR_UNIE | TMU_TCR_TPSC_PCK4;
    u16_t tmp;
    do {
        mapped_reg_write16(TMU_TCR, reg);
        tmp = mapped_reg_read16(TMU_TCR);
    } while (tmp != reg);

    mapped_reg_write(TMU_TCOR, count);
    mapped_reg_write(TMU_TCNT, count);
}

void
tmu_start(void)
{
    u8_t reg;
    reg = mapped_reg_read8(TMU_TSTR);
    reg |= TMU_TSTR_N;
    mapped_reg_write8(TMU_TSTR, reg);
}

void
tmu_stop(void)
{
    u8_t reg;
    reg = mapped_reg_read8(TMU_TSTR);
    reg &= ~TMU_TSTR_N;
    mapped_reg_write8(TMU_TSTR, reg);
}

void
tmu_clear(void)
{
    u16_t   reg;

    reg = mapped_reg_read16(TMU_TCR);
    reg &= ~TMU_TCR_UNF;
    mapped_reg_write16(TMU_TCR, reg);
}
