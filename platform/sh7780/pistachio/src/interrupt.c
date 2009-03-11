/**
 * @brief   Interrupt handlers and utilities
 * @since   January 2009
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <interrupt.h>
#include <soc.h>
#include <timer.h>
#include <kernel/arch/registers.h>
#include <kernel/arch/continuation.h>

typedef struct {
    soc_ref_t   handler;
    word_t      notify_mask;
} irq_mapping_t;

typedef struct {
    space_h     owner;
} irq_owner_t;

/* Converts INTEVT to the internal interrupt number */
static word_t evttbl[112] = {
    IRQ7, 0, IRQ0, 0, IRQ1, 0, IRQ2, 0, IRQ3, 0,
    /* 10 */
    IRQ4, 0, IRQ5, 0, IRQ6, 0, 0, 0, 0, 0,
    /* 20 */
    RTC_ATI, RTC_PRI, RTC_CUI, 0, 0, 0, 0, WDT, TMU0, TMU1,
    /* 30 */
    TMU2, TMU2_TICPI, HUDI, 0, DMAC0_DMINT0, DMAC0_DMINT1, DMAC0_DMINT2,
    DMAC0_DMINT3, DMAC0_DMAE, 0,
    /* 40 */
    SCIF0_ERI, SCIF0_RXI, SCIF0_BRI, SCIF0_TXI, DMAC0_DMINT4, DMAC0_DMINT5,
    DMAC1_DMINT6, DMAC1_DMINT7, 0, 0,
    /* 50 */
    0, 0, 0, 0, 0, 0, CMT, 0, 0, 0,
    /* 60 */
    HAC, 0, 0, 0, PCISERR, PCIINTA, PCIINTB, PCIINTC, PCIINTD, PCIERR,
    /* 70 */
    PCIPWD3, PCIPWD2, PCIPWD1, PCIPWD0, 0, 0, SCIF1_ERI, SCIF1_RXI, SCIF1_BRI,
    SCIF1_TXI,
    /* 80 */
    SIOF, 0, 0, 0, HSPI, 0, 0, 0, MMCIF_FSTAT, MMCIF_TRAN,
    /* 90 */
    MMCIF_ERR, MMCIF_FRDY, DMAC1_DMINT8, DMAC1_DMINT9, DMAC1_DMINT10,
    DMAC1_DMINT11, TMU3, TMU4, TMU5, 0,
    /* 100 */
    SSI, 0, 0, 0, FLSTE, FLTEND, FLTRQ0, FLTRQ1, GPIO0, GPIO1,
    /* 110 */
    GPIO2, GPIO3
};

#define IRQ_VECT(evt, mask)     mask

