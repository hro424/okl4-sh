/*
 * Copyright (c) 2004, National ICT Australia
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
 * Authors: Ben Leslie Created: Tue Jul 6 2004 
 */

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "clist.h"
#include "util.h"
#include "memsection.h"
#include "objtable.h"
#include "pd.h"
#include "security.h"
#include "thread.h"
#include "vm.h"

static int
refcmp(const void *x, const void *y)
{
    uintptr_t r1;
    uintptr_t r2;

    r1 = *(const uintptr_t *)x >> 3 << 3;
    r2 = *(const uintptr_t *)y >> 3 << 3;

    if (r1 == r2)
        return 0;
    return (r1 > r2) ? 1 : -1;
}

#if 0
static inline void
__clist_print(struct clist *clist)
{
    int i;

    DEBUG_PRINT("%p(%d/%d)", clist, clist->used, clist->size);
    for (i = 0; i < clist->used; i++) {
        DEBUG_PRINT(" %d:%" PRIxPTR, i, clist->cap[i].ref.obj);
    }
    DEBUG_PRINT("\n");
}
#endif

/*
 * Returns a bitmap of valid interface numbers.
 */
uintptr_t
security_check(struct pd *pd, uintptr_t ref)
{
    cap_t cap;
    struct clist *clist;
    struct clist_node *clists;
    int i;
    uintptr_t interfaces = 0;
    cap_t *p;
    
    if (pd == NULL) {
        return 0;
    }

    cap.ref.obj = ref;
    for (clists = pd->clists.first; clists->next != pd->clists.first; clists = clists->next) {
        clist = clists->data.clist;

        p = bsearch(&cap, clist->cap, clist->used, sizeof(cap_t), refcmp);
        if (p == NULL) {
            continue;
        }

        i = p - clist->cap;

        /* Find the first matching cap in the list. */
        for (; i > 0; i--) {
            if (refcmp(&ref, &clist->cap[i - 1]) != 0) {
                break;
            }
        }

        /* Add in all the rights in this list. */
        for (; i < clist->used; i++) {
            if (refcmp(&ref, &clist->cap[i]) != 0) {
                break;
            }
            if ((clist->cap[i].ref.obj & IID_MASK) == MASTER_IID) {
                interfaces |= ~0UL;
                break;
            }
            interfaces |= (1UL << (clist->cap[i].ref.obj & IID_MASK));
        }
    }
#ifdef ENABLE_CAPS
    return interfaces;
#else
    return ~0UL;
#endif
}

/**
 * Generate a good random number for use as a capability password
 *
 * \return The random number
 */
static uintptr_t
password_gen(void)
{
    /*
     * FIXME: For now we return a really *bad* random number 
     */
    return rand();
}

cap_t
security_create_capability(uintptr_t reference)
{
    cap_t cap;

    cap.ref.obj = reference;
    cap.passwd = password_gen();

    return cap;
}
