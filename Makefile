.POSIX:

PREFIX = /usr/local
HDR = rawr_arena.h rawr_dynarr.h

all: ${HDR}

install: all
	mkdir -p -- "${DESTDIR}${PREFIX}/include"
	cp -f ${HDR} -- "${DESTDIR}${PREFIX}/include"
	cd -- "${DESTDIR}${PREFIX}/include" && chmod 0644 ${HDR}

uninstall:
	cd -- "${DESTDIR}${PREFIX}/include" && rm -f ${HDR}

.PHONY: all install uninstall
