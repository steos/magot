override CFLAGS += -Wall -Wextra -pedantic -std=c99 -I.

PREFIX = /usr/local

.PHONY: all
all: libmagot.so libmagot.a demo

.PHONY: clean
clean:
	rm -f *.so *.o *.a demo README.html

install: libmagot.so libmagot.a
	cp magot.h $(PREFIX)/include
	cp libmagot.so libmagot.a $(PREFIX)/lib

libmagot.so: magot.o
	$(CC) $(CFLAGS) -shared -Wl,-soname,$@ -fPIC -o $@

libmagot.a: magot.o
	$(AR) $(ARFLAGS) $@ $^

demo: demo.o magot.o
demo.o: magot.o

%.html: %.md
	pandoc -f markdown -t html -s $< > $@

.PHONY: doc
doc: README.html

