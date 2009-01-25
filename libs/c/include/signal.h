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

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <sys/types.h>

/* Notification Types. */
/* No asynchronous notification is delivered when the event of interest occurs. */
#define SIGEV_NONE   0
/* The signal specified in sigev_signo shall be generated for the process when
the event of interest occurs. */
#define SIGEV_SIGNAL 1
/* A notification function is called to perform notification. */
#define SIGEV_THREAD 2

#define SIGCHLD 3
#define SIGFPE  8
#define SIGPIPE 13

#define SA_ONSTACK     0x0001
#define SA_RESTART     0x0002
#define SA_RESETHAND   0x0004
#define SA_NOCLDSTOP   0x0008
#define SA_NODEFER     0x0010
#define SA_NOCLDWAIT   0x0020
#define SA_SIGINFO     0x0040

#define SIG_DFL  (void (*)(int))0

typedef unsigned long int sigset_t;

union sigval {
    int sival_int;    /* Integer signal value. */
    void *sival_ptr;  /* Pointer signal value. */
};

struct sigevent {
    int sigev_notify;                             /* Notification type.       */
    int sigev_signo;                              /* Signal number.           */
    union sigval sigev_value;                     /* Signal value.            */
    void (*sigev_notify_function)(union sigval);  /* Notification function.   */

    /* Notification attributes, type is pthread_attr_t*, not void* */
    void *sigev_notify_attributes;
};

typedef struct siginfo
{
    int si_signo;           /* Signal number.                              */
    int si_code;            /* Signal code.                                */
    int si_errno;           /* An errno value associated with this signal. */
    pid_t si_pid;           /* Sending process ID.                         */
    uid_t si_uid;           /* Real user ID of sending process.            */
    void *si_addr;          /* Address of faulting instruction.            */
    int si_status;          /* Exit value or signal.                       */
    long si_band;           /* Band event for SIGPOLL.                     */
    union sigval si_value;  /* Signal value.                               */

} siginfo_t;

/* Structure describing the action to be taken when a signal arrives.  */
struct sigaction {
    /* Pointer to a signal-catching function or one of the macros SIG_IGN or
     * SIG_DFL */
    void (*sa_handler)(int);

    /* Pointer to a signal-catching function. */
    void (*sa_sigaction)(int, siginfo_t *, void *);

    /* Additional set of signals to be blocked
    during execution of signal-catching function. */
    sigset_t sa_mask;

    /* Special flags to affect behavior of signal. */
    int sa_flags;
};

const char * const sys_siglist[1];

int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact);
void (*signal(int sig, void (*func)(int)))(int);
char *strsignal(int sig);

int kill(pid_t pid, int sig);
int raise(int sig);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigismember(const sigset_t *set, int signo);
int sigpending(sigset_t *set);
int sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict oset);
int sigsuspend(const sigset_t *sigmask);
int sigwait(const sigset_t *restrict set, int *restrict sig);
int sigqueue(pid_t pid, int signo, const union sigval value);

#ifdef __USE_POSIX
#include <posix_signal.h>
#endif



#endif /*_SIGNAL_H_*/

