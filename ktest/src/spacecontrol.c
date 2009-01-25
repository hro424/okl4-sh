/*
 * Copyright (c) 2005, National ICT Australia
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

#include <ktest/ktest.h>
#include <ktest/utility.h>
#include <stddef.h>
#include <l4/ipc.h>
#include <l4/caps.h>
#include <l4/schedule.h>
#include <l4/thread.h>
#include <l4/misc.h>
#include <l4/kdebug.h>
#include <l4/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <compat/c.h>
#include <okl4/kspaceid_pool.h>

static L4_ThreadId_t main_thread;
//static L4_ThreadId_t pager_thread;
static L4_ThreadId_t g_tid;
//#define INVALID_TID 0x14151337
#define PAGESIZE 0x1000
ALIGNED(PAGESIZE) static char global_utcb_area[PAGESIZE] = {1};
#ifndef NO_UTCB_RELOCATE
ALIGNED(PAGESIZE) static char global_other_area[PAGESIZE] = {1};
#endif
ALIGNED(PAGESIZE) static char global_stack_area[PAGESIZE] = {1};
#define stack_top ((L4_Word_t)(global_stack_area + PAGESIZE))

L4_Fpage_t UTCB_AREA;
L4_Fpage_t UTCB_AREA2;

#if defined(L4_64BIT)
#define KERNEL_AREA_START 0xfffffffe80000000
#define INVALID_USER_AREA 0xfffffffe80000000
#else
#define KERNEL_AREA_START 0xa0000000
#define INVALID_USER_AREA 0xffff0000
#endif

static void *global_utcb;

static void check_utcb(void)
{
    ARCH_THREAD_INIT
    L4_MsgTag_t tag;
    L4_Msg_t msg;
    L4_ThreadId_t dummy;

    L4_Call(main_thread);

#ifndef NO_UTCB_RELOCATE
    fail_unless((L4_Word_t)L4_GetUtcbBase() == (L4_Word_t)global_utcb,
                "Utcb does not appear to be valid");
#endif

    L4_MsgClear(&msg);
    tag.raw = 0;
    L4_Set_SendBlock(&tag);
    L4_Set_ReceiveBlock(&tag);
    L4_Set_MsgMsgTag(&msg, tag);
    L4_Set_MsgLabel(&msg, 0xdead);
    L4_MsgLoad(&msg);
    tag = L4_Ipc(main_thread, main_thread, L4_MsgMsgTag(&msg), &dummy);
    if (L4_IpcFailed(tag)) {
        printf("ERROR CODE %"PRI_D_WORD"\n", L4_ErrorCode());
        assert(!"should not get here");
    }
}

static void privilege_check(void)
{
    L4_Word_t result;
    L4_Fpage_t utcb_area;
    L4_MsgTag_t tag;
    L4_Msg_t msg;
    L4_ThreadId_t dummy;
    
    utcb_area = UTCB_AREA;

    result = L4_SpaceControl(KTEST_SPACE, L4_SpaceCtrl_resources, KTEST_CLIST, utcb_area, 0, NULL);

    fail_unless(result == 0, "Failed to return error");
    fail_unless(L4_ErrorCode() == L4_ErrInvalidSpace, "Wrong error code");

    L4_MsgClear(&msg);
    tag.raw = 0;
    L4_Set_SendBlock(&tag);
    L4_Set_ReceiveBlock(&tag);
    L4_Set_MsgMsgTag(&msg, tag);
    L4_Set_MsgLabel(&msg, 0xdead);
    L4_MsgLoad(&msg);
    tag = L4_Ipc(main_thread, main_thread, L4_MsgMsgTag(&msg), &dummy);
    if (L4_IpcFailed(tag)) {
        printf("ERROR CODE %"PRI_D_WORD"\n", L4_ErrorCode());
        assert(!"should not get here");
    }
}

/*
\begin{test}{SPACE0100}
  \TestDescription{Verify SpaceControl handles an invalid space-id as target}
  \TestFunctionalityTested{\Func{SpaceSpecifier}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Invoke \Func{SpaceControl} with an invalid spaceid
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for invalid space
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE0100)
{
    L4_SpaceId_t invalid_sid;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    invalid_sid = L4_SpaceId(kernel_spaceid_base + kernel_spaceid_entries);
    utcb_area = UTCB_AREA;

    result = L4_SpaceControl(invalid_sid, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

    fail_unless(result == 0, "Failed to return error");
    fail_unless(L4_ErrorCode() == L4_ErrInvalidSpace, "Wrong error code");
}
END_TEST

/*
\begin{test}{SPACE1000}
  \TestDescription{Verify SpaceControl with UtcbArea set to nilpage}
  \TestFunctionalityTested{\Func{KernelInterfacePageArea}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} on the new space with \Func{UtcbArea} set to \Func{nilpage}
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for invalid utcb area
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE1000)
{
    L4_Word_t result;
    L4_SpaceId_t space;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");

    utcb_area = L4_Nilpage;

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

#ifndef NO_UTCB_RELOCATE
    fail_unless(result == 0, "Failed to return error");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");
#else
    fail_unless(result == 1, "Returned error");
    //Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control failed");
#endif
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST

/*
\begin{test}{SPACE1100}
  \TestDescription{Verify SpaceControl handles UtcbArea set to CompleteAddressSpace}
  \TestFunctionalityTested{\Func{KernelInterfacePageArea}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} with \Func{UtcbArea} set to \Func{CompleteAddressSpace} on the new address space
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for invalid kip area
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE1100)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");

    utcb_area = L4_CompleteAddressSpace;

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

    fail_unless(result == 0, "Failed to return error");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST

/*
\begin{test}{SPACE1200}
  \TestDescription{Verify SpaceControl handles a UtcbArea with no permissions}
  \TestFunctionalityTested{\Func{KernelInterfacePageArea}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} with \Func{UtcbArea} with no permissions on the new address space
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for invalid kip area
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE1200)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");

    utcb_area = L4_Fpage((L4_Word_t)global_utcb_area, PAGESIZE);
    L4_Set_Rights(&utcb_area, L4_NoAccess);

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

    
#ifndef NO_UTCB_RELOCATE 
        fail_unless(result == 1, "Permissions caused failure");
        //Delete Space
        result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
        fail_unless(result == 1, "Space control failed");
#else
        fail_unless(result == 0, "Failed to return error");
        fail_unless(L4_ErrorCode() == 6, "Wrong error code");
#endif
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST

/*
\begin{test}{SPACE1300}
  \TestDescription{Verify SpaceControl handles a UtcbArea with read-only permission}
  \TestFunctionalityTested{\Func{KernelInterfacePageArea}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} with a \Func{UtcbArea} with read-only permission on the new address space 
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for invalid kip area
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE1300)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");

    utcb_area = L4_Fpage((L4_Word_t)global_utcb_area, PAGESIZE);
    L4_Set_Rights(&utcb_area, L4_ReadeXecOnly);

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

#ifndef NO_UTCB_RELOCATE
    fail_unless(result == 1, "Permissions caused failure");
    //Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control failed");
#else
    fail_unless(result == 0, "Failed to return error");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");
#endif
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST


/*
\begin{test}{SPACE1500}
  \TestDescription{Verify SpaceControl handles a UtcbArea with base + size $>$ size of address space}
  \TestFunctionalityTested{\Func{KernelInterfacePageArea}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} with a \Func{UtcbArea} with base + size $>$ size of address space on the new address space
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for invalid kip area
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE1500)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");

    utcb_area = L4_Fpage((L4_Word_t)KERNEL_AREA_START, 0x80000000);
    L4_Set_Rights(&utcb_area, L4_FullyAccessible);

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

    fail_unless(result == 0, "Failed to return error");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST

/*
\begin{test}{SPACE1600}
  \TestDescription{Verify SpaceControl handles a UtcbArea in kernel virtual address area}
  \TestFunctionalityTested{\Func{KernelInterfacePageArea}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} with a \Func{UtcbArea} in the kernel virtual address area  on the new address space
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for invalid kip area
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE1600)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");

    utcb_area = L4_Fpage((L4_Word_t)INVALID_USER_AREA, PAGESIZE);
    L4_Set_Rights(&utcb_area, L4_FullyAccessible);

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

    fail_unless(result == 0, "Failed to return error");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST


/*
\begin{test}{SPACE1900}
  \TestDescription{Verify UTCB region is valid after SpaceControl}
  \TestFunctionalityTested{\Func{UtcbRegion}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} on the new space with UtcbRegion set to a known address
      \item Check that \Func{SpaceControl} returns successfully
      \item Invoke \Func{ThreadControl} with the first UTCB address
      \item Check that \Func{ThreadControl} returns successfully
      \item Invoke \Func{ThreadControl} with the last UTCB address in the area
      \item Check that \Func{ThreadControl} returns successfully
      \item Each thread in external address space checks the pager value in it's UTCB
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE1900)
{
    L4_Word_t result, dummy;
    L4_SpaceId_t space;
    L4_Fpage_t utcb_area;
    L4_MsgTag_t tag;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");

    utcb_area = UTCB_AREA;
    
    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 1, "Space control failed");
    g_tid = allocateThread(space);
#ifdef NO_UTCB_RELOCATE
    global_utcb = (void *)-1UL;
#else
    global_utcb = global_utcb_area;
#endif
    result = L4_ThreadControl(g_tid, space, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    if (result != 1) {
        L4_Word_t err = L4_ErrorCode();
        printf("Error is %lu", err);
    }
    fail_unless(result == 1, "Thread control failed");
    L4_Schedule(g_tid, -1, 0x00000001, -1, -1, 0, &dummy);
    L4_Start_SpIp(g_tid, (L4_PtrSize_t)stack_top, (L4_Word_t)(L4_PtrSize_t)check_utcb);

    tag = L4_Receive(g_tid);
    fail_unless(L4_IpcSucceeded(tag), "KIP checking thread did not start");

    // Delete thread
    result = L4_ThreadControl(g_tid, L4_nilspace, L4_nilthread, L4_nilthread, L4_nilthread, 0, (void *)0);
    fail_unless(result == 1, "Thread control failed");
    deallocateThread(g_tid);
    //Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control failed");

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 1, "Space control failed");
    g_tid = allocateThread(space);
    //result = L4_ThreadControl(g_tid, space, default_thread_handler, L4_nilthread, L4_nilthread, 0, (void *)-1UL);
    //fail_unless(result == 1, "Thread control failed");
#ifndef NO_UTCB_RELOCATE
    global_utcb = (void *)((word_t)global_utcb_area + PAGESIZE -
            L4_GetUtcbSize());
#else
    global_utcb = (void *)-1UL;
#endif
    result = L4_ThreadControl(g_tid, space, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 1, "Thread control failed");
    L4_Schedule(g_tid, -1, 0x00000001, -1, -1, 0, &dummy);
    L4_Start_SpIp(g_tid, (L4_PtrSize_t)stack_top, (L4_Word_t)(L4_PtrSize_t)check_utcb);

    tag = L4_Receive(g_tid);
    fail_unless(L4_IpcSucceeded(tag), "UTCB checking thread did not start");

    // Delete Thread
    result = L4_ThreadControl(g_tid, L4_nilspace, L4_nilthread, L4_nilthread, L4_nilthread, 0, (void *)0);
    fail_unless(result == 1, "Thread control failed");
    deallocateThread(g_tid);
    //Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control failed");
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST

/*
\begin{test}{SPACE2000}
  \TestDescription{Verify UTCB region is enforced after SpaceControl}
  \TestFunctionalityTested{\Func{UtcbRegion}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create an inactive thread in a new address space
      \item Invoke \Func{SpaceControl} on the new space with UtcbRegion set to a known address
      \item Check that \Func{SpaceControl} returns successfully
      \item Attempt to create a thread with a UTCB one slot past the UTCB region using \Func{ThreadControl}
      \item Check that \Func{ThreadControl} fails with the \Func{ErrorCode} set to UTCB error
      \item Attempt to create a thread with a UTCB one slot before the UTCB region using \Func{ThreadControl}
      \item Check that \Func{ThreadControl} fails with the \Func{ErrorCode} set to  UTCB error
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2000)
{

#ifdef NO_UTCB_RELOCATE
    return;
#else
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");
    
    utcb_area = UTCB_AREA;

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 1, "Space control create failed");
    g_tid = allocateThread(space);
    global_utcb = global_utcb_area - L4_GetUtcbSize();
    result = L4_ThreadControl(g_tid, space, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 0, "Thread control 1 did not fail");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");

    global_utcb = (void *)((word_t)global_utcb_area + PAGESIZE);
    result = L4_ThreadControl(g_tid, space, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 0, "Thread control 2 did not fail");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");

    deallocateThread(g_tid);
    //Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control delete failed");
    okl4_kspaceid_free(spaceid_pool, space);
#endif /* NO_UTCB_RELOCATE */
}
END_TEST

