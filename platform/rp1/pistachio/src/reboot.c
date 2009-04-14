/* $Id$ */

/**
 * @since   February 2009
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <soc.h>

#if defined(CONFIG_KDB)
void
soc_reboot()
{
    long    tmp;

    __asm__ __volatile__ (
        "    stc    sr, %0      \n"
        "    or     %1, %0      \n"
        "    ldc    %0, sr      \n"
        "    trapa  #0xFF       \n"
        : "=&z" (tmp)
        : "r" (0x10000000)
        : "memory"
    );

    for (;;) ;
}
#endif

void
soc_panic()
{
    for (;;) ;
}
