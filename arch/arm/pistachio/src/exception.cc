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
 * Description:   ARM generic exception handling
 */

#include <l4.h>
#include <debug.h>
#include <space.h>
#include <tcb.h>
#include <ipc.h>
#include <arch/ipc.h>
#include <linear_ptab.h>
#include <kdb/tracepoints.h>
#include <kernel/kdb/console.h>
#include <kdb/check_breakin.h>
#include <kernel/generic/lib.h>
#include <schedule.h>
#include <mutex.h>
#include <kdb/names.h>
#include <soc/arch/soc.h>
#include <arch/intctrl.h>

DECLARE_TRACEPOINT(EXCEPTION_IPC_SYSCALL);
DECLARE_TRACEPOINT(EXCEPTION_IPC_GENERAL);

#if defined(CONFIG_PERF) &&                                             \
    (defined(CONFIG_CPU_ARM_XSCALE) || (defined(ARCH_ARM) && (ARCH_VER == 6)))
#include <plat/interrupt.h>
unsigned long count_CCNT_overflow;
unsigned long count_PMN0_overflow;
unsigned long count_PMN1_overflow;
#endif

NORETURN INLINE void
halt_user_thread(continuation_t continuation)
{
    tcb_t * current = get_current_tcb();
    scheduler_t * scheduler = get_current_scheduler();

    scheduler->deactivate_sched(current, thread_state_t::halted,
                                current, continuation,
                                scheduler_t::sched_default);
}

/* As on MIPS */

#define SYSCALL_SAVED_REGISTERS (EXCEPT_IPC_SYS_MR_NUM + 1)

extern "C" NORETURN void
send_exception_ipc( word_t exc_num, word_t exc_code,
        arm_irq_context_t *context, continuation_t continuation);


extern "C" NORETURN void undefined_exception(arm_irq_context_t *context)
{
    word_t instr;
    continuation_t continuation = ASM_CONTINUATION;

#ifdef CONFIG_ARM_THUMB_SUPPORT
    if (context->cpsr & CPSR_THUMB_BIT)
        instr = *(u16_t *)(PC(context->pc));
    else
#endif
        instr = *(word_t *)(PC(context->pc));

    if (EXPECT_FALSE((context->cpsr & CPSR_MODE_MASK) != CPSR_USER_MODE)) {
        printf("Kernel undefined exception at %p\n", (addr_t)PC(context->pc));
        enter_kdebug("exception");
    }

    send_exception_ipc(1, instr, context, continuation);
}

extern "C" NORETURN void reset_exception(arm_irq_context_t *context)
{
    continuation_t continuation = ASM_CONTINUATION;

    printf(TXT_BRIGHT "--- KD# %s ---\n" TXT_NORMAL,
                "Unhandled Reset Exception");

    kdebug_entry_t kdebug_entry = kdebug_entries.kdebug_entry;
    if (EXPECT_FALSE(kdebug_entry != NULL)) {
        kdebug_entry(context);
    }
    halt_user_thread(continuation);
}

