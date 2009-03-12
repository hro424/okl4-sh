

all:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=ktest

clean:
	rm -vfr build/
