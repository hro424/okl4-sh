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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <iguana/thread.h>
#include <l4/kdebug.h>
#include <l4/ipc.h>
#include <l4/schedule.h>
#include <mutex/mutex.h>
#include <iguana/env.h>
#include <rtos/rtos.h>

static volatile L4_ThreadId_t pi_main;
static volatile L4_ThreadId_t high_prio_thread;
static volatile L4_ThreadId_t medium1_prio_thread;
static volatile L4_ThreadId_t medium2_prio_thread;
static volatile L4_ThreadId_t medium3_prio_thread;
static volatile L4_ThreadId_t low_prio_thread;
static volatile int flag1 = 0;
static volatile int flag3 = 0;
static volatile int cnt_h = 0;
static volatile int cnt_m1 = 0;
static volatile int cnt_m2 = 0;
static volatile int cnt_l = 0;
static volatile int cnt_i1 = 0;
static volatile int cnt_i2 = 0;
static volatile int spinning = 1;
static volatile int libs_ready = 0;

static char *test_name = "IPC priority inversion";

void ipc_pi_high(int argc, char **argv);
void ipc_pi_medium(int argc, char **argv);
void ipc_pi_low(int argc, char **argv);
void ipc_pi_intermediate(int argc, char **argv);

/*
 * The highest priority thread blocks sending IPC to medium thread in the first
 * phase. It increments a counter on success.
 * It makes a Call to medium thread in the second phase. It increments a counter on success.
 */
void
ipc_pi_high(int argc, char **argv)
{
    L4_MsgTag_t tag;

    while (!libs_ready) ;
    L4_Receive(pi_main);

    //printf("High thread %lx/%lx starts IPC PI test\n", me, pi_main.raw);
    while (1) {
        while (spinning) ;
        /** First phase **/
        //printf("High thread %lx/%lx blocks sending ipc to Medium thread\n", me, pi_main.raw);
        L4_LoadMR(0, 0);
        tag = L4_Send(medium2_prio_thread);
        if (L4_IpcSucceeded(tag))
            cnt_h++;
        // If intermediate threads have run, then increment respective counter.
        if (flag1) 
            cnt_i1++;
        if (flag3) 
            cnt_i2++;
        /** Second phase **/
        // Re-initialise.
        L4_Receive(low_prio_thread);
        if (flag1) {
            L4_Receive_Nonblocking(medium1_prio_thread);
            flag1 = 0;
        }
        if (flag3) {
            L4_Receive_Nonblocking(medium3_prio_thread);
            flag3 = 0;
        }
        // Do second phase.
        //printf("High thread %lx/%lx blocks sending ipc to Medium thread\n", me, pi_main.raw);
        L4_LoadMR(0, 0);
        tag = L4_Call(medium2_prio_thread);
        if (L4_IpcSucceeded(tag))
            cnt_h++;
        // If intermediate threads have run, then increment counter.
        if (flag1) 
            cnt_i1++;
        if (flag3) 
            cnt_i2++;

        // Tell main thread iteration is done.
        L4_LoadMR(0, 0);
        L4_Call(pi_main);
        // Re-initialise.
        if (flag1) {
            L4_Receive_Nonblocking(medium1_prio_thread);
            flag1 = 0;
        }
        if (flag3) {
            L4_Receive_Nonblocking(medium3_prio_thread);
            flag3 = 0;
        }
    }
}

/*
 * The medium thread blocks receiving from lowest priority thread and then receives from
 * highest priority thread in the first phase. It increments 2 counters on success, one for each operation.
 * It receives from highest thread and makes a Call to lowest thread before
 * sending to highest thread in the second phase. It increments 2 counters on success, one for each operation.
 */
