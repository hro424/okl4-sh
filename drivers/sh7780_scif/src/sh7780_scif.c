/**
 * @brief   SH7780 SCIF (Serial Communication Interface) driver
 * @since   March 2009
 */

#include <sh7780_scif.h>

static int
stream_sync_impl
(
    struct stream_interface *si,
    struct sh7780_scif      *device
)
{
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
    device->tx.device = device;
    device->rx.device = device;
    device->tx.ops    = stream_ops;
    device->rx.ops    = stream_ops;
    return 0;
}

static int
device_enable_impl
(
    struct device_interface *di,
    struct sh7780_scif      *device
)
{
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
