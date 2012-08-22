
FLAGS=-O3 -g -fPIC -Wall 
LDFLAGS=-O3 -g -fPIC -Wall
INCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
LIBS=-L/usr/local/lib -lasound -lavformat -lavdevice -lswscale -lavutil -lavcodec



all: f642
	gcc ${FLAGS} -o f642-record.xo  ${INCL} ${LIBS} -lm

f642: f642_x264.hh f642_x264.cc
	g++ ${LDFLAGS} ${INCL} ${LIBS} -c f642_x264.cc
#	g++ ${LDFLAGS} ${INCL} ${LIBS} -c f642_j264.cc
#	g++ -Wall -fPIC -shared -o libf642-x264.so f642_x264.o f642_j264.o -I/usr/java/jdk/include ${LIBS}
#	g++ ${LDFLAGS} -c f642_test.cc
#	g++ -Wall -fPIC -o f642-test.xo f642_test.o f642_x264.o ${LIBS}
#	g++ -Wall -fPIC -o f642-test.xo f642_test.o -lf642-x264 ${LIBS}

clean:
	rm -f *.o *.so

