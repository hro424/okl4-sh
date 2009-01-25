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
 * Author: Josh Matthews <jmatthews@ok-labs.com>
 * Created: Friday December 14 2007
 * Description: POSIX type defs as per IEEE 1003.13 (POSIX)
 * These only define POSIX-specific types that are defined to be
 * included in libc/sys/types.h
 * This file is included by libc/sys/types.h
 */

#ifndef POSIX_SYS_TYPES_H_
#define POSIX_SYS_TYPES_H_

#include <l4/types.h>
#include <stddef.h>

/* Define pthread opaque data types
   They may have already been defined in threadstate.h */
#ifndef __POSIX_PTHREAD_DEFINED
#define __POSIX_PTHREAD_DEFINED
typedef struct pthread_i * pthread_t;
typedef struct pthread_attr_i * pthread_attr_t;
typedef struct pthread_key_i * pthread_key_t;
/* pthread_once_t is not a pointer - see comments in pthread_internal.h */
typedef struct _pthread_once_t { L4_Word_t padding[6]; } pthread_once_t;
/* pthread_mutex_t is not a pointer - see comments in pthread_internal.h */
typedef struct _pthread_mutex_t { L4_Word_t padding[4]; } pthread_mutex_t;
typedef struct pthread_mutexattr_i * pthread_mutexattr_t;
/* pthread_cond_t is not a pointer - see comments in pthread_internal.h */
typedef struct _pthread_cond_t { L4_Word_t padding[6]; } pthread_cond_t;
typedef struct pthread_condattr_i * pthread_condattr_t;
#endif

#ifndef _LINUX_TYPES_H
#ifndef _PID_T
#define _PID_T
typedef unsigned int pid_t;
#endif
#endif /* _LINUX_TYPES_H */

#ifndef _LINUX_TYPES_H
#ifndef _UID_T
#define _UID_T
typedef unsigned int uid_t;
#endif
#endif /* _LINUX_TYPES_H */

#ifndef _LINUX_TYPES_H
#ifndef _INO_T
#define _INO_T
typedef unsigned long ino_t;
#endif
#endif /* _LINUX_TYPES_H */


#endif /* !POSIX_SYS_TYPES_H_ */
