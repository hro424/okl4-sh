/*
 * Copyright (c) 2003, Karlsruhe University
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
 * Description:   Default L4 kernel debugger functionality
 */
#ifndef __L4__KDEBUG_H__
#define __L4__KDEBUG_H__

#include <l4/types.h>

typedef enum {
    OKL4_KDB_RES_OK = 0,
    OKL4_KDB_RES_UNIMPLEMENTED = 1
} okl4_kdb_res_t;


INLINE okl4_kdb_res_t L4_KDB_TracepointCountStop(void);
INLINE okl4_kdb_res_t L4_KDB_TracepointCountReset(void);
INLINE okl4_kdb_res_t L4_KDB_PrintChar(char c);
INLINE okl4_kdb_res_t L4_KDB_ReadChar(char * val);
INLINE okl4_kdb_res_t L4_KDB_ReadChar_Blocked(char * val);
INLINE okl4_kdb_res_t L4_KDB_SetThreadName(L4_ThreadId_t tid,
        const char * name);
INLINE okl4_kdb_res_t L4_KDB_SetSpaceName(L4_SpaceId_t sid, const char * name);
INLINE okl4_kdb_res_t L4_KDB_GetTracepointName(word_t tp, char * dest);
INLINE okl4_kdb_res_t L4_KDB_SetMutexName(L4_MutexId_t mutexid,
        const char * name);
L4_Word_t L4_KDB_Bounce_Interrupt_ASM(L4_SpaceId_t s);
INLINE okl4_kdb_res_t L4_KDB_Bounce_Interrupt(L4_SpaceId_t space);

#define KDB_FEATURE_CLI             (1UL << 0)
#define KDB_FEATURE_CONSOLE         (1UL << 1)
#define KDB_FEATURE_THREADNAMES     (1UL << 8)
#define KDB_FEATURE_SPACENAMES      (1UL << 9)
#define KDB_FEATURE_MUTEX_NAMES     (1UL << 10)

/** Maximum length of a debugging name. */
#define L4_KDB_MAX_DEBUG_NAME_LENGTH   16

/**
 * Constants to specify what object a given function call is operating on
 * for L4_KDB_SetObjectName().
 */
#define L4_KDB_OBJECT_THREAD           1
#define L4_KDB_OBJECT_SPACE            2
#define L4_KDB_OBJECT_MUTEX            3

#ifdef NDEBUG

INLINE void L4_KDB_Enter(char * s);

INLINE okl4_kdb_res_t
L4_KDB_SetMutexName(L4_MutexId_t mutexid, const char *name)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_TracepointCountStop(void)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_TracepointCountReset(void)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_PrintChar(char c)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_ReadChar(char * val)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_ReadChar_Blocked(char * val)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_SetThreadName(L4_ThreadId_t tid, const char * name)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_SetSpaceName(L4_SpaceId_t sid, const char * name)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE okl4_kdb_res_t
L4_KDB_GetTracepointName(word_t tp, char * dest)
{
    return OKL4_KDB_RES_UNIMPLEMENTED;
}

INLINE void L4_KDB_Enter(char * str) {}

#else

#include <l4/arch/kdebug.h>

/* Generic KDB stubs. */

INLINE okl4_kdb_res_t
L4_KDB_SetThreadName(L4_ThreadId_t tid, const char * name)
{
    return L4_KDB_SetObjectName(L4_KDB_OBJECT_THREAD, tid.raw, name);
}

INLINE okl4_kdb_res_t
L4_KDB_SetSpaceName(L4_SpaceId_t spaceid, const char * name)
{
    return L4_KDB_SetObjectName(L4_KDB_OBJECT_SPACE, spaceid.raw, name);
}

INLINE okl4_kdb_res_t
L4_KDB_SetMutexName(L4_MutexId_t mutexid, const char * name)
{
    return L4_KDB_SetObjectName(L4_KDB_OBJECT_MUTEX, mutexid.raw, name);
}

#endif /* NDEBUG */

/* Backwards compatability for Iguana. */
INLINE okl4_kdb_res_t
L4_KDB_Bounce_Interrupt(L4_SpaceId_t space)
{
    return (okl4_kdb_res_t)L4_KDB_Bounce_Interrupt_ASM(space);
}

#endif /* !__L4__KDEBUG_H__ */
