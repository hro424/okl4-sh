/*
 * Copyright (c) 2003-2006, National ICT Australia (NICTA)
 */
/*
 * Copyright (c) 2007 Open Kernel Labs, Inc. (Copyright Holder).
 * All rights reserved.
 *
 * 1. Redistribution and use of OKL4 (Software) in source and binary
 * forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *     (a) Redistributions of source code must retain this clause 1
 *         (including paragraphs (a), (b) and (c)), clause 2 and clause 3
 *         (Licence Terms) and the above copyright notice.
 *
 *     (b) Redistributions in binary form must reproduce the above
 *         copyright notice and the Licence Terms in the documentation and/or
 *         other materials provided with the distribution.
 *
 *     (c) Redistributions in any form must be accompanied by information on
 *         how to obtain complete source code for:
 *        (i) the Software; and
 *        (ii) all accompanying software that uses (or is intended to
 *        use) the Software whether directly or indirectly.  Such source
 *        code must:
 *        (iii) either be included in the distribution or be available
 *        for no more than the cost of distribution plus a nominal fee;
 *        and
 *        (iv) be licensed by each relevant holder of copyright under
 *        either the Licence Terms (with an appropriate copyright notice)
 *        or the terms of a licence which is approved by the Open Source
 *        Initative.  For an executable file, "complete source code"
 *        means the source code for all modules it contains and includes
 *        associated build and other files reasonably required to produce
 *        the executable.
 *
 * 2. THIS SOFTWARE IS PROVIDED ``AS IS'' AND, TO THE EXTENT PERMITTED BY
 * LAW, ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED.  WHERE ANY WARRANTY IS
 * IMPLIED AND IS PREVENTED BY LAW FROM BEING DISCLAIMED THEN TO THE
 * EXTENT PERMISSIBLE BY LAW: (A) THE WARRANTY IS READ DOWN IN FAVOUR OF
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT) AND (B) ANY LIMITATIONS PERMITTED BY LAW (INCLUDING AS TO
 * THE EXTENT OF THE WARRANTY AND THE REMEDIES AVAILABLE IN THE EVENT OF
 * BREACH) ARE DEEMED PART OF THIS LICENCE IN A FORM MOST FAVOURABLE TO
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT). IN THE LICENCE TERMS, "PARTICIPANT" INCLUDES EVERY
 * PERSON WHO HAS CONTRIBUTED TO THE SOFTWARE OR WHO HAS BEEN INVOLVED IN
 * THE DISTRIBUTION OR DISSEMINATION OF THE SOFTWARE.
 *
 * 3. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY OTHER PARTICIPANT BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Description:   Cache control implementation
 */

#include <l4.h>
#include <debug.h>
#include <config.h>
#include <tcb.h>
#include <capid.h>
#include <cache.h>
#include <schedule.h>
#include <space.h>
#include <spaceid.h>
#include <linear_ptab.h>
#include <arch/pgent.h>
#include <kdb/tracepoints.h>

#if 0
#define TRACE_CACHE TRACEF
#else
#define TRACE_CACHE(...)
#endif

DECLARE_TRACEPOINT(SYSCALL_CACHE_CONTROL);
DECLARE_TRACEPOINT(CACHE_FLUSH);

extern spaceid_lookup_t space_lookup;

CONTINUATION_FUNCTION(range_flush);
CONTINUATION_FUNCTION(full_flush);
CONTINUATION_FUNCTION(range_flush_full);

