/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_PROFILE_H
#define OKL4_ARCH_SH_PROFILE_H

#ifdef CONFIG_L4_PROFILING

#define ARCH_NUM_EVENTS             0
#define ARCH_EVENT_TYPE
#define ARCH_EVENT_STR

INLINE u64_t
profile_arch_read_timer()
{
    return 0;
}

#define PROFILE_KERNEL_TIME_START()
#define PROFILE_KERNEL_TIME_STOP()

#else

#define PROFILE_KERNEL_TIME_START()
#define PROFILE_KERNEL_TIME_STOP()

#endif /* CONFIG_L4_PROFILING */

#endif /* OKL4_ARCH_SH_PROFILE_H */
