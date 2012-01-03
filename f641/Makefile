

CC      = gcc
CFLAGS  = -g -pg  -O3
LDFLAGS = -L/usr/lib -L/usr/local/lib -lasound -lfftw3 -lfftw3f -lf051-0.1 -lm -lncurses

INCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
LIBS=-L/usr/local/lib -lavformat -lavdevice -lswscale -lavutil -lavcodec

OBJS_GRAPH = ../f640/f640_graphics.c ../f640/f640_gray.c ../f640/f640_rgb.c ../f640/f640_yuv.c ../f640/f640_rgba.c
OBJS  = f641_main.c ../f640/f640_queues.c ../f640/f641/f641_v4l2.c  ../f640/f641/f641_ffmpeg.c  ../f640/f641/f641_processing.c
OBJS_PPP = ../f640/f641_processing.c

all: f641


f641: $(OBJS) $(OBJS_GRAPH) $(OBJS_PPP) ../f640/f640_graphics.h ../f640/f640_queues.h  ../f640/f641/f641.h
	$(CC) ${CFLAGS} -o f641.xo $(OBJS) $(OBJS_GRAPH) $(OBJS_PPP) $(LDFLAGS) $(INCL) $(LIBS)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@
