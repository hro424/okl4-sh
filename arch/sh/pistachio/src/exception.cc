/**
 * @brief   Exception handlers
 * @file    arch/sh/pistachio/src/exception.cc
 * @since   January 2009
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#include <l4.h>
#include <debug.h>
#include <linear_ptab.h>
#include <schedule.h>
#include <space.h>
#include <tcb.h>
#include <tracebuffer.h>
#include <arch/exception.h>
#include <arch/ipc.h>
#include <arch/tlb.h>
#include <kdb/check_breakin.h>
#include <kdb/names.h>
#include <kdb/tracepoints.h>
#include <kernel/kdb/console.h>

DECLARE_TRACEPOINT(EXCEPTION_IPC_GENERAL);
//DECLARE_TRACEPOINT(EXCEPTION_TLB_MISS);

NORETURN INLINE void
halt_user_thread(continuation_t continuation)
{
    tcb_t*          current = get_current_tcb();
    scheduler_t*    scheduler = get_current_scheduler();

    scheduler->deactivate_sched(current, thread_state_t::halted,
                                current, continuation,
                                scheduler_t::sched_default);
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
        // NOT REACHED
    }

    // Clean-up.
    current->restore_state(1);
    current->clear_exception_ipc();

    ACTIVATE_CONTINUATION(current->
                          arch.misc.exception.exception_ipc_continuation);
}

/**
 * Delivers an exception IPC message to user.
 */
