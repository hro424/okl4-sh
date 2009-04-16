/**
 * @brief   SH kernel debugger interface
 */
#ifndef OKL4_ARCH_SH_L4_KDEBUG_H
#define OKL4_ARCH_SH_L4_KDEBUG_H

#include <l4/types.h>
#include <l4/arch/syscalls.h>

int L4_KDB_PrintChar_ASM(int c);

#if defined(CONFIG_DEBUG)
void L4_KDB_Set_Breakin_ASM(int);
#endif

int L4_KDB_ReadChar_ASM(void);
int L4_KDB_ReadChar_Blocked_ASM(void);
void L4_KDB_SetObjectName_ASM(word_t type, word_t id, const char *name);

word_t L4_KDB_GetTick(void);
word_t L4_KDB_GetNumTracepoints(void);
word_t L4_KDB_GetTracepointCount(word_t);
void L4_KDB_GetTracepointNameIntroMRs(word_t);
void L4_KDB_TCCtrl(word_t);

#ifndef NDEBUG
void L4_KDB_Enter(char * s);
#endif


INLINE okl4_kdb_res_t
L4_KDB_TracepointCountStop(void)
{
    L4_KDB_TCCtrl(0);
    return OKL4_KDB_RES_OK;
}

INLINE okl4_kdb_res_t
L4_KDB_TracepointCountReset(void)
{
    L4_KDB_TCCtrl(1);
    return OKL4_KDB_RES_OK;
}

INLINE okl4_kdb_res_t
L4_KDB_PrintChar(char c)
{
    L4_KDB_PrintChar_ASM(c);
    return OKL4_KDB_RES_OK;
}

#if defined(CONFIG_DEBUG)
INLINE void
L4_KDB_Breakin_Disable(void)
{
    L4_KDB_Set_Breakin_ASM(0);
}

INLINE void
L4_KDB_Breakin_Enable(void)
{
    L4_KDB_Set_Breakin_ASM(1);
}
#endif

INLINE okl4_kdb_res_t
L4_KDB_ReadChar(char *val)
{
    *val = (char)L4_KDB_ReadChar_ASM();
    return OKL4_KDB_RES_OK;
}

INLINE okl4_kdb_res_t
L4_KDB_ReadChar_Blocked(char *val)
{
    *val = (char)L4_KDB_ReadChar_Blocked_ASM();
    return OKL4_KDB_RES_OK;
}

INLINE okl4_kdb_res_t
L4_KDB_SetObjectName(word_t type, word_t id, const char *name)
{
    char buffer[L4_KDB_MAX_DEBUG_NAME_LENGTH];
    word_t i;

    /* Copy the name to our buffer. */
    for (i = 0; i < L4_KDB_MAX_DEBUG_NAME_LENGTH; i++) {
        buffer[i] = name[i];
        if (name[i] == '\0') {
            break;
        }
    }

    /* Perform the call. */
    L4_KDB_SetObjectName_ASM(type, id, buffer);
    return OKL4_KDB_RES_OK;
}

INLINE okl4_kdb_res_t
L4_KDB_GetTracepointName(word_t tp, char *dest)
{
    char *src = (char *)&(L4_GetUtcbBase())[__L4_TCR_MR_OFFSET];

    L4_KDB_GetTracepointNameIntroMRs(tp);

    /*
     * Ewww! Another reimplementation of strcpy!
     */
    while (*src)
        *dest++ = *src++;
    *dest = '\0';

    return OKL4_KDB_RES_OK;
}

//XXX: CONFIDENTIAL START
void L4_KDB_PPC_Enable(void);
void L4_KDB_PPC_Disable(void);
void L4_KDB_PPC_Config(word_t cit);
void L4_KDB_PPC_SetDirectMode(void);
void L4_KDB_PPC_SetTriggerMode(void);
void L4_KDB_PPC_Start(void);
void L4_KDB_PPC_Stop(void);
void L4_KDB_PPC_Restart(void);
void L4_KDB_PPC_Reset(void);
void L4_KDB_PPC_ExpandCounter(void);
void L4_KDB_PPC_GetCounter_ASM(L4_Word32_t* hi, L4_Word32_t* lo);

INLINE L4_Word64_t
L4_KDB_PPC_GetCounter(void)
{
    L4_Word32_t hi, lo;
    L4_Word64_t c;

    L4_KDB_PPC_GetCounter_ASM(&hi, &lo);

    c = (L4_Word64_t)hi;
    c <<= 32;
    c |= lo;

    return c;
}

#endif /* OKL4_ARCH_SH_L4_KDEBUG_H */
