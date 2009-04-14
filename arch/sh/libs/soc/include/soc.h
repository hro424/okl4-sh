/**
 * @file
 * @brief Contains the interface for functionality provided on a per-platform
 *        basis, specific to ARM-based platforms.
 *
 */

#ifndef OKL4_ARCH_SH_SOC_SOC_H
#define OKL4_ARCH_SH_SOC_SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_PERF)
/**
 * @brief This word contains the flags which indicate that performance
 *        counters are enabled.
 */
extern word_t soc_perf_counter_irq;

void soc_perf_counter_unmask(void);
#endif

/**
 * @brief Interrupt dispatch routine.
 *
 * This is normally called from assembly code common to the
 * architecture.
 *
 * @param ctx A pointer to the context at interrupt time.
 * @param fiq If set to 0 indicates a normal interrupt.  !0 indicates
 *            an FIQ.
 */
void soc_handle_interrupt(word_t ctx,
                          word_t fiq);

/*
 * SOC platform cache manipulation functions
 *
 * These functions provide a "common" API for all caches that are
 * not integrated in CPU but are implemented on platform.
 */

#if defined(CONFIG_HAS_SOC_CACHE)
    /*
     * @brief Perform outer cache range operation.
     *
     * This is normally called from cache control system call.
     *
     * @param pa physical address that the cache line contains.
     * @param sizelog2 log base 2 of the size of the cache range.
     * @param attrib Instuction or Data side of the cache.
     */
    void soc_cache_range_op_by_pa(addr_t pa, word_t sizelog2, word_t attrib);
    /*
     * @brief Perform outer cache full operation.
     *
     * This is normally called from cache control system call.
     *
     * @param attrib Instuction or Data side of the cache.
     */
    void soc_cache_full_op(word_t attrib);
    /*
     * @brief Drain all write buffers on the outer cache (Outer Memory Barrier).
     *
     * This is normally called from cache control system call.
     *
     */
    void soc_cache_drain_write_buffer(void);
    /* Will add more functions for cache control system call and kdb in the future. */
#endif

/*
 * SOC platform KDB cache and evtmon functions
 *
 * These functions prints debug information of L2 cache and event monitor.
 */
#if defined(CONFIG_KDB_CONS)
#if defined(CONFIG_HAS_SOC_CACHE)
/*
 * @brief Perform dumping all cache lines in level 2 cache.
 */
void soc_kdb_dump_whole_l2cache(void);
/*
 * @brief Dump cache lines of all the ways that shares the same index.
 *
 * @param index the cache index.
 */
void soc_kdb_dump_l2cache(word_t index);
#endif
#if defined(CONFIG_HAS_L2_EVTMON)
/*
 * @brief Dump level 2 event monitor counters.
 */
void soc_kdb_read_event_monitor(void);
/*
 * @brief Set level 2 event monitor counter and start to count a event.
 *
 * @param counter which counter to use.
 * @param event which level 2 event to be counted.
 */
void soc_kdb_set_event_monitor(word_t counter, word_t event);
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* OKL4_ARCH_SH_SOC_SOC_H */
