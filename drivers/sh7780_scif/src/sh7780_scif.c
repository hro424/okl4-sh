/**
 * @brief   SH7780 SCIF (Serial Communication Interface) driver
 * @since   March 2009
 */

#include <sh7780_scif.h>

enum parity { PARITY_NONE, PARITY_EVEN, PARITY_ODD };

#define DEFAULT_BAUD        115200
#define DEFAULT_SIZE        8
#define DEFAULT_PARITY      PARITY_NONE
#define DEFAULT_STOP        1

#define TX_FIFO_DEPTH       64

/* Seven or eight bits for parity */
/* One or two stop bits */
/*
static int serial_set_params(struct sh7780_scif *self, unsigned baud, int data_size,
                      enum parity parity, int stop_bits);
*/

static void
do_tx_work
(
    struct sh7780_scif *device
)
{
    struct stream_interface *si = &device->tx;
    struct stream_pkt *packet = stream_get_head(si);

    if(packet == NULL)
        goto out;

    // to write
    if (scfsr0_get_tdfe() ){ 
        while (packet->xferred < packet->length) {
            assert(packet->data);
            // write to buffer register
            scftdr0_write(packet->data[packet->xferred++]);
            assert(packet->xferred <= packet->length);
            if (packet->xferred == packet->length &&
                    (packet = stream_switch_head(si)) == NULL)
                break;
        }
    }
out:
    return;
}

static int 
do_rx_work
(
    struct sh7780_scif *device
)
{
    struct stream_interface *si = &device->rx;
    struct stream_pkt *packet = stream_get_head(si);
    uint8_t data;
    int got_some = 0;

    while (scrfdr0_get_r()){
        got_some = 1;
        data = scfrdr0_get_data();

#if defined(OKL4_KERNEL_MICRO)
        /* Check for the magic Ctrl-k kernel break-in keyboard shortcut. */
        if( data == 0xb ) { // Ctrl-k
            L4_KDB_Enter("breakin");
            continue;
        }
#endif

        if (!packet)
            continue;

        /* Save the received byte. */
        packet->data[packet->xferred++] = data;

        /* Have we reaced the end of the buffer? */
        if (packet->xferred == packet->length) {
            /* Yes, swap streams. */
            packet = stream_switch_head(si);
        }
    }

    if (packet && packet->xferred)
        packet = stream_switch_head(si);

    if (got_some) {
        // clear error status
        // short status = *SCFSR(self->base);
        // status &= ~(SCFSR_ER|SCFSR_TEND|SCFSR_TDFE|SCFSR_BRK|SCFSR_FER|SCFSR_PER|SCFSR_DR|SCFSR_RDF);
        // *SCFSR(self->base) = status;
        scfsr0_set_er(0);
        scfsr0_set_tend(0);
        scfsr0_set_tdfe(0);
        scfsr0_set_brk(0);
        scfsr0_set_fer(0);
        scfsr0_set_per(0);
        scfsr0_set_dr(0);
        scfsr0_set_rdf(0);
    }

    return 0;
}

static int
stream_sync_impl
(
    struct stream_interface *si,
    struct sh7780_scif      *device
)
{
    int retval = 0;
    
    if(si == &device->tx)
        do_tx_work(device);
    else if (si == &device->rx)
        retval = do_rx_work(device);
    
    return retval;
}

/* Seven or eight bits for parity */
/* One or two stop bits */
static int serial_set_params(struct sh7780_scif *device, unsigned baud, int data_size,
                      enum parity parity, int stop_bits)
{
    if (data_size < 7 || data_size > 8) {
        /* Invalid data_size */
        return -1;
    }

    if (stop_bits < 1 || stop_bits > 2) {
        /* Invalid # of stop bits */
        return -1;
    }

    if (parity != PARITY_NONE &&
            parity != PARITY_EVEN &&
            parity != PARITY_ODD) {
        /* Invalid parity setting */
        return -1;
    }

