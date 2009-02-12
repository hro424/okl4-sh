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
    init_clocks();
}

