INCLUDEDIR		= -I/usr/include/ncurses

OBJS			= input.o menurt.o listbox.o winops.o labels.o \
				msgboxes.o txbox.o

all: libtextbox.a

libtextbox.a: $(OBJS)
	rm -rf libtextbox.a
	$(AR) cq libtextbox.a $(OBJS)
	$(RANLIB) libtextbox.a
#	gcc -shared -o libtextbox.so $(OBJS)

%.o: %.c *.h
	$(CC) -O2 -g -Wall $(INCLUDEDIR) -c -o $*.o $<
#	$(CC) -msoft-float -mips16 -minterlink-mips16 -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -O2 -g -Wall -fPIC $(INCLUDEDIR) -c -o $*.o $<

clean:
	rm -rf *.o *~ libtextbox.a libtextbox.so


