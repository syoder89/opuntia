TARGET=stats
VERSION=`cat version`
OBJECTS=main.o summary.o detail.o csu.o info.o misc.o
HEADERS=include/interface.h include/sdl_stats.h
LIBS=-lStats -ltextbox -lpanel -lncurses

OPTIONS=-O3 -L../lib -L../libtextbox -L/usr/local/lib
#OPTIONS=-O3 -L../lib -L../libtextbox -L/usr/local/lib -msoft-float -mips16 -minterlink-mips16 -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro
# Uncomment this line for debugging information
#OPTIONS += -D __DEBUG__ -g
INCLUDES=-I../include/ -I/usr/local/include/sand/ -I../libtextbox
LIBRARY=../lib/libStats.a
LIBTEXTBOX=../libtextbox/libtextbox.a

.SUFFIXES: .cc .o

.c.o: ${HEADERS}
	${CC} ${OPTIONS} ${INCLUDES} -c ${C_FLAGS} $<

all: ${OBJECTS} ${TARGET} ${LIBRARY} ${LIBTEXTBOX}

${LIBRARY}:
	make -C ../lib

lib_clean:
	make -C ../lib clean

${LIBTEXTBOX}:
	make -C ../libtextbox

libtextbox_clean:
	make -C ../libtextbox clean

${TARGET}:${OBJECTS} ${LIBRARY} ${LIBTEXTBOX}
	${CC} ${OBJECTS} ${LIBS} ${INCLUDE} ${OPTIONS} -o ${TARGET}
	make strip

strip:
	$(STRIP) ${TARGET}

clean: libtextbox_clean lib_clean
	rm -f ${OBJECTS} ${CORE} ${TARGET}
