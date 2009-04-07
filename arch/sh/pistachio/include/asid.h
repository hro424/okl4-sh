/**
 * @file    arch/sh/pistachio/include/asid.h
 * @since   December 2008
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#ifndef OKL4_ARCH_SH_ASID_H
#define OKL4_ARCH_SH_ASID_H

/* SH has 8-bit ASID space */
typedef s16_t   hw_asid_t;

INLINE hw_asid_t
get_hw_asid()
{
    word_t  asid;

    __asm__ __volatile__ (
        "    mov     %1, %0"
        : "=r" (asid)
        : "m" (REG_PTEH)
    );
    asid &= REG_PTEH_ASID_MASK;
    return (hw_asid_t)asid;
}

INLINE void
set_hw_asid(hw_asid_t asid)
{
    word_t  tmp = (word_t)asid;

    __asm__ __volatile__(
        "    mov.l   %1, r0\n"
        "    and     %2, r0\n"
        "    or      %0, r0\n"
        "    mov.l   r0, %1\n"
        :
        : "r" (tmp), "m" (*(word_t*)REG_PTEH), "r" (REG_PTEH_VPN_MASK)
        : "r0", "memory"
    );

    UPDATE_REG();
}

INLINE void
flush_asid(hw_asid_t asid)
{
    //TODO
    /*XXX Do nothing here */
}

#endif /* OKL4_ARCH_SH_ASID_H */

