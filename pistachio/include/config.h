/*
 * Copyright (c) 2002, 2003, Karlsruhe University
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
 * Copyright (c) 2006, National ICT Australia
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
 * Description:   global configuration, include wrapper
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <kernel/config.h>

#define CONFIG_MAX_UNITS_PER_DOMAIN CONFIG_NUM_UNITS / CONFIG_NUM_DOMAINS

/**
 * SMP derives
 */
#if defined(CONFIG_NUM_DOMAINS) && CONFIG_NUM_DOMAINS > 1
#define CONFIG_MDOMAINS
#endif

#if defined(CONFIG_NUM_UNITS) && CONFIG_NUM_UNITS > 1
#define CONFIG_MUNITS
#endif

#if defined(CONFIG_IS_32BIT)
# define L4_GLOBAL_THREADNO_BITS                18
# define L4_GLOBAL_INTRNO_BITS                  18
# define L4_GLOBAL_VERSION_BITS                 14
# define L4_FPAGE_BASE_BITS                     22
#elif defined(CONFIG_IS_64BIT)
# define L4_GLOBAL_THREADNO_BITS                32
# define L4_GLOBAL_INTRNO_BITS                  32
# define L4_GLOBAL_VERSION_BITS                 32
# define L4_FPAGE_BASE_BITS                     54
#else
# error undefined architecture width (32/64bit?)
#endif

#define DEFAULT_TIMESLICE_LENGTH        10000
#define MAX_PRIO                        255
#define DEFAULT_PRIORITY                0
#define DEFAULT_CONTEXT_BITMASK         ~(0UL)

#if defined(CONFIG_DEBUG)
#if defined(CONFIG_KDEBUG_TIMER) || defined(CONFIG_TRACEBUFFER)
#define CONFIG_KEEP_CURRENT_TIME
#endif
#endif

/*
 * root server configuration
 */
#define ROOT_MAX_THREADS                256

#if defined(CONFIG_IS_32BIT)
#define L4_WORD_SIZE    4
#elif defined(CONFIG_IS_64BIT)
#define L4_WORD_SIZE    8
#endif

/*
 * Size of the bounce buffer for the remote memory copy feature.
 *
 * It should be less than or equal the minimum page size. Otherwise, it is a
 * waste of space since the unit of copying is min(REMOTE_MEMCPY_BUFSIZE,
 * minimum page size).
 *
 * TODO : Ensure it during compilation time
 */
#define REMOTE_MEMCPY_BUFSIZE           0x400UL

#include <arch/config.h>

#ifndef ARCH_MAX_SPACES
// no space-id limit
#define ARCH_MAX_SPACES     0
#endif

#endif /* !__CONFIG_H__ */
