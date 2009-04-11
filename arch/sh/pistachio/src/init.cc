/**
 * @brief   Collection of kernel initialization routines
 * @since   February 2009
 * @author  Hiroo Ishikawa <hiroo.ishikawa@gmail.com>
 */

#include <l4.h>
#include <debug.h>
#include <config.h>
#include <interrupt.h>
#include <schedule.h>
#include <space.h>
#include <arch/tlb.h>
#include <soc/soc.h>

#if defined(__GNUC__)

#define jump_to(address)                                                \
    do {                                                                \
        __asm__ __volatile__ (                                          \
            "    jmp     @%0     \n"                                    \
            "    nop             \n"                                    \
            :                                                           \
            : "r" (address)                                             \
            : "memory"                                                  \
        );                                                              \
    } while (1)

#else
#error Unknown compiler
#endif


NORETURN void SECTION(".init") generic_init();


INLINE void SECTION(".init")
show_processor_info()
{
    word_t val;
    word_t rev;

    val = mapped_reg_read(REG_PVR);
    val &= 0xFFFFFF00;

    TRACE_INIT("Processor Id => %lx: ", val);
    switch (val >> 24) {
        case 0x10: TRACE_INIT("SH-4A"); break;
        default: TRACE_INIT("UNKNOWN"); break;
    }

    rev = mapped_reg_read(REG_PRR);
    rev &= 0x0000FFF0;
    TRACE_INIT(", rev %lx", rev);
    TRACE_INIT("\n");

    val = mapped_reg_read(REG_CPG_FRQCR);
    switch ((val >> 20) & 0x1F) {
        case 0x2:
            TRACE_INIT("Ick x12, SHck x6, ");
            break;
        case 0x4:
            TRACE_INIT("Ick x12, SHck x4, ");
            break;
        default:
            TRACE_INIT("Ick N/A, SHck N/A, ");
            break;
    }

    switch ((val >> 16) & 0x0F) {
        case 0x3:
            TRACE_INIT("Bck x3, ");
            break;
        case 0x4:
            TRACE_INIT("Bck x2, ");
            break;
        case 0x5:
            TRACE_INIT("Bck x3/2, ");
            break;
        case 0x6:
            TRACE_INIT("Bck x1, ");
            break;
        default:
            TRACE_INIT("Bck N/A, ");
            break;
    }

    switch (val & 0xF) {
        case 0x5:
            TRACE_INIT("Pck x3/2\n");
            break;
        case 0x6:
            TRACE_INIT("Pck x3/2\n");
            break;
        default:
            TRACE_INIT("Pck N/A\n");
            break;
    }
}

INLINE void SECTION(".init")
init_globals(word_t base_phys)
{
    extern char __kernel_space_object[];

    get_globals()->phys_addr_ram = (word_t)base_phys;
    get_globals()->kernel_space = (space_t*)&__kernel_space_object;
}

/**
 * Obtains a 1M page table index corresponding to the virtual address.
 *
 * @param vaddr     the virtual address
 * @return          the page table index
 */
INLINE word_t SECTION(".init")
page_table_1m(addr_t vaddr)
{
    return ((word_t)vaddr >> 20) & (SH_L1_SIZE - 1);
}

/**
 * Adds the mapping information to the page table.  This function works with
 * MMU disabled.
 *
 * @param pdir      the page table
 * @param vaddr     the virtual address
 * @param paddr     the physical address
 * @param attrib    the attribute of the address
 */
INLINE void SECTION(".init")
add_mapping_init(pgent_t* pdir, addr_t vaddr, addr_t paddr, memattrib_e attrib)
{
    pgent_t*    pg = pdir + page_table_1m(vaddr);

    pg->set_entry_1m(NULL, paddr, true, true, true, attrib);
}

/**
 * Initializes the kernel page table and maps the kernel and data areas.
 *
 * @param pdir_phys     the physical address of the kernel page table
 * @param base_phys     the physical base address of the kernel address space
 */
