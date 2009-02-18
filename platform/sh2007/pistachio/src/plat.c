/* $Id$ */

/**
 * @since   February 2009
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <soc.h>
#include <intctrl.h>
#include <console.h>
#include <timer.h>
#include <interrupt.h>


/**
 * Initializes the platform specific mappings needed to start the kernel.
 * Add other hardware initialization here as well.
 */
void SECTION(".init")
soc_init()
{
    //TODO: Initialize mapped I/O registers

    /*
    simplesoc_init(IRQS);

    init_clocks();
    */
}

word_t
soc_do_platform_control(tcb_h current, plat_control_t control,
                        word_t param1, word_t param2, word_t param3,
                        continuation_t cont)
{
    //TODO
    utcb_t* current_utcb = kernel_get_utcb(current);
    current_utcb->error_code = ENOT_IMPLEMENTED;
    return 0;
}
