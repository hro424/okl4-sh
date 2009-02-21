/* $Id$ */

/**
 * @since   February 2009
 */

#ifndef OKL4_PLATFORM_SH2007_TIMER_H
#define OKL4_PLATFORM_SH2007_TIMER_H

static const int TIMER_TICK_LENGTH = 10000;

void init_clocks(void);
void NORETURN handle_timer_interrupt(int wakeup, continuation_t cont);

#endif /* OKL4_PLATFORM_SH2007_TIMER_H */

