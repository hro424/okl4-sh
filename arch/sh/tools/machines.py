# $Id$

##
##  @file   arch/sh/tools/machines.py
##  @since  December 9, 2008
##

from machines import Machine, Region
#from simulators import skyeye_sim
from toolchains import gnu_sh_toolchain

# SuperH Family
class sh(Machine):
    page_sizes = [0x400, 0x1000, 0x10000, 0x100000]
    preferred_alignment = 0x10000L
    wordsize = 32
    arch = "sh"
    endian = "little"
    default_method = ''
    default_toolchain = gnu_sh_toolchain

# SH-4 (SH7750 Series)
class sh4(sh):
    memory = sh.memory.copy()
    memory['virtual'] = [Region(0x1000, 0x7ef00000)]
    arch_version = 4

# SH-4A (SH7780 Series)
class sh4a(sh):
    memory = sh.memory.copy();
    memory['virtual'] = [Region(0x1000, 0x7ef00000)]
    arch_version = 4

