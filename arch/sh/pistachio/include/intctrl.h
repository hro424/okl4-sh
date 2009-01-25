/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_INTERRUPT_CONTROL_H
#define OKL4_ARCH_SH_INTERRUPT_CONTROL_H

void SECTION(".init") init_sh_interrupts();

INLINE word_t
arch_config_interrupt(struct irq_desc* desc, int notify_bit, tcb_t* handler,
                      space_t* owner, soc_irq_action_e action, utcb_t* utcb)
{
    return soc_config_interrupt(desc, notify_bit, (tcb_h)handler,
                                (space_h)owner, action, utcb);
}

INLINE word_t
arch_ack_interrupt(struct irq_desc* desc, tcb_t* handler)
{
    return soc_ack_interrupt(desc, (tcb_h)handler);
}

INLINE word_t
security_control_interrupt(struct irq_desc* desc, space_t* owner,
                           word_t control)
{
    return soc_security_control_interrupt(desc, (space_h)owner, control);
}

#endif /* OKL4_ARCH_SH_INTERRUPT_CONTROL_H */
