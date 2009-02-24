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
    virtual = False
    platform = "sh7780"
    platform_dir = "sh7780"
    timer_driver = ""
    serial_driver = ""
    drivers = [timer_driver, serial_driver] + sh4a.drivers

    # Define memory layout based on the configuration of sh7780
    memory = sh4a.memory.copy()
    memory['physical'] = [Region(0x88000000, 0x90000000)]
    memory['rom'] = [Region(0xA0000000, 0xA8000000)]
    cpp_defines = sh4a.cpp_defines = [("PLATFORM_SH7780", 1)]

