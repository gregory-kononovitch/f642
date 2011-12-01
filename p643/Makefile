
##
# CFLAGS  = -g -pg -O0 -Wall
LDFLAGS = -L/usr/lib -L/usr/local/lib -lasound -lfftw3 -lfftw3f -lf051-0.1 -lm

INCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
LIBS=-L/usr/local/lib -lavformat -lavdevice -lswscale -lavutil -lavcodec

##############
OBJ = p643_brdg.c ../../f610-equa/f611/f611_ffmpeg.c

p643-brdg: $(OBJS) p643_brdg.h
	gcc -g -O2 -Wall -o p643-brdg.xo ${OBJ} ${LDFLAGS} ${INCL} ${LIBS}

	
all: p643-brdg

clean:
	rm -f *.o p643-brdg.xo
