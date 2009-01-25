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

#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <arch/profile.h>

#ifndef CONFIG_L4_PROFILING

#define PROFILE_START(e)
#define PROFILE_STOP(e)
#define PROFILE_STOP_IF_RUNNING(t, e)
#define PROFILE_STOP_ALL(tcb)
#define PROFILE_TCB_INIT(t)
#define PROFILE_SWITCH_FROM(f)
#define PROFILE_SWITCH_TO(t)

#else

#define PROFILE_START(e)                profile_start(e)
#define PROFILE_STOP(e)                 profile_stop(e)
#define PROFILE_STOP_IF_RUNNING(tcb, e) profile_stop_if_running(tcb, e)
#define PROFILE_STOP_ALL(tcb)           profile_stop_all(tcb)
#define PROFILE_TCB_INIT(tcb)           profile_tcb_init(tcb)
#define PROFILE_SWITCH_FROM(from)       profile_switch_from(from)
#define PROFILE_SWITCH_TO(to)           profile_switch_to(to)

#define NUM_EVENTS  13

enum event_type_e
{
    sys_cache_ctrl          = 0,
    sys_map_ctrl            = 1,
    sys_threadswitch        = 2,
    sys_space_ctrl          = 3,
    sys_thread_ctrl         = 4,
    sys_platform_ctrl       = 5,
    sys_spaceswitch         = 6,
    sys_sched               = 7,
    sys_exchg_regs          = 8,
    sys_ipc_e               = 9,
    sys_interrupt_ctrl      = 10,
    sys_cap_ctrl            = 12,
    sys_remote_memcopy      = 13,
    /* Arch specific events */
#if defined(ARCH_NUM_EVENTS) && defined(ARCH_EVENT_TYPE) \
                             && defined(ARCH_EVENT_STR)
    ARCH_EVENT_TYPE
#else
#define ARCH_NUM_EVENTS     0
#endif
    MAX_EVENT_TYPE          = (ARCH_NUM_EVENTS + NUM_EVENTS),
};

typedef struct profile_thread_data_t
{
    word_t running : MAX_EVENT_TYPE;
    word_t paused : MAX_EVENT_TYPE;
    u64_t start;
    u64_t time;
    word_t switches;
};

typedef struct profile_stats_t
{
    u64_t start;
    u64_t counter;
    u64_t cycles;
    u64_t switches;
    int errors;
};

typedef struct
{
    u64_t start;
    u64_t time;
    word_t entry_counter;
} profile_kernel_time_t;

extern "C" profile_kernel_time_t profile_kernel_time;

class tcb_t;

extern "C" void profile_start(int e);
extern "C" void profile_stop(int e);
extern "C" void profile_stop_all(tcb_t *tcb);
extern "C" void profile_stop_if_running(tcb_t *tcb, int e);
extern "C" void profile_handler(word_t op);
extern "C" void profile_tcb_init(tcb_t *tcb);
extern "C" void profile_switch_to(tcb_t *);
extern "C" void profile_switch_from(tcb_t *);

#endif

#endif /* __PROFILE_H__ */
