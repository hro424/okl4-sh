

all:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh7780 PROJECT=ktest

clean:
	rm -vfr build/
