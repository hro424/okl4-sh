/**
 * @brief   Utilities to access memory mapped registers
 * @since   February 2009
 * @author  Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 */

#ifndef OKL4_ARCH_SH_KERNEL_REGISTERS_H
#define OKL4_ARCH_SH_KERNEL_REGISTERS_H

#if !defined(ASSEMBLY)

INLINE word_t
mapped_reg_read(word_t reg)
{
    return *(volatile word_t*)reg;
}

INLINE void
mapped_reg_write(word_t reg, word_t val)
{
    *(volatile word_t*)reg = val;
}

INLINE u16_t
mapped_reg_read16(word_t reg)
{
    return *(volatile u16_t*)reg;
}

INLINE void
mapped_reg_write16(word_t reg, u16_t val)
{
    *(volatile u16_t*)reg = val;
}

INLINE u8_t
mapped_reg_read8(word_t reg)
{
    return *(volatile u8_t*)reg;
}

INLINE void
mapped_reg_write8(word_t reg, u8_t val)
{
    *(volatile u8_t*)reg = val;
}

#endif

#endif /* OKL4_ARCH_SH_KERNEL_REGISTERS_H */
