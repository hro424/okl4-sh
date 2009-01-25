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
 * Framework to test Nano system calls and functionality.
 */

#ifndef NANOTEST_H
#define NANOTEST_H

typedef int(*test_entry_point_t)(void);

struct nano_test {
    int type;
    char *category;
    int number;
    char *name;
    test_entry_point_t function;
};

/* Priority of the main thread. */
#define ROOT_TASK_PRIORITY  16
#define MAX_PRIORITY        31

/* Test Categories */
#define TEST_THREAD     0
#define TEST_FUTEX      1
#define TEST_INTERRUPT  2

/* Test categories. */
struct nano_test **get_atomic_tests(void);
struct nano_test **get_thread_tests(void);
struct nano_test **get_futex_tests(void);
struct nano_test **get_interrupt_tests(void);
struct nano_test **get_signal_tests(void);
struct nano_test **get_ipc_tests(void);
struct nano_test **get_qmsgq_tests(void);
struct nano_test **get_qthread_tests(void);
struct nano_test **get_qinterrupt_tests(void);

/* Test types. */
#define TEST_TYPE_STANDARD        0
#define TEST_TYPE_TRACED          1
#define TEST_TYPE_TIMED           2

/* Define a test. */
#define TEST(category, number, description)                            \
    static int test_##category##_##number##_func(void);                \
    static struct nano_test category##_##number                         \
            = {TEST_TYPE_STANDARD, #category, number, description,     \
                    test_##category##_##number##_func};                \
    static int test_##category##_##number##_func(void)                 \

/* Define a test. */
#define TIMED_TEST(category, number, description)                      \
    static int test_##category##_##number##_func(void);                \
    static struct nano_test category##_##number                         \
            = {TEST_TYPE_TIMED, #category, number, description,        \
                    test_##category##_##number##_func};                \
    static int test_##category##_##number##_func(void)                 \

/* Static information about a performance test. */
#define PERF_TEST_INFO(cat, number, description, start_lbl, end_lbl)   \
    const char *__PERF_TEST_INFO__ ## cat ## _ ## number               \
            = "PERF_TEST name:" #cat #number " "                       \
              "desc:\"" description "\" "                              \
              "start:" start_lbl " end:" end_lbl "\n"                  \

/* Labels for start/ending performance measurement sections. */
#define LABEL(x)                                                       \
        __asm__ __volatile__(                                          \
                ".global __before_" x "\n"                             \
                "__before_" x ":       \n"                             \
                "    nop               \n"                             \
                ".global " x "         \n"                             \
                x ":                   \n"                             \
                "    nop               \n"                             \
                ".global __after_" x " \n"                             \
                "__after_" x ":        \n"                             \
                : /* no outputs */                                     \
                : /* no inputs */                                      \
                : "memory")

/* In-test messages. */
#define message(x...)  printf("                : " x)

#endif /* NANOTEST_H */

