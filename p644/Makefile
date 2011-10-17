
##
# CFLAGS  = -g -pg -O0 -Wall
# 	gcc -O3 -fPIC -Wall -shared -Wl,-soname,libp644-brodger.so -o libp644-brodger.so \
#	$(objects) p644_jbrodger.cpp -I/usr/java/jdk/include
##############
OBJ = p644_point.cpp p644_osc.cpp p644_brodger.cpp

p644-brodger: $(OBJS) p644_point.h p644_osc.h p644_brodger.h u640_CWrap.h
	g++ -O3 -Wall -o p644-brodger.xo p644_main.c ${OBJ}
	g++ -Wall -fPIC -shared -o libp644-brodger.so $(objects) p644_jbrodger.cpp -I/usr/java/jdk/include


all: p644-brodger

clean:
	rm -f *.o p644-brodger.xo libp644-brodger.so
