/**
 * @brief   SH sytem calls ABI
 * @since   December 2009
 */

#ifndef OKL4_ARCH_SH_L4_SYSCALLS_H
#define OKL4_ARCH_SH_L4_SYSCALLS_H

#include <l4/types.h>
#include <l4/message.h>

/*
 * L4_KernelInterface
 */
void *L4_KernelInterface(L4_Word_t *ApiVersion,
                         L4_Word_t *ApiFlags,
                         L4_Word_t *KernelId);

/*
 * L4_ExchangeRegisters
 */
L4_Word_t L4_ExchangeRegisters(L4_ThreadId_t dest,
                                   L4_Word_t control,
                                   L4_Word_t sp,
                                   L4_Word_t ip,
                                   L4_Word_t flags,
                                   L4_Word_t UserDefHandle,
                                   L4_ThreadId_t pager,
                                   L4_Word_t *old_control,
                                   L4_Word_t *old_sp,
                                   L4_Word_t *old_ip,
                                   L4_Word_t *old_flags,
                                   L4_Word_t *old_UserDefHandle);

/*
 * L4_ThreadControl
 */
L4_Word_t L4_ThreadControl(L4_ThreadId_t dest,
                           L4_SpaceId_t SpaceSpecifier,
                           L4_ThreadId_t Scheduler,
                           L4_ThreadId_t Pager,
                           L4_ThreadId_t ExceptionHandler,
                           L4_Word_t resources,
                           void *UtcbLocation);

/*
 * L4_ThreadSwitch
 */
void L4_ThreadSwitch(L4_ThreadId_t dest);

/*
 * L4_Schedule
 */
L4_Word_t L4_Schedule(L4_ThreadId_t dest,
                      L4_Word_t timeslice,
                      L4_Word_t hw_thread_bitmask,
                      L4_Word_t ProcessorControl,
                      L4_Word_t prio,
                      L4_Word_t flags,
                      L4_Word_t *rem_timeslice);

/*
 * L4_Ipc
 */
L4_MsgTag_t L4_Ipc(L4_ThreadId_t to,
                   L4_ThreadId_t FromSpecifier,
                   L4_MsgTag_t tag,
                   L4_ThreadId_t *from);

/*
 * L4_Notify
 */
L4_MsgTag_t L4_Notify(L4_ThreadId_t to, L4_Word_t mask);

/*
 * L4_WaitNotify
 */
L4_MsgTag_t L4_WaitNotify(L4_Word_t *mask);

/*
 * L4_Lipc
 */
L4_MsgTag_t L4_Lipc(L4_ThreadId_t to,
                    L4_ThreadId_t FromSpecifier,
                    L4_MsgTag_t tag, L4_ThreadId_t *from);

/*
 * L4_MapControl
 */
L4_Word_t L4_MapControl(L4_SpaceId_t SpaceSpecifier, L4_Word_t control);

/*
 * L4_SpaceControl
 */
L4_Word_t L4_SpaceControl(L4_SpaceId_t SpaceSpecifier,
                          L4_Word_t control,
                          L4_ClistId_t clist,
                          L4_Fpage_t UtcbArea,
                          L4_Word_t resources,
                          L4_Word_t *old_resources);

/*
 * L4_CacheControl
 */
L4_Word_t L4_CacheControl(L4_SpaceId_t SpaceSpecifier, L4_Word_t control);

/*
 * L4_PlatformControl
 */
L4_Word_t L4_PlatformControl(L4_Word_t control,
                             L4_Word_t param1,
                             L4_Word_t param2,
                             L4_Word_t param3);

/*
 * L4_SpaceSwitch
 */
L4_Word_t L4_SpaceSwitch(L4_ThreadId_t dest,
                         L4_SpaceId_t SpaceSpecifier,
                         void *UtcbLocation);

/*
 * L4_Mutex
 */
word_t L4_Mutex(L4_MutexId_t    MutexSpecifier,
                L4_Word_t       flags,
                word_t*         state_p);

/*
 * L4_MutexControl
 */
L4_Word_t L4_MutexControl(L4_MutexId_t  MutexSpecifier,
                          L4_Word_t     control);


/*
 * L4_InterruptControl
 */
L4_Word_t L4_InterruptControl(L4_ThreadId_t dest,
                              L4_Word_t     control);

/*
 * L4_CapControl
 */
L4_Word_t L4_CapControl(L4_ClistId_t    clist,
                        L4_Word_t       control);

/*
 * L4_MemoryCopy
 */
L4_Word_t L4_MemoryCopy(L4_ThreadId_t   remote,
                        L4_Word_t       local,
                        L4_Word_t*      size,
                        L4_Word_t       direction);


L4_Word_t L4_Wbtest(L4_Word_t   op,
                    L4_Word_t*  arg0,
                    L4_Word_t*  arg1,
                    L4_Word_t*  arg2);

#endif /* OKL4_ARCH_SH_L4_SYSCALLS_H */
