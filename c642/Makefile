#
# Makefile for USB Core files and filesystem
#
include /lib/modules/$(shell uname -r)/build/.config
####	Modules
ccflags-m := -Wall -g	#

obj-m             += c642-brodge.o

c642-brodge-y     := c642_module.o
c642-brodge-y     += c642_pictu4.o


all: c642_trig
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules



clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean


##############
OBJ = c642_trig.c

c642_trig: $(OBJS)
	gcc -g -O2 -Wall -o c642-trig.xo c642_trig.c -lm
