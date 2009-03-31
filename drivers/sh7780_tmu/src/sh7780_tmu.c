/**
 * @brief   SH7780 TMU (Timer Unit) driver
 * @since   March 2009
 */

#include <l4/kdebug.h>
#ifdef RP1
#include <rp1_tmu.h>
#else
#include <sh7780_tmu.h>
#endif

#define TIMER_RATE              (25000000UL)
#define TIMER_RATE_MHZ          (TIMER_RATE / 1000000UL)
#define COUNT_FROM_DEFAULT      ((1ULL << 32) - 1ULL)

static uint64_t
get_ticks(struct sh7780_tmu *device)
{
    uint64_t diff, value;

    value = tcnt0_read();

    if (value <= device->next_rov){
        diff = device->next_rov - value;
    } else {
        diff = (COUNT_FROM_DEFAULT + device->next_rov) - value;
    }
    return device->ticks + diff;
}

static uint64_t
timer_get_ticks_impl (struct timer_interface *ti, struct sh7780_tmu *device)
{
    return get_ticks(device);
}

static void
timer_set_ticks_impl (struct timer_interface *ti, struct sh7780_tmu *device, uint64_t ticks)
{
    device->ticks = ticks;
}

static uint64_t
timer_get_tick_frequency_impl (struct timer_interface  *ti, struct sh7780_tmu *device)
{
    return TIMER_RATE_MHZ;
}


static uint64_t
timer_set_tick_frequency_impl (struct timer_interface *ti, struct sh7780_tmu *device, uint64_t hz)
{
    // not implemented yet
    return TIMER_RATE_MHZ;
}

static inline void
start_timer(struct sh7780_tmu *device)
{
    /* stop counter */
    tstr0_set_str0(0x0);
    tstr0_set_str1(0x0);
    tstr0_set_str2(0x0);

    /* Set interval */
    tcnt0_write(device->next_rov);

    /* start counter */
    tstr0_set_str0(0x1);
}


static int
timer_timeout_impl (struct timer_interface *ti, struct sh7780_tmu *device, uint64_t timeout)
{
    uint64_t ticks = get_ticks(device);
    
    if (device->remaining_ticks != 0){
        if (timeout < ticks) { //expire
            device->remaining_ticks = 0;
            device->next_rov = COUNT_FROM_DEFAULT;
            start_timer(device);
            return 1;
        }
    }
    device->remaining_ticks -= timeout;
    device->next_rov = (device->remaining_ticks >= COUNT_FROM_DEFAULT) ? (COUNT_FROM_DEFAULT) : device->remaining_ticks;
    
    start_timer(device);
    return 0;
}

static inline void
timer_enable (struct sh7780_tmu *device)
{
    /* clear flag */
    tcr0_set_unf(0x0); // underflag off
    tcr0_set_unie(0x1); // interrupt on 

    /* start timer */
    start_timer(device);
}

static inline void
timer_disable (struct sh7780_tmu *device)
{
    tcr0_set_unie(0x0); // interrupt off
    tstr0_set_str0(0x0); // stop counter
    tcr0_set_unf(0x0); // underflag off
}

static void
reset_register_state (struct sh7780_tmu *device)
{
    /* clear flag */
    tcr0_set_unf(0x0); // underflag off
    // tcr0_set_ckeg(0x2); // Both Edge
    tcr0_set_tpsc(0x0); // P/4 count
    tcr0_set_unie(0x0); // interrupt off 
    tcnt0_write(COUNT_FROM_DEFAULT);
    tcor0_write(COUNT_FROM_DEFAULT);
    device->next_rov = COUNT_FROM_DEFAULT;
    device->remaining_ticks = 0;
    device->ticks = 0;
}

static int
device_setup_impl (struct device_interface *di, struct sh7780_tmu *device, struct resource *resources)
{
    int i, n_mem = 0;
    for (i = 0; i < 8; i++)
    {
        switch (resources->type)
        {
        case MEMORY_RESOURCE:
            if (n_mem == 0)
                device->main = *resources;
            else
                printf("sh7780_tmu: got more memory than expected!\n");
            n_mem++;
            break;
            
        case INTERRUPT_RESOURCE:
        case BUS_RESOURCE:
        case NO_RESOURCE:
            /* do nothing */
            break;
            
        default:
            printf("sh7780_tmu: Invalid resource type %d!\n", resources->type);
            break;
        }
        resources++;
    }

    device->timer.device = device;
    device->timer.ops    = timer_ops;

    reset_register_state(device);

    return DEVICE_SUCCESS;
}

static int
device_enable_impl (struct device_interface *di, struct sh7780_tmu *device)
{
    device->state = STATE_ENABLED;
    device->remaining_ticks = 0;

    timer_enable(device);

    return DEVICE_ENABLED;
}

static int
device_disable_impl (struct device_interface *di, struct sh7780_tmu *device)
{
    device->state = STATE_DISABLED;

    timer_disable(device);

    return DEVICE_DISABLED;
}

static int
device_poll_impl (struct device_interface *di, struct sh7780_tmu *device)
{
    //not implemented yet
    return 0;
}

static int
device_interrupt_impl (struct device_interface *di, struct sh7780_tmu *device, int irq)
{
    int rev = 0;

    tcr0_set_unf(0x0);
    device->ticks += device->next_rov;

    if(device->remaining_ticks > 0 && device->next_rov != COUNT_FROM_DEFAULT) { // Timeout occur
        rev = 1;
    }
    if ( device->remaining_ticks > COUNT_FROM_DEFAULT ) {
        device->remaining_ticks -= COUNT_FROM_DEFAULT;
        device->next_rov = (device->remaining_ticks >= COUNT_FROM_DEFAULT) ? COUNT_FROM_DEFAULT : device->remaining_ticks;
    } else {
        device->next_rov = COUNT_FROM_DEFAULT;
        device->remaining_ticks = 0;
    }
    start_timer(device);
    return rev;
}

static int
device_num_interfaces_impl (struct device_interface *di, struct sh7780_tmu *dev)
{
    return 1;
}

static struct generic_interface *
device_get_interface_impl (struct device_interface *di, struct sh7780_tmu *device, int interface)
{
    return (struct generic_interface *)(void *)&device->timer;
}

