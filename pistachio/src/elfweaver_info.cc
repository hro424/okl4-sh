/*
 * Copyright (c) 2008 Open Kernel Labs, Inc. (Copyright Holder).
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
 * Description:   Information for Elfweaver.
 */

#include <debug.h>
#include <init.h>
#include <l4.h>
#include <tcb.h>
#include <utcb.h>
#include <clist.h>
#include <l4/cache_attribs.h>
#include <l4/elfweaver_info.h>

SECTION(".elfweaver_info") extern const struct elfweaver_info weaver_info = {
    {
        L4_DefaultMemory,
        L4_CachedMemory,
        L4_UncachedMemory,
        L4_WriteBackMemory,
        L4_WriteThroughMemory,
        L4_CohearentMemory,
        L4_IOMemory,
        L4_IOCombinedMemory,
        L4_StrongOrderedMemory,
        L4_DefaultMemory,
        L4_DefaultMemory,
        L4_DefaultMemory
    },
    sizeof(utcb_t),
    ARCH_MAX_SPACES,
    __L4_VALID_HW_PAGE_PERMS,
    sizeof(clist_t),
    sizeof(cap_t),
    sizeof(space_t),
    sizeof(tcb_t),
    sizeof(mutex_t),
    sizeof(segment_list_t),
    SMALL_OBJECT_BLOCKSIZE,
    KMEM_CHUNKSIZE,
    PG_TOP_SIZE,
    {
        L4_Info_Uncached,
        L4_Info_Cached,
        L4_Info_IOMemory,
        L4_Info_IOMemoryShared,
        L4_Info_WriteThrough,
        L4_Info_WriteBack,
        L4_Info_Shared,
        L4_Info_NonShared,
        L4_Info_Custom,
        L4_Info_Strong,
        L4_Info_Buffered
    }
};
