

all:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=examples EXAMPLE=hello

hello:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=examples EXAMPLE=hello

clean:
	rm -vfr build/
