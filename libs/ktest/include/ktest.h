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

#ifndef L4TEST_H
#define L4TEST_H

#include <check/check.h>
#include <compat/c.h>
#include <ktest/arch/constants.h>

#ifdef __ARMv__
#if (__ARMv__ == 5)
#define NO_UTCB_RELOCATE
#endif
#endif /* __ARMv__ */

#define KTEST_SERVER    L4_rootserver
extern L4_SpaceId_t KTEST_SPACE;
extern L4_ClistId_t KTEST_CLIST;

TCase * make_kip_tcase(void);
TCase * make_heap_exhaustion_tcase(void);
TCase * make_kdb_tcase(void);
TCase * make_chk_breakin_test_tcase(void);
TCase * make_memdesc_tcase(void);
TCase * make_mapcontrol_tcase(void);
TCase * make_kmem_tcase(void);
TCase * make_spacecontrol_tcase(void);
TCase * make_ipc_tcase(void);
TCase * make_xas_ipc_tcase(void);
TCase * make_aipc_tcase(void);
TCase * make_xas_aipc_tcase(void);
TCase * make_thread_tcase(void);
TCase * make_cache_tcase(void);
TCase * make_caps_tcase(void);
TCase * make_capcomplex_tcase(void);
TCase * make_virtual_register_tcase(void);
TCase * make_thread_id_tcase(void);
TCase * make_cap_id_tcase(void);
TCase * make_threadcontrol_tcase(void);
TCase * make_exchange_registers_tcase(void);
TCase * make_ipc_control_tcase(void);
TCase * make_fuzz_tcase(void);
TCase * make_schedule_tcase(void);
TCase * make_exception_tcase(void);
TCase * make_preempt_tcase(void);
TCase * make_ipc_cpx_tcase(void);
TCase * make_cust_tcase(void);
TCase * make_smt_tcase(void);
TCase * make_platform_control_tcase(void);
TCase * make_spaceswitch_tcase(void);
TCase * make_ipc_schedule_inheritance_tcase(void);
TCase * make_mutex_tcase(void);
TCase * make_mutex_schedule_inheritance_tcase(void);
TCase * make_interrupt_control_tcase(void);
TCase * make_schedule_inheritance_graph_tcase(void);
TCase * make_remote_memcpy_tcase(void);

TCase * make_arch_tcase(void);

/* Architecture-specific setup of L4-Test. */
void ktest_arch_setup(void);

#endif /* L4TEST_H */
