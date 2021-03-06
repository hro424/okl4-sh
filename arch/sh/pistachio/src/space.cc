/**
 *  @brief  Architecture-specific address space implementation
 *  @since  February 2009
 *  @author Hiroo Ishikawa <ishikawa@dcl.info.waseda.ac.jp>
 */

#include <l4.h>
#include <space.h>
#include <tcb.h>
#include <linear_ptab.h>
#include <kernel/bitmap.h>
#include <kernel/generic/lib.h>
#include <arch/tlb.h>

asid_cache_t asid_cache UNIT("cpulocal");

/* The kernel space is statically defined beause it is needed
 * before the virtual memory has been setup or the kernel
 * memory allocator.
 */

ALIGNED(SH_L1_SIZE) char UNIT("kspace") _kernel_space_pagetable[SH_L1_SIZE];

ALIGNED(PAGE_SIZE_4K) unsigned char utcb_ref_page[PAGE_SIZE_4K];


void SECTION(".init")
init_kernel_space()
{
    bool        success;
    space_t*    kspace;

    TRACE_INIT("Initializing kernel space @ %p...\n\r", get_kernel_space());

    kspace = get_kernel_space();

    success = kspace->get_asid()->init(kspace);
    ASSERT(DEBUG, success);

    kspace->init_kernel_mappings();

    kspace->enqueue_spaces();

    kspace->pgbase = (word_t)page_table_to_phys(kspace->pgent(0));
}

/**
 * initialize THE kernel space
 * @see get_kernel_space()
 */
void SECTION(".init")
generic_space_t::init_kernel_mappings()
{
    /* Kernel space's mappings already setup in initial root page table */
}

/**
 * initialize a space
 *
 * @param utcb_area     fpage describing location of UTCB area
 */
bool
generic_space_t::init (fpage_t utcb_area, kmem_resource_t *kresource)
{
    word_t i;
    word_t offset = USER_AREA_SECTIONS + UTCB_AREA_SECTIONS;

    asid_t *asid = ((space_t *)this)->get_asid();

    if (!asid->init((space_t *)this)) {
        get_current_tcb()->set_error_code(EINVALID_SPACE);
        return false;
    }

    this->utcb_area = utcb_area;

    pgent_t *pg_to = pgent(offset);
    pgent_t *pg_from = get_kernel_space()->pgent(offset);

    for (i = 0; i < KERNEL_AREA_SECTIONS; i++) {
        *pg_to++ = *pg_from++;
    }
    for (i = 0; i < UNCACHE_AREA_SECTIONS; i++) {
        *pg_to++ = *pg_from++;
    }
    for (i = 0; i < MISC_AREA_SECTIONS; i++) {
        *pg_to++ = *pg_from++;
    }
    for (i = 0; i < VAR_AREA_SECTIONS; i++) {
        *pg_to++ = *pg_from++;
    }

    ((space_t*)this)->pgbase = (word_t)page_table_to_phys(this->pdir);

    return true;
}

/**
 * Clean up a Space
 */
void
generic_space_t::arch_free(kmem_resource_t *kresource)
{
    asid_t *asid = ((space_t *)this)->get_asid();

    asid->release();
}

#define PAGE_COLOR_ALIGN    \
    ((CACHE_SIZE/PAGE_SIZE_4K/CACHE_WAYS - 1UL) << PAGE_BITS_4K)

/**
 * Allocate a UTCB
 *
 * @param tcb   Owner of the utcb
 */
