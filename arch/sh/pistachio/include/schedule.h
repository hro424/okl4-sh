/**
 * @brief   Utilities for the scheduler
 * @since   December 2008
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#ifndef OKL4_ARCH_SH_SCHEDULE_H
#define OKL4_ARCH_SH_SCHEDULE_H

#include <cpu/cpu.h>

PURE INLINE tcb_t*
get_active_schedule()
{
    return get_globals()->current_schedule;
}

INLINE void
set_active_schedule(tcb_t* tcb)
{
    get_globals()->current_schedule = tcb;
}

INLINE void
processor_sleep()
{
    sh_cpu::sti();
    sh_cpu::sleep();
    sh_cpu::cli();
}

INLINE void
preempt_enable(continuation_t continuation)
{
    get_current_tcb()->enable_preempt_recover(continuation);
    sh_cpu::sti();
}

INLINE void
preempt_disable()
{
    tcb_t* current = get_current_tcb();
    sh_cpu::cli();
    current->disable_preempt_recover();
}

/**
 * Enable timer interrupt
 */
INLINE void
arch_enable_timer()
{
    soc_enable_timer();
}

INLINE void
arch_disable_timer()
{
    soc_disable_timer();
}

#endif /* OKL4_ARCH_SH_SCHEDULE_H */
