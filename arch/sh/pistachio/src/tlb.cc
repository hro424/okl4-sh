/**
 *  @brief  TLB utility
 *  @since  March 2009
 *  @author Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#include <arch/tlb.h>
#include <linear_ptab.h>

void
fill_tlb(addr_t vaddr, space_t* space, pgent_t* pg, pgent_t::pgsize_e pgsize)
{
    static u8_t entry = 0;
    word_t      tmp;
    word_t      reg;
    word_t      addr;

    reg  = mapped_reg_read(REG_MMUCR);
    // Clear the URC field
    reg &= ~(REG_MMUCR_URC_MASK);
    // TODO: mask it with URB value
    tmp = entry;
    reg |= (tmp << 10) & REG_MMUCR_URC_MASK;
    mapped_reg_write(REG_MMUCR, reg);
    entry++;    // overflow -> go back to 0
    if (entry == UTLB_UTCB) {
        entry = 0;
    }

    addr = (word_t)vaddr;
    addr = (addr >> hw_pgshifts[pgsize]) << hw_pgshifts[pgsize];

    //TRACE_INIT("  fill %p:%x\n", vaddr, pg->ptel(pgsize));
    mapped_reg_write(REG_PTEH,
                 (addr & REG_PTEH_VPN_MASK) |
                 ((word_t)space->get_asid()->get(space) & REG_PTEH_ASID_MASK));
    // Add dirty bit to avoid the initial write exception
    mapped_reg_write(REG_PTEL, pg->ptel(pgsize) | (1 << 2));

    __asm__ __volatile__ ("ldtlb");

    UPDATE_REG();

    //dump_utlb();
    //TODO
    sh_cache::flush();
}

void
fill_tlb(int entry, addr_t vaddr, space_t* space, pgent_t* pg,
         pgent_t::pgsize_e pgsize)
{
    word_t      reg;
    word_t      addr;

    reg  = mapped_reg_read(REG_MMUCR);
    // Clear the URC field
    reg &= ~(REG_MMUCR_URC_MASK);
    reg |= (entry << 10) & REG_MMUCR_URC_MASK;
    mapped_reg_write(REG_MMUCR, reg);

    addr = (word_t)vaddr;
    addr = (addr >> hw_pgshifts[pgsize]) << hw_pgshifts[pgsize];

    //TRACE_INIT("  fill %p:%x\n", vaddr, pg->ptel(pgsize));
    mapped_reg_write(REG_PTEH,
                 (addr & REG_PTEH_VPN_MASK) |
                 ((word_t)space->get_asid()->get(space) & REG_PTEH_ASID_MASK));
    // Add dirty bit to avoid the initial write exception
    mapped_reg_write(REG_PTEL, pg->ptel(pgsize) | (1 << 2));

    __asm__ __volatile__ ("ldtlb");

    UPDATE_REG();

    //dump_utlb();
    //TODO
    sh_cache::flush();
}

void
dump_utlb()
{
    word_t  utlb_addr;
    word_t  utlb_data;

    printf("id    asid   virt            phys\n");
    for (word_t i = 0; i < 64; i++) {
        utlb_addr = mapped_reg_read(0xF6000000 | (i << 8));
        utlb_data = mapped_reg_read(0xF7000000 | (i << 8));
        printf("%x:\t%2x %.8x %c%c\t-- %.8x %c%c%c%c%c szpr:%x\n",
               i,
               utlb_addr & 0xFF,
               utlb_addr & 0xFFFFFC00,
               utlb_addr & (1 << 9) ? 'd' : ' ',
               utlb_addr & (1 << 8) ? 'v' : ' ',
               utlb_data & 0x1FFFFC00,
               utlb_data & (1 << 8) ? 'v' : ' ',
               utlb_data & (1 << 3) ? 'c' : ' ',
               utlb_data & (1 << 2) ? 'd' : ' ',
               utlb_data & (1 << 1) ? 's' : ' ',
               utlb_data & 1 ? 't' : 'w',
               (utlb_data >> 4) & 0xF);
    }
}

