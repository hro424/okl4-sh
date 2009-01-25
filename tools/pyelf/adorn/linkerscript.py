##############################################################################
# Copyright (c) 2007 Open Kernel Labs, Inc. (Copyright Holder).
# All rights reserved.
# 
# 1. Redistribution and use of OKL4 (Software) in source and binary
# forms, with or without modification, are permitted provided that the
# following conditions are met:
# 
#     (a) Redistributions of source code must retain this clause 1
#         (including paragraphs (a), (b) and (c)), clause 2 and clause 3
#         (Licence Terms) and the above copyright notice.
# 
#     (b) Redistributions in binary form must reproduce the above
#         copyright notice and the Licence Terms in the documentation and/or
#         other materials provided with the distribution.
# 
#     (c) Redistributions in any form must be accompanied by information on
#         how to obtain complete source code for:
#        (i) the Software; and
#        (ii) all accompanying software that uses (or is intended to
#        use) the Software whether directly or indirectly.  Such source
#        code must:
#        (iii) either be included in the distribution or be available
#        for no more than the cost of distribution plus a nominal fee;
#        and
#        (iv) be licensed by each relevant holder of copyright under
#        either the Licence Terms (with an appropriate copyright notice)
#        or the terms of a licence which is approved by the Open Source
#        Initative.  For an executable file, "complete source code"
#        means the source code for all modules it contains and includes
#        associated build and other files reasonably required to produce
#        the executable.
# 
# 2. THIS SOFTWARE IS PROVIDED ``AS IS'' AND, TO THE EXTENT PERMITTED BY
# LAW, ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED.  WHERE ANY WARRANTY IS
# IMPLIED AND IS PREVENTED BY LAW FROM BEING DISCLAIMED THEN TO THE
# EXTENT PERMISSIBLE BY LAW: (A) THE WARRANTY IS READ DOWN IN FAVOUR OF
# THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
# PARTICIPANT) AND (B) ANY LIMITATIONS PERMITTED BY LAW (INCLUDING AS TO
# THE EXTENT OF THE WARRANTY AND THE REMEDIES AVAILABLE IN THE EVENT OF
# BREACH) ARE DEEMED PART OF THIS LICENCE IN A FORM MOST FAVOURABLE TO
# THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
# PARTICIPANT). IN THE LICENCE TERMS, "PARTICIPANT" INCLUDES EVERY
# PERSON WHO HAS CONTRIBUTED TO THE SOFTWARE OR WHO HAS BEEN INVOLVED IN
# THE DISTRIBUTION OR DISSEMINATION OF THE SOFTWARE.
# 
# 3. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY OTHER PARTICIPANT BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""
This file finds and parses linker scripts
"""

import os, sys
import tempfile
from parsers.gnu.gnu_parser import extract_section_names, extract_segment_names

def get_linker_script_text(linker_name, scripts, additional_scripts,
                           linker_args):
    script = ""
    if scripts == []:
        # get the default inker script
        cmdline = linker_name + " --verbose"
        for arg in linker_args:
            cmdline = cmdline + " " +  arg
        if linker_name == "rvct":
            sys.stderr.write("not getting built in linker script for rvct\n")
            scriptlines = []
        else:
            scriptlines = os.popen(cmdline).readlines()
        in_script = False
        for line in scriptlines:
            if line.startswith("==="):
                in_script = not in_script
            elif in_script:
                script += line

    else:
        for s in scripts:
            script += open(s, "r").read()

    if additional_scripts:
        print "WARNING: additional script reading not yet implemented"
#        print "\tThe following files were not parsed as linker scripts:"
#        for s in additional_scripts:
#            print"\t\t" + s
    return script

def linker_script_sections(script):
    file_ = tempfile.TemporaryFile()
    file_.write(script)
    file_.seek(0)
    return extract_section_names(file_)
    
def linker_script_segments(script):
    file_ = tempfile.TemporaryFile()
    file_.write(script)
    file_.seek(0)
    return extract_segment_names(file_)
    
def write_linker_script(sections, filename):
    script = open(filename, "w")
    if len(sections) > 0:
        script.write("PHDRS\n{\n")
        for sect in sections:
            script.write("\t%s PT_LOAD;\n" % sect)
        script.write("}\n\nSECTIONS\n{\n")
        script.write("\t_adorn_saved_dot = .;\n")
        script.write("\t_adorn_sections = _end;\n")
        for sect in sections:
            script.write("\t. = _adorn_sections;\n\t. = ALIGN(0x1000);\n")
            script.write("\t%s . : {*(%s)} : %s\n" % (sect, sect, sect))
            script.write("\t_adorn_sections = .;\n")
        script.write("\t. = _adorn_saved_dot;\n")
        script.write("}\n")
    script.close()
