

CC      = gcc
CFLAGS  = -g -pg -O0
LDFLAGS = -L/usr/lib -L/usr/local/lib -lasound -lfftw3 -lfftw3f -lf051-0.1 -lm

INCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
LIBS=-L/usr/local/lib -lavformat -lavdevice -lswscale -lavutil -lavcodec

OBJS  = f641_main.c ../f640/f640_queues.c


all: f641


f641: $(OBJS) ../f640/f640_queues.h
	$(CC) ${CFLAGS} -o f641.xo $(OBJS) $(LDFLAGS) $(INCL) $(LIBS)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@
