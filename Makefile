all: hello

test:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=ktest

hello:
	./tools/build.py PYFREEZE=false VERBOSE_STR=true MACHINE=sh2007 PROJECT=examples EXAMPLE=hello kernel_type=debug

clean:
	rm -vfr build/