/*
\begin{test}{SPACE2100}
  \TestDescription{Verify we can not move the UTCB location out of the UTCB region}
  \TestFunctionalityTested{\Func{UtcbRegion}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create a new address space with UtcbRegion set to a known address
      \item Check that \Func{SpaceControl} returns successfully
      \item Create a thread with a valid UTCB address
      \item Attempt to move the UTCB one slot past the UTCB region using \Func{ThreadControl}
      \item Check that \Func{ThreadControl} fails with the \Func{ErrorCode} set to UTCB error
      \item Attempt to move the UTCB one slot before the UTCB region using \Func{ThreadControl}
      \item Check that \Func{ThreadControl} fails with the \Func{ErrorCode} set to  UTCB error
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2100)
{
    // Test does not apply if architecture manages Utcbs
#ifdef NO_UTCB_RELOCATE
    return;
#else
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");
    
    utcb_area = UTCB_AREA;

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 1, "Space control create failed");
    g_tid = allocateThread(space);
    global_utcb = global_utcb_area;
    result = L4_ThreadControl(g_tid, space, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 1, "Thread control failed");

    global_utcb = (void *)((word_t)global_utcb_area - L4_GetUtcbSize());
    result = L4_ThreadControl(g_tid, L4_nilspace, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 0, "Thread control did not fail");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");

    result = L4_ThreadControl(g_tid, L4_nilspace, L4_nilthread, L4_nilthread, L4_nilthread, 0, (void *)0);
    fail_unless(result == 1, "Thread control failed");
    global_utcb = global_utcb_area;
    result = L4_ThreadControl(g_tid, space, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 1, "Thread control failed");

    global_utcb = (void *)((word_t)global_utcb_area + PAGESIZE);
    result = L4_ThreadControl(g_tid, L4_nilspace, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 0, "Thread control did not fail");
    fail_unless(L4_ErrorCode() == 6, "Wrong error code");
    result = L4_ThreadControl(g_tid, L4_nilspace, L4_nilthread, L4_nilthread, L4_nilthread, 0, (void *)0);
    fail_unless(result == 1, "Thread control failed");

    deallocateThread(g_tid);
    //Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control delete failed");
    okl4_kspaceid_free(spaceid_pool, space);
#endif /* NO_UTCB_RELOCATE */
}
END_TEST

