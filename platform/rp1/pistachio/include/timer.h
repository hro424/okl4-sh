/* $Id$ */

/**
 * @since   February 2009
 */

#ifndef OKL4_PLATFORM_SH7780_TIMER_H
#define OKL4_PLATFORM_SH7780_TIMER_H

#include <interrupt.h>

#define DEFAULT_TIMER   TMU2

// In micro-second
static const u32_t TIMER_TICK_LENGTH = 10000;

void init_clocks(void);
void NORETURN handle_timer_interrupt(int wakeup, continuation_t cont);

#endif /* OKL4_PLATFORM_SH7780_TIMER_H */