extern "C" word_t sys_wbtest(word_t op, word_t* arg0, word_t* arg1, word_t* arg2);
extern "C" void sys_arm_misc(arm_irq_context_t *context)
{
    continuation_t continuation = ASM_CONTINUATION;

    switch (context->r12 & 0xff) {

#if defined(CONFIG_WBTEST)
    case L4_TRAP_WBTEST: {
            word_t res;
            res = sys_wbtest(context->r0, &context->r1, &context->r2, &context->r3);
            context->r0 = res;
        }
        return;
#endif

#if defined(CONFIG_KDB_CONS)
    case L4_TRAP_KPUTC:
        putc((char)context->r0);
        return;
    case L4_TRAP_KGETC:
        context->r0 = (s32_t)(s8_t)getc(true);
        return;
    case L4_TRAP_KGETC_NB:
        context->r0 = (s32_t)(s8_t)getc(false);
        return;
#elif defined(CONFIG_DEBUG)
    case L4_TRAP_KPUTC:
        return;
    case L4_TRAP_KGETC:
    case L4_TRAP_KGETC_NB:
        context->r0 = 0;
        return;
#endif
#if defined(CONFIG_DEBUG)
    case L4_TRAP_CKBR:
         kdb_breakin_handler(context->r0);
         return ;
#endif
#if defined(CONFIG_THREAD_NAMES) \
        || defined(CONFIG_SPACE_NAMES) \
        || defined(CONFIG_MUTEX_NAMES)
    case L4_TRAP_KSET_OBJECT_NAME:
        {
            char name[MAX_DEBUG_NAME_LENGTH];
            word_t type = context->r0;
            word_t id = context->r1;
            word_t *name_words = (word_t *)name;

            /* Copy the name from the user's registers. */
            name_words[0] = context->r2;
            name_words[1] = context->r3;
            name_words[2] = context->r4;
            name_words[3] = context->r5;
            name[MAX_DEBUG_NAME_LENGTH - 1] = '\0';

            /* Set the object's name. */
            (void)kdb_set_object_name((object_type_e)type, id, name);
            return;
        }
#endif /* CONFIG_THREAD_NAMES || CONFIG_SPACE_NAMES || CONFIG_MUTEX_NAMES */
#ifdef CONFIG_DEBUG
    case L4_TRAP_KDEBUG:
        {
#if defined(CONFIG_KDB_CONS)
            space_t *space = get_current_space();
            char *string = (char *)context->r0;

#ifdef CONFIG_ENABLE_FASS
            /* PID relocation */
            if ((word_t)string < PID_AREA_SIZE) {
                word_t pid = space->get_pid();
                string = (char *)((word_t)string + (pid << 25));
            }
#endif

            if (space == NULL) {
                space = get_kernel_space();
            }
            printf(TXT_BRIGHT "--- KD# ");
            word_t user = ((context->cpsr & CPSR_MODE_MASK)
                            == CPSR_USER_MODE);
            if (user) {
                printf("User: ");

                if (space->is_user_area(string))
                {
                    char c;
                    while (readmem(space, string, &c) && (c != 0)) {
                        putc(c);
                        string++;
                    }
                    if (c != 0) {
                        printf("[string not mapped]");
                    }
                }
                else {
                    printf("[illegal string]");
                }
            }
            else {

                printf("Kernel: ");

                if (space->is_user_area(string)) {
                    printf("[illegal string]");
                }
                else {
                    printf(string);
                }

            }

            printf(" ---\n" TXT_NORMAL);
#endif
            kdebug_entry_t kdebug_entry = kdebug_entries.kdebug_entry;
            if (EXPECT_FALSE(kdebug_entry != NULL)) {
                kdebug_entry(context);
            }
            return;
        }
#if defined(CONFIG_PERF)
#if defined(CONFIG_CPU_ARM_XSCALE)
    case L4_TRAP_PMN_READ:
           {
                unsigned long reg = 0;
                switch(context->r0) {
                case 0:
                            /* Return Performance Monitor Control Register */
                            __asm__  __volatile__ (
                                                   "   mrc          p14, 0, %0, c0, c0, 0   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    case 1:
                            /* Return Cycle Counter Register */
                            __asm__  __volatile__ (
                                                   "   mrc          p14, 0, %0, c1, c0, 0   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    case 2:
                            /* Return Count Register 0 */
                            __asm__  __volatile__ (
                                                   "   mrc          p14, 0, %0, c2, c0, 0   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    case 3:
                            /* Return Count Register 1 */
                            __asm__  __volatile__ (
                                                   "   mrc          p14, 0, %0, c3, c0, 0   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    }
                    context->r0 = reg;
                    return;
            }
    case L4_TRAP_PMN_WRITE:
            {
                    unsigned long reg = context->r1;
                    switch(context->r0) {
                    case 0:
                            reg &= ~0xF0000880; /* clear bit[31:28], bit[11], bit[7] */
                            if (reg & soc_perf_counter_irq) /* if any of the counter irq is enabled */
                            {
                                /* enable pmu_irq */
                                soc_perf_counter_unmask();
                            }
                            /* Write Performance Monitor Control Register */
                            __asm__  __volatile__ (
                                                   "   mcr          p14, 0, %0, c0, c0, 0   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    case 1:
                            /* Write Cycle Counter Register */
                            __asm__  __volatile__ (
                                                   "   mcr          p14, 0, %0, c1, c0, 0   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    case 2:
                            /* Write Count Register 0 */
                            __asm__  __volatile__ (
                                                   "   mcr          p14, 0, %0, c2, c0, 0   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    case 3:
                            /* Write Count Register 1 */
                            __asm__  __volatile__ (
                                                   "   mcr          p14, 0, %0, c3, c0, 0   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    }
                    return;
            }
    case L4_TRAP_PMN_OFL_READ:
            {
                unsigned long reg = 0;
                switch(context->r0) {
                case 1:
                    /* read CCNT overflow counter */
                    reg = count_CCNT_overflow; 
                    break;
                case 2:
                    /* read PMN0 overflow counter */
                    reg = count_PMN0_overflow;
                    break;
                case 3:
                    /* read PMN1 overflow counter */
                    reg = count_PMN1_overflow;
                    break;
                }
                context->r0 = reg;
                return;
            }
    case L4_TRAP_PMN_OFL_WRITE:
            {
                unsigned long reg = context->r1;
                switch(context->r0) {
                case 1:
                    /* write CCNT overflow counter */
                    count_CCNT_overflow = reg;
                    break;
                case 2:
                    /* write PMN0 overflow counter */
                    count_PMN0_overflow = reg;
                    break;
                case 3:
                    /* write PMNB1 overflow counter */
                    count_PMN1_overflow = reg;
                    break;
                }
                return;
            }
#elif (defined(ARCH_ARM) && (ARCH_VER == 6)) 
    case L4_TRAP_PMN_READ:
            {
                    unsigned long reg = 0;
                    switch(context->r0) {
                    case 0:
                            /* Return Performance Monitor Control Register */
                            __asm__  __volatile__ (
                                                   "   mrc          p15, 0, %0, c15, c12, 0   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    case 1:
                            /* Return Cycle Counter Register */
                            __asm__  __volatile__ (
                                                   "   mrc          p15, 0, %0, c15, c12, 1   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    case 2:
                            /* Return Count Register 0 */
                            __asm__  __volatile__ (
                                                   "   mrc          p15, 0, %0, c15, c12, 2   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    case 3:
                            /* Return Count Register 1 */
                            __asm__  __volatile__ (
                                                   "   mrc          p15, 0, %0, c15, c12, 3   \n"
                                                   :  "=r" (reg)
                                                   : /* no input */
                                                   : /* no trash */
                                                   );
                            break;
                    }
                    context->r0 = reg;
                    return;
            }
    case L4_TRAP_PMN_WRITE:
            {
                    unsigned long reg = context->r1;
                    switch(context->r0) {
                    case 0:
                            reg &= ~0xF0000080; /* clear bit[31:28] and bit[7] */
                            if (reg & soc_perf_counter_irq) /* if any of the counter irq is enabled */
                            {
                                /* enable pmu_irq */
                                soc_perf_counter_unmask();
                            }
                            /* Write Performance Monitor Control Register */
                            __asm__  __volatile__ (
                                                   "   mcr          p15, 0, %0, c15, c12, 0   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    case 1:
                            /* Write Cycle Counter Register */
                            __asm__  __volatile__ (
                                                   "   mcr          p15, 0, %0, c15, c12, 1   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    case 2:
                            /* Write Count Register 0 */
                            __asm__  __volatile__ (
                                                   "   mcr          p15, 0, %0, c15, c12, 2   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    case 3:
                            /* Write Count Register 1 */
                            __asm__  __volatile__ (
                                                   "   mcr          p15, 0, %0, c15, c12, 3   \n"
                                                   : /* no output */
                                                   : "r" (reg)
                                                   : /* no trash */
                                                   );
                            break;
                    }
                    return;
            }
    case L4_TRAP_PMN_OFL_READ:
            {
                unsigned long reg = 0;
                switch(context->r0) {
                case 1:
                    /* read CCNT overflow counter */
                    reg = count_CCNT_overflow; 
                    break;
                case 2:
                    /* read PMN0 overflow counter */
                    reg = count_PMN0_overflow;
                    break;
                case 3:
                    /* read PMN1 overflow counter */
                    reg = count_PMN1_overflow;
                    break;
                }
                context->r0 = reg;
                return;
            }
    case L4_TRAP_PMN_OFL_WRITE:
            {
                unsigned long reg = context->r1;
                switch(context->r0) {
                case 1:
                    /* write CCNT overflow counter */
                    count_CCNT_overflow = reg;
                    break;
                case 2:
                    /* write PMN0 overflow counter */
                    count_PMN0_overflow = reg;
                    break;
                case 3:
                    /* write PMNB1 overflow counter */
                    count_PMN1_overflow = reg;
                    break;
                }
                return;
            }
#endif
#endif
#if defined(CONFIG_KDEBUG_TIMER)
    case L4_TRAP_GETTICK:
        context->r0 = (u32_t) (get_current_time() / soc_get_timer_tick_length());
        return;
#endif
#if defined(CONFIG_TRACEPOINTS)
    case L4_TRAP_GETCOUNTER:
            {
                    context->r0 = tp_list.get(context->r0)->counter[0];
                    return;
            }
    case L4_TRAP_GETNUMTPS:
            {
                    context->r0 = tp_list.size();
                    return;
            }
    case L4_TRAP_GETTPNAME:
            {
                    const char *name = trace_names[tp_list.get(context->r0)->id];
                    if (name != NULL) {
                            /* NOTE: This is _not_ trying to be strlen().
                               It's trying to be strlen() + 1 */
                            unsigned int len = 0;
                            while (name[len++] != '\0')
                                    ;
                            (void)memcpy(&get_current_tcb()->get_utcb()->mr[0],
                                         name,
                                         len);
                    }
                    return;
            }
    case L4_TRAP_TCCTRL:
            {
                    if (context->r0 == 0) {
                            /*stop*/
                            GLOBAL(tracepoints_enabled) = false;
                    } else {
                            /*reset and go */
                            tracepoint_t * tp;

                            GLOBAL(tracepoints_enabled) = true;
                            tp_list.reset();
                            while ((tp = tp_list.next ()) != NULL)
                                    tp->reset_counter();

                    }
                    return;
            }
#endif
#endif
#if defined(CONFIG_BOUNCE_INTERRUPT)
    case L4_TRAP_BOUNCE_INTERRUPT:
            {
                space_t * space = get_current_space()->lookup_space(spaceid_t::spaceid(context->r0));
                tcb_t * current = get_current_tcb();
                word_t r = security_control_interrupt((struct irq_desc*)&current->get_utcb()->mr[0], 
                        get_current_space(), 1 << 16);
                if (r == 0) {
                    // we were the owner of the interrupt
                    r = security_control_interrupt((struct irq_desc*)&current->get_utcb()->mr[0],
                            space, 0);
                    if (r == 0) {
                        // successfully reassigned the interrupt
                        context->r0 = 1;
                        return;
                    }
                    // we have lost the interrupt here
                }

                get_current_tcb()->set_error_code(r);
                context->r0 = 0;

                return;
            }
#endif
    default:
        break;
    }

    /* XXX - should deliver this as a exception IPC */
    printf("Illegal misc syscall: syscall no = %ld\n", context->r12 & 0xff);
    halt_user_thread(continuation);
}

CONTINUATION_FUNCTION(finish_exception_ipc);

extern "C" NORETURN void
send_exception_ipc( word_t exc_num, word_t exc_code,
        arm_irq_context_t * context, continuation_t continuation)
{
    tcb_t * current = get_current_tcb();
    msg_tag_t tag;

    if (current->get_exception_handler() == NULL) {
        ACTIVATE_CONTINUATION(continuation);
    }
    /* XXX we should check if the kernel faulted */

    if (current->resource_bits.have_resource(EXCEPTIONFP)) {
#ifdef CONFIG_ARM_THUMB_SUPPORT
        TRACEPOINT (EXCEPTION_IPC_SYSCALL,
                word_t instr;
                if (context->cpsr & CPSR_THUMB_BIT)
                instr = *(u16_t *)(PC(context->pc)-2);
                else
                instr = *(word_t *)(PC(context->pc)-4);
                printf ("EXCEPTION_IPC_SYSCALL: (%p) IP = %p (%p)\n",
                    current, PC(context->pc),
                    (void*)instr));
#else
        TRACEPOINT (EXCEPTION_IPC_SYSCALL,
                word_t instr;
                instr = *(word_t *)(PC(context->pc)-4);
                printf ("EXCEPTION_IPC_SYSCALL: (%p) IP = %p (%p)\n",
                    current, PC(context->pc),
                    (void*)instr));
#endif
        // Create the message tag.
        tag.set(EXCEPT_IPC_SYS_MR_NUM, EXCEPTION_TAG, true, true);
    } else {
        TRACEPOINT(EXCEPTION_IPC_GENERAL,
                printf("EXCEPTION_IPC_GENERAL: (%p) exc_num %d, exc_code %08lx, IP = %p\n",
                    current, exc_num, exc_code, PC(context->pc)));

        current->arch.exc_code = exc_code;
        // Create the message tag.
        tag.set(EXCEPT_IPC_GEN_MR_NUM, EXCEPTION_TAG, true, true);
    }
    current->set_exception_ipc(exc_num);

    /* We are just about to send an IPC. */
    current->sys_data.set_action(tcb_syscall_data_t::action_ipc);

    // Save message registers.
    current->save_state(1);

    /* don't allow receiving async */
    current->set_notify_mask(0);

    current->set_tag(tag);

    current->arch.misc.exception.exception_ipc_continuation = continuation;
    current->arch.misc.exception.exception_context = context;

    // Deliver the exception IPC.
    current->do_ipc(current->get_exception_handler(),
                    current->get_exception_handler(),
                    finish_exception_ipc);
}

CONTINUATION_FUNCTION(finish_exception_ipc)
{
    msg_tag_t tag;
    tcb_t * current = get_current_tcb();

    tag.raw = current->get_mr(0);

    // Alter the user context if necessary.
    if (EXPECT_TRUE(tag.is_error())) {
        kdebug_entry_t kdebug_entry = kdebug_entries.kdebug_entry;
        if (EXPECT_FALSE(kdebug_entry != NULL)) {
            printf(TXT_BRIGHT "--- KD# %s at %p ---\n" TXT_NORMAL,
                   "Unhandled user exception",
                   (addr_t)PC(current->
                              arch.misc.exception.exception_context->pc));
            kdebug_entry(current->arch.misc.exception.exception_context);
        }
        else {
            TRACEF("%t: Unhandled user exception at %p\n",
                   current,
                   (addr_t)PC(current->
                              arch.misc.exception.exception_context->pc));
        }
        halt_user_thread(current->arch.misc.exception.exception_ipc_continuation);
    }

    // Clean-up.
    current->restore_state(1);
    current->clear_exception_ipc();

    ACTIVATE_CONTINUATION(current->
                          arch.misc.exception.exception_ipc_continuation);
}

#if defined(CONFIG_ARM_VFP)
NORETURN void
send_vfp_exception_ipc(arm_irq_context_t *context, continuation_t continuation);
#endif

extern "C" void check_coproc_fault(arm_irq_context_t *context, word_t instr, addr_t faddr)
{
    //printf("check ins %p at %p\n", instr, faddr);

#if defined(CONFIG_ARM_VFP) || defined(CONFIG_ARM_FPA) /* ... */
    if (((instr >> 28) != 0xf) && (((instr >> 26) & 0x3) == 0x3))
    {
        word_t copro = (instr >> 8) & 0xf;
#if defined(CONFIG_ARM_VFP)
        if (copro == 10 || copro == 11)
        {
            tcb_t *current = get_current_tcb();
            /* try handle vfp exception - if not arm_vfp_exception will
             * return and we then send vfp or undefined instruction msg
             */
            bool vfp_fault =
                current->resources.arm_vfp_exception(current);

            if (vfp_fault)
            {
                extern continuation_t vector_arm_abort_return;

                current->arch.misc.exception.exception_context = context;
                send_vfp_exception_ipc(context, vector_arm_abort_return);
                /* never returns here */
                while (1);
            }
        }
#endif
        //TRACEF("fault on copro %d instruction\n", copro);
    }
#endif
}

bool tcb_t::copy_exception_mrs_from_frame(tcb_t *dest)
{
    arm_irq_context_t * RESTRICT context = &arch.context;

    word_t * RESTRICT mr = &dest->get_utcb()->mr[0];

    // Create the message.
    if (!this->resource_bits.have_resource(EXCEPTIONFP)) {
        mr[EXCEPT_IPC_GEN_MR_IP] = PC(context->pc);
        mr[EXCEPT_IPC_GEN_MR_SP] = context->sp;
        mr[EXCEPT_IPC_GEN_MR_FLAGS] = context->cpsr;
        mr[EXCEPT_IPC_GEN_MR_EXCEPTNO] = arch.exc_num;
        mr[EXCEPT_IPC_GEN_MR_ERRORCODE] = arch.exc_code;
    } else {
        mr[EXCEPT_IPC_SYS_MR_PC] = PC(context->pc);
        mr[EXCEPT_IPC_SYS_MR_R0] = context->r0;
        mr[EXCEPT_IPC_SYS_MR_R1] = context->r1;
        mr[EXCEPT_IPC_SYS_MR_R2] = context->r2;
        mr[EXCEPT_IPC_SYS_MR_R3] = context->r3;
        mr[EXCEPT_IPC_SYS_MR_R4] = context->r4;
        mr[EXCEPT_IPC_SYS_MR_R5] = context->r5;
        mr[EXCEPT_IPC_SYS_MR_R6] = context->r6;
        mr[EXCEPT_IPC_SYS_MR_R7] = context->r7;
        mr[EXCEPT_IPC_SYS_MR_SP] = context->sp;
        mr[EXCEPT_IPC_SYS_MR_LR] = context->lr;
#ifdef CONFIG_ARM_THUMB_SUPPORT
        if (context->cpsr & CPSR_THUMB_BIT)
            mr[EXCEPT_IPC_SYS_MR_SYSCALL] = *(u16_t *)(PC(context->pc)-2);
        else
#endif
            mr[EXCEPT_IPC_SYS_MR_SYSCALL] = *(word_t *)(PC(context->pc)-4);

        mr[EXCEPT_IPC_SYS_MR_FLAGS] = context->cpsr;
    }

    return true;
}

bool tcb_t::copy_exception_mrs_to_frame(tcb_t *dest)
{
    if (!dest->resource_bits.have_resource(EXCEPTIONFP)) {
        dest->set_user_ip( (addr_t)get_mr(EXCEPT_IPC_GEN_MR_IP) );
        dest->set_user_sp( (addr_t)get_mr(EXCEPT_IPC_GEN_MR_SP) );
        dest->set_user_flags( get_mr(EXCEPT_IPC_GEN_MR_FLAGS) );
    } else {
        arm_irq_context_t * RESTRICT context = &dest->arch.context;
        word_t * RESTRICT mr = &this->get_utcb()->mr[0];

        dest->set_user_ip( (addr_t)get_mr(EXCEPT_IPC_SYS_MR_PC) );
        dest->set_user_sp( (addr_t)get_mr(EXCEPT_IPC_SYS_MR_SP) );
        dest->set_user_flags( get_mr(EXCEPT_IPC_SYS_MR_FLAGS) );

        context->r0 = mr[EXCEPT_IPC_SYS_MR_R0];
        context->r1 = mr[EXCEPT_IPC_SYS_MR_R1];
        context->r2 = mr[EXCEPT_IPC_SYS_MR_R2];
        context->r3 = mr[EXCEPT_IPC_SYS_MR_R3];
        context->r4 = mr[EXCEPT_IPC_SYS_MR_R4];
        context->r5 = mr[EXCEPT_IPC_SYS_MR_R5];
        context->r6 = mr[EXCEPT_IPC_SYS_MR_R6];
        context->r7 = mr[EXCEPT_IPC_SYS_MR_R7];
        context->lr = mr[EXCEPT_IPC_SYS_MR_LR];
    }

    return true;
}

void tcb_t::copy_user_regs(tcb_t *src)
{
    arm_irq_context_t * RESTRICT to = &arch.context;
    arm_irq_context_t * RESTRICT from = &(src->arch.context);

    to->r0 = from->r0;
    to->r1 = from->r1;
    to->r2 = from->r2;
    to->r3 = from->r3;
    to->r4 = from->r4;
    to->r5 = from->r5;
    to->r6 = from->r6;
    to->r7 = from->r7;
    to->r8 = from->r8;
    to->r9 = from->r9;
    to->r10 = from->r10;
    to->r11 = from->r11;
    to->r12 = from->r12;
    to->lr = from->lr;
    to->cpsr = from->cpsr;
}

void tcb_t::copy_regs_to_mrs(tcb_t *src)
{
    arm_irq_context_t * RESTRICT from = &(src->arch.context);
    word_t * RESTRICT mr = &this->get_utcb()->mr[0];

    mr[0] = from->r0;
    mr[1] = from->r1;
    mr[2] = from->r2;
    mr[3] = from->r3;
    mr[4] = from->r4;
    mr[5] = from->r5;
    mr[6] = from->r6;
    mr[7] = from->r7;
    mr[8] = from->r8;
    mr[9] = from->r9;
    mr[10] = from->r10;
    mr[11] = from->r11;
    mr[12] = from->r12;
    mr[13] = from->sp;
    mr[14] = from->lr;
    mr[15] = (word_t)src->get_user_ip();
    mr[16] = from->cpsr;
}

void tcb_t::copy_mrs_to_regs(tcb_t *dest)
{
    arm_irq_context_t * RESTRICT from = &(dest->arch.context);
    word_t * RESTRICT mr = &this->get_utcb()->mr[0];

    from->r0  = mr[0];
    from->r1  = mr[1];
    from->r2  = mr[2];
    from->r3  = mr[3];
    from->r4  = mr[4];
    from->r5  = mr[5];
    from->r6  = mr[6];
    from->r7  = mr[7];
    from->r8  = mr[8];
    from->r9  = mr[9];
    from->r10 = mr[10];
    from->r11 = mr[11];
    from->r12 = mr[12];
    from->sp  = mr[13];
    from->lr  = mr[14];
    dest->set_user_flags(mr[16]);
    dest->set_user_ip((addr_t)mr[15]);
}

