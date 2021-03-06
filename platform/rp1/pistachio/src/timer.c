/**
 * @brief   Interface to control the timer unit
 * @since   February 2009
 * @author  Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
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
    tmu_clear();
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
    unmask_intevt(DEFAULT_TIMER);
    set_intprio(DEFAULT_TIMER, INTC_PRIORITY_HIGHEST);
    tmu_init();
    tmu_start();
}

