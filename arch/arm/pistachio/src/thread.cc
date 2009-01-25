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
 * Description:   ARM specific thread handling functions
 */

#include <l4.h>
#include <debug.h>
#include <kdb/tracepoints.h>
#include <tcb.h>
#include <arch/thread.h>
#include <cpu/syscon.h>


/**
 * This function cleans up after an in-kernel
 * ipc and continues through the continuation
 * passed to do_ipc
 *
 * This function does not use any other stored
 * state from the TCB
 */
CONTINUATION_FUNCTION(do_ipc_helper)
{
    tcb_t * current = get_current_tcb();

#ifdef CONFIG_IPC_FASTPATH
    current->resources.clear_kernel_ipc( current );
    current->resources.clear_except_fp( current );
#endif

    ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->do_ipc_continuation);
}

CONTINUATION_FUNCTION(restart_do_ipc)
{
    tcb_t *current = get_current_tcb();
    tcb_t *to_tcb = (tcb_t*)TCB_SYSDATA_IPC(current)->to_tid.get_raw();
    tcb_t *from_tcb = (tcb_t*)TCB_SYSDATA_IPC(current)->from_tid.get_raw();

    to_tcb = acquire_read_lock_tcb(to_tcb);
    from_tcb = acquire_read_lock_tcb(from_tcb);

    /* Call sys_ipc with a custom continuation as the return address. */
    activate_function((word_t)to_tcb, (word_t)from_tcb, 3, (word_t)ipc);
}

/**
 * Setup TCB to execute a function when switched to
 * @param func pointer to function
 *
 * The old stack state of the TCB does not matter.
 */
void tcb_t::create_startup_stack (void (*func)())
{
    init_stack();

    notify(func);

    arm_irq_context_t *context = &(arch.context);

    /* set user mode */
    context->cpsr = CPSR_USER_MODE;
}

