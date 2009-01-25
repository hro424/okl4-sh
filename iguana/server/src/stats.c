#if defined(CONFIG_STATS)
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
 * Author: Malcolm Purvis
 *
 * Return memory usage statistics.
 */

#include <string.h>

#include "stats.h"
#include "pd.h"
#include "memsection.h"
#include "thread.h"
#include "pgtable.h"
#include "pool.h"
#include "objtable.h"

enum {
    MALLOC_OVERHEAD = sizeof(void*) + sizeof(unsigned)
};

const iguana_stats_t iguana_stats = 
{
    (size_t) sizeof(struct eas) + MALLOC_OVERHEAD, /* eas_size */
    sizeof(struct memsection_node), /* ms_size */
    sizeof(pte_t), /* pte_size */
    sizeof(struct pd_entry), /* active_pd_size */
    (size_t) 0, /* objtable_page_size */
    (size_t) 0, /* objtable_order */
    (size_t) 0, /* objtable_entry_size */
    sizeof(struct physmem) + sizeof(struct memory), /* physmem_list_size */
    sizeof(struct physmem), /* physmem_size */
    sizeof(struct memory), /* physmem_mem_size */
    sizeof(struct virtmem), /* virtmem_size */
    sizeof(struct memory), /* virtmem_mem_size */
    sizeof(struct physpool) + MALLOC_OVERHEAD, /* pool_size */
    sizeof(struct memory), /* pool_mem_size */
    sizeof(struct pd_node), /* pd_size */
    sizeof(struct ms_entry), /* active_ms_size */
    sizeof(struct clist_node) + MALLOC_OVERHEAD, /* clist_node_size */
    sizeof(struct cb_alloc_handle) + MALLOC_OVERHEAD, /* cb_buffer_handle_size */
#if defined(CONFIG_SESSION)
    sizeof(struct session) + MALLOC_OVERHEAD, /* session_size */
    sizeof(struct session_p_node) + MALLOC_OVERHEAD, /* session_node_size */
#else
    (size_t) 0,
    (size_t) 0,
#endif
    sizeof(struct thread_node) + MALLOC_OVERHEAD /* thread_size */
};

#else
/* This is only here because ADS is silly. */
#if defined (__RVCT__) || defined(__RVCT_GNU__) || defined (__ADS__)
void __this_is_never_used_stats(void);
void __this_is_never_used_stats(void) {
}
#endif
#endif
