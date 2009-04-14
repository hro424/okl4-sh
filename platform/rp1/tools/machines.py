##
##  @file   platform/rp1/tools/machines.py
##  @since  December 9, 2008
##

# References
#   http://sh2000.sh-linux.org
#   http://sh-linux.org
#   http://linuxsh.sourceforge.net
#   http://japan.renesas.com

import copy

class rp1(sh4a):
    platform = "rp1"
    platform_dir = "rp1"
    memory = sh4a.memory.copy()
    cpp_defines = sh4a.cpp_defines + [("PLATFORM_RP1", 1)]
    timer_driver = "rp1_tmu"
    serial_driver = "rp1_scif"
    drivers = [timer_driver, serial_driver]
    memory_serial = [Region(0xFF000000, 0xFFF00000, "all", "uncached")]
    interrupt_serial = [0x700, 0x720, 0x740, 0x760, 0xB80, 0xBA0, 0xBC0, 0xBE0]
    memory_timer = [Region(0xFF000000, 0xFFFF0000, "all", "uncached")]
    interrupt_timer = [0x580, 0x5A0, 0x5C0, 0x5E0, 0xE00, 0xE20, 0xE40]
    v2_drivers = [
            ("rp1_scif", "vserial", memory_serial, interrupt_serial),
            ("rp1_tmu", "vtimer", memory_timer, interrupt_timer)
            ]

class msrp1(rp1):
    virtual = False
    subplatform = "rp1"
    # Necessary for building device drivers
    device_core = "msrp1"
    # Define memory layout based on the configuration of sh2007
    memory = sh4a.memory.copy()
    #XXX:   Temporal fix of physical base.  Need to find the way to specify
    #       the IPL offset.
    memory['physical'] = [Region(0x08210000, 0x10000000)]
    memory['rom'] = [Region(0x00000000, 0x08000000)]
