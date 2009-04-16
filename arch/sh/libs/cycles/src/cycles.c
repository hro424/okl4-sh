/**
 * @brief   Cycle counter
 * @since   April 2009
 */

#include <cycles/arch/cycles.h>
#include <l4/kdebug.h>

//XXX: CONFIDENTIAL START
void
setup_PPC(void)
{
    L4_KDB_PPC_Config(0);
    L4_KDB_PPC_SetDirectMode();
    L4_KDB_PPC_ExpandCounter();
}

void
restart_PPC(void)
{
    L4_KDB_PPC_Enable();
    L4_KDB_PPC_Restart();
}

void
stop_PPC(void)
{
    L4_KDB_PPC_Stop();
    L4_KDB_PPC_Disable();
}

uint64_t
get_PPC(void)
{
    return L4_KDB_PPC_GetCounter();
}
//XXX: CONFIDENTIAL END
