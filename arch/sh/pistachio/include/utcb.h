/* $Id$ */

/**
 *  @since  February 2009
 */

#ifndef OKL4_ARCH_SH_UTCB_H
#define OKL4_ARCH_SH_UTCB_H

#undef USER_UTCB_REF
#define USER_UTCB_REF           (*(word_t*)(USER_UTCB_PAGE + 0xff0))

#endif /* OKL4_ARCH_SH_UTCB_H */
