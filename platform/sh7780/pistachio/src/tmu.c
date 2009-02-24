/* $Id$ */

/**
 * @brief   SH7780 Timer Unit
 * @since   February 2009
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <kernel/arch/registers.h>
#include <timer.h>
#include <tmu.h>

void
tmu_init(void)
{
    u32_t   count;
    u16_t   reg;

    tmu_stop();

    count = TIMER_RATE / (1000000 / TIMER_TICK_LENGTH);

    mapped_reg_write(TMU_CONSTANT0, count);
    mapped_reg_write(TMU_COUNTER0, count);

    reg = TMU_CONTROL_UNIE | TMU_CONTROL_TPSC_PCK4;
    mapped_reg_write16(TMU_CONTROL0, reg);
}

void
tmu_start(void)
{
    u8_t reg;
    reg = mapped_reg_read8(TMU_START0);
    reg |= TMU_START0_0;
    mapped_reg_write8(TMU_START0, reg);
}

void
tmu_stop(void)
{
    u8_t reg;
    reg = mapped_reg_read8(TMU_START0);
    reg &= ~TMU_START0_0;
    mapped_reg_write8(TMU_START0, reg);
}
