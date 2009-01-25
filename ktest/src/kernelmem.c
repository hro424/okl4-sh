/*
 * Copyright (c) 2006, National ICT Australia
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

/*
 * Author: Matthew Warton
 * Date  : Wed 20 Sep 2006
 */

#include <l4/ipc.h>
#include <l4/kdebug.h>
#include <l4/map.h>
#include <l4/space.h>
#include <l4/thread.h>
#include <l4/config.h>

#include <l4e/map.h>

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#include <ktest/ktest.h>
#include <ktest/utility.h>
#include <okl4/kspaceid_pool.h>


/* Defines -----------------------------------------------------------------*/


/* Some utility functions want to check a condition and if the
 * condition fails return an error. */
#define FAIL_UNLESS(cond, desc) fail_unless((cond), (desc)); \
if (!(cond)) { return 1; }
#define FAIL_IF(cond, desc) fail_if((cond), (desc)); \
if ((cond)) { return 1; }


/* Prototypes --------------------------------------------------------------*/

static L4_ThreadId_t main_thread;
static L4_ThreadId_t thread_offset(int i);

static int map_series(L4_SpaceId_t space, L4_Word_t page_size, uintptr_t n_pages, uintptr_t v, uintptr_t offs);

/* Utility functions -------------------------------------------------------*/

static L4_ThreadId_t thread_offset(int i)
{
    L4_ThreadId_t t = main_thread;
    return L4_GlobalId(L4_ThreadNo(t) + (i+16), 1);
}

/**
 * Utility function for creating address spaces.
 *
 * @param[in] space the space id to use for the new space
 * @param[in] utcb_fpage the memory to be used for UTCBs
 */
static int
create_address_space(L4_SpaceId_t space, L4_Fpage_t utcb_fpage)
{
    L4_Word_t res;

#ifdef NO_UTCB_RELOCATE
    utcb_fpage = L4_Nilpage;
#endif

    res = L4_SpaceControl(space, L4_SpaceCtrl_new, KTEST_CLIST,
            utcb_fpage, 0, NULL);
    return res;
}

/**
 * Setup a series of mappings.
 *
 * @param[in] page_size the page size to use for all the mappings
 * @param[in] n_pages how many pages to use in the sequence
 * @param[in] v the virtual address to start the sequence of mappings
 * @param[in] p the physical address to start the sequence of mappings
 *
 * @return 0 for success and non-zero on error
 */
static int
map_series(L4_SpaceId_t space, L4_Word_t page_size, uintptr_t n_pages, uintptr_t v, uintptr_t offs)
{
    int i, res = 1;
    uintptr_t page_bits = __L4_Msb(page_size);
    L4_MapItem_t map;

    for (i = 0; i < n_pages; i++, offs += page_size) {
        L4_MapItem_Map(&map, kernel_test_segment_id, offs,
                v + i*page_size, page_bits,
                L4_DefaultMemory, L4_FullyAccessible);

        res = L4_ProcessMapItem(space, map);
        if(res != 1)
            break;
    }

    return res;
}


/* Space IDs */

/*
\begin{test}{KMEM01}
  \TestDescription{Check that IPC's do not cause a failure even after kernel memory has been exhausted}
  \TestFunctionalityTested{Kernel memory allocator}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Exhaust kernel memory by repeatedly allocating address spaces with 2gb mappings
      \item Attempt to send IPC's to a series of non existant threads
      \item Delete all the allocated spaces
      \item Attempt to send IPC's to non existent threads
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(KMEM01)
{
    int j, i = 0;
    L4_MsgTag_t tag;
    int res = 1;
    int max_offset;
    L4_Word_t result, end;
    L4_SpaceId_t space;
    okl4_allocator_attr_t attr;

    while(res == 1)
    {
        result = okl4_kspaceid_allocany(spaceid_pool, &space);
        fail_unless(result == OKL4_OK, "Failed to allocate any space id.");
        res = create_address_space(space,
                                   L4_Fpage(0xb10000, 0x1000));

        if(res != 1)
            break;
        i++;
        /* 2gb mapping of 512k pages*/
        res = map_series(space, 0x80000, 4096, 0, 0);
    }

    printf("Created %d address spaces\n", i);

    /* test dummy tcb allocation */
    L4_LoadMR(0, TAG_RBLOCK);
    max_offset = kernel_max_threads;
    for(j = 128; j < max_offset; j+=128)
    {
        tag = L4_Send(thread_offset(j));
    }

    /* clean up */
    okl4_kspaceid_getattribute(spaceid_pool, &attr);
    end = attr.base + attr.size;
    for (i = attr.base; i < end; i++)
    {
        L4_SpaceId_t id = L4_SpaceId(i);
        if (okl4_kspaceid_isallocated(spaceid_pool, id) &&
            id.space_no != KTEST_SPACE.space_no)
        {
            deleteSpace(id);
        }
    }

    /* test dummy tcb allocation */
    L4_LoadMR(0, TAG_SRBLOCK);
    for(j = 1; j < 128; j++)
    {
        tag = L4_Send(thread_offset(j*128));
    }
}
END_TEST