SYS_CACHE_CONTROL (spaceid_t space_id, word_t control)
{
    PROFILE_START(sys_cache_ctrl);
    tcb_t * current = get_current_tcb();

    current->sys_data.set_action(tcb_syscall_data_t::action_cache_control);

    TCB_SYSDATA_CACHE(current)->cache_continuation = ASM_CONTINUATION;
    NULL_CHECK(TCB_SYSDATA_CACHE(current)->cache_continuation);

    TRACEPOINT (SYSCALL_CACHE_CONTROL,
                printf ("SYSCALL_CACHE_CONTROL: current=%p control=%lx, space=%ld\n",
                        current, control, space_id.get_spaceno());
            );

    TCB_SYSDATA_CACHE(current)->space_id = space_id;

    /* setup continuation info */
    TCB_SYSDATA_CACHE(current)->ctrl = control;
    TCB_SYSDATA_CACHE(current)->op_index = 0;
    TCB_SYSDATA_CACHE(current)->op_offset = 0;

    if (TCB_SYSDATA_CACHE(current)->ctrl.operation() != cop_flush_range) {
        ACTIVATE_CONTINUATION(full_flush);
    } else {
        word_t num_ops = TCB_SYSDATA_CACHE(current)->ctrl.highest_item() + 1;

        if ((num_ops * 2) > IPC_NUM_MR)
        {
            current->set_error_code(EINVALID_PARAM);
            PROFILE_STOP(sys_cache_ctrl);
            /*XXX: sh-linux-g++ fails.
            return_cache_control(0,
                                 TCB_SYSDATA_CACHE(current)->cache_continuation);
             */
            continuation_t cont = TCB_SYSDATA_CACHE(current)->cache_continuation;
            return_cache_control(0, cont);
        }
        ACTIVATE_CONTINUATION(range_flush);
    }
}

/**
 * @brief The full cache (i, d, or both) is being flushed by explicit request
 */
CONTINUATION_FUNCTION(full_flush)
{
    tcb_t *current = get_current_tcb();
    int    retval = 1;

    preempt_enable(full_flush);

    word_t *offset = &(TCB_SYSDATA_CACHE(current)->op_offset);

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKINNER)
    {
        switch (TCB_SYSDATA_CACHE(current)->ctrl.operation())
        {
        case cop_flush_all:
            cache_t::flush_all_attribute(attr_clean_id, offset);
            break;
        case cop_flush_I_all:
            cache_t::flush_all_attribute(attr_clean_i, offset);
            break;
        case cop_flush_D_all:
            cache_t::flush_all_attribute(attr_clean_d, offset);
            break;

        default:
            current->set_error_code (EINVALID_PARAM);
            retval = 0;
        }
    }

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKOUTER)
    {
        switch (TCB_SYSDATA_CACHE(current)->ctrl.operation())
        {
        case cop_flush_all:
            arch_outer_cache_full_op(attr_clean_inval_id);
            break;
        case cop_flush_I_all:
            arch_outer_cache_full_op(attr_clean_inval_i);
            break;
        case cop_flush_D_all:
            arch_outer_cache_full_op(attr_clean_inval_d);
            break;

        default:
            current->set_error_code (EINVALID_PARAM);
            retval = 0;
        }

        arch_outer_drain_write_buffer();
    }

    preempt_disable();
    PROFILE_STOP(sys_cache_ctrl);

    /*XXX: sh-linux-g++ fails.
    return_cache_control(retval,
                         TCB_SYSDATA_CACHE(current)->cache_continuation);
                         */
    continuation_t cont = TCB_SYSDATA_CACHE(current)->cache_continuation;
    return_cache_control(retval, cont);
}

#define MAX_RANGE (1024 * 8)

/**
 * @brief Flush one or more explicit ranges as specified in the 
 *        Message Registers.
 */
