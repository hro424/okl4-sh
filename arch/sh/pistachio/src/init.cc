/* $Id$ */

/**
 * @since   February 2009
 */

#include <l4.h>
#include <debug.h>
#include <config.h>
#include <interrupt.h>
#include <schedule.h>
#include <space.h>
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
    word_t ver;
    word_t rev;

    ver = mapped_reg_read(REG_PVR);
    ver &= 0xFFFFFF00;

    TRACE_INIT("Processor Id => %lx: ", ver);
    switch (ver >> 24) {
        case 0x10: TRACE_INIT("SH-4A"); break;
        default: TRACE_INIT("UNKNOWN"); break;
    }

    rev = mapped_reg_read(REG_PRR);
    rev &= 0x0000FFF0;
    TRACE_INIT(", rev %lx", rev);
    TRACE_INIT("\n");
}

INLINE void SECTION(".init")
init_globals(word_t* base_phys)
{
    extern char __kernel_space_object[];

    get_globals()->phys_addr_ram = *base_phys;
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
         i < (word_t)VIRT_ADDR_BASE + KERNEL_AREA_SIZE;
         i += PAGE_SIZE_1M) {
        word_t phys = virt_to_phys(i);
        add_mapping_init(pdir_phys, (addr_t)i, (addr_t)phys, writeback);
        /*
        add_mapping_init(pdir_phys,
                         (addr_t)phys_to_page_table_init(phys, base_phys),
                         (addr_t)phys, writethrough);
                         */
    }
}

INLINE void SECTION(".init")
activate_mmu()
{
    mapped_reg_write(REG_MMUCR, REG_MMUCR_SQMD | REG_MMUCR_TI | REG_MMUCR_AT);
    UPDATE_REG();

    /*TODO: Set ASID */
}

extern "C" void NORETURN SECTION(".init")
startup_system()
{
    /*
     * Initialize the platform, map the console and other memory.
     */
    soc_init();

    /*
     * Initialize the L4 console
     */
    init_console();
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
 * Initializes the kernel page table and maps the kernel and data areas.
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

    /*
     * Initialize global pointers
     */
    init_globals((word_t*)PHYS_KERNEL_BASE);

    run_init_script(INIT_PHASE_FIRST_HEAP);

    /*
     * Initialize the kernel space data structure and link it to the pagetable
     */
    space_t::set_kernel_page_directory((pgent_t*)_kernel_space_pagetable);
    kspace = get_kernel_space();
    kresource = get_current_kmem_resource();

    jump_to((word_t)startup_system);
}