/*
\begin{test}{SPACE2200}
  \TestDescription{Ensure we cannot create a space already created}
  \TestFunctionalityTested{\Func{UtcbRegion}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create a new address space
      \item Try to create a second address space with the same space id as the previously created space
      \item Check that \Func{SpaceControl} fails with the \Func{ErrorCode} set to Invalid Space error
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2200)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    space = createSpace();
    utcb_area = UTCB_AREA;
    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 0, "Space control failed to return error");
    fail_unless(L4_ErrorCode() == 3, "Wrong error code");

    deleteSpace(space);
}
END_TEST

/*
\begin{test}{SPACE2300}
  \TestDescription{Ensure we cannot delete a non created space}
  \TestFunctionalityTested{\Func{UtcbRegion}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Try to delete a space not created
      \item Check that \Func{SpaceControl} fails with the \Func{ErrorCode} set to Invalid Space error
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2300)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");
    utcb_area = UTCB_AREA;
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 0, "Space control failed to return error");
    fail_unless(L4_ErrorCode() == 3, "Wrong error code");
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST

/*
\begin{test}{SPACE2400}
  \TestDescription{Ensure we cannot delete a non empty space}
  \TestFunctionalityTested{\Func{UtcbRegion}}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create a new address space
      \item Create a thread in this new address space
      \item Try to delete the space
      \item Check that \Func{SpaceControl} fails with the \Func{ErrorCode} set to SpaceNotEmpty error
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2400)
{
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");
    utcb_area = UTCB_AREA;

    result = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST, utcb_area, 0, NULL);

    fail_unless(result == 1, "Space control failed to create space");
    g_tid = allocateThread(space);
#ifndef NO_UTCB_RELOCATE
    global_utcb = global_utcb_area;
#else
    global_utcb = (void *)-1UL;
#endif
    result = L4_ThreadControl(g_tid, space, default_thread_handler, default_thread_handler, L4_nilthread, 0, global_utcb);
    fail_unless(result == 1, "Thread control failed to create thread");
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 0, "Space control failed to return error");
    fail_unless(L4_ErrorCode() == 9, "Wrong error code");

    // Delete Thread
    result = L4_ThreadControl(g_tid, L4_nilspace, L4_nilthread, L4_nilthread, L4_nilthread, 0, (void *)0);
    fail_unless(result == 1, "Thread control failed");
    deallocateThread(g_tid);
    // Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control failed");
    okl4_kspaceid_free(spaceid_pool, space);
}
END_TEST

/*
\begin{test}{SPACE2500}
  \TestDescription{Verify SpaceControl cannot be called from an unprivileged address space}
  \TestFunctionalityTested{Privilege}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create a thread in an external address space
      \item Invoke \Func{SpaceControl} with valid arguments
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for privilege error
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2500)
{
    L4_ThreadId_t tid;
    L4_SpaceId_t space;
    L4_MsgTag_t tag;

    tid = createThreadInSpace(L4_nilspace, privilege_check);
    space = lookupSpace(tid);

    tag = L4_Receive(tid);
    fail_unless(L4_IpcSucceeded(tag), "unprivileged checking thread did not start");

    deleteThread(tid);
    deleteSpace(space);
}
END_TEST

static void
dummy_thread(void)
{
    for (;;);
}

static void
privileged_thread(void)
{
    L4_ThreadId_t dummy, tmp;
    L4_SpaceId_t space;

    L4_Wait(&tmp);

    dummy = createThreadInSpace(L4_nilspace, dummy_thread);
    space = lookupSpace(dummy);
    fail_unless(!L4_IsNilThread(dummy), "ThreadControl failed to create thread");
    deleteThread(dummy);
    deleteSpace(space);
    L4_Set_MsgTag(L4_Niltag);
    L4_Call(main_thread);
}

#ifndef NO_UTCB_RELOCATE
extern L4_Word_t utcb_addr;
#endif

/*
\begin{test}{SPACE2600}
  \TestDescription{Verify SpaceControl can be called from a created privileged address space}
  \TestFunctionalityTested{Privilege}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create a new privileged space by invoking \Func{SpaceControl}
      \item Create a thread in the new privileged address space
      \item Privileged thread runs and attempts to create a thread in an external space
      \item Check that \Func{ThreadControl} succeeds
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2600)
{
    L4_ThreadId_t priv_tid;
    L4_MsgTag_t tag, zerotag;
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    zerotag.raw = 0;
    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");
#ifndef NO_UTCB_RELOCATE
    utcb_area = L4_Fpage(utcb_addr, NUM_THREADS * UTCB_SIZE);
#else
    utcb_area = L4_Nilpage;
#endif

    result = L4_SpaceControl(space, L4_SpaceCtrl_prio(100) | L4_SpaceCtrl_new | L4_SpaceCtrl_kresources_accessible, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 1, "Space control failed to create privileged space");
    priv_tid = createThreadInSpace(space, privileged_thread);

    L4_Set_MsgTag(zerotag);
    tag = L4_Send(priv_tid);
    fail_unless(L4_IpcSucceeded(tag), "privileged thread did not start properly");
    L4_Set_MsgTag(zerotag);
    tag = L4_Receive(priv_tid);
    fail_unless(L4_IpcSucceeded(tag), "privileged thread did not finished properly");
    deleteThread(priv_tid);
    // Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control failed to delete space");
    okl4_kspaceid_free(spaceid_pool, space);
    //L4_DeleteClist(new_clist);
}
END_TEST

/*
\begin{test}{SPACE2601}
  \TestDescription{Verify a non-privileged address space can not be given privileged}
  \TestFunctionalityTested{Privilege}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create a new non-privileged space
      \item Attempt to give privilege to the new space by invoking \Func{SpaceControl}
      \item Check that \Func{SpaceControl} returns an error in result
      \item Check the \Func{ErrorCode} for EINVALID_PARAM error
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2601)
{
    L4_SpaceId_t space;
    L4_Word_t result;

    space = createSpace();

    result = L4_SpaceControl(space, L4_SpaceCtrl_kresources_accessible, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 0, "Space control unexpectedly gave privilege to existing space");
    fail_unless(L4_ErrorCode() == L4_ErrInvalidParam, "Wrong error code");

    deleteSpace(space);
}
END_TEST

/* ------------------------------------------------------------------- */