static word_t intc_mask[IRQS] = {
    IRQ_VECT(IRQ0,          MASK_IRQ0),
    IRQ_VECT(IRQ1,          MASK_IRQ1),
    IRQ_VECT(IRQ2,          MASK_IRQ2),
    IRQ_VECT(IRQ3,          MASK_IRQ3),
    IRQ_VECT(IRQ4,          MASK_IRQ4),
    IRQ_VECT(IRQ5,          MASK_IRQ5),
    IRQ_VECT(IRQ6,          MASK_IRQ6),
    IRQ_VECT(IRQ7,          MASK_IRQ7),
    IRQ_VECT(RTC_ATI,       MASK_RTC),
    IRQ_VECT(RTC_PRI,       MASK_RTC),
    IRQ_VECT(RTC_CUI,       MASK_RTC),
    IRQ_VECT(WDT,           MASK_WDT),
    IRQ_VECT(TMU0,          MASK_TMU012),
    IRQ_VECT(TMU1,          MASK_TMU012),
    IRQ_VECT(TMU2,          MASK_TMU012),
    IRQ_VECT(TMU2_TICPI,    MASK_TMU012),
    IRQ_VECT(HUDI,          MASK_HUDI),
    IRQ_VECT(DMAC0_DMINT0,  MASK_DMAC0),
    IRQ_VECT(DMAC0_DMINT1,  MASK_DMAC0),
    IRQ_VECT(DMAC0_DMINT2,  MASK_DMAC0),
    IRQ_VECT(DMAC0_DMINT3,  MASK_DMAC0),
    IRQ_VECT(DMAC0_DMINT3,  MASK_DMAC0),
    IRQ_VECT(SCIF0_ERI,     MASK_SCIF0),
    IRQ_VECT(SCIF0_RXI,     MASK_SCIF0),
    IRQ_VECT(SCIF0_BRI,     MASK_SCIF0),
    IRQ_VECT(SCIF0_TXI,     MASK_SCIF0),
    IRQ_VECT(DMAC0_DMINT4,  MASK_DMAC0),
    IRQ_VECT(DMAC0_DMINT5,  MASK_DMAC0),
    IRQ_VECT(DMAC0_DMINT6,  MASK_DMAC0),
    IRQ_VECT(DMAC0_DMINT7,  MASK_DMAC0),
    IRQ_VECT(CMT,           MASK_CMT),
    IRQ_VECT(HAC,           MASK_HAC),
    IRQ_VECT(PCISERR,       MASK_PCIC0),
    IRQ_VECT(PCIINTA,       MASK_PCIC1),
    IRQ_VECT(PCIINTB,       MASK_PCIC2),
    IRQ_VECT(PCIINTC,       MASK_PCIC3),
    IRQ_VECT(PCIINTD,       MASK_PCIC4),
    IRQ_VECT(PCIERR,        MASK_PCIC5),
    IRQ_VECT(PCIPWD3,       MASK_PCIC5),
    IRQ_VECT(PCIPWD2,       MASK_PCIC5),
    IRQ_VECT(PCIPWD1,       MASK_PCIC5),
    IRQ_VECT(PCIPWD0,       MASK_PCIC5),
    IRQ_VECT(SCIF1_ERI,     MASK_SCIF1),
    IRQ_VECT(SCIF1_RXI,     MASK_SCIF1),
    IRQ_VECT(SCIF1_BRI,     MASK_SCIF1),
    IRQ_VECT(SCIF1_TXI,     MASK_SCIF1),
    IRQ_VECT(SIOF,          MASK_SIOF),
    IRQ_VECT(HSPI,          MASK_HSPI),
    IRQ_VECT(MMCIF_FSTAT,   MASK_MMCIF),
    IRQ_VECT(MMCIF_TRAN,    MASK_MMCIF),
    IRQ_VECT(MMCIF_ERR,     MASK_MMCIF),
    IRQ_VECT(MMCIF_FRDY,    MASK_MMCIF),
    IRQ_VECT(DMAC1_DMINT8,  MASK_DMAC1),
    IRQ_VECT(DMAC1_DMINT9,  MASK_DMAC1),
    IRQ_VECT(DMAC1_DMINT10, MASK_DMAC1),
    IRQ_VECT(DMAC1_DMINT11, MASK_DMAC1),
    IRQ_VECT(TMU3,          MASK_TMU345),
    IRQ_VECT(TMU4,          MASK_TMU345),
    IRQ_VECT(TMU5,          MASK_TMU345),
    IRQ_VECT(SSI,           MASK_SSI),
    IRQ_VECT(FLSTE,         MASK_FLCTL),
    IRQ_VECT(FLTEND,        MASK_FLCTL),
    IRQ_VECT(FLTRQ0,        MASK_FLCTL),
    IRQ_VECT(FLTRQ1,        MASK_FLCTL),
    IRQ_VECT(GPIO0,         MASK_GPIO),
    IRQ_VECT(GPIO1,         MASK_GPIO),
    IRQ_VECT(GPIO2,         MASK_GPIO),
    IRQ_VECT(GPIO3,         MASK_GPIO),
};

/* Map interrupt numbers to handler/notify mask information. */
static irq_mapping_t    irq_mapping[IRQS];

/* Get the owner space for each interrupt */
static irq_owner_t      irq_owners[IRQS];

/* Bitmap of pending IRQs */
static bitmap_t         irq_pending[IRQS];

/* Initialise interrupt data strctures. */
void
init_intc(void)
{
    int i;

    //TODO:

    /* Mask IRQs */
    mapped_reg_write(INTC_INTMSK0, 0xFF000000);
    mapped_reg_write(INTC_INTMSK1, 0xC0000000);
    mapped_reg_write(INTC_INT2MSKR, 0x03FFF3BF);

    /* Setup memory. */
    for (i = 0; i < IRQS + 1; i++) {
        kernel_ref_init(&irq_mapping[i].handler);
    }
    bitmap_init(irq_pending, IRQS, false);
}

static inline word_t
evt2index(word_t intevt)
{
    word_t i = (intevt - INTEVT_MIN) >> 5;
    return evttbl[i];
}

static inline void
mask_external_interrupt(word_t index)
{
    mapped_reg_write(INTC_INTMSK0, intc_mask[index]);
}

static inline void
unmask_internal_interrupt(word_t index)
{
    mapped_reg_write(INTC_INTMSKCLR0, intc_mask[index]);
}

static inline void
mask_internal_interrupt(word_t index)
{
    mapped_reg_write(INTC_INT2MSKR, intc_mask[index]);
}

