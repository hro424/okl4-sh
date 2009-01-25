/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_SYSCALLS_H
#define OKL4_ARCH_SH_SYSCALLS_H

#include <arch/config.h>
#include <compat/c.h>
#include <kernel/arch/asm.h>
#include <l4/arch/syscalls_asm.h>

/*
 * Upper boundon syscall number
 */
#define SYSCALL_limit                   SYSCALL_last

/*
 * Attributes for system call functions
 */
#if defined(_lint)
#define SYSCALL_ATTR(x)
#else
#define SYSCALL_ATTR(x) NORETURN
#endif

#define SYS_IPC_RETURN_TYPE                     void
#define SYS_THREAD_CONTROL_RETURN_TYPE          void
#define SYS_EXCHANGE_REGISTERS_RETURN_TYPE      void
#define SYS_MAP_CONTROL_RETURN_TYPE             void
#define SYS_SPACE_CONTROL_RETURN_TYPE           void
#define SYS_SCHEDULE_RETURN_TYPE                void
#define SYS_CACHE_CONTROL_RETURN_TYPE           void
#define SYS_THREAD_SWITCH_RETURN_TYPE           void
#define SYS_PLATFORM_CONTROL_RETURN_TYPE        void
#define SYS_SPACE_SWITCH_RETURN_TYPE            void
#define SYS_MUTEX_RETURN_TYPE                   void
#define SYS_MUTEX_CONTROL_RETURN_TYPE           void
#define SYS_INTERRUPT_CONTROL_RETURN_TYPE       void
#define SYS_CAP_CONTROL_RETURN_TYPE             void
#define SYS_MEMORY_COPY_RETURN_TYPE             void

/*
 * Syscall declaration wrappers.
 */

#define SYS_IPC(to, from)                                       \
  SYS_IPC_RETURN_TYPE /*SYSCALL_ATTR ("ipc")*/                  \
  sys_ipc (to, from)

#define SYS_THREAD_CONTROL(dest, space, scheduler, pager,       \
                except_handler, thread_resources, utcb)         \
  SYS_THREAD_CONTROL_RETURN_TYPE SYSCALL_ATTR ("thread_control")\
  sys_thread_control (dest, space, scheduler, pager,            \
                      except_handler, thread_resources, utcb)

#define SYS_SPACE_CONTROL(space, control, clist, utcb_area,  \
                space_resources)                                \
  SYS_SPACE_CONTROL_RETURN_TYPE SYSCALL_ATTR ("space_control")  \
  sys_space_control (space, control, clist, utcb_area, space_resources)

#define SYS_SCHEDULE(dest, ts_len, hw_thread_bitmask,           \
                     processor_control, prio, flags)            \
  SYS_SCHEDULE_RETURN_TYPE SYSCALL_ATTR ("schedule")            \
  sys_schedule (dest, ts_len, hw_thread_bitmask,                \
          processor_control, prio, flags)

#define SYS_EXCHANGE_REGISTERS(dest, control, usp, uip, uflags, \
                        uhandle)                                \
  SYS_EXCHANGE_REGISTERS_RETURN_TYPE SYSCALL_ATTR ("exchange_registers")\
  sys_exchange_registers (dest, control, usp, uip,              \
                          uflags, uhandle)

#define SYS_THREAD_SWITCH(dest)                                 \
  SYS_THREAD_SWITCH_RETURN_TYPE SYSCALL_ATTR ("thread_switch")  \
  sys_thread_switch (dest)

#define SYS_MAP_CONTROL(space, control)                         \
  SYS_MAP_CONTROL_RETURN_TYPE SYSCALL_ATTR ("map_control")      \
  sys_map_control (space, control)

#define SYS_CACHE_CONTROL(space, control)                       \
  SYS_CACHE_CONTROL_RETURN_TYPE SYSCALL_ATTR ("cache_control")  \
  sys_cache_control (space, control)

#define SYS_PLATFORM_CONTROL(control, param1, param2, param3)   \
  SYS_PLATFORM_CONTROL_RETURN_TYPE SYSCALL_ATTR ("platform_control")\
  sys_platform_control (control, param1, param2, param3)

#define SYS_SPACE_SWITCH(thread, space, utcb_location)          \
  SYS_SPACE_SWITCH_RETURN_TYPE SYSCALL_ATTR ("space_switch")    \
  sys_space_switch(thread, space, utcb_location)