    /*
     * Pck = 50MHz, n=0 (use Pck)
     * N = Pck / (64 * 2^(2n-1) * bps) * 10^6 -1
     */

    int brt = (50/64*2*1000000*10/baud+5-10)/10;

    scscr0_write(0);
    
    scfcr0_set_rfcl(1);
    scfcr0_set_tfcl(1);
    scfcr0_set_ttrg(1);
    scfcr0_set_rtrg(1);
//    scfcr0_write = SCFCR_RFCL | SCFCR_TFCL | /* clear FIFO */
//        SCFCR_TTRG_0 | SCFCR_RTRG_1; /* set FIFO trigger */
        

    /*uint16_t mode = 0;*/
    if (data_size == 7) {
        scsmr0_set_chr(1);
        /*mode |= SCSMR_CHR; *//* 7bit data */
    }

    switch(parity) {
        case PARITY_EVEN:
            scsmr0_set_pe(1);
            /*mode |= (SCSMR_PE);*/
            break;
        case PARITY_ODD:
            scsmr0_set_pe(1);
            scsmr0_set_oe(1);
            /*mode |= (SCSMR_PE|SCSMR_OE);*/
        case PARITY_NONE:
        default:
            break;
    }

    if (stop_bits == 2) {
        scsmr0_set_stop(1);
        /*mode |= SCSMR_STOP;*/
    }

    /*scsmr0_write(mode);*/
    scbrr0_write(brt);
    /*scsptr0_write(SCSPTR_RTSIO);*/
    scsptr0_set_rtsio(1);
    scfcr0_write(0);
    /*scscr0_write(SCSCR_TE | SCSCR_RE);*/
    scscr0_set_te(1);
    scscr0_set_re(1);

    return 0;
}

static int
device_setup_impl
(
    struct device_interface *di,
    struct sh7780_scif      *device,
    struct resource         *resources
)
{
    int i, n_mem = 0;
    for (i = 0; i < 8; i++) {
        switch(resources->type) {
            case MEMORY_RESOURCE:
                if (n_mem == 0)
                    device->scif0 = *resources;
                else
                    printf("sh7780_scif: got more memory than expected!\n");
                n_mem++;
                break;
            case INTERRUPT_RESOURCE:
            case BUS_RESOURCE:
            case NO_RESOURCE:
                /* do nothing */
                break;
            default:
                printf("sh7780_scif: Invalid resource type %d!\n", resources->type);
                break;
        }
        resources++;
    }
    device->tx.device = device;
    device->rx.device = device;
    device->tx.ops    = stream_ops;
    device->rx.ops    = stream_ops;

    // set parameter
    serial_set_params(device, DEFAULT_BAUD, DEFAULT_SIZE, DEFAULT_PARITY,
                      DEFAULT_STOP);
    return DEVICE_SUCCESS;
}

static int
device_enable_impl
(
    struct device_interface *di,
    struct sh7780_scif      *device
)
{
    serial_set_params(device, DEFAULT_BAUD, DEFAULT_SIZE, DEFAULT_PARITY,
                      DEFAULT_STOP);
    device->state = STATE_ENABLED;

    return 0;
}

static int
device_disable_impl
(
    struct device_interface *di,
    struct sh7780_scif      *device
)
{
    return 0;
}

static int
device_interrupt_impl
(
    struct device_interface *di,
    struct sh7780_scif      *device,
    int                     irq
)
{
    return 0;
}

static int
device_poll_impl
(
    struct device_interface *di,
    struct sh7780_scif      *device
    )
{
    // return device_interrupt_impl(di, device, -1);
    return 0;
}

static int
device_num_interfaces_impl
(
    struct device_interface *di,
    struct sh7780_scif      *dev
    )
{
    return 0;
}

static struct generic_interface *
device_get_interface_impl
(
    struct device_interface *di,
    struct sh7780_scif      *device,
    int                     interface
)
{
    return NULL;
}
