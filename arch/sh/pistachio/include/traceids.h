/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_TRACEIDS_H
#define OKL4_ARCH_SH_TRACEIDS_H

#define ARCH_TRACEIDS                   \
    EXCEPTION_IPC_GENERAL =         70, \
    EXCEPTION_IPC_SYSCALL =         71,

#define ARCH_TRACE_MAJORIDS             \
    EXCEPTION_IPC_GENERAL_major =   3,  \
    EXCEPTION_IPC_SYSCALL_major =   3,

#define ARCH_TRACE_STRINGS              \
    "EXCEPTION_IPC_GENERAL",            \
    "EXCEPTION_IPC_SYSCALL",

#endif /* OKL4_ARCH_SH_TRACEIDS_H */