#include <l4/map.h>
#include <l4/space.h>

L4_SpaceId_t priv_mapping_thread_space;

static void
privileged_mapping_thread(void)
{
    L4_ThreadId_t tmp;
    L4_MapItem_t map_item;
    L4_Word_t res;
    L4_Word_t pgsize = L4_GetMinPageBits();

    L4_Wait(&tmp);

    L4_MapItem_Map(&map_item, kernel_test_segment_id, 0,
                   kernel_test_segment_vbase, pgsize, 0, 0x7);
    res = L4_ProcessMapItem(priv_mapping_thread_space, map_item);
    if (res != 1) {
        printf("%lx\n", L4_ErrorCode());
    }
    fail_unless(res == 1, "mapping thread could not map memory!");

    L4_MapItem_Unmap(&map_item, kernel_test_segment_vbase, pgsize);
    res = L4_ProcessMapItem(priv_mapping_thread_space, map_item);
    fail_unless(res == 1, "mapping thread could not unmap memory!");

    L4_Set_MsgTag(L4_Niltag);
    L4_Call(main_thread);
}

/*
\begin{test}{SPACE2700}
  \TestDescription{Verify MapControl can be called from a created privileged address space}
  \TestFunctionalityTested{Privilege}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create a new privileged space by invoking \Func{SpaceControl}
      \item Create a thread in the new privileged address space
      \item Privileged thread runs and attempts to map memory
      \item Check that \Func{MapControl} succeeds
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(SPACE2700)
{
    L4_ThreadId_t priv_tid;
    L4_MsgTag_t tag, zerotag;
    L4_SpaceId_t space;
    L4_Word_t result;
    L4_Fpage_t utcb_area;

    zerotag.raw = 0;
    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate space id");
#ifndef NO_UTCB_RELOCATE
    utcb_area = L4_Fpage(utcb_addr, NUM_THREADS * UTCB_SIZE);
#else
    utcb_area = L4_Nilpage;
#endif

    priv_mapping_thread_space = space;

    result = L4_SpaceControl(space, L4_SpaceCtrl_new | L4_SpaceCtrl_kresources_accessible, KTEST_CLIST, utcb_area, 0, NULL);
    fail_unless(result == 1, "Space control failed to create privileged space");
    priv_tid = createThreadInSpace(space, privileged_mapping_thread);
    L4_Set_Priority(priv_tid, 90);

    L4_Set_MsgTag(zerotag);
    tag = L4_Send(priv_tid);
    fail_unless(L4_IpcSucceeded(tag), "privileged mapping thread did not start properly");
    L4_Set_MsgTag(zerotag);
    tag = L4_Receive(priv_tid);
    fail_unless(L4_IpcSucceeded(tag), "privileged mapping thread did not finished properly");
    deleteThread(priv_tid);
    // Delete Space
    result = L4_SpaceControl(space, L4_SpaceCtrl_delete, KTEST_CLIST, L4_Nilpage, 0, NULL);
    fail_unless(result == 1, "Space control failed to delete space");
    okl4_kspaceid_free(spaceid_pool, space);
    //L4_DeleteClist(new_clist);
}
END_TEST

extern L4_ThreadId_t test_tid;

static void test_setup(void)
{
    UTCB_AREA = L4_Nilpage;
    UTCB_AREA2 = L4_Nilpage;

/* because the syscall addresses are mapped 1-1 we need to keep the KIP in the same place as roottasks */
#ifndef NO_UTCB_RELOCATE
    UTCB_AREA = L4_Fpage((L4_Word_t)global_utcb_area, PAGESIZE);
    UTCB_AREA2 = L4_Fpage((L4_Word_t)global_other_area, PAGESIZE);
#endif

    initThreads(0);
    main_thread = test_tid;
    g_tid = L4_nilthread;
}

static void test_teardown(void)
{
}

TCase *
make_spacecontrol_tcase(void)
{
    TCase *tc;

    initThreads(0);

    tc = tcase_create("SpaceControl");
    tcase_add_checked_fixture(tc, test_setup, test_teardown);
    tcase_add_test(tc, SPACE0100);
    tcase_add_test(tc, SPACE1000);
    tcase_add_test(tc, SPACE1100);
    tcase_add_test(tc, SPACE1200);
    tcase_add_test(tc, SPACE1300);
    tcase_add_test(tc, SPACE1500);
    tcase_add_test(tc, SPACE1600);
    tcase_add_test(tc, SPACE1900);
    tcase_add_test(tc, SPACE2000);
    tcase_add_test(tc, SPACE2100);
    tcase_add_test(tc, SPACE2200);
    tcase_add_test(tc, SPACE2300);
    tcase_add_test(tc, SPACE2400);
    tcase_add_test(tc, SPACE2500);
    tcase_add_test(tc, SPACE2600);
    tcase_add_test(tc, SPACE2601);
    tcase_add_test(tc, SPACE2700);

    return tc;
}

