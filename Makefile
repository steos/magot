override CFLAGS += -Wall -Wextra -pedantic -std=c99 -I. -fPIC

PREFIX = /usr/local

.PHONY: all
all: libmagot.so libmagot.a demo

.PHONY: clean
clean:
	rm -f *.so *.o *.a demo README.html

install: libmagot.so libmagot.a magot.h
	cp magot.h $(PREFIX)/include
	cp libmagot.so libmagot.a $(PREFIX)/lib

install-links: libmagot.so libmagot.a magot.h
	ln -sf $(shell readlink -e ./magot.h) $(PREFIX)/include/magot.h
	ln -sf $(shell readlink -e ./libmagot.a) $(PREFIX)/lib/libmagot.a
	ln -sf $(shell readlink -e ./libmagot.so) $(PREFIX)/lib/libmagot.so

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/include/magot.h \
	$(PREFIX)/lib/libmagot.*


demo: demo.o magot.o
demo.o: magot.o

.PHONY: doc
doc: README.html

# -- general rules

%.html: %.md
	pandoc -f markdown -t html -s $< > $@

lib%.so: %.o
	$(LINK.c) -shared -Wl,-soname,$@ -fPIC -o $@ $<

lib%.a: %.o
	$(AR) $(ARFLAGS) $@ $^
