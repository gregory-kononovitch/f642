
##
# CFLAGS  = -g -pg -O0 -Wall

##############
OBJ = p643_brdg.c

p643-brdg: $(OBJS) p643_brdg.h
	gcc -g -pg -O2 -Wall -o p643-brdg.xo ${OBJ} -lm -lf051-0.1

	
all: p643-brdg

clean:
	rm -f *.o p643-brdg.xo