INLINE void SECTION(".init")
init_pagetable(pgent_t* pdir_phys, word_t* base_phys)
{
    for (int i = 0; i < (int)(SH_L1_SIZE / sizeof(word_t)); i++) {
        ((word_t*)pdir_phys)[i] = 0;
    }

    for (word_t i = (word_t)VIRT_ADDR_BASE;
         i < (word_t)KERNEL_AREA_END;
         i += PAGE_SIZE_1M) {
        word_t phys = virt_to_phys(i);
        add_mapping_init(pdir_phys, (addr_t)i, (addr_t)phys, writeback);
    }
}

INLINE void SECTION(".init")
activate_mmu()
{
    mapped_reg_write(REG_MMUCR, REG_MMUCR_TI | REG_MMUCR_AT);
    // UTLB cleanup
    for (word_t i = 0; i < 64; i++) {
        mapped_reg_write(REG_MMUCR, REG_MMUCR_AT | (i << 10));
        mapped_reg_write(REG_PTEH, 0);
        mapped_reg_write(REG_PTEL, 0);
        __asm__ __volatile__ ("ldtlb");
    }
    UPDATE_REG();
}

extern unsigned char utcb_ref_page[];

INLINE void SECTION(".init")
init_tlb()
{
    word_t  ptel;

    ptel = ((word_t)utcb_ref_page & REG_PTEL_PPN_MASK) | REG_PTEL_V |
        REG_PTEL_PR1 | REG_PTEL_PR0 | REG_PTEL_SZ0 |
        REG_PTEL_D | REG_PTEL_SH;

    mapped_reg_write(REG_MMUCR, REG_MMUCR_AT | UTLB_UTCB << 10);
    mapped_reg_write(REG_PTEH, USER_UTCB_REF & REG_PTEH_VPN_MASK);
    mapped_reg_write(REG_PTEL, ptel);

    __asm__ __volatile__ ("ldtlb");

    UPDATE_REG();
}

extern "C" void NORETURN SECTION(".init")
startup_system()
{
    /*
     * Initialize the platform, map the console and other memory.
     */
    soc_init();

    init_hello();

    get_asid_cache()->init();
    get_asid_cache()->set_valid(1, CONFIG_MAX_NUM_ASIDS - 1);

    /*
     * Initialize the tracebuffer
     */
    init_tracebuffer();

    /*
     * Initialize TCB memory allocator
     */
    init_tcb_allocator();
    
    /*
     * Setup the kernel address space
     */
    init_kernel_space();

    TRACE_INIT("Initializing kernel debugger...\n");

    /*
     * Initialize kernel debugger if any
     */
    if (kdebug_entries.kdebug_init) {
        kdebug_entries.kdebug_init();
    }
    else {
        TRACE_INIT("No kernel debugger!\n");
    }

    /*
     * Configure IRQ hardware
     */
    TRACE_INIT("Initializing interrupts...\n");
    init_sh_interrupts();

    init_idle_tcb();
    get_globals()->current_tcb = get_idle_tcb();

    show_processor_info();

    /*
     * Architecture independent initialization.
     */
    generic_init();

    NOTREACHED();
}

/**
 * Initializes the kernel page table and maps the kernel text and data areas.
 *
 * @param base      the base physical address of the kernel address space
 */
extern "C" void NORETURN SECTION(".init")
init_memory()
{
    extern char         _kernel_space_pagetable[];
    pgent_t*            kspace_phys;
    space_t*            kspace;
    kmem_resource_t*    kresource;

    kspace_phys = (pgent_t*)virt_to_phys(_kernel_space_pagetable);
    init_pagetable(kspace_phys, (word_t*)PHYS_KERNEL_BASE);

    /*
     * Enable virtual memory, caching, etc
     */
    activate_mmu();
    init_tlb();

    /*
     * Initialize global pointers
     */
    init_globals(PHYS_KERNEL_BASE);

    /*
     * Initialize the L4 console
     */
    init_console();

    run_init_script(INIT_PHASE_FIRST_HEAP);

    /*
     * Initialize the kernel space data structure and link it to the pagetable
     */
    space_t::set_kernel_page_directory((pgent_t*)_kernel_space_pagetable);
    kspace = get_kernel_space();
    kresource = get_current_kmem_resource();

    jump_to((word_t)startup_system);
}

