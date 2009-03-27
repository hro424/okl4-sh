all: hello

test:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=ktest
	make img

hello:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=examples EXAMPLE=hello
	make img

img:
	./update_cs
	cp build/images/image.boot .
	sh-linux-objcopy -O binary image.boot
	#cp image.boot /nfsroot/okl4/bootimage.bin

clean:
	rm -vfr build/
	rm -vf image.boot
	rm -vf *.lds
	rm -vf *cscope*
