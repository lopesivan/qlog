# Makefile.  Generated from Makefile.in by configure.

prefix         = /usr/local
includedir     = ${prefix}/include
srcdir         = .

Makefile: Makefile.in ./config.status

./config.status: ./configure
	./config.status --recheck

install:
	mkdir -p $(DESTDIR)$(includedir)
	cp $(srcdir)/qdiilog.hpp $(DESTDIR)$(includedir)

uninstall:
	-rm $(DESTDIR)$(includedir)/qdiilog.hpp

.PHONY: install uninstall
