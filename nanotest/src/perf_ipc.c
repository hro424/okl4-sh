/*
 * Copyright (c) 2008 Open Kernel Labs, Inc. (Copyright Holder).
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
 * Nano IPC Performance Tests
 */

#include <stdio.h>
#include <stdlib.h>

#include <nano/nano.h>
#include <atomic_ops/atomic_ops.h>

#include <nanotest/nanotest.h>
#include <nanotest/system.h>
#include <nanotest/performance.h>

/* Send an IPC message with a dummy payload. */
static inline unsigned long
send_null_ipc(unsigned long destination, unsigned long operation)
{
    return okn_syscall_ipc_send(
            0, 0, 0, 0, 0, 0, 0,
            destination, operation);
}

/* Receive an IPC message, discarding the payload. */
static inline unsigned long
recv_null_ipc(unsigned long src, unsigned long operation)
{
    return okn_syscall_ipc_recv(
            0, 0, 0, 0, 0, 0, 0, 0,
            src, operation);
}

/*
 * IPC1000 : IPC call to high priority server (closed wait)
 */

static void
ipc1000_server(int *tids)
{
    recv_null_ipc(tids[1], 0);
    end_test("IPC1000_end");
    send_null_ipc(tids[1], 0);
}

static void
ipc1000_client(int *tids)
{
    start_test("IPC1000_start");
    send_null_ipc(tids[0], OKN_IPC_CALL);
}

static test_thread_t ipc1000_functions[] = {
    {ipc1000_server, 2},
    {ipc1000_client, 1},
    {NULL, 0},
};

MULTITHREADED_PERF_TEST(PERF_IPC, 1000,
        "IPC call to high priority server (closed wait)",
        "IPC1000_start", "IPC1000_end", PERF_CYCLE_COUNTER, 1,
        ipc1000_functions);

/*
 * IPC1005 : IPC call to high priority server (open wait)
 */

static void
ipc1005_server(int *tids)
{
    recv_null_ipc(OKN_IPC_OPEN_WAIT, 0);
    end_test("IPC1005_end");
    send_null_ipc(tids[1], 0);
}

static void
ipc1005_client(int *tids)
{
    start_test("IPC1005_start");
    send_null_ipc(tids[0], OKN_IPC_CALL);
}

static test_thread_t ipc1005_functions[] = {
    {ipc1005_server, 2},
    {ipc1005_client, 1},
    {NULL, 0},
};

MULTITHREADED_PERF_TEST(PERF_IPC, 1005,
        "IPC call to high priority server (open wait)",
        "IPC1005_start", "IPC1005_end", PERF_CYCLE_COUNTER, 1,
        ipc1005_functions);

/*
 * IPC1010 : IPC server reply to calling thread.
 */

static void
ipc1010_server(int *tids)
{
    recv_null_ipc(tids[1], 0);

    /* We do the reply/wait as two syscalls, because Nano has
     * no atomic reply/wait call. */
    start_test("IPC1010_start");
    send_null_ipc(tids[1], OKN_IPC_NON_BLOCKING);
    recv_null_ipc(OKN_IPC_OPEN_WAIT, 0);
}

static void
ipc1010_client(int *tids)
{
    send_null_ipc(tids[0], OKN_IPC_CALL);
    end_test("IPC1010_end");

    /* Wake server up again. */
    send_null_ipc(tids[0], 0);
}

static test_thread_t ipc1010_functions[] = {
    {ipc1010_server, 2},
    {ipc1010_client, 1},
    {NULL, 0},
};

MULTITHREADED_PERF_TEST(PERF_IPC, 1010,
        "IPC call to high priority server (closed wait)",
        "IPC1010_start", "IPC1010_end", PERF_CYCLE_COUNTER, 1,
        ipc1010_functions);

/*
 * IPC Performance Tests
 */
performance_test_t **
get_ipc_perf_tests(void)
{
    static performance_test_t *tests[] = {
        &PERF_IPC_1000,
        &PERF_IPC_1005,
        &PERF_IPC_1010,
        NULL
        };
    return tests;
}

