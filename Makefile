all: hello

test:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=ktest
	make img

hello:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=examples EXAMPLE=hello
	make img

img:
	cp build/images/image.boot .
	sh-linux-objcopy -O binary image.boot

clean:
	rm -vfr build/