/* use up all kmem by creating spaces with 512mb worth of mappings, then deleting all of them */
/* returns the number of spaces it was able to create */
static int CreateMaxSpaces(void);
static int CreateMaxSpaces(void)
{
    int res = 1, i = 0;
    int max;
    L4_Word_t result, end;
    L4_SpaceId_t space;
    okl4_allocator_attr_t attr;

    while(res == 1)
    {
        result = okl4_kspaceid_allocany(spaceid_pool, &space);
        fail_unless(result == OKL4_OK, "Failed to allocate any space id.");
        res = create_address_space(space,
                                   L4_Fpage(0xb10000, 0x1000));

        if(res != 1)
            break;
        /* 2gb mapping of 512k pages*/
        res = map_series(space, 0x80000, 4096, 0, 0);
        i++;
    }

    max = i;

    /* clean up */
    okl4_kspaceid_getattribute(spaceid_pool, &attr);
    end = attr.base + attr.size;
    for (i = attr.base; i < end; i++)
    {
        L4_SpaceId_t id = L4_SpaceId(i);
        if (okl4_kspaceid_isallocated(spaceid_pool, id) &&
            id.space_no != KTEST_SPACE.space_no)
        {
            deleteSpace(id);
        }
    }

    return max;
}

/* use up all kmem by creating threads, delete all threads */
/* returns the number of threads it was able to create */
/* On arm this is limited due to running out of UTCB space */
static int CreateMaxThreads(void);
static int CreateMaxThreads(void)
{
    int i;
    int res = 1;
   
    // MIPS32 address space ends at 0x80000000
#if (defined(L4_ARCH_MIPS) && defined(L4_32BIT))
    unsigned long utcb_base = 0x70000000;
#else
    unsigned long utcb_base = 0xb0000000;
#endif

    void * utcb = (void *)utcb_base;
    L4_Fpage_t utcb_area = L4_Fpage(utcb_base, 0x01000000);
    int max;
    L4_Word_t result;
    L4_SpaceId_t space;

    result = okl4_kspaceid_allocany(spaceid_pool, &space);
    fail_unless(result == OKL4_OK, "Failed to allocate any space id.");

    /* Create first thread (and address space) */
#ifdef NO_UTCB_RELOCATE
    utcb_area = L4_Nilpage;
    utcb = (void*)-1ul;
#endif

    res = create_address_space(space, utcb_area);

    i = 3;  // start creating threads after controlling space

    fail_unless(res == 1, "Failed to create controlling space\n");

    /* create threads */
    do {
        if (utcb) {
            utcb = (void *)(utcb_base + i * L4_GetUtcbSize());
        }
        if (!isSystemThread(thread_offset(i))) {
            res = L4_ThreadControl(thread_offset(i), space,
                    default_thread_handler, L4_nilthread, L4_nilthread, 0, (void *)utcb);
        }
        i++;
    } while (res == 1);

    max = i-3;

    /* delete threads */
    for (; i >= 3; i--) {
        if (!isSystemThread(thread_offset(i))) {
            res = L4_ThreadControl(thread_offset(i), L4_nilspace, L4_nilthread,
                    L4_nilthread, L4_nilthread, 0, (void *)0);
        }
    }

    res = L4_SpaceControl(space, L4_SpaceCtrl_delete,
            KTEST_CLIST, L4_Nilpage, 0, NULL);
    okl4_kspaceid_free(spaceid_pool, space);

    return max;
}

/*
\begin{test}{KMEM02}
  \TestDescription{Check that the creation and deletion of address spaces and threads does not leak memory}
  \TestFunctionalityTested{Kernel memory allocator}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Use up kernel memory by creating threads and then deleting the threads (to remove fragmentation issue)
      \item Repeat twice
      \begin{enumerate}
        \item Create threads until out of memory then delete all threads
        \item Create address spaces until out of memory then delete all created spaces
      \end{enumerate}
      \item Fail unless the created number of spaces and threads in each iteration are the same
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(KMEM02)
{
    int threads, threads2;
    int spaces, spaces2;

    threads = CreateMaxThreads();
    /* second call to CreateMaxThreads as the first call can return an inconsitent number
     * due to fragmentation. If there is a difference in the number of threads created between
     * the second and third calls, we actually do have a memory leak. 
     */
    threads = CreateMaxThreads();
    spaces = CreateMaxSpaces();

    threads2 = CreateMaxThreads();
    spaces2 = CreateMaxSpaces();

    fail_unless(threads == threads2, "Failed to free all memory used in creating spaces\n");
    fail_unless(spaces == spaces2, "Failed to free all memory used in creating threads\n");

}
END_TEST

