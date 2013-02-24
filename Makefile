override CFLAGS += -Wall -Wextra -pedantic -std=c99 -I.

PREFIX = /usr/local

.PHONY: all clean

all: libmagot.so libmagot.a demo

clean:
	rm -f *.so *.o *.a demo

install: libmagot.so libmagot.a
	cp magot.h $(PREFIX)/include
	cp libmagot.so libmagot.a $(PREFIX)/lib

libmagot.so: magot.o
	$(CC) $(CFLAGS) -shared -Wl,-soname,$@ -fPIC -o $@

libmagot.a: magot.o
	$(AR) $(ARFLAGS) $@ $^

demo: demo.o magot.o
demo.o: magot.o

