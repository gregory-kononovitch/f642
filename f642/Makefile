
FLAGS=-O3 -g -fPIC -Wall 
LDFLAGS=-O3 -g -fPIC -Wall
#INCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
INCL=-I/usr/local/include -I/usr/local/include/libswscale
#LIBS=-L/usr/local/lib -lasound -lavformat -lavdevice -lswscale -lavutil -lavcodec
LIBS=-L/usr/local/lib -lswscale -lx264


all: f642


f642: f642_x264.hh f642_x264.cc f642_j264.cc f642_test.cc
	g++ ${LDFLAGS} ${INCL} -c muxers/raw.c
	g++ ${LDFLAGS} ${INCL} -c muxers/flv.c
	g++ ${LDFLAGS} ${INCL} -c muxers/flv_bytestream.c
	g++ ${LDFLAGS} ${INCL} ${LIBS} -c f642_x264.cc
	g++ ${LDFLAGS} ${INCL} ${LIBS} -c f642_j264.cc
	g++ -Wall -fPIC -shared -o libf642-x264.so f642_x264.o f642_j264.o raw.o flv.o flv_bytestream.o -I/usr/java/jdk/include ${LIBS}
	g++ ${LDFLAGS} ${INCL} -c f642_test.cc
	g++ -Wall -fPIC -o f642-test.xo f642_test.o f642_x264.o raw.o flv.o flv_bytestream.o ${LIBS}
#	g++ -Wall -fPIC -o f642-test.xo f642_test.o ${INCL} -L. -lf642-x264 ${LIBS}

clean:
	rm -f *.o *.so

