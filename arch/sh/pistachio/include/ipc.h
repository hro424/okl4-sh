/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_IPC_H
#define OKL4_ARCH_SH_IPC_H

void do_ipc_helper();
void restart_do_ipc();

INLINE void
tcb_t::do_ipc(tcb_t* to_tcb, tcb_t* from_tcb, continuation_t continuation)
{
    ASSERT(DEBUG, this == get_current_tcb());

    /* setup IPC restart/return info */
    this->sys_data.set_action(tcb_syscall_data_t::action_ipc);
    TCB_SYSDATA_IPC(this)->from_tid.set_raw((word_t)from_tcb);
    TCB_SYSDATA_IPC(this)->to_tid.set_raw((word_t)to_tcb);
    TCB_SYSDATA_IPC(this)->ipc_restart_continuation = restart_do_ipc;
    TCB_SYSDATA_IPC(this)->ipc_return_continuation = do_ipc_helper;

    /* Keep track of where we need to come back to. */
    TCB_SYSDATA_IPC(this)->do_ipc_continuation = continuation;

    to_tcb = acquire_read_lock_tcb(to_tcb);
    from_tcb = acquire_read_lock_tcb(from_tcb);

    /* Call sys_ipc with a custom continuation as the return address. */
    activate_function((word_t)to_tcb, (word_t)from_tcb, 3, (word_t)ipc);
}

NORETURN INLINE void
SYS_IPC_SLOW(capid_t to_tid, capid_t from_tid, continuation_t continuation)
{
    call_with_asm_continuation(to_tid.get_raw(), from_tid.get_raw(),
                               (word_t)continuation, (word_t)sys_ipc);
}

NORETURN INLINE void
SYS_IPC_RESTART(capid_t to_tid, capid_t from_tid, continuation_t continuation)
{
    call_with_asm_continuation(to_tid.get_raw(), from_tid.get_raw(),
                               (word_t)continuation, (word_t)sys_ipc);
}

#endif /* OKL4_ARCH_SH_IPC_H */

