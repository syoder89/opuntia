CFLAGS=-O2 -L../GobiAPI/GobiConnectionMgmt/ -I../GobiAPI/GobiConnectionMgmt/ -I../GobiAPI/Shared -I../GobiAPI/Core
LDFLAGS=-lGobiConnectionMgmt -lpthread -lrt -liconv

all: man3g 3g_connmgr

common.o: common.cpp
	$(CXX) -c ${CXXFLAGS} ${CFLAGS} -o common.o common.cpp

pdu.o: pdu.cpp
	$(CXX) -c ${CXXFLAGS} ${CFLAGS} -o pdu.o pdu.cpp

man3g: man3g.c common.o pdu.o
	$(CXX) ${CXXFLAGS} ${CFLAGS} -o man3g man3g.c common.o pdu.o ${LDFLAGS} ${LXXFLAGS}

3g_connmgr: 3g_connmgr.c common.o pdu.o
	$(CXX) ${CXXFLAGS} ${CFLAGS} -o 3g_connmgr 3g_connmgr.c common.o pdu.o ${LDFLAGS} ${LXXFLAGS}

gobi-gps: gobi-gps.cpp common.o
	$(CXX) -O2 ${CXXFLAGS} ${CFLAGS} -o gobi-gps gobi-gps.cpp common.o ${LDFLAGS} ${LXXFLAGS}

clean:
	rm -rf gobi-gps man3g 3g_connmgr *.o

install: man3g 3g_connmgr
	mkdir -p $(DESTDIR)/bin
	cp -f man3g $(DESTDIR)/bin
	cp -f 3g_connmgr $(DESTDIR)/bin

