/**
 * @brief   SH7780 SCIF (Serial Communication Interface) driver
 * @since   March 2009
 */

#include <sh7780_scif.h>


static int
device_setup_impl(struct device_interface* di, struct sh7780_scif* device,
                  struct resource* resources)
{
    return 0;
}

static int
device_enable_impl(struct device_interface* di, struct sh7780_scif* device)
{
    return 0;
}

static int
device_disable_impl(struct device_interface* di, struct sh7780_scif* device)
{
    return 0;
}

static int
device_num_interfaces_impl(struct device_interface* di,
                           struct sh7780_scif* device)
{
    return 2;
}

static struct generic_interface*
device_get_interface_impl(struct device_interface* di,
                          struct sh7780_scif* device,
                          int i)
{
    return NULL;
}

static int
device_interrupt_impl(struct device_interface* di,
                      struct sh7780_scif* device,
                      int irq)
{
    return 0;
}

static int
device_poll_impl(struct device_interface* di, struct sh7780_scif* device)
{
    return 0;
}

static int
stream_sync_impl(struct stream_interface* si, struct sh7780_scif* device)
{
    return 0;
}