utcb_t*
generic_space_t::allocate_utcb(tcb_t* tcb, kmem_resource_t* kresource)
{
    ASSERT (DEBUG, tcb);

    addr_t  utcb;
    word_t  section;
    pgent_t leaf;
    pgent_t level1;
    bool    is_valid;
    addr_t  page;

    utcb = (addr_t)tcb->get_utcb_location ();

    /* Try lookup the UTCB page for this utcb */
    section = (word_t)utcb >> PAGE_BITS_1M;

    level1 = *this->pgent(section);
    is_valid = true;

    if (EXPECT_TRUE(level1.is_valid(this, pgent_t::size_1m)))
    {
        if (EXPECT_TRUE(level1.is_subtree(this, pgent_t::size_1m))) {
            leaf = *level1.subtree(this, pgent_t::size_1m)->next(
                    this, UTCB_AREA_PGSIZE,
                    ((word_t)utcb & (PAGE_SIZE_1M - 1)) >> UTCB_AREA_PAGEBITS);

            is_valid = leaf.is_valid(this, pgent_t::size_4k);
        }
        else {
            enter_kdebug("1MB page in UTCB area");
            return (utcb_t *)0;
        }
    }
    else {
        is_valid = false;
    }

    if (is_valid)
    {
        addr_t  kaddr;
        utcb_t* retv;

        kaddr = addr_mask(leaf.address(this, UTCB_AREA_PGSIZE),
                          ~page_mask (UTCB_AREA_PGSIZE));
        retv = (utcb_t *)ram_to_virt(
             addr_offset(kaddr, (word_t)utcb & page_mask(UTCB_AREA_PGSIZE)));
        memset(retv, 0, UTCB_SIZE);
        return retv;
    }

    page = kresource->alloc_aligned (kmem_group_utcb,
                                     page_size(UTCB_AREA_PGSIZE),
                                     (word_t)utcb, PAGE_COLOR_ALIGN, true);

    if (page == NULL) {
        return NULL;
    }

    if (! ((space_t *)this)->add_mapping(
                                (addr_t)addr_align(utcb, UTCB_AREA_PAGESIZE),
                                virt_to_phys(page), UTCB_AREA_PGSIZE,
                                space_t::read_write, false, kresource))
    {
        get_current_tcb()->set_error_code(ENO_MEM);
        kresource->free(kmem_group_utcb, page, page_size(UTCB_AREA_PGSIZE));
        return NULL;
    }

    return (utcb_t*)addr_offset(page,
                                addr_mask(utcb, page_mask(UTCB_AREA_PGSIZE)));
}

/** 
 * Free a UTCB in this space
 * @param utcb  The utcb
 */
void
generic_space_t::free_utcb(utcb_t* utcb)
{
    /* Do nothing, because in SH, UTCB isn't allocated by kernel,
     * kernel isn't responsible for removing utcb mapping from space
     * when delting or switching thread from the space,
     * the mapping will only be removed when space is deleted.
     */
}

/**
 * Set up hardware context to run the tcb in this space.
 */
void
generic_space_t::activate(tcb_t *tcb)
{
    word_t              dest_asid;
    word_t              new_pt;

    dest_asid = ((space_t *)this)->get_asid()->get((space_t *)this);
    get_globals()->current_clist = this->get_clist();
    new_pt = ((space_t*)this)->pgbase;

    // Change ASID
    set_hw_asid(dest_asid);
    mapped_reg_write(REG_TTB, new_pt);

    word_t* user_utcb_ref_kernel = (word_t*)(utcb_ref_page + 0xF00);
    *user_utcb_ref_kernel = tcb->get_utcb_location();
    //sh_cache::invalidate_d((addr_t)USER_UTCB_REF, 5);
    sh_cache::writeback_d(user_utcb_ref_kernel, 5);
}

/**
 * Try to copy a mapping from kernel space into the current space
 * @param addr the address for which the mapping should be copied
 * @return true if something was copied, false otherwise.
 * Synchronization must happen at the highest level, allowing sharing.
 */
