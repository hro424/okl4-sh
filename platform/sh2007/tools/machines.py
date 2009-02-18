# $Id$

##
##  @file   platform/sh2007/tools/machines.py
##  @since  December 9, 2008
##

# References
#   http://sh2000.sh-linux.org
#   http://sh-linux.org
#   http://linuxsh.sourceforge.net
#   http://japan.renesas.com

import copy

class sh2007(sh7780):
    virtual = False
    platform = "sh2007"
    platform_dir = "sh2007"
    timer_driver = ""
    serial_driver = ""
    drivers = [timer_driver, serial_driver] + sh7780.drivers

    # Define memory layout based on the configuration of sh7780
    memory = sh7780.memory.copy()
    memory['physical'] = [Region(0x88000000, 0x90000000)]
    memory['rom'] = [Region(0xA0000000, 0xA8000000)]
    cpp_defines = sh7780.cpp_defines = [("PLATFORM_SH2007", 1)]

