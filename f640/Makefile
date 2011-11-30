SHELL = /bin/sh

prefix      = /usr/local
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
mandir      = ${datarootdir}/man
datarootdir = ${prefix}/share

CC      = gcc
CFLAGS  = -g -pg -O0
LDFLAGS = -L/usr/lib -L/usr/local/lib -lasound -lfftw3 -lfftw3f -lf051-0.1 -lm

INCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
LIBS=-L/usr/local/lib -lavformat -lavdevice -lswscale -lavutil -lavcodec

OBJS  = f640_main.c ../../f610-equa/f611/f611_ffmpeg.c


all: f640

install: all
	mkdir -p ${DESTDIR}${bindir}
	mkdir -p ${DESTDIR}${mandir}/man1
	install -m 755 f640 ${DESTDIR}${bindir}
	install -m 644 f640.1.gz ${DESTDIR}${mandir}/man1

f640: $(OBJS) f640.h
	$(CC) ${CFLAGS} -o f640.xo $(OBJS) $(LDFLAGS) $(INCL) $(LIBS)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

f640.1.gz: f640.1
	gzip -c --best f640.1 > f640.1.gz

clean:
	rm -f core* *.o f640.xo f640.1.gz

#distclean: clean
#	rm -rf config.h *.cache config.log config.status Makefile *.jp*g *.png
