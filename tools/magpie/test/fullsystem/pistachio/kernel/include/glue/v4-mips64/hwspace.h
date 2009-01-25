/*********************************************************************
 *                
 * Copyright (C) 2002-2003,  University of New South Wales
 *                
 * File path:     glue/v4-mips64/hwspace.h
 * Description:   Conversion between kernel addrs and physical addrs 
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
 ********************************************************************/

#ifndef __GLUE__V4_MIPS64__HWSPACE_H__
#define __GLUE__V4_MIPS64__HWSPACE_H__

#include <debug.h>
#include INC_ARCH(types.h)
#include INC_API(config.h)

#include INC_ARCH(page.h)

INLINE addr_t virt_to_phys(addr_t addr)
{
    ASSERT(addr >= (addr_t) AS_KSEG_START && addr < (addr_t) AS_KSEG_END);
    return (addr_t) ((word_t) addr - AS_KSEG_START);
}

INLINE addr_t phys_to_virt(addr_t addr)
{
    return (addr_t) ((word_t) addr +  AS_KSEG_START);
}

INLINE addr_t tree_to_addr(addr_t addr)
{
    return (addr_t) ((((word_t) addr)&0x00FFFFFFFFFFFFFF)  | (AS_KSEG_START&0xFF00000000000000));
}

#endif /* __GLUE__V4_MIPS64__HWSPACE_H__ */
