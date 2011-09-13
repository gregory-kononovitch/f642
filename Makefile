SHELL = /bin/sh

prefix      = /usr/local
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
mandir      = ${datarootdir}/man
datarootdir = ${prefix}/share

CC      = gcc
CFLAGS  = -g -O2
LDFLAGS = -lf051-0.1 -lm

OBJS  = f640_main.c


all: f640

install: all
	mkdir -p ${DESTDIR}${bindir}
	mkdir -p ${DESTDIR}${mandir}/man1
	install -m 755 f640 ${DESTDIR}${bindir}
	install -m 644 f640.1.gz ${DESTDIR}${mandir}/man1

f640: $(OBJS) f640.h
	$(CC) -o f640 $(OBJS) $(LDFLAGS)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

f640.1.gz: f640.1
	gzip -c --best f640.1 > f640.1.gz

clean:
	rm -f core* *.o f640 f640.1.gz

#distclean: clean
#	rm -rf config.h *.cache config.log config.status Makefile *.jp*g *.png
