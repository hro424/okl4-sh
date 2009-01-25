/*
 * Copyright (c) 2004, National ICT Australia
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

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iguana/env.h>
#include <iguana/memsection.h>
#include <iguana/object.h>
#include <iguana/tls.h>
#include <iguana/thread.h>
#include <l4/thread.h>
#include <pthread.h>
#include <threadstate.h>

extern void __thread_state_init(void *);

/*
 * FIXME: (benjl) Is this the best place to define heap size? 
 */
#define HEAP_SIZE (1 * 1024 * 1024)     /* Each program gets a 1MB heap */

void __lib_init(uintptr_t *buf);
void __libc_setup(void *callback, void *stdin_p,
                  void *stdout_p, void *stderr_p,
                  void *heap_base_p, void *heap_end_p);
extern void __malloc_init(void *bss_p, void *top_p);
struct clist;
extern void __cap_init(void);
extern void serial_init(void);


extern void __morecore_init(void);


#ifdef THREAD_SAFE
static struct thread_state __ts;
#endif
static uintptr_t __tls_buffer[32];

void
__libc_setup(void *callback, void *stdin_p, void *stdout_p, void *stderr_p,
             void *heap_base, void *heap_end)
{
    __malloc_init(heap_base, heap_end);
}

/* Initialise all the libraries.  */
void
__lib_init(uintptr_t *buf)
{
    char *heap_base;
    size_t heap_size;

    __lib_iguana_init(buf);

    heap_base = (char*) env_const(iguana_getenv("HEAP_BASE"));
    heap_size = env_const(iguana_getenv("HEAP_SIZE"));

    __libc_setup(NULL, NULL, NULL, NULL, heap_base, heap_base + heap_size - 1);
    __cap_init();
    __morecore_init();

    __tls_init(&__tls_buffer);

#ifdef THREAD_SAFE
    __thread_state_init(&__ts);
#endif

    /* Initialise system serial. */
    serial_init();

    /* Initialize the pthread library */
    __pthread_lib_init();

    /* Tell other boot-created threads that the libraries are initialised. */
    __lib_iguana_library_ready();

}

void *__sys_stack;
long __sys_stack_size;

void __sys_entry(void *buf, int argc, char** argv);
void __sys_thread_entry(pthread_t pthread);
int main(int argc, char** argv);

/* Generic entry point for the main thread. */
void
__sys_entry(void *buf, int argc, char** argv)
{
    int ret;

    __lib_init(buf);
    ret = main(argc, argv);

    exit(ret);
}

/* Generic entry point for non-main threads.  */
void
__sys_thread_entry(pthread_t pthread)
{
    /* Initialize the threads TLS */
    __tls_init_malloc();
#ifdef THREAD_SAFE
    __thread_state_init(malloc(sizeof(struct thread_state)));
#endif

    __pthread_thread_init(pthread);

    /* Drop off and back into __pthread_init, which will call the user
       start routine */
}
