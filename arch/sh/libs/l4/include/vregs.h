/* $Id$ */

/**
 * @since   December 2008
 */

#ifndef OKL4_ARCH_SH_L4_VREGS_H
#define OKL4_ARCH_SH_L4_VREGS_H

#include <l4/cust/utcb.h>

/**
 * Number of message registers.
 */
#define __L4_NUM_MRS                32

/**
 * Number of thread words copied by kernel.
 */
#define __L4_THREAD_WORDS_COPIED    4

/**
 * UTCB structure layout
 */
#define UTCB_STRUCT struct {                                            \
        capid_t             mutex_thread_handle;     /* 0 */            \
        word_t              user_defined_handle;     /* 4 */            \
        u8_t                preempt_flags;           /* 8 */            \
        u8_t                cop_flags;               /* 9 */            \
        u8_t                __reserved0;             /* 10 */           \
        u8_t                __reserved1;             /* 11 */           \
        acceptor_t          acceptor;                /* 12 */           \
        word_t              notify_mask;             /* 16 */           \
        notify_bits_t       notify_bits;             /* 20 */           \
        word_t              processor_no;            /* 24 */           \
        word_t              error_code;              /* 28 */           \
        word_t              __reserve2;              /* 32 */           \
        word_t              preempt_callback_ip;     /* 36 */           \
        word_t              preempted_ip;            /* 40 */           \
        word_t              share_fault_addr;        /* 44 */           \
        spaceid_t           sender_space;            /* 48 */           \
        u16_t               __reserved3;             /* 52 */           \
        u16_t               __reserved4;             /* 54 */           \
        /* Reserved for future kernel use.              56 .. 63  */    \
        word_t              kernel_reserved[__L4_TCR_RESERVED_NUM];     \
                                                                        \
        word_t              mr[__L4_NUM_MRS];        /* 64 .. 191 */    \
                                                                        \
        word_t              tls_word;                /* 192 */          \
        /* Reserved for future platform specifics use.  196 .. 215 */   \
        word_t              platform_reserved[__L4_TCR_PLATFORM_NUM];   \
        /* Thread private words.                        216 .. 255 */   \
        word_t              thread_word[__L4_TCR_USER_NUM];             \
    }


/**
 * Location of TCRs within UTCB
 */
#define __L4_TCR_USER_NUM           (10)
#define __L4_TCR_PLATFORM_NUM       (5)
#define __L4_TCR_RESERVED_NUM       (2)

#define __L4_TCR_MR_OFFSET          (64)

//#define USER_UTCB_REF               (SYS_AREA_START + 0xFF0)
#define USER_UTCB_REF               (0x7F000FF0)

#if !defined(__ASSEMBLER__)

/**
 * Obtains the pointer to the UTCB (User Thread Control Block).
 */
INLINE word_t* L4_GetUtcbBase(void) CONST;
INLINE word_t*
L4_GetUtcbBase()
{
    return *(word_t**)USER_UTCB_REF;
}

#endif

#endif /* OKL4_ARCH_SH_L4_VREGS_H */
