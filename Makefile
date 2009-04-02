
MACHINE=		sh2007
#MACHINE=		rp1
VERBOSE_INIT=	false
VERBOSE_BUILD=	true

COMMAND=		./tools/build.py
OPTIONS=		PYFREEZE=false MACHINE=${MACHINE} VERBOSE_STR=${VERBOSE_BUILD}

ifeq (${VERBOSE_INIT},true)
	OPTIONS+=	VERBOSE_INIT=1
endif


all: hello

test:
	${COMMAND} ${OPTIONS} PROJECT=ktest

hello:
	${COMMAND} ${OPTIONS} PROJECT=examples EXAMPLE=hello

clean:
	rm -vfr build/
	rm -vf *.lds