#define SYS_MUTEX(mutex_id, flags, state_p)             \
    SYS_MUTEX_RETURN_TYPE SYSCALL_ATTR("mutex")         \
    sys_mutex(mutex_id, flags, state_p)

#define SYS_MUTEX_CONTROL(mutex_id, control)            \
    SYS_MUTEX_RETURN_TYPE SYSCALL_ATTR("mutex_control") \
    sys_mutex_control(mutex_id, control)

#define SYS_INTERRUPT_CONTROL(thread, control)                  \
  SYS_INTERRUPT_CONTROL_RETURN_TYPE SYSCALL_ATTR ("interrupt_control") \
  sys_interrupt_control(thread, control)

#define SYS_CAP_CONTROL(clist, control)                         \
  SYS_CAP_CONTROL_RETURN_TYPE SYSCALL_ATTR ("cap_control")      \
  sys_cap_control(clist, control)

#define SYS_MEMORY_COPY(remote, local, size, direction)          \
  SYS_MEMORY_COPY_RETURN_TYPE SYSCALL_ATTR ("memory_copy") \
  sys_memory_copy(remote, local, size, direction)

#if defined(_lint)
void empty(word_t foo);
#define return_platform_control(result, cont) cont(); empty (result); return
#define return_cache_control(result, cont) cont(); empty (result); return
#define return_schedule(result, rem_ts, cont) cont(); empty(rem_ts); empty(result); return
#define return_ipc() return 
#define return_exchange_registers(result, control, sp, ip, flags, pager, handle, cont) cont(); empty(control); empty (result); return
#define return_thread_control(result, cont) cont(); empty (result); return
#define return_thread_switch(cont) cont(); return
#define return_map_control(result, cont) cont(); empty (result); return
#define return_space_control(result, space_resources, cont) cont(); empty(space_resources); empty (result); return
#define return_space_switch(result, cont) cont(); empty (result); return
#define return_mutex(result, cont) cont(); empty(result); return
#define return_mutex_control(result, cont) cont(); empty(result); return
#define return_interrupt_control(result, cont) cont(); empty (result); return
#define return_cap_control(result, cont) cont(); empty (result); return
#define return_memory_copy(result, size, cont) cont(); empty(result); empty(size); return

#elif defined(__GNUC__)

/**
 * Preload registers and return from sys_ipc
 * @param from The FROM value after the system call
 */
#define return_ipc() \
    do {                                                \
        ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->ipc_return_continuation);    \
    } while(false)

/**
 * Preload registers and return from sys_thread_control
 * @param result The RESULT value after the system call
 */
#define return_thread_control(result, cont) {           \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Preload registers and return from sys_exchange_registers
 * @param result The RESULT value after the system call
 * @param control The CONTROL value after the system call
 * @param _sp The SP value after the system call
 * @param ip The IP value after the system call
 * @param flags The FLAGS value after the system call
 * @param pager The PAGER value after the system call
 * @param handle The USERDEFINEDHANDLE value after the system call
 */
#define return_exchange_registers(result, control, _sp, ip, flags, pager, handle, continuation)\
{                                                                       \
    register word_t rslt    ASM_REG("r1") = result;                     \
    register word_t ctrl    ASM_REG("r2") = control;                    \
    register word_t sp_r    ASM_REG("r3") = _sp;                        \
    register word_t ip_r    ASM_REG("r4") = ip;                         \
    register word_t fl_r    ASM_REG("r5") = flags;                      \
    register word_t hdl_r   ASM_REG("r6") = handle;                     \
    register word_t pgr_r   ASM_REG("r7") = (pager).get_raw();          \
                                                                        \
    __asm__ __volatile__ (                                              \
        CHECK_ARG("r1", "%1")                                           \
        CHECK_ARG("r2", "%2")                                           \
        CHECK_ARG("r3", "%3")                                           \
        CHECK_ARG("r4", "%4")                                           \
        CHECK_ARG("r5", "%5")                                           \
        CHECK_ARG("r6", "%6")                                           \
        CHECK_ARG("r7", "%7")                                           \
        "    or      %8, r15         \n"                                \
        "    jmp     @%0             \n"                                \
        "    nop                     \n"                                \
        :                                                               \
        : "r"  (continuation), "r" (rslt), "r" (ctrl), "r" (sp_r),      \
          "r" (ip_r), "r" (fl_r), "r" (hdl_r), "r" (pgr_r),             \
          "r" (STACK_TOP)                                               \
        : "r15", "memory"                                               \
    );                                                                  \
    while (1);                                                          \
}

