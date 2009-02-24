#ifndef OKL4_PLATFORM_SH7780_INTERRUPT_H
#define OKL4_PLATFORM_SH7780_INTERRUPT_H

#include <soc/interface.h>
#include <soc/arch/soc.h>

#define IRQS                8

#define USE_SIMPLESOC       1

void soc_mask_irq(word_t irq);
void soc_unmask_irq(word_t irq);

#endif /* OKL4_PLATFORM_SH7780_INTERRUPT_H */
