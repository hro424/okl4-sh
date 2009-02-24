/* $Id$ */

/**
 * @since   February 2009
 */

#include <soc/soc.h>
#include <soc/interface.h>
#include <soc.h>
#include <intctrl.h>
#include <interrupt.h>
#include <timer.h>
#include <tmu.h>

#if defined(CONFIG_KDEBUG_TIMER)
word_t
soc_get_timer_tick_length(void)
{
    return TIMER_TICK_LENGTH;
}
#endif

void NORETURN
handle_timer_interrupt(int wakeup, continuation_t cont)
{
    kernel_scheduler_handle_timer_interrupt(wakeup, TIMER_TICK_LENGTH, cont);
}

word_t soc_timer_disabled;

void
soc_disable_timer(void)
{
    soc_timer_disabled = 1;
    tmu_stop();
}

void
soc_enable_timer(void)
{
    soc_timer_disabled = 0;
    tmu_start();
}

void
init_clocks(void)
{
    tmu_init();
}

