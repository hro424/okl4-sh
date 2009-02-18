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
//    SOC_ASSERT(DEBUG, irq < (word_t)IRQS);
}

void
soc_unmask_irq(word_t irq)
{
//    SOC_ASSERT(DEBUG, irq < (word_t)IRQS);
}


void
soc_handle_interrupt(word_t context, word_t irq)
{
//    SOC_ASSERT(DEBUG, irq < (word_t)IRQS);
}

