
MACHINE=		sh2007
#MACHINE=		msrp1
VERBOSE_INIT=	false
VERBOSE_BUILD=	false

COMMAND=		./tools/build.py
OPTIONS=		PYFREEZE=false MACHINE=${MACHINE}

ifeq (${VERBOSE_INIT},true)
	OPTIONS+=	VERBOSE_INIT=1
endif

ifeq (${VERBOSE_BUILD},true)
	OPTIONS+=	VERBOSE_STR=1
endif


all: hello

test:
	${COMMAND} ${OPTIONS} PROJECT=ktest

bench:
	${COMMAND} ${OPTIONS} PROJECT=kbench

hello:
	${COMMAND} ${OPTIONS} PROJECT=examples EXAMPLE=hello

clean:
	rm -vfr build/
	rm -vf *.lds
