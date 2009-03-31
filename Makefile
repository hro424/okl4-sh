
COMMAND=	./tools/build.py
OPTIONS=	PYFREEZE=false MACHINE=sh2007

all: hello

test:
	${COMMAND} ${OPTIONS} VERBOSE_STR=true PROJECT=ktest
	make img

hello:
	${COMMAND} ${OPTIONS} VERBOSE_STR=true PROJECT=examples EXAMPLE=hello
	make img

rp1:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=msrp1 PROJECT=examples EXAMPLE=hello
	make img

rp1-test:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=msrp1 PROJECT=ktest
	make img

img:
	cp build/images/image.boot .
	sh-linux-objcopy -O binary image.boot
	cp image.boot /nfsroot/okl4/bootimage.bin

clean:
	rm -vfr build/
	rm -vf image.boot
	rm -vf *.lds
	rm -vf *cscope*
