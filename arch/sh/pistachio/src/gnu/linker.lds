/* $Id$ */

/**
 *  @since  December 2008
 */

#include <l4.h>

ENTRY(_start)

MEMORY
{
    ram(RWX) : ORIGIN = VIRT_ADDR_RAM, LENGTH = 128M
}

#define rom     ram

SECTIONS
{
    . = VIRT_ADDR_RAM;
    .text :
    {
        . = ALIGN(64K);
        *(.init.head)
        *(.init.start)
        *(.base)
        *(.text.startup)
        *(.text)
        *(.gnu.linkonce.*)
    } > rom

    .rodata . :
    {
        *(.rodata)
        *(.rodata.*)
    } > rom

    .kdebug . :
    {
        *(.kdebug)
    } > rom

    _end_rom = .;

    _start_init = .;

    .init . :
    {
        *(.init)
        *(.init.*)
    } > rom

    .roinit . :
    {
        *(.roinit)
    } > rom

    .elfweaver_info :
    {
        *(.elfweaver_info)
    } > rom

    _end_init = .;

    .data :
    {
        _start_ram = .;
        _start_data = .;

        *(.data.cpulocal);
        *(.data.cpulocal.stack)
        *(.data.cpulocal.utcb)
        *(.data.cpulocal.tcb)
        *(.sdata)
        *(.data)
    } > ram

    .kdebug-data . :
    {
        *(.data.kdebug)
        *(.kdebug.data)
    } > ram

    .bss . :
    {
        . = ALIGN(16);
        _start_setlist = .;
        *(.setlist)
        _end_setlist = .;
        . = ALIGN(16);
        _start_sets = .;
        *(SORT(set_*))
        _end_sets = .;
        . = ALIGN(32);
        *(.bss)
        *(.kdebug-bss)
    } > ram
        
    . = ALIGN(16K);
    .kspace . :
    {
        *(data.kspace)
    } > ram

    . = ALIGN(4K);
    .traps . :
    {
        *(.data.traps)
    } > ram

    .utcb_page . :
    {
        *(.data.utcb)
    } > ram

    . = ALIGN(4K);
    _end_ram = .;

    . = ALIGN(4K);
    _end = .;

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.note)
        *(.comment)
        *(.delete)
    }
}

