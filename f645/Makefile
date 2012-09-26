# Options
EXTENSION_C=.c
EXTENSION_H=.h
EXTENSION_A=.asm
EXTENSION_S=.s

ifeq (yes,$(DEBUG))
DEBUGFLAGS=-g -DDEBUG
endif

COMPILEFLAGS=-g -DDEBUG


# Path
HDR_DIR=.
SRC_DIR=.
ASM_DIR=.
EXC_DIR=exc

# Librarires
FFINCL=-I/usr/local/include/libavformat -I/usr/local/include/libavdevice -I/usr/local/include/libswscale -I/usr/local/include/libavcodec -I/usr/local/include/libavutil
FFLIBS=-L/usr/local/lib -lavformat -lavdevice -lswscale -lavutil -lavcodec

INCLUDE=FFINCL
LIBS=-lm -lpthread -lX11 -lxcb -lXext FFLIBS

#
# source-files
sources_c:= $(wildcard ${SRC_DIR}/*$(EXTENSION_C))

# header-files
sources_h:= $(wildcard ${SRC_DIR}/*$(EXTENSION_H))

# asm files
sources_a:=$(wildcard ${ASM_DIR}/*$(EXTENSION_A))
sources_a+=$(wildcard ../f650/asm/*$(EXTENSION_A))

sources_s+=$(wildcard ./inclu/*$(EXTENSION_S))

# main.o files
sources_e:=$(wildcard ${EXC_DIR}/*$(EXTENSION_C))


# object files; replace .c extension with .o
objects:=$(patsubst %$(EXTENSION_C),%.o,$(sources_c))

# object files; replace .asm extension with .ao
objects_ao:=$(patsubst %$(EXTENSION_A),%.ao,$(sources_a))

# object files; replace .c extension with .o
objects_e:=$(patsubst %$(EXTENSION_C),%.o,$(sources_e))


all: exec

exec: $(objects) $(objects_ao) $(objects_e) asm
#	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o mjpeg.ao mjpeg.asm
#	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o mjpeg2.ao mjpeg2.asm
	gcc -o mjpeg645-test1.xo exc/mjpeg645_test1.o $(objects) $(objects_ao) $(LIBS)

asm: $(sources_a) $(sources_s)
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o mjpeg.ao mjpeg.asm
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o mjpeg2.ao mjpeg2.asm

# Build rules
%.o: %$(EXTENSION_C)
	gcc -c $(COMPILEFLAGS) $(INCLUDE) -o $@ $<

%.ao: %$(EXTENSION_A) 				# $(sources_s)
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o $@ $<

clean:
	rm -f *.o *.xo exc/*.o exc/*.xo


