##
##  @file   platform/sh7780/tools/machines.py
##  @since  December 9, 2008
##

# References
#   http://sh2000.sh-linux.org
#   http://sh-linux.org
#   http://linuxsh.sourceforge.net
#   http://japan.renesas.com

import copy

class sh7780(sh4a):
    platform = "sh7780"
    platform_dir = "sh7780"
    memory = sh4a.memory.copy()
    timer_driver = "sh7780_tmu"
    serial_driver = "sh7780_scif"
    drivers = [timer_driver, serial_driver]
    memory_serial = [Region(0xFF000000, 0xFFF00000, "all", "uncached")]
    interrupt_serial = [0x700, 0x720, 0x740, 0x760, 0xB80, 0xBA0, 0xBC0, 0xBE0]
    memory_timer = [Region(0xFF000000, 0xFFFF0000, "all", "uncached")]
    interrupt_timer = [0x580, 0x5A0, 0x5C0, 0x5E0, 0xE00, 0xE20, 0xE40]
    v2_drivers = [
            ("sh7780_scif", "vserial", memory_serial, interrupt_serial),
            ("sh7780_tmu", "vtimer", memory_timer, interrupt_timer)
            ]

class rp1(sh4a):
    platform = "sh7780"
    platform_dir = "sh7780"
    memory = sh4a.memory.copy()
    timer_driver = "sh7780_tmu"
    serial_driver = "sh7780_scif"
    drivers = [timer_driver, serial_driver]
    memory_serial = [Region(0xFF000000, 0xFFF00000, "all", "uncached")]
    interrupt_serial = [0x700, 0x720, 0x740, 0x760, 0xB80, 0xBA0, 0xBC0, 0xBE0]
    memory_timer = [Region(0xFF000000, 0xFFFF0000, "all", "uncached")]
    interrupt_timer = [0x580, 0x5A0, 0x5C0, 0x5E0, 0xE00, 0xE20, 0xE40]
    v2_drivers = [
            ("sh7780_scif", "vserial", memory_serial, interrupt_serial),
            ("sh7780_tmu", "vtimer", memory_timer, interrupt_timer)
            ]

class sh2007(sh7780):
    virtual = False
    subplatform = "sh2007"
    # Necessary for building device drivers
    device_core = "sh2007"
    # Define memory layout based on the configuration of sh2007
    memory = sh4a.memory.copy()
    #XXX:   Temporal fix of physical base.  Need to find the way to specify
    #       the IPL offset.
    memory['physical'] = [Region(0x08210000, 0x10000000)]
    memory['rom'] = [Region(0x00000000, 0x08000000)]
    cpp_defines = sh7780.cpp_defines + [("PLATFORM_SH7780", 1)]

class msrp1(rp1):
    virtual = False
    subplatform = "rp1"
    # Necessary for building device drivers
    device_core = "sh2007"
    # Define memory layout based on the configuration of sh2007
    memory = sh4a.memory.copy()
    #XXX:   Temporal fix of physical base.  Need to find the way to specify
    #       the IPL offset.
    memory['physical'] = [Region(0x08210000, 0x10000000)]
    memory['rom'] = [Region(0x00000000, 0x08000000)]
    cpp_defines = sh7780.cpp_defines + [("PLATFORM_SH7780", 1)] + [("RP1", 1)]

