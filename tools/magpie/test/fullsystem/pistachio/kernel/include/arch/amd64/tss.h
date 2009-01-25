/*********************************************************************
 *                
 * Copyright (C) 2002-2005,  Karlsruhe University
 *                
 * File path:     arch/amd64/tss.h
 * Description:   AMD64 Task State Segment
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
#ifndef __ARCH__AMD64__TSS_H__
#define __ARCH__AMD64__TSS_H__

#include <debug.h> 

#if defined(CONFIG_IO_FLEXPAGES)
#define AMD64_IOPERMBITMAP_BITS          (1 << 16)
#define AMD64_IOPERMBITMAP_ALIGNMENT     __attribute__((aligned(4096)));
#else
#define AMD64_IOPERMBITMAP_BITS 0
#define AMD64_IOPERMBITMAP_ALIGNMENT
#endif

#define IOPERMBITMAP_SIZE		(AMD64_IOPERMBITMAP_BITS / 8)


class amd64_tss_t 
{
public:
    void init();
    void set_rsp0(u64_t rsp0);
    u64_t get_rsp0();
    addr_t get_io_bitmap();
    static word_t get_io_bitmap_offset();

private:
    u32_t	reserved0;
    u64_t	rsp[3] __attribute__((packed));	      
    u64_t	reserved1;
    u64_t	ist[7] __attribute__((packed));	      
    u64_t	reserved2;
    u16_t	reserved3;
    u16_t	iopbm_offset;    
    u8_t	io_bitmap[IOPERMBITMAP_SIZE+1] AMD64_IOPERMBITMAP_ALIGNMENT;
    u8_t	stopper;
} __attribute__((packed));

INLINE void amd64_tss_t::init()
{
    iopbm_offset = (u16_t)((u64_t)io_bitmap - (u64_t)this);
    stopper = 0xff;
}

INLINE void amd64_tss_t::set_rsp0(u64_t rsp0)
{
    rsp[0] = rsp0;
}

INLINE u64_t amd64_tss_t::get_rsp0()
{
    return rsp[0];
}


INLINE addr_t amd64_tss_t::get_io_bitmap()
{
    return (addr_t) io_bitmap;
}

INLINE word_t amd64_tss_t::get_io_bitmap_offset()
{
    amd64_tss_t *t = (amd64_tss_t *) 0;
    return (word_t) &(t->io_bitmap);
}

extern amd64_tss_t tss;


#endif /* !__ARCH__AMD64__TSS_H__ */
