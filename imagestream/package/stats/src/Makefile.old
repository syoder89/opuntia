SUBDIRS=library src
TARGET=stats
VERSION=`cat version`

.PHONY : library src clean

all: ${SUBDIRS}

library:
	VERSION="${VERSION}" && make -C lib CC=${CC}

src:
	VERSION="${VERSION}" && make -C src ${TARGET} CC=${CC}

clean:
	make -C lib clean
	make -C src clean