void
ipc_pi_medium(int argc, char **argv)
{
    L4_ThreadId_t any_thread;
    L4_MsgTag_t tag;

    while (!libs_ready) ;

    // Initialisation
    L4_Wait(&any_thread);
    while (L4_IsNilThread(medium1_prio_thread)) ;
    L4_LoadMR(0, 0);
    L4_Send(medium1_prio_thread);

    //printf("Middle thread %lx/%lx starts PI test\n", me, pi_main.raw);
    while (1) {
        while (spinning) ;
        /*** First phase ***/ 
        //printf("Medium thread %lx/%lx blocks receiving ipc to Low thread\n", me, pi_main.raw);
        tag = L4_Receive(low_prio_thread);
        if (L4_IpcSucceeded(tag))
            cnt_m1++;
        //printf("Medium thread %lx/%lx blocks receiving ipc from High thread\n", me, pi_main.raw);
        tag = L4_Receive(high_prio_thread);
        if (L4_IpcSucceeded(tag))
            cnt_m2++;
        L4_Yield();
        /** Second phase **/
        tag = L4_Receive(high_prio_thread);
        if (L4_IpcSucceeded(tag))
            cnt_m2++;
        L4_LoadMR(0, 0);
        tag = L4_Call(low_prio_thread);
        if (L4_IpcSucceeded(tag))
            cnt_m1++;
        L4_LoadMR(0, 0);
        tag = L4_Send(high_prio_thread);
        //printf("Medium thread %lx/%lx received ipc from Low thread\n", me, pi_main.raw);
        if (L4_IpcFailed(tag)) {
            //printf("IPC PI Error (Medium thread %lx/%lx): IPC failed, Error code=%lu\n", me, pi_main.raw, L4_ErrorCode());
            cnt_m2--;
        }
        L4_ThreadSwitch(low_prio_thread);
    }
}

/*
 * The lowest priority thread sends to medium thread in the first phase. It increments a counter on success.
 * It receives from medium thread and then sends to medium thread in the second phase. It increments a counter on success.
 */
void
ipc_pi_low(int argc, char **argv)
{
    L4_MsgTag_t tag;
    L4_ThreadId_t tid;

    while (!libs_ready) ;

    // Initalisation
    while (L4_IsNilThread(medium3_prio_thread)) ;
    L4_LoadMR(0, 0);
    L4_Send(medium3_prio_thread);

    //printf("Low thread %lx/%lx starts PI test\n", me, pi_main.raw);
    while (1) {
        while (spinning) ;
        /*** First phase ***/ 
        //printf("Low thread %lx/%lx blocks sending ipc from Medium thread\n", me, pi_main.raw);
        L4_LoadMR(0, 0);
        tag = L4_Send(medium2_prio_thread);
        if (L4_IpcSucceeded(tag))
            cnt_l++;
        //printf("Low thread %lx/%lx received ipc from Medium thread\n", me, pi_main.raw);
        L4_Yield();
        /** Second phase **/
        tag = L4_Niltag;
        L4_Set_SendBlock(&tag);
        L4_Set_ReceiveBlock(&tag);
        // Make an atomic IPC to be ready to receive
        tag = L4_Ipc(high_prio_thread, medium2_prio_thread, tag, &tid);
        if (L4_IpcSucceeded(tag))
            cnt_l++;
        L4_LoadMR(0, 0);
        tag = L4_Send(medium2_prio_thread);
        if (L4_IpcFailed(tag)) {
            //printf("IPC PI Error (Low thread %lx/%lx): IPC failed, Error code=%lu\n", me, pi_main.raw, L4_ErrorCode());
            cnt_l--;
        }
        L4_Yield();
    }
}

/*
 * Intermediate thread 1 has priority in between highest and medium thread.
 * Intermediate thread 2 has priority in between medium and lowest thread.
 * They set their flag each time they run during the PI test.
 */
void
ipc_pi_intermediate(int argc, char **argv)
{
    int num = 0;
    L4_ThreadId_t any_thread;
    L4_MsgTag_t tag = L4_Niltag;

    while (!libs_ready) ;
    if (argc) {
        num = atoi(argv[0]);
    } else {
        printf("(%s Intermediate Thread) Error: Argument(s) missing!\n", test_name);
        L4_Set_Label(&tag, 0xdead);
        L4_Set_MsgTag(tag);
        L4_Call(pi_main);
    }
    // Initialisation
    if (num == 1) {
        L4_Wait(&any_thread);
        L4_LoadMR(0, 0);
        L4_Send(pi_main);
    } else if (num == 2) {
        L4_Wait(&any_thread);
        while (L4_IsNilThread(medium2_prio_thread)) ;
        L4_LoadMR(0, 0);
        L4_Send(medium2_prio_thread);
    }
    L4_Yield();

    // Thread is now ready to set the flag the next time it is run.
    while(1) {
        if (num == 1) {
            flag1 = 1;
        } else if (num == 2) {
            flag3 = 1;
        }
        L4_LoadMR(0, 0);
        L4_Send(high_prio_thread);
        L4_Yield();
    }
}

