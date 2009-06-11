/**
 *  @brief  TLB utility
 *  @since  March 2009
 *  @author Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 */

#include <arch/tlb.h>
#include <linear_ptab.h>
#include <tcb.h>

#define TLB_LRU

#ifdef TLB_LRU
// LRU list of UTLB entries
static word_t utlb_entry[UTLB_SIZE];
#endif // TLB_LRU

void
utlb_init()
{
#ifdef TLB_LRU
    for (word_t i = 0; i < UTLB_SIZE; i++) {
        utlb_entry[i] = i;
    }
#else
    word_t  reg;
    reg = mapped_reg_read(REG_MMUCR);
    reg &= ~REG_MMUCR_URC_MASK;
    reg |= (UTLB_LAST << 18) & REG_MMUCR_URB_MASK;
    mapped_reg_write(REG_MMUCR, reg);
#endif // TLB_LRU
}

#ifdef TLB_LRU
static inline word_t
utlb_get_last()
{
    word_t last = utlb_entry[UTLB_LAST];

    for (word_t i = UTLB_LAST; i > UTLB_FIRST; i--) {
        utlb_entry[i] = utlb_entry[i - 1];
    }
    utlb_entry[UTLB_FIRST] = last;
    return last;
}

static inline void
utlb_sort(word_t index)
{
    word_t entry = utlb_entry[index];
    for (word_t i = index; i > UTLB_FIRST; i--) {
        utlb_entry[i] = utlb_entry[i - 1];
    }
    utlb_entry[UTLB_FIRST] = entry;
}
#endif // TLB_LRU

void
fill_tlb(addr_t vaddr, space_t* space, pgent_t* pg, pgent_t::pgsize_e pgsize)
{
    word_t      addr;
#ifdef TLB_LRU
    word_t      reg;

    reg  = mapped_reg_read(REG_MMUCR);
    // Clear the URC field
    reg &= ~(REG_MMUCR_URC_MASK);
    reg |= (utlb_get_last() << 10) & REG_MMUCR_URC_MASK;
    mapped_reg_write(REG_MMUCR, reg);
#endif // TLB_LRU

    addr = (word_t)vaddr;
    addr = (addr >> hw_pgshifts[pgsize]) << hw_pgshifts[pgsize];

    //printf("  fill %p:%x\n", vaddr, pg->ptel(pgsize));
    mapped_reg_write(REG_PTEH,
                 (addr & REG_PTEH_VPN_MASK) |
                 ((word_t)space->get_asid()->get(space) & REG_PTEH_ASID_MASK));
    // Add dirty bit to avoid the first write exception
    //mapped_reg_write(REG_PTEL, pg->ptel(pgsize) | (1 << 2));
    mapped_reg_write(REG_PTEL, pg->ptel(pgsize));

    __asm__ __volatile__ ("ldtlb");

    UPDATE_REG();
}

void
refill_tlb(addr_t vaddr, space_t* space)
{
    // To read the mapped UTLB
    //ENTER_P2();
    word_t      addr;
    hw_asid_t   asid;
    pgent_t*    pg;
    pgent_t::pgsize_e   pgsize;

    addr = (word_t)vaddr;
    asid = space->asid;

    //printf("  refill %p@%u\n", vaddr, asid);
    // Compare ASID and address
    for (word_t i = UTLB_FIRST; i < UTLB_LAST; i++) {
        word_t      compare;
        word_t      utlb_addr;
        word_t      utlb_data;
        word_t      utlb_vpn;
        word_t      utlb_sz;
        hw_asid_t   utlb_asid;

#ifdef TLB_LRU
        utlb_addr = mapped_reg_read(REG_UTLB_ADDRESS | (utlb_entry[i] << 8));
        utlb_data = mapped_reg_read(REG_UTLB_DATA | (utlb_entry[i] << 8));
#else
        utlb_addr = mapped_reg_read(REG_UTLB_ADDRESS | (i << 8));
        utlb_data = mapped_reg_read(REG_UTLB_DATA | (i << 8));
#endif // TLB_LRU

        utlb_asid = (hw_asid_t)(utlb_addr & 0x000000FF);
        if (utlb_asid != asid) {
            continue;
        }

        utlb_vpn = utlb_addr & 0xFFFFFC00;
        // Map SH4A page size to pgsize_e
        utlb_sz = ((utlb_data >> 4) & 0x1) + ((utlb_data >> 6) & 0x2) - 1;
        compare = (addr >> hw_pgshifts[utlb_sz]) << hw_pgshifts[utlb_sz];

        if (utlb_vpn == compare) {
            //ENTER_P1();
#ifdef TLB_LRU
            utlb_sort(i);
#endif // TLB_LRU
            return;
        }
    }
    //ENTER_P1();

    if (!space->lookup_mapping(vaddr, &pg, &pgsize)) {
        tcb_t*          current;
        continuation_t  cont;

        current = get_current_tcb();
        TCB_SYSDATA_USER_ACCESS(current)->fault_address = vaddr;
        cont = current->user_access_continuation();
        ACTIVATE_CONTINUATION(cont);
    }

    fill_tlb(vaddr, space, pg, pgsize);

    return;
}

void
dirty_tlb(word_t vpn)
{
    static word_t mask[4] = { 0xFFFFFCFF, 0xFFFFF0FF, 0xFFFF00FF, 0xFFF000FF };

    //printf("  dirty %x\n", vpn);
    for (word_t i = UTLB_FIRST; i < UTLB_LAST; i++) {
        word_t  utlb_addr;
        word_t  utlb_data;
        word_t  utlb_sz;

        utlb_addr = mapped_reg_read(REG_UTLB_ADDRESS | (utlb_entry[i] << 8));
        utlb_data = mapped_reg_read(REG_UTLB_DATA | (utlb_entry[i] << 8));
        utlb_sz = ((utlb_data >> 4) & 0x1) + ((utlb_data >> 6) & 0x2);

        // Compare VPN and ASID only
        if ((vpn & mask[utlb_sz]) == (utlb_addr & 0xFFFFFCFF)) {
            ENTER_P2();
            mapped_reg_write(REG_UTLB_ADDRESS | (utlb_entry[i] << 8),
                             utlb_addr | 0x200);
            UPDATE_REG();
            ENTER_P1();
            utlb_sort(i);
            return;
        }
    }
}

void
dump_utlb()
{
    word_t  utlb_addr;
    word_t  utlb_data;

    printf("id    asid   virt            phys\n");
    for (word_t i = 0; i < UTLB_SIZE; i++) {
        utlb_addr = mapped_reg_read(REG_UTLB_ADDRESS | (i << 8));
        utlb_data = mapped_reg_read(REG_UTLB_DATA | (i << 8));
        printf("%x:\t%2x %.8x %c%c\t-- %.8x %c%c%c%c%c szpr:%x\n",
               i,
               utlb_addr & 0x000000FF,
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

