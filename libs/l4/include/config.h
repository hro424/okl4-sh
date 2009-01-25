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

#ifndef __L4__CONFIG_H__
#define __L4__CONFIG_H__

#ifndef ASSEMBLY
#include <l4/types.h>
#endif

#include <l4/arch/config.h>
#include <l4/arch/vregs.h>

/* OKL4 Api Version, previously in KIP
 * Top 8 bits is always 0xA0, identifying OKL
 * Second 8 bits is kernel api version
 * Bottom half-word can be unique identifier for build/patch within
 * kernel api version */
#define OKL4_API_VERSION 0xA0040000

/* "Helper" functions to keep the accessing of this data (which was
   previously in the KIP) as close to the previous for mas possible */

#ifndef ASSEMBLY

INLINE L4_Word_t
L4_GetMessageRegisters(void)
{
    return (L4_Word_t)__L4_NUM_MRS;
}

INLINE L4_Word_t
L4_GetUtcbSize(void)
{
    return (L4_Word_t)UTCB_SIZE;
}

INLINE L4_Word_t
L4_GetUtcbBits(void)
{
    return (L4_Word_t)UTCB_BITS;
}

/**
 *  @todo FIXME: Doing this properly would require pulling lots out of
 *  arch-specific config files to do with pages sizes.  So coped for
 *  now - peterh.
 */
INLINE L4_Word_t
L4_GetUtcbAreaSize(void)
{
    return (1UL << 12);
}

INLINE L4_Word_t
L4_GetPageMask(void)
{
    return (L4_Word_t)USER_HW_VALID_PGSIZES;
}

INLINE L4_Word_t
L4_GetPagePerms(void)
{
    return (L4_Word_t)USER_HW_PAGE_PERMS;
}

INLINE L4_Word_t
L4_GetMinPageBits(void)
{
    return (L4_Word_t)__L4_MIN_PAGE_BITS;
}

/* If L4_UtcbIsKernelManaged() returns true, it means that L4 manages
 * the allocation of space for the UTCB, not the user. */
#ifdef NO_UTCB_RELOCATE
INLINE L4_Bool_t
L4_UtcbIsKernelManaged(void)
{
    return 1;
}
#else
INLINE L4_Bool_t
L4_UtcbIsKernelManaged(void)
{
    return 0;
}
#endif

/* Return the L4 Api Version */
INLINE L4_Word_t
L4_ApiVersion(void)
{
    return OKL4_API_VERSION;
}

#endif /* !ASSEMBLY */


#endif /* !__L4__CONFIG_H__ */
