/* $Id$ */

/**
 * @brief   Compare Match Timer
 * @since   February 2009
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <kernel/arch/registers.h>
#include <cmt.h>

#define TIMER_TICK_LENGTH       10000
#define TIMER_RATE              50000000

void
cmt_init(void)
{
    u32_t count;
    count = TIMER_RATE / (1000000 / TIMER_TICK_LENGTH);
}

void
cmt_start(void)
{
}

void
cmt_stop(void)
{
}
