
##
# CFLAGS  = -g -pg -O0 -Wall
# 	gcc -O3 -fPIC -Wall -shared -Wl,-soname,libp644-brodger.so -o libp644-brodger.so \
#	$(objects) p644_jbrodger.cpp -I/usr/java/jdk/include

# g++ -O3 -Wall -o p644-brodger.xo p644_main.c ${OBJ}

LDFLAGS = -L/usr/lib -L/usr/local/lib -lasound -lfftw3 -lfftw3f -lf051-0.1 -lm

INCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
LIBS=-L/usr/local/lib -lavformat -lavdevice -lswscale -lavutil -lavcodec

##############
OBJ = p644_point.cpp p644_osc.cpp p644_brodger.cpp

p644-brodger: $(OBJS) p644_point.h p644_osc.h p644_brodger.h u640_CWrap.h
	gcc -O3 -Wall -c ../../f610-equa/f611/f611_ffmpeg.c ${LDFLAGS} ${INCL} ${LIBS}
	g++ -O3 -Wall -c p644_point.cpp
	g++ -O3 -Wall -c p644_osc.cpp
	g++ -O3 -Wall -c p644_brodger.cpp
	g++ -O3 -Wall -c p644_main.c
	g++ -O3 -Wall -o p644-brodger.xo f611_ffmpeg.o p644_point.o p644_osc.o p644_brodger.o p644_main.o ${LDFLAGS} ${INCL} ${LIBS}
	g++ -Wall -fPIC -shared -o libp644-brodger.so $(OBJ) p644_jbrodger.cpp -I/usr/java/jdk/include


all: p644-brodger

clean:
	rm -f *.o p644-brodger.xo libp644-brodger.so