CONTINUATION_FUNCTION(range_flush)
{
    tcb_t *current = get_current_tcb();
    spaceid_t space_id = TCB_SYSDATA_CACHE(current)->space_id;
    space_t *space;

    word_t op_last = TCB_SYSDATA_CACHE(current)->ctrl.highest_item();
    word_t retval = 1;

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKINNER)
    {
        /* Lookup target address space */
        if (EXPECT_TRUE(space_id.is_nilspace())) {
            /* implicit my_own_space reference */
            space = get_current_space();
        } else {
            /* lookup the space */
            space = get_current_space()->lookup_space(space_id);
        }

        /* Ensure that a valid space was found */
        if (EXPECT_FALSE(space == NULL)) {
            get_current_tcb()->set_error_code (EINVALID_SPACE);
            PROFILE_STOP(sys_cache_ctrl);
            /*XXX: sh-linux-g++ fails.
            return_cache_control(0, TCB_SYSDATA_CACHE(current)->cache_continuation);
            */
            continuation_t cont = TCB_SYSDATA_CACHE(current)->cache_continuation;
            return_cache_control(0, cont);
        }

        for (word_t i = TCB_SYSDATA_CACHE(current)->op_index; i <= op_last; i++)
        {
            word_t offset = TCB_SYSDATA_CACHE(current)->op_offset;

            word_t start = current->get_mr(i*2);
            word_t region = current->get_mr(i*2 + 1);;
            cacheattr_e attr = (cacheattr_e)(region >> (BITS_WORD-4));
            word_t size = region & ((1UL << (BITS_WORD-4)) - 1);

            if (offset) {
                TRACE_CACHE("Restart after preemption\n");
                size = size - offset;
                start = start + offset;
            } else {
                TRACE_CACHE("New manual flush: start = %x, region = %x, size = %x\n",
                            start, region, size);
                TRACEPOINT_TB (CACHE_FLUSH,
                        printf ("CACHE_FLUSH: %p..%p (%d)\n", start, start + size, attr);,
                        "flush: %p..%p", start, start + size
                        );

            }

            if (EXPECT_FALSE(((word_t)attr & CACHE_ATTRIB_MASK_D_OP) ==
                             CACHE_ATTRIB_INVAL_D)) {
                /* XXX we don't support d-cache invalidate currently
                 * Invalidate MUST be privileged OR must do carefull checks so that
                 * you can't invalidate cache lines from a different address space
                 */
                // Check privilege
                if (! is_kresourced_space(get_current_space())) {
                    current->set_error_code (EINVALID_SPACE);
                    retval = 0;
                    goto error_out_range;
                }
            }

            if (cache_t::full_flush_cheaper(size)) {
                TRACE_CACHE("Converting to full flush\n");
                TCB_SYSDATA_CACHE(current)->op_offset = 0;
                ACTIVATE_CONTINUATION(range_flush_full);
            } else {
#ifdef CACHE_NEED_PGENT
                pgent_t * r_pg[pgent_t::size_max];
                pgent_t * pg;
                addr_t next;
#endif

                word_t blocksize;
                pgent_t::pgsize_e t_size = pgent_t::size_max;
                word_t pagesize = page_size(t_size);

                TRACE_CACHE("Starting manual flush; start = %x, size = %x\n",
                            start, size);

#ifdef CACHE_NEED_PGENT
                pg = space->pgent(0)->next(space, t_size, page_table_index(t_size, (addr_t)start));
                next = addr_offset(addr_align((addr_t)start, pagesize), pagesize);
#endif

                /* Start flushing.  The range to be flushed is broken up into
                   smaller amounts to reduce the amount of repeated work 
                   when restarted after preemption */
                while (size > 0) {
                    /* lookup pagetable for start, cache upper levels if we can
                       for re-use on subsequent segments */

#ifdef CACHE_NEED_PGENT
                    if (!pg->is_valid(space, t_size)) {
                        if (size <= pagesize) {
                            /* If we get here, we're on an invalid page and
                               the remaining size doesn't get to the next page.
                               So we can give up */
                            TRACE_CACHE("Skipping flush\n");
                            break;
                        } else {
                            TRACE_CACHE("Skipping part of flush\n");
                            /* Jump to the next page in the hope that it may be
                               valid */
                            TCB_SYSDATA_CACHE(current)->op_offset += pagesize;
                            size -= pagesize;
                            start += pagesize;
                            goto calc_next;
                        }
                    } else if (pg->is_subtree(space, t_size)) {
                        t_size --;
                        r_pg[t_size] = pg->next(space, t_size+1, 1);
                        pagesize = page_size(t_size);
                        pg = pg->subtree(space, t_size+1)->next(space, t_size,
                                page_table_index(t_size, (addr_t)start));
                        next = addr_offset(addr_align((addr_t)start, pagesize), pagesize);
                        continue;
                    }
#endif
#ifdef CACHE_HANDLE_FAULT
                // assume minimum page size
                pagesize = page_size(pgent_t::size_min);
#endif

                    /* If we get here we have found the pagetable entry, (or we
                     * don't care about it) so it is valid to flush the address
                     * from the cache - but only for the current page */

                    blocksize = min(pagesize, MAX_RANGE);

#ifdef CACHE_NEED_PGENT
    again:
#endif
                    preempt_enable(range_flush);

                    {
                        word_t base = (word_t)addr_align((addr_t)start, blocksize);
                        word_t limit = base + blocksize;

                        if ((start+size) <= limit) {
                            blocksize = size;
                        } else {
                            blocksize = limit - start;
                        }
                    }

                    preempt_disable();
                    TRACE_CACHE("Flush, size = %d\n", size);

                    // do this early in fault case
                    TCB_SYSDATA_CACHE(current)->op_offset += blocksize;
#ifdef CACHE_HANDLE_FAULT
                    current->set_user_access(range_flush);
#endif
                    cache_t::flush_range_attribute(space,
                                                   (addr_t)start,
                                                   (addr_t)(start+blocksize),
                                                   attr);
#ifdef CACHE_HANDLE_FAULT
                    current->clear_user_access();
#endif
                    size -= blocksize;
                    start += blocksize;

#ifdef CACHE_NEED_PGENT
    calc_next:
                    if (start >= (word_t)next) {
                        if (page_table_index(t_size, (addr_t)start) == 0) {
                            do {
                                pg = r_pg[t_size];
                                t_size ++;
                                pagesize = page_size(t_size);
                            } while ((t_size < pgent_t::size_max) &&
                                    (page_table_index(t_size, (addr_t)start) == 0));
                        } else {
                            pg = pg->next(space, t_size, 1);
                        }
                        next = addr_offset(addr_align((addr_t)start, pagesize), pagesize);
                    } else {
                        if (size > 0) {
                            goto again;
                        }
                    }
#endif
                }
            }

            /*
             * Update for preemption purposes
             */
            TCB_SYSDATA_CACHE(current)->op_index = i + 1;
            TCB_SYSDATA_CACHE(current)->op_offset = 0;
        }
    }

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKOUTER)
    {
        /*
         * Outer cache flush always using physical address, therefore, no need to
         * use space_id and can be performed from another space as long as it is
         * privileged.
         */
        for (word_t i = TCB_SYSDATA_CACHE(current)->op_index; i <= op_last; i++)
        {
            word_t start = current->get_mr(i*2);
            word_t region = current->get_mr(i*2 + 1);;
            cacheattr_e attr = (cacheattr_e)(region >> (BITS_WORD-4));
            word_t size = region & ((1UL << (BITS_WORD-4)) - 1);

            // Check privilege
            if (! is_kresourced_space(get_current_space())) {
                current->set_error_code (EINVALID_SPACE);
                retval = 0;
                goto error_out_range;
            }

            preempt_enable(range_flush);
            arch_outer_cache_range_op((addr_t)start, size, attr);
            preempt_disable();
        }

        arch_outer_drain_write_buffer();
    }

error_out_range:
    PROFILE_STOP(sys_cache_ctrl);
    /*XXX: sh-linux-g++ fails.
    return_cache_control(retval, TCB_SYSDATA_CACHE(current)->cache_continuation);
    */
    continuation_t cont = TCB_SYSDATA_CACHE(current)->cache_continuation;
    return_cache_control(retval, cont);
}

/**
 * @brief Jumping from a "range" flush to flushing the whole cache, then 
 *        resuming the iteration over MRs.
 */
CONTINUATION_FUNCTION(range_flush_full)
{
    tcb_t *current = get_current_tcb();

    word_t i = TCB_SYSDATA_CACHE(current)->op_index;
    word_t region = current->get_mr(i*2 + 1);
    cacheattr_e attr = (cacheattr_e)(region >> (BITS_WORD-4));

    preempt_enable(range_flush_full);
    cache_t::flush_all_attribute(attr, &TCB_SYSDATA_CACHE(current)->op_offset);
    preempt_disable();

    TCB_SYSDATA_CACHE(current)->op_index ++;
    TCB_SYSDATA_CACHE(current)->op_offset = 0;
    /* Continue with remaining range flushes */
    ACTIVATE_CONTINUATION(range_flush);
}