/**
 * Return from sys_thread_switch
 */
#define return_thread_switch(cont)                                      \
{                                                                       \
    ACTIVATE_CONTINUATION(cont);                                        \
}

/**
 * Return from sys_map_control
 * @param result The RESULT value after the system call
 */
#define return_map_control(result, cont) {              \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Preload registers and return from sys_space_control
 * @param result The RESULT value after the system call
 * @param space_resources The original SPACE_RESOURCES value from before the system call
 * @param cont The continuation function to jump to
 */
#define return_space_control(result, space_resources, cont) {           \
    register word_t rslt    ASM_REG("r4") = result;                     \
    register word_t spcres  ASM_REG("r5") = space_resources;            \
                                                                        \
    __asm__ __volatile__ (                                              \
        CHECK_ARG("r4", "%0")                                           \
        CHECK_ARG("r5", "%1")                                           \
        "    or      %3, r15         \n"                                \
        "    jmp     @%2             \n"                                \
        "    nop                     \n"                                \
        :                                                               \
        : "r" (rslt), "r" (spcres), "r" (cont), "r" (STACK_TOP)         \
        : "r15", "memory"                                               \
    );                                                                  \
    while (1);                                                          \
}

/**
 * Preload registers and return from sys_schedule
 * @param result The RESULT value after the system call
 */
#define return_schedule(result, rem_ts, continuation) {                 \
    register word_t rslt    ASM_REG("r4") = result;                     \
    register word_t remts   ASM_REG("r5") = rem_ts;                     \
                                                                        \
    __asm__ __volatile__ (                                              \
        CHECK_ARG("r4", "%1")                                           \
        CHECK_ARG("r5", "%2")                                           \
        "    or      %3, r15         \n"                                \
        "    jmp     @%0             \n"                                \
        "    nop                     \n"                                \
        :                                                               \
        : "r" (continuation), "r" (rslt), "r" (remts), "r" (STACK_TOP)  \
        : "r15", "memory"                                               \
    );                                                                  \
    while (1);                                                          \
}

/**
 * Return from sys_cache_control
 */
#define return_cache_control(result, cont) {            \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Return from sys_platform_control
 * @param result The RESULT value after the system call
 */
#define return_platform_control(result, cont) {         \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Return from sys_space_switch
 * @param result The RESULT value after the system call
 */
#define return_space_switch(result, cont) {             \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Return from sys_mutex
 *
 * @param result The RESULT value after the system call
 */
#define return_mutex(result, cont) {                    \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Return from sys_mutex_control
 *
 * @param result The RESULT value after the system call
 */
#define return_mutex_control(result, cont) {            \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}


/**
 * Return from sys_interrupt_control_
 * @param result The RESULT value after the system call
 */
#define return_interrupt_control(result, cont) {        \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Return from sys_cap_control
 * @param result The RESULT value after the system call
 */
#define return_cap_control(result, cont) {              \
    register word_t rslt    ASM_REG("r4") = result;     \
                                                        \
    __asm__ __volatile__ (                              \
        CHECK_ARG("r4", "%0")                           \
        "    or      %2, r15         \n"                \
        "    jmp     @%1             \n"                \
        "    nop                     \n"                \
        :                                               \
        : "r" (rslt), "r" (cont), "r" (STACK_TOP)       \
        : "r15", "memory"                               \
    );                                                  \
    while (1);                                          \
}

/**
 * Return from sys_interrupt_control_
 * @param result The RESULT value after the system call
 */
#define return_memory_copy(result, size, cont) {                \
    register word_t rslt    ASM_REG("r4") = result;             \
    register word_t siz     ASM_REG("r5") = size;               \
                                                                \
    __asm__ __volatile__ (                                      \
        CHECK_ARG("r4", "%0")                                   \
        CHECK_ARG("r5", "%1")                                   \
        "    or      %3, r15         \n"                        \
        "    jmp     @%2             \n"                        \
        "    nop                     \n"                        \
        :                                                       \
        : "r" (rslt), "r" (siz), "r" (cont), "r" (STACK_TOP)    \
        : "r15", "memory"                                       \
    );                                                          \
    while (1);                                                  \
}

#else
#error "Unknown compiler"
#endif

#endif /* OKL4_ARCH_SH_SYSCALLS_H */
