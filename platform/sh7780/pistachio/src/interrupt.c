/* $Id$ */

/**
 * @since   January 2009
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <interrupt.h>
#include <simplesoc.h>

void
soc_mask_irq(word_t irq)
{
    SOC_ASSERT(DEBUG, irq < (word_t)IRQS);
    mapped_reg_write(INT_MASK0, 1 << (31 - irq));
}

void
soc_unmask_irq(word_t irq)
{
    SOC_ASSERT(DEBUG, irq < (word_t)IRQS);
    mapped_reg_write(INT_MASK0_CLR, 1 << (31 - irq));
}

NORETURN void
soc_handle_irq(word_t irq)
{
    continuation_t cont = ASM_CONTINUATION;
    simplesoc_handle_irq_reschedule(irq, cont);
    /* NOT REACHED */
}

void
soc_handle_interrupt(word_t req)
{
    continuation_t cont = ASM_CONTINUATION;

    //TODO

    if (INTEVT_IRQ0 <= req && req <= INTEVT_IRQ7) {
        reg = mapped_reg_read(INT_REQUEST);
        irq = 1 << (reg >> 24);
        simplesoc_handle_irq_reschedule(irq, cont);
    }

    if (EXPECT_TRUE(req == INTEVT_TMU0)) {
        handle_timer_interrupt(0, cont);
    }

    ACTIVATE_CONTINUATION(cont);
}