/*
\begin{test}{KMEM03}
  \TestDescription{Check that creating and deleting different numbers of address spaces does not leak any memory}
  \TestFunctionalityTested{Kernel memory allocator}
  \TestImplementationProcess{
    \begin{enumerate}
      \item Create address spaces
      \item Delete all created spaces
      \item Recreate all address spaces
      \item Delete then create all even numbered address spaces
      \item Delete then create all odd numbered address spaces
      \item Delete and create the middle numbered address spaces
      \item Delete all created spaces
    \end{enumerate}
  }
  \TestImplementationStatus{Implemented}
  \TestIsFullyAutomated{Yes}
  \TestRegressionStatus{In regression test suite}
\end{test}
*/
START_TEST(KMEM03)
{
    int i;
    int SPACES_TO_TEST;
    okl4_allocator_attr_t attr;

    okl4_kspaceid_getattribute(spaceid_pool, &attr);
    SPACES_TO_TEST = attr.base + attr.size - 1;

    /* create and delete all spaces */
    for(i = attr.base; i < SPACES_TO_TEST; i++)
        create_address_space(L4_SpaceId(i), L4_Fpage(0xb10000, 0x1000));

    for(i = attr.base; i < SPACES_TO_TEST; i++)
        if (i != KTEST_SPACE.space_no) {
            L4_SpaceControl(L4_SpaceId(i), L4_SpaceCtrl_delete,
                            KTEST_CLIST, L4_Nilpage, 0, NULL);
        }

    /* delete and create odd and even spaces */
    for(i = attr.base; i < SPACES_TO_TEST; i++)
        create_address_space(L4_SpaceId(i), L4_Fpage(0xb10000, 0x1000));

    for(i = attr.base; i < SPACES_TO_TEST; i+=2)
        if (i != KTEST_SPACE.space_no) {
            L4_SpaceControl(L4_SpaceId(i), L4_SpaceCtrl_delete,
                            KTEST_CLIST, L4_Nilpage, 0, NULL);
        }

    for(i = attr.base; i < SPACES_TO_TEST; i+=2)
        create_address_space(L4_SpaceId(i), L4_Fpage(0xb10000, 0x1000));

    for(i = attr.base + 1; i < SPACES_TO_TEST; i+=2)
        if (i != KTEST_SPACE.space_no) {
            L4_SpaceControl(L4_SpaceId(i), L4_SpaceCtrl_delete,
                            KTEST_CLIST, L4_Nilpage, 0, NULL);
        }

    for(i = attr.base + 1; i < SPACES_TO_TEST; i+=2)
        create_address_space(L4_SpaceId(i), L4_Fpage(0xb10000, 0x1000));

    /* delete the middle range of spaces */
    for(i = attr.base + 2; i < SPACES_TO_TEST-2; i++)
        if (i != KTEST_SPACE.space_no) {
            L4_SpaceControl(L4_SpaceId(i), L4_SpaceCtrl_delete,
                            KTEST_CLIST, L4_Nilpage, 0, NULL);
        }

    for(i = attr.base + 2; i < SPACES_TO_TEST-2; i++)
        create_address_space(L4_SpaceId(i), L4_Fpage(0xb10000, 0x1000));

    /* clean up */
    for(i = attr.base; i < SPACES_TO_TEST; i++)
        if (i != KTEST_SPACE.space_no) {
            L4_SpaceControl(L4_SpaceId(i), L4_SpaceCtrl_delete,
                            KTEST_CLIST, L4_Nilpage, 0, NULL);
        }

}
END_TEST

extern L4_ThreadId_t test_tid;

static void test_setup(void)
{
    initThreads(1);
    main_thread = test_tid;
}


static void test_teardown(void)
{
}

TCase *
make_kmem_tcase(void)
{
    TCase *tc;

    tc = tcase_create("Kernel Memory Tests");
    tcase_add_checked_fixture(tc, test_setup, test_teardown);
    tcase_add_test(tc, KMEM01);
    tcase_add_test(tc, KMEM02);
    tcase_add_test(tc, KMEM03);

    return tc;
}