bool
generic_space_t::sync_kernel_space(addr_t addr)
{
    pgent_t::pgsize_e   size;
    word_t              section;
    pgent_t*            dst_pgent;
    pgent_t*            src_pgent;

    /* We set everything up at initialisation time */
    if (this == get_kernel_space()) return false;

    /* get the 1m entries in the pagetables to compare */
    size = pgent_t::size_1m;
    section = ((word_t)addr) >> page_shift(size);
    dst_pgent = this->pgent(0)->next(this, size, section);
    src_pgent = get_kernel_space()->pgent(0)->next(this, size, section);

    /* (already valid) || (kernel space invalid) */
    if ((dst_pgent->raw & 0x3) || !(src_pgent->raw & 0x3))
    {
        return false;
    }

    *dst_pgent = *src_pgent;
    return true;
}


void
generic_space_t::flush_tlb(space_t *curspace)
{
    //TODO: Get a different ASID
    asid_t *asid = ((space_t *)this)->get_asid();
    if (asid->is_valid()) {
        flush_asid(asid->value());
    }
}

void
generic_space_t::flush_tlbent_local(space_t *curspace, addr_t vaddr,
                                    word_t log2size)
{
    asid_t *asid = ((space_t *)this)->get_asid();

    vaddr = addr_align(vaddr, 1 << log2size);

    if (asid->is_valid()) {
        this->activate(get_current_tcb());

        //TODO: Work-around: Crash when the entry (vaddr) does not exist
        //      in the operand cache. (MAP0500)
        //sh_cache::flush_d(vaddr, log2size);
        //
        // We flush the entire operand cache at this moment.
        sh_cache::flush_d();
        sh_cache::invalidate_tlb_entry(asid->value(), vaddr);

        curspace->activate(get_current_tcb());
    }
}

bool
generic_space_t::allocate_page_directory(kmem_resource_t *kresource)
{
    addr_t addr;

    addr = kresource->alloc(kmem_group_pgtab, SH_L1_SIZE, true);
    if (!addr) {
        return false;
    }

    /* kmem.alloc zeros out the page, in cached memory. Since we'll be using
     * this for uncached accesses, need to flush this out now.
     */
    sh_cache::flush_d(addr, SH_L1_BITS);
    pdir = (pgent_t*)addr;
    return true;
}

void
generic_space_t::free_page_directory(kmem_resource_t *kresource)
{
    kresource->free(kmem_group_pgtab, pdir, SH_L1_SIZE);
    pdir = NULL;
}

#if defined (CONFIG_DEBUG)
/**
 * reads a word from a given physical address, uses a remap window and
 * maps a 1MB page for the access
 *
 * @param vaddr         virtual address (if caches need to be flushed)
 * @param paddr         physical address to read from
 * @return the value at the given address
 */
word_t
generic_space_t::readmem_phys(addr_t vaddr, addr_t paddr)
{
    return *(word_t*)((word_t)paddr + P1_START);
}
#endif /* CONFIG_DEBUG */

/**
 * Add mapping information.
 *
 * @param vaddr         the destination address
 * @param paddr         the source address
 * @param size          the size of mapping
 * @param rwx           the access rights of the mapping
 * @param kernel        whether it's privileged or not
 * @param attrib        the caching policy
 * @param kresource     used when a subtree is created
 */
bool
space_t::add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                     rwx_e rwx, bool kernel, memattrib_e attrib,
                     kmem_resource_t* kresource)
{
    pgent_t::pgsize_e   pgsize = pgent_t::size_max;
    pgent_t*            pg = this->pgent(0);

    pg = pg->next(this, pgsize, page_table_index(pgsize, vaddr));

    /* Lookup mapping */
    for (;;) {
        if (pgsize == size) {
            break;
        }

        if (!pg->is_valid(this, pgsize)) {
            if (!pg->make_subtree(this, pgsize, kernel, kresource)) {
                return false;
            }
        }

        pg = pg->subtree(this, pgsize)->next(this, pgsize - 1,
                                       page_table_index(pgsize - 1, vaddr));
        pgsize--;
    }

    bool r = (word_t)rwx & 4;
    bool w = (word_t)rwx & 2;
    bool x = (word_t)rwx & 1;

    pg->set_entry(this, pgsize, paddr, r, w, x, kernel, attrib);

    return true;
}

