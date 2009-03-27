#include <ktest/arch/constants.h>
#include <ktest/ktest.h>
#include <ktest/utility.h>

#include <l4/misc.h>

const L4_Word_t default_attr = L4_WriteBackMemory;

const char* arch_excludes[] = {NULL};

L4_Word_t*
get_arch_utcb_base(void)
{
    return L4_GetUtcbBase();
}

L4_Word_t* get_mr_base(void)
{
    return &(L4_GetUtcbBase())[__L4_TCR_MR_OFFSET];
}
