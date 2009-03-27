/* $Id$ */

/**
 *  @since  January 2009
 */

#include <l4.h>
#include <mkasmsym.h>
#include <tcb.h>

MKASMSYM_START

#define cpp_offsetof(type, field)   ((unsigned) &(((type*)4)->field) - 4)

MKASMSYM(OFS_TCB_ARCH_CONTEXT, cpp_offsetof(tcb_t, arch.context));
MKASMSYM(OFS_TCB_ARCH_EXC_NUM, cpp_offsetof(tcb_t, arch.exc_num));
MKASMSYM(OFS_TCB_ARCH_EXP_CTXT, cpp_offsetof(tcb_t, arch.misc.exception.exception_context));

MKASMSYM(OFS_GLOBAL_KSPACE, cpp_offsetof(globals_t, kernel_space));
MKASMSYM(OFS_GLOBAL_CURRENT_TCB, cpp_offsetof(globals_t, current_tcb));
MKASMSYM(OFS_GLOBAL_CURRENT_SCHEDULE, cpp_offsetof(globals_t, current_schedule));
MKASMSYM(OFS_GLOBAL_CURRENT_CLIST, cpp_offsetof(globals_t, current_clist));

#undef cpp_offsetof

MKASMSYM_END