static inline void
unmask_external_interrupt(word_t index)
{
    mapped_reg_write(INTC_INT2MSKCR, intc_mask[index]);
}

static void
mask_intevt(word_t index)
{
    if (index > IRQ7) {
        mask_internal_interrupt(index);
    }
    else {
        mask_external_interrupt(index);
    }
}

static void
unmask_intevt(word_t index)
{
    if (index > IRQ7) {
        unmask_internal_interrupt(index);
    }
    else {
        unmask_external_interrupt(index);
    }
}

/*
 * Handle an IRQ. If continuation is non-NULL, we activate it after delivering
 * an interrupt, or return if an error occurred. If it is NULL, return 1 if a
 * reschedule is required, 0 otherwise.
 */
static int
handle_irq(word_t intevt, continuation_t cont)
{
    tcb_h   handler;
    utcb_t* utcb;
    word_t  mask;
    word_t  index;
    word_t* ievt_desc;

    index = evt2index(intevt);

    /* Mask off the IRQ. */
    mask_intevt(index);

    /* Ensure that we have a handler for it. */
    handler = kernel_ref_get_tcb(irq_mapping[index].handler);
    if (EXPECT_FALSE(!handler)) {
        /* Spurious interrupt. */
        return 0;
    }

    /* Fetch the handler thread's IRQ UTCB word. */
    utcb = kernel_get_utcb(handler);
    SOC_ASSERT(DEBUG, utcb);
    ievt_desc = &utcb->platform_reserved[0];

    /* If the handler hasn't dealt with the previous interrupt yet,
     * mark the interrupt as pending.
     */
    if (EXPECT_FALSE(*ievt_desc != ~0UL)) {
        bitmap_set(irq_pending, index);
        return 0;
    }

    /* Otherwise, deliver the nofitication, and return whether
     * we require a reschedule.
     */
    *ievt_desc = intevt;
    mask = irq_mapping[index].notify_mask;
    return kernel_deliver_notify(handler, mask, cont);
}

/*
 * Handle an IRQ, and perform a reschedule.
 */
static NORETURN void
handle_irq_reschedule(word_t intevt, continuation_t cont)
{
    /* Deliver the IRQ. */
    handle_irq(intevt, cont);

    /* If the IRQ could not be delivered, just activate the continuation. */
    ACTIVATE_CONTINUATION(cont);
}

void
soc_handle_interrupt(word_t context, word_t intevt)
{
    continuation_t cont = ASM_CONTINUATION;

    if (INTEVT_MIN <= intevt && intevt <= INTEVT_MAX) {
        handle_irq_reschedule(intevt, cont);
    }

    //TODO:
    if (EXPECT_TRUE(evt2index(intevt) == TMU0)) {
        handle_timer_interrupt(0, cont);
    }

    ACTIVATE_CONTINUATION(cont);
}

/**
 * Register and unregister interrupts
 * 
 * @param desc          the exception code to be configured
 * @param notify_bit    the notify bit to be sent to the receiving thread
 * @param handler
 * @param owner         the address space where the receiving thread resides
 * @param control       register or unregister
 * @param utcb
 */
word_t
soc_config_interrupt(struct irq_desc *desc, int notify_bit, tcb_h handler,
                     space_h owner, soc_irq_action_e control, utcb_t *utcb)
{
    word_t  intevt;
    word_t  index;

    /* Get the IRQ to configure. */
    intevt = *((word_t*)desc);
    if (intevt < INTEVT_MIN || intevt > INTEVT_MAX) {
        return EINVALID_PARAM;
    }

    index = evt2index(intevt);

    /* Ensure we have permission to configure this IRQ. */
    if (irq_owners[index].owner != owner) {
        return EINVALID_PARAM;
    }

    switch (control) {
        case soc_irq_register:
        {
            /* Ensure that the owner is correct. */
            if (irq_owners[index].owner != owner) {
                return EINVALID_PARAM;
            }

            /* Ensure that no other thread is already registered. */
            if (kernel_ref_get_tcb(irq_mapping[index].handler)) {
                return EINVALID_PARAM;
            }

            /*
             * HACK: If the UTCB has not been initialised to (-1) do that now.
             * This is problematic if the user is using IRQ 0 and has received
             * such an IRQ when they do this register.
             *
             * The correct way to handle this is for the kernel to give the SoC
             * a callback each time a new thread is created, allowing us to
             * initialise it then.
             */
            if (utcb->platform_reserved[0] == 0) {
                utcb->platform_reserved[0] = ~0UL;
            }

            /* Store the information about the handler. */
            kernel_ref_set_referenced(handler, &irq_mapping[index].handler);
            irq_mapping[index].notify_mask = (1UL << notify_bit);

            /* Callback the SoC. */
            //soc_register_irq_callback(irq);

            /* Unmask the IRQ. */
            unmask_intevt(index);
            break;
        }

        case soc_irq_unregister:
        {
            /* Ensure that the owner is correct. */
            if (irq_owners[index].owner != owner) {
                return EINVALID_PARAM;
            }

            /* Ensure that the handler is correct. */
            if (kernel_ref_get_tcb(irq_mapping[index].handler) != handler) {
                return EINVALID_PARAM;
            }

            /* If userspace has not acknowledged the interrupt he is
             * deregistering, do that now.
             */
            if (utcb->platform_reserved[0] == intevt) {
                utcb->platform_reserved[0] = ~0UL;
            }

            /* Clear out the reference. */
            kernel_ref_remove_referenced(&irq_mapping[index].handler);
            irq_mapping[index].notify_mask = 0;

            /* Mask off the interrupt. */
            mask_intevt(index);

            /* Callback the SoC. */
            //soc_deregister_irq_callback(irq);

            break;
        }
    }

    return 0;
}

