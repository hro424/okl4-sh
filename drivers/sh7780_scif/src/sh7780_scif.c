/**
 * @brief   SH7780 SCIF (Serial Communication Interface) driver
 * @since   March 2009
 */

#include <l4/kdebug.h>
#include <sh7780_scif.h>


#define TX_FIFO_DEPTH       64

#define SCIF_BITRATE_9600       161     /* n: 0, Error rate: 0.47 */
#define SCIF_BITRATE_19200      80      /* n: 0, Error rate: 0.47 */
#define SCIF_BITRATE_38400      39      /* n: 0, Error rate: 1.72 */
#define SCIF_BITRATE_57600      26      /* n: 0, Error rate: 0.47 */
#define SCIF_BITRATE_115200     12      /* n: 0, Error rate: 4.33 */
#define SCIF_BITRATE_230400     5       /* n: 0, Error rate: 13.0 */
#define SCIF_BITRATE_460800     2       /* n: 0, Error rate: 13.0 */
#define SCIF_BITRATE_500000     2       /* n: 0, Error rate: 4.17 */
#define SCIF_BITRATE_576000     1       /* n: 0, Error rate: 35.6 */

typedef enum {
    SCIF_SCFCR_RTRG1 =          0x00,
    SCIF_SCFCR_RTRG16 =         0x40,
    SCIF_SCFCR_RTRG32 =         0x80,
    SCIF_SCFCR_RTRG48 =         0xC0,
    SCIF_SCFCR_TTRG32 =         0x00,
    SCIF_SCFCR_TTRG16 =         0x10,
    SCIF_SCFCR_TTRG2 =          0x20,
    SCIF_SCFCR_TTRG0 =          0x30,
} scif_scfcr_e;


/**
 * Delivers the given data to a serial port.  A private function.
 */
static void
do_tx_work
(
    struct sh7780_scif *device
)
{
    struct stream_interface*    si;
    struct stream_pkt*          packet;
    uint8_t*                    ptr;
    size_t                      len;

    si = &device->tx;

    while ((packet = stream_get_head(si)) != NULL) {
        assert(packet->data);
        ptr = packet->data;
        do {
            while (!scfsr1_get_tdfe()) ;

            len = packet->length - packet->xferred;

            if (len > TX_FIFO_DEPTH) {
                len = TX_FIFO_DEPTH;
            }

            for (size_t i = 0; i < len; i++) {
                scftdr1_write(ptr[i]);
            }

            scfsr1_set_tdfe(0);
            scfsr1_set_tend(0);
            ptr += len;
            packet->xferred += len;
            assert(packet->xferred <= packet->length);
        } while (packet->xferred < packet->length);

        stream_switch_head(si);
    }

    return;
}

/**
 * Receives data from a serial port.  A private function.
 */
static int 
do_rx_work
(
    struct sh7780_scif *device
)
{
    struct stream_interface*    si;
    struct stream_pkt*          packet;
    uint8_t                     data;

    si = &device->rx;
    packet = stream_get_head(si);

    do {
        if (scfsr1_get_er()) {
            scfsr1_set_er(0);
            return -1;
        }

        if (scfsr1_get_dr()) {
            scfsr1_set_dr(0);
            return -1;
        }

        if (scfsr1_get_brk()) {
            scfsr1_set_brk(0);
            return -1;
        }

        if (sclsr1_get_orer()) {
            sclsr1_set_orer(0);
            return -1;
        }

        // Nothing in the buffer
        if (!scfsr1_get_rdf()) {
            break;
        }

        data = scfrdr1_read();
        scfsr1_set_rdf(0);

#if defined(OKL4_KERNEL_MICRO)
        if (data == 0xb) {  /* Ctrl-k */
            /* Check for the magic Ctrl-k kernel break-in keyboard shortcut. */
            L4_KDB_Enter("breakin");
            continue;
        }
#endif
        if (!packet) {
            continue;
        }

        /* Save the received byte. */
        packet->data[packet->xferred++] = data;

        if (packet->xferred == packet->length) {
            packet = stream_switch_head(si);
            break;
        }
    } while (scfsr1_get_rdf());

    if (packet && packet->xferred) {
        packet = stream_switch_head(si);
    }

    return 0;
}

/**
 * Triggers delivery or receive of data.
 */
static int
stream_sync_impl
(
    struct stream_interface*    si,
    struct sh7780_scif*         device
)
{
    int retval = 0;
    
    if(si == &device->tx) {
        do_tx_work(device);
    }
    else if (si == &device->rx) {
        retval = do_rx_work(device);
    }
    
    return retval;
}

/**
 * Handles interrupt
 */
static int
device_interrupt_impl
(
    struct device_interface*    di,
    struct sh7780_scif*         device,
    int                         irq
)
{
    /*
     * not implemented yet.
     */ 
    return 0;
}

static int
device_poll_impl
(
    struct device_interface*    di,
    struct sh7780_scif*         device
)
{
    return device_interrupt_impl(di, device, -1);
    /*return 0;*/
}

/**
 * Otains the number of interfaces of this device driver.
 */
static int
device_num_interfaces_impl
(
    struct device_interface *di,
    struct sh7780_scif      *dev
)
{
    return 2;
}

/**
 * Obtains an interface of this device driver.
 */
static struct generic_interface*
device_get_interface_impl
(
    struct device_interface*    di,
    struct sh7780_scif*         device,
    int                         interface
)
{
    switch(interface) {
        case 0:
            return (struct generic_interface *)(void *)&device->tx;
        case 1:
            return (struct generic_interface *)(void *)&device->rx;
        default:
            return NULL;
    }
}

/**
 * Initializes the device.
 *
 * @param di        the device interface
 * @param device    the device
 * @param resources the resource that this driver occupies
 */
static int
device_setup_impl
(
    struct device_interface*    di,
    struct sh7780_scif*         device,
    struct resource*            resources
)
{
    int i, n_mem = 0;
    for (i = 0; i < 8; i++) {
        switch(resources->type) {
            case MEMORY_RESOURCE:
                if (n_mem == 0) {
                    device->main = *resources;
                }
                else {
                    //printf("sh7780_scif: got more memory than expected!\n");
                    L4_KDB_Enter("sh7780_scif: got more memory than expected!");
                }
                n_mem++;
                break;
            case INTERRUPT_RESOURCE:
            case BUS_RESOURCE:
            case NO_RESOURCE:
                /* do nothing */
                break;
            default:
                L4_KDB_Enter("sh7780_scif: Invalid resource type");
                //printf("sh7780_scif: Invalid resource type %d!\n", resources->type);
                break;
        }
        resources++;
    }
    device->tx.device = device;
    device->rx.device = device;
    device->tx.ops    = stream_ops;
    device->rx.ops    = stream_ops;

    // 115200 8N1
    scscr1_write(0);
    scfcr1_set_tfcl(1);
    scfcr1_set_rfcl(1);
    scsmr1_set_cks(0);

    scbrr1_write(SCIF_BITRATE_115200);

    scfcr1_set_rtrg(SCIF_SCFCR_RTRG1);
    scfcr1_set_ttrg(SCIF_SCFCR_TTRG0);

    return DEVICE_SUCCESS;
}

static int
device_enable_impl
(
    struct device_interface*    di,
    struct sh7780_scif*         device
)
{
    scscr1_set_te(1);
    scscr1_set_re(1);
    device->state = STATE_ENABLED;

    return 0;
}

static int
device_disable_impl
(
    struct device_interface*    di,
    struct sh7780_scif*         device
)
{
    device->state = STATE_DISABLED;
    scscr1_set_te(0);
    scscr1_set_re(0);
    return 0;
}

