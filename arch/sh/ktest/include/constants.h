#ifndef OKL4_ARCH_KTEST_CONSTANTS_H
#define OKL4_ARCH_KTEST_CONSTNATS_H

#include <stdint.h>
#include <l4/arch/config.h>

#define N_MSG_REGS 32
#define MAX_MAP_ITEMS (N_MSG_REGS/3)

/* The default number of thread bits, as found in pistachio's Sconscript. */
#define MAX_THREAD_BITS 13

#define NTHREADS 32

#define CLEAR_ERROR_CODE (L4_GetUtcbBase())[ __L4_TCR_ERROR_CODE ] = 0;

#endif  /* OKL4_ARCH_KTEST_CONSTANTS_H */