extern "C" NORETURN void
send_exception_ipc(word_t exc_num, word_t exc_code, sh_context_t* context,
                   continuation_t continuation)
{
    tcb_t*      current = get_current_tcb();
    msg_tag_t   tag;

    if (current->get_exception_handler() == NULL) {
        ACTIVATE_CONTINUATION(continuation);
    }
    /* XXX we should check if the kernel faulted */

    TRACEPOINT(EXCEPTION_IPC_GENERAL,
               printf("EXCEPTION_IPC_GENERAL: (%p) exc_num %d, exc_code %08lx, IP = %p\n",
                      current, exc_num, exc_code, PC(context->pc)));

    current->arch.exc_code = exc_code;
    // Create the message tag.
    tag.set(EXCEPT_IPC_GEN_MR_NUM, EXCEPTION_TAG, true, true);

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

/**
 * Deals with non-L4 system calls.
 * Invoked by the trap handler.
 */
extern "C" NORETURN void
handle_syscall_exception(sh_context_t* context)
{
    continuation_t  continuation = ASM_CONTINUATION;
    u16_t           instr;

    instr = *(u16_t*)(PC(context->pc));
    send_exception_ipc(1, instr, context, continuation);
}

/**
 * Deals with invalid instruction exceptions and the FPU exception.  Simply
 * delivers it to user via IPC.
 *
 * @param ecode     the error code
 * @param context   the context of the exception
 */
extern "C" NORETURN void
handle_instruction_exception(word_t ecode, sh_context_t* context)
{
    continuation_t  continuation;
    u16_t           instr;

    continuation = ASM_CONTINUATION;
    instr = *(u16_t*)(PC(context->pc));

    if (EXPECT_FALSE((context->sr & REG_SR_MD) == REG_SR_MD)) {
        printf("Kernel instruction exception at %p\n",
               (addr_t)PC(context->pc));
        enter_kdebug("exception");
    }

    send_exception_ipc(ecode, instr, context, continuation);
}

extern "C" NORETURN void
handle_address_error(word_t ecode, sh_context_t* context)
{
    continuation_t  continuation;
    u16_t           instr;

    continuation = ASM_CONTINUATION;
    instr = *(u16_t*)(PC(context->pc));

    printf("Address error (0x%x) at %p\n", ecode, (addr_t)PC(context->pc));
    enter_kdebug("exception");

    send_exception_ipc(ecode, instr, context, continuation);
}

/**
 * Handles TLB miss and memory access violation.  Invoked by the trap handler.
 *
 * @param ecode     the exception code
 * @param context   the context where this exception was raised
 */
extern "C" void
handle_tlb_exception(word_t ecode, sh_context_t* context)
{
    addr_t              faddr;
    pgent_t*            pg;
    space_t*            space;
    tcb_t*              current;
    pgent_t::pgsize_e   pgsize;
    space_t::access_e   access;
    continuation_t      continuation;

    //TODO: Ensure the policy to TLB fault and init write is correct.
    switch (ecode) {
        case ECODE_TLB_FAULT_W:
        case ECODE_INIT_WRITE:
        case ECODE_TLB_MISS_W:
            access = space_t::write;
            break;
        case ECODE_TLB_FAULT_R:
        case ECODE_TLB_MISS_R:
        default:
            access = space_t::read;
            break;
    }
       
    continuation = ASM_CONTINUATION;
    current = get_current_tcb();
    current->arch.misc.exception.exception_continuation = continuation;
    current->arch.misc.exception.exception_context = context;
    sh_cache::flush_d();

    faddr = (addr_t)mapped_reg_read(REG_TEA);
    space = current->get_space();
    if (space == NULL) {
        space = get_kernel_space();
    }

    ecode = mapped_reg_read(REG_EXPEVT);
    if (space->lookup_mapping(faddr, &pg, &pgsize)) {
        if (((access == space_t::write) && pg->is_writable(space, pgsize)) ||
            ((access == space_t::read) && pg->is_readable(space, pgsize))) {
            fill_tlb(faddr, space, pg, pgsize);
            //return;
            ACTIVATE_CONTINUATION(continuation);
        }
    }

    /* Need to raise a page fault */

    bool kernel;
    if (context->sr & REG_SR_MD) {
        kernel = true;
    }
    else {
        kernel = false;
    }

    space->handle_pagefault(faddr, (addr_t)context->pc, access, kernel,
                            continuation);
}


extern "C" word_t sys_wbtest(word_t op, word_t* arg0, word_t* arg1, word_t* arg2);

/**
 * Deals with miscellaneous system calls such as KDB calls.
 * Invoked by the trap handler.
 */
extern "C" void
sys_sh_misc(sh_context_t* context)
{
    continuation_t continuation = ASM_CONTINUATION;

    switch (context->r8 & 0xFF) {
#if defined(CONFIG_WBTEST)
        case L4_TRAP_WBTEST:
        {
            word_t res;
            res = sys_wbtest(context->r4, &context->r5, &context->r6,
                             &context->r1);
            context->r0 = res;
        }
        return;
#endif

#if defined(CONFIG_KDB_CONS)
        case L4_TRAP_KPUTC:
            putc((char)context->r4);
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
            kdb_breakin_handler(context->r4);
            return;
#endif

#if defined(CONFIG_THREAD_NAMES) ||         \
            defined(CONFIG_SPACE_NAMES) ||  \
            defined(CONFIG_MUTEX_NAMES)
        case L4_TRAP_KSET_OBJECT_NAME:
        {
            char    name[MAX_DEBUG_NAME_LENGTH];
            word_t  type;
            word_t  id;
            word_t* name_words;

            type = context->r4;
            id = context->r5;
            name_words = (word_t*)name;

            /* Copy the name from the user's registers */
            name_words[0] = context->r6;
            name_words[1] = context->r1;
            name_words[2] = context->r2;
            name_words[3] = context->r3;
            name[MAX_DEBUG_NAME_LENGTH - 1] = '\0';

            /* Set the object's name. */
            kdb_set_object_name((object_type_e)type, id, name);
            return;
        }
#endif

#ifdef CONFIG_DEBUG
        case L4_TRAP_KDEBUG:
        {
#if defined(CONFIG_KDB_CONS)
            space_t*    space;
            char*       string;
            word_t      user;

            space = get_current_space();
            if (space == NULL) {
                space = get_kernel_space();
            }

            string = (char*)context->r4;

            printf(TXT_BRIGHT "---KD# ");
            user = ((context->sr & REG_SR_MD) == REG_SR_MD);

            if (user) {
                printf("User: ");

                if (space->is_user_area(string)) {
                    char    c;
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
#endif

#if defined(CONFIG_KDEBUG_TIMER)
        case L4_TRAP_GETTICK:
            context->r0 = (u32_t)(get_current_time() / soc_get_timer_tick_length());
            return;
#endif

#if defined(CONFIG_TRACEPOINTS)
        case L4_TRAP_GETCOUNTER:
            context->r0 = tp_list.get(context->r4)->counter[0];
            return;
        case L4_TRAP_GETNUMTPS:
            context->r0 = tp_list.size();
            return;
        case L4_TRAP_GETTPNAME:
        {
            const char* name;

            name = trace_names[tp_list.get(context->r4)->id];
            if (name != NULL) {
                unsigned int len = 0;
                while (name[len++] != '\0') ;
                memcpy(&get_current_tcb()->get_utcb()->mr[0], name, len);
            }
        }
        case L4_TRAP_TCCTRL:
            if (context->r4 == 0) {
                GLOBAL(tracepoints_enabled) = false;
            }
            else {
                tracepoint_t* tp;
                GLOBAL(tracepoints_enabled) = true;
                tp_list.reset();
                while ((tp = tp_list.next()) != NULL) {
                    tp->reset_counter();
                }
            }
            return;
#endif
#endif // CONFIG_KDEBUG
        default:
            break;
    }

    /* XXX - should deliver this as a exception IPC */
    printf("Illegal misc syscall: syscall no = %ld\n", context->r12 & 0xff);
    halt_user_thread(continuation);
}

extern "C" void
handle_user_break(sh_context_t* context)
{
    //TODO
    /* Call kernel debugger */
}