static void
print_metrics(int iter)
{
    int nb_char;
    char *str, *tmp_str;

    str = malloc(800 * sizeof(char));
    tmp_str = str;
    nb_char = sprintf(tmp_str, "\n%s (0x%lx) results on %d iterations:", test_name, pi_main.raw, iter);
    tmp_str += nb_char;
    nb_char = sprintf(tmp_str, "\n* High thread - messages sent successfully to Medium thread: %d/%d", cnt_h, iter);
    tmp_str += nb_char;
    nb_char = sprintf(tmp_str, "\n* Medium thread - messages received successfully to Low thread: %d/%d, received successfully from High thread: %d/%d", cnt_m1, iter, cnt_m2, iter);
    tmp_str += nb_char;
    nb_char = sprintf(tmp_str, "\n* Low thread - messages sent successfully from Medium thread: %d/%d", cnt_l, iter);
    tmp_str += nb_char;
    if (cnt_i1) {
        nb_char = sprintf(tmp_str, "\n* Intermediate thread 1 ran %d times, ", cnt_i1);
        tmp_str += nb_char;
    } else {
        nb_char = sprintf(tmp_str, "\n* Intermediate thread 1 did not run, ");
        tmp_str += nb_char;
    }
    if (cnt_i2) {
        nb_char = sprintf(tmp_str, "Intermediate thread 2 ran %d times\n\n", cnt_i2);
        tmp_str += nb_char;
    } else {
        nb_char = sprintf(tmp_str, "Intermediate thread 2 did not run\n\n");
        tmp_str += nb_char;
    }
    printf("%s", str);
    free(str);
}

int
main(int argc, char **argv)
{
    L4_ThreadId_t tid;
    int i, max_iteration, eg_num, server_on;
    L4_Word_t me;
    L4_MsgTag_t tag = L4_Niltag;

    /*** Initialisation ***/
    pi_main = thread_l4tid(env_thread(iguana_getenv("MAIN")));
    me = pi_main.raw;
    high_prio_thread = medium1_prio_thread = medium2_prio_thread = medium3_prio_thread = low_prio_thread = L4_nilthread;

    high_prio_thread = thread_l4tid(env_thread(iguana_getenv("IPC_PI_HIGH")));
    medium3_prio_thread = thread_l4tid(env_thread(iguana_getenv("IPC_PI_INTERMEDIATE_2")));
    medium2_prio_thread = thread_l4tid(env_thread(iguana_getenv("IPC_PI_MEDIUM")));
    medium1_prio_thread = thread_l4tid(env_thread(iguana_getenv("IPC_PI_INTERMEDIATE_1")));
    low_prio_thread = thread_l4tid(env_thread(iguana_getenv("IPC_PI_LOW")));

    // Tell other threads that it is safe to use libraries
    libs_ready = 1;
    eg_num = max_iteration = server_on = 0;
    if (argc == 3) {
        eg_num = atoi(argv[0]);
        max_iteration = atoi(argv[1]);
        server_on = atoi(argv[2]);
    } else {
        printf("(%s 0x%lx) Error: Argument(s) missing!\n", test_name, me);
        return 1;
    }

    if (!server_on)
        printf("Start %s test #%d(0x%lx)\n", test_name, eg_num, me);
    // Wait for threads to be ready
    tag = L4_Wait(&tid);
    // If one thread had a problem while initialisation, then stop the test and notify
    // server that the test is dead.
    if (L4_Label(tag) == 0xdead) {
        rtos_init();
        test_died(test_name, eg_num);
        rtos_cleanup();
        return 1;
    }

    /*** Start test ***/
    for (i = 0; i < max_iteration; i++) {
        // Tell high prio thread to start the next iteration.
        L4_LoadMR(0, 0);
        tag = L4_Send(high_prio_thread);
        spinning = 0;
        // Wait for the iteration to finish.
        tag = L4_Receive(high_prio_thread);
        spinning = 1;
    }

    /*** Test finished ***/
    thread_delete(medium1_prio_thread);
    thread_delete(medium3_prio_thread);
    thread_delete(high_prio_thread);
    thread_delete(medium2_prio_thread);
    thread_delete(low_prio_thread);

    // If RTOS server is on, report results to it.
    if (server_on) {
        rtos_init();
        ipc_priority_inversion_results(eg_num, max_iteration, cnt_h, cnt_m1, cnt_m2, cnt_l, cnt_i1, cnt_i2);
        rtos_cleanup();
    } else {
        print_metrics(max_iteration);
        printf("%s test #%d(0x%lx) finished\n", test_name, eg_num, me);
    }

    return 0;
}
