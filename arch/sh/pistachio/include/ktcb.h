/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_KTCB_H
#define OKL4_ARCH_SH_KTCB_H

#include <arch/thread.h>

class arch_ktcb_t
{
public:
    sh_context_t    context;
    word_t          exc_code;
    word_t          exc_num;
};

#endif /* OKL4_ARCH_SH_KTCB_H */
