/**
 *  @since  February 2009
 */

#include <arch/exception.h>
#include <tcb.h>


bool
tcb_t::copy_exception_mrs_from_frame(tcb_t *dest)
{
    sh_irq_context_t* RESTRICT  context;
    word_t* RESTRICT            mr;

    context = &arch.context;
    mr = &dest->get_utcb()->mr[0];

    // Create the message.
    /*XXX Fastpath related
    if (!this->resource_bits.have_resource(EXCEPTIONFP)) {
    */
        mr[EXCEPT_IPC_GEN_MR_IP] = PC(context->pc);
        mr[EXCEPT_IPC_GEN_MR_SP] = context->sp;
        mr[EXCEPT_IPC_GEN_MR_FLAGS] = context->sr;
        mr[EXCEPT_IPC_GEN_MR_EXCEPTNO] = arch.exc_num;
        mr[EXCEPT_IPC_GEN_MR_ERRORCODE] = arch.exc_code;
    /*
    } else {
        mr[EXCEPT_IPC_SYS_MR_PC] = PC(context->pc);
        mr[EXCEPT_IPC_SYS_MR_R0] = context->r0;
        mr[EXCEPT_IPC_SYS_MR_R1] = context->r1;
        mr[EXCEPT_IPC_SYS_MR_R2] = context->r2;
        mr[EXCEPT_IPC_SYS_MR_R3] = context->r3;
        mr[EXCEPT_IPC_SYS_MR_R4] = context->r4;
        mr[EXCEPT_IPC_SYS_MR_R5] = context->r5;
        mr[EXCEPT_IPC_SYS_MR_R6] = context->r6;
        mr[EXCEPT_IPC_SYS_MR_R7] = context->r7;
        mr[EXCEPT_IPC_SYS_MR_PR] = context->pr;
        mr[EXCEPT_IPC_SYS_MR_SP] = context->sp;
        mr[EXCEPT_IPC_SYS_MR_SYSCALL] = *(word_t *)(PC(context->pc)-4);

        mr[EXCEPT_IPC_SYS_MR_FLAGS] = context->sr;
    }
    */

    return true;
}

bool
tcb_t::copy_exception_mrs_to_frame(tcb_t *dest)
{
    /*XXX: Fastpath related
    if (!dest->resource_bits.have_resource(EXCEPTIONFP)) {
    */
        dest->set_user_ip( (addr_t)get_mr(EXCEPT_IPC_GEN_MR_IP) );
        dest->set_user_sp( (addr_t)get_mr(EXCEPT_IPC_GEN_MR_SP) );
        dest->set_user_flags( get_mr(EXCEPT_IPC_GEN_MR_FLAGS) );
    /*
    } else {
        sh_irq_context_t* RESTRICT  context;
        word_t* RESTRICT        mr;

        context = &dest->arch.context;
        mr = &this->get_utcb()->mr[0];

        dest->set_user_ip( (addr_t)get_mr(EXCEPT_IPC_SYS_MR_PC) );
        dest->set_user_sp( (addr_t)get_mr(EXCEPT_IPC_SYS_MR_SP) );
        dest->set_user_flags( get_mr(EXCEPT_IPC_SYS_MR_FLAGS) );

        context->r0 = mr[EXCEPT_IPC_SYS_MR_R0];
        context->r1 = mr[EXCEPT_IPC_SYS_MR_R1];
        context->r2 = mr[EXCEPT_IPC_SYS_MR_R2];
        context->r3 = mr[EXCEPT_IPC_SYS_MR_R3];
        context->r4 = mr[EXCEPT_IPC_SYS_MR_R4];
        context->r5 = mr[EXCEPT_IPC_SYS_MR_R5];
        context->r6 = mr[EXCEPT_IPC_SYS_MR_R6];
        context->r7 = mr[EXCEPT_IPC_SYS_MR_R7];
        context->pr = mr[EXCEPT_IPC_SYS_MR_PR];
    }
    */

    return true;
}

void
tcb_t::copy_user_regs(tcb_t *src)
{
    sh_irq_context_t* RESTRICT  to;
    sh_irq_context_t* RESTRICT  from;

    to = &arch.context;
    from = &(src->arch.context);

    to->r0 = from->r0;
    to->r1 = from->r1;
    to->r2 = from->r2;
    to->r3 = from->r3;
    to->r4 = from->r4;
    to->r5 = from->r5;
    to->r6 = from->r6;
    to->r7 = from->r7;
    to->r8 = from->r8;
    to->r9 = from->r9;
    to->r10 = from->r10;
    to->r11 = from->r11;
    to->r12 = from->r12;
    to->pr = from->pr;
    to->sr = from->sr;
}

void
tcb_t::copy_regs_to_mrs(tcb_t *src)
{
    sh_irq_context_t* RESTRICT  from;
    word_t* RESTRICT            mr;

    from = &(src->arch.context);
    mr = &this->get_utcb()->mr[0];

    mr[0] = from->r0;
    mr[1] = from->r1;
    mr[2] = from->r2;
    mr[3] = from->r3;
    mr[4] = from->r4;
    mr[5] = from->r5;
    mr[6] = from->r6;
    mr[7] = from->r7;
    mr[8] = from->r8;
    mr[9] = from->r9;
    mr[10] = from->r10;
    mr[11] = from->r11;
    mr[12] = from->r12;
    mr[13] = from->sp;
    mr[14] = from->pr;
    mr[15] = (word_t)src->get_user_ip();
    mr[16] = from->sr;
}

void
tcb_t::copy_mrs_to_regs(tcb_t *dest)
{
    sh_irq_context_t* RESTRICT  from;
    word_t* RESTRICT            mr;

    from = &(dest->arch.context);
    mr = &this->get_utcb()->mr[0];

    from->r0  = mr[0];
    from->r1  = mr[1];
    from->r2  = mr[2];
    from->r3  = mr[3];
    from->r4  = mr[4];
    from->r5  = mr[5];
    from->r6  = mr[6];
    from->r7  = mr[7];
    from->r8  = mr[8];
    from->r9  = mr[9];
    from->r10 = mr[10];
    from->r11 = mr[11];
    from->r12 = mr[12];
    from->sp  = mr[13];
    from->pr  = mr[14];
    dest->set_user_ip((addr_t)mr[15]);
    dest->set_user_flags(mr[16]);
}

