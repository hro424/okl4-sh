/*********************************************************************
 *
 * Copyright (C) 2006,  Karlsruhe University
 *
 * File path:    glue/v4-amd64/ia32/trap-sysenter.S
 * Description:  Trap handlers for compatibility mode
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 *
 *********************************************************************/
#include INC_ARCH(asm.h)
#include INC_ARCH(pagebits.h)
#include <tcb_layout.h>
#include <asmsyms.h>

	.global syscall_return_32

BEGIN_PROC(sysenter_entry_32)
	movq (%rsp), %rsp		// load KSP (tss->rsp0)
	subq $8, %rsp			// skip SS
	pushq %rbp			// save USP
	pushfq				// save UFLAGS
	pushq $0			// set CS to SYSCALL marker (0)
	pushq %rcx			// save UIP (after sysenter)
	pushq %rbx			// push arg7
	pushq %rax			// push arg6
	call syscall_dispatcher_32
syscall_return_32:
	addq $16, %rsp			// remove arg6, arg7
	popq %rcx			// restore UIP
	addq $8, %rsp			// pop (undef)
	popq %r11			// restore UFLAGS
	popq %rsp			// restore USP
	sysretl
END_PROC(sysenter_entry_32)

.end
