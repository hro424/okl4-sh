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

#ifndef __L4__ARM__V6__CACHE_ATTRIBS_H__
#define __L4__ARM__V6__CACHE_ATTRIBS_H__

/*
 * These are for use with the custom attribute macro.
 */
#define CACHE_ATTRIB_NC     0
#define CACHE_ATTRIB_WT     2
#define CACHE_ATTRIB_WB     3
#define CACHE_ATTRIB_WBa    1

#define CACHE_ATTRIB_CUSTOM_L1L2        0x10
#define CACHE_ATTRIB_CUSTOM_MASK        (~(0xf | CACHE_ATTRIB_SHARED))

#define CACHE_ATTRIB_CUSTOM(l1, l2) (CACHE_ATTRIB_CUSTOM_L1L2 | (l1 | (l2 << 2)))

#define CACHE_ATTRIB_STRONG             0
#define CACHE_ATTRIB_UNCACHED           4
#define CACHE_ATTRIB_WRITETHROUGH       2
#define CACHE_ATTRIB_WRITEBACK          3
#define CACHE_ATTRIB_WRITEBACK_ALLOC    7
/*
 * CACHE_ATTRIB_SHARED can be used together (OR) with
 * any of the above attributes, but not with IOMEMORY.
 */
#define CACHE_ATTRIB_SHARED             0x80

#define CACHE_ATTRIB_IOMEMORY           8
#define CACHE_ATTRIB_IOMEMORY_SHARED    1

#define L4_DefaultMemory            CACHE_ATTRIB_WRITEBACK
#define L4_CachedMemory             CACHE_ATTRIB_WRITEBACK
#define L4_UncachedMemory           CACHE_ATTRIB_UNCACHED
#define L4_WriteBackMemory          CACHE_ATTRIB_WRITEBACK
#define L4_WriteThroughMemory       CACHE_ATTRIB_WRITETHROUGH
#define L4_CohearentMemory          (CACHE_ATTRIB_WRITEBACK | CACHE_ATTRIB_SHARED)
#define L4_IOMemory                 CACHE_ATTRIB_IOMEMORY
#define L4_IOCombinedMemory         CACHE_ATTRIB_IOMEMORY_SHARED
#define L4_StrongOrderedMemory      CACHE_ATTRIB_STRONG

#endif /* !__L4_ARM__V6__CACHE_ATTRIBS_H__ */
