# $Id$

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
    timer_driver = "sh7780_timer"
    serial_driver = "sh7780_scif"
    drivers = [timer_driver, serial_driver]
    memory_serial = [Region(0xFFE00000, 0xFFE10000, "all", "uncached")]
    interrupt_serial = [0x700, 0x720, 0x740, 0x760]
    v2_drivers = [
            ("test_device", "vtest", [], [1,2]),
            ("sh7780_scif", "vserial", memory_serial, interrupt_serial)
            ]

class sh2007(sh7780):
    virtual = False
    subplatform = "sh2007"
    # Necessary for building drivers
    device_core = "sh2007"
    # Define memory layout based on the configuration of sh2007
    memory = sh4a.memory.copy()
    memory['physical'] = [Region(0x88000000, 0x90000000)]
    memory['rom'] = [Region(0xA0000000, 0xA8000000)]
    cpp_defines = sh7780.cpp_defines + [("PLATFORM_SH7780", 1)]