/*
 * Get an interrupt pending for the given handler, or (-1) to
 * indicate that no such interrupt is pending.
 */
static int
get_pending_interrupt(tcb_h handler)
{
    word_t      i;
    bitmap_t    tmp_pending[BITMAP_SIZE(IRQS)];
    int         pend;

    /* Are there any bits set at all in the bitmap? */
    pend = bitmap_findfirstset(irq_pending, IRQS);
    if (pend == -1) {
        return -1;
    }

    /* Make a copy of the bitmap, so we can destroy it as we search through it
     * below. */
    for (i = 0; i < BITMAP_SIZE(IRQS); i++) {
        tmp_pending[i] = irq_pending[i];
    }

    /* Search through the copy for a pending interrupt registered to the
     * handler. */
    do {
        bitmap_clear(tmp_pending, pend);

        tcb_h itr_handler = kernel_ref_get_tcb(irq_mapping[pend].handler);
        if (itr_handler == handler) {
            return pend;
        }
        pend = bitmap_findfirstset(tmp_pending, IRQS);
    } while (pend != -1);

    return -1;
}


/*
 * Ack the interrupt.
 */
word_t
soc_ack_interrupt(struct irq_desc *desc, tcb_h handler)
{
    int     pend;
    word_t  intevt;
    word_t  index;
    utcb_t* utcb;

    intevt = *(word_t *)desc;

    /* Ensure IRQ number is same. */
    if (intevt >= IRQS) {
        return EINVALID_PARAM;
    }

    index = evt2index(intevt);

    /* Ensure the handler is associated with this IRQ. */
    if (kernel_ref_get_tcb(irq_mapping[index].handler) != handler) {
        return EINVALID_PARAM;
    }

    /* Clear the IRQ in the thread's UTCB. */
    utcb = kernel_get_utcb(handler);
    if (utcb->platform_reserved[0] == intevt) {
        utcb->platform_reserved[0] = ~0UL;
    }
    unmask_intevt(index);

    /* Are there any pending interrupts? */
    pend = get_pending_interrupt(handler);
    if (pend == -1) {
        return 0;
    }

    /* Otherwise, handle the interrupt. */
    bitmap_clear(irq_pending, pend);
    utcb->platform_reserved[0] = pend;
    /* BUG: The kernel may need to perform a full schedule after this,
     * but we have no way of indicating this back to them.
     */
    kernel_deliver_notify(handler, irq_mapping[pend].notify_mask, NULL);

    return 0;
}

/*
 * Configure access controls for interrupts
 */
word_t
soc_security_control_interrupt(struct irq_desc *desc, space_h owner,
                               word_t control)
{
    word_t  intevt;
    word_t  index;

    /* Get the IRQ to configure. */
    intevt = *(word_t *)desc;
    if (intevt >= IRQS) {
        return EINVALID_PARAM;
    }

    index = evt2index(intevt);

    /* Determine what type of operation we are performing. */
    switch (control >> 16) {
        case 0: /* Grant */
            if (irq_owners[index].owner) {
                return EINVALID_PARAM;
            }
            irq_owners[index].owner = owner;
            break;

        case 1: /* Revoke */
            if (irq_owners[index].owner != owner) {
                return EINVALID_PARAM;
            }
            irq_owners[index].owner = NULL;
            break;

        default:
            return EINVALID_PARAM;
    }

    return 0;
}


