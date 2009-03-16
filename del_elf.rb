#!/usr/bin/ruby

##
## Define parameter
##
EXT = ".noelf"
ELF_SIZE = 0x0010000

##
## Program
##

# check arguments
if ARGV.length != 1 then
    print "usage: del_elf.rb [filename]\n"
    exit(1)
end

# open a file and check whether it can be opened
filename = ARGV.shift
f = open(filename,"r")
if f == nil then
    print "Can't open #{filename}\n"
    exit(1)
end

# get a filename removed an extension string.
basename = File::basename(filename, ".*")

# get data without elf-header
f.read(ELF_SIZE)
buf = f.read
f.close

# create a file without elf-header
new_file = basename+EXT
boot_img = open(new_file,"w")
if boot_img == nil
    print "Can't create file: #{new_file}\n"
    exit(1)
end
boot_img.write(buf)
boot_img.close
