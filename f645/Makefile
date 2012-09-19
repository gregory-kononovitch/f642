# Options
EXTENSION_C=.c
EXTENSION_A=.asm

ifeq (yes,$(DEBUG))
DEBUGFLAGS=-g -DDEBUG
endif

COMPILEFLAGS=-g -DDEBUG


# Path
SRC_DIR=.
ASM_DIR=.
EXC_DIR=exc

# Librarires
INCLUDE=
LIBS=-lm -lpthread -lX11 -lxcb -lXext

#
# source-files
sources_c:= $(wildcard ${SRC_DIR}/*$(EXTENSION_C))

# asm files
sources_a:=$(wildcard ${ASM_DIR}/*$(EXTENSION_A))

# main.o files
sources_e:=$(wildcard ${EXC_DIR}/*$(EXTENSION_C))


# object files; replace .c extension with .o
objects:=$(patsubst %$(EXTENSION_C),%.o,$(sources_c))

# object files; replace .asm extension with .ao
objects_ao:=$(patsubst %$(EXTENSION_A),%.ao,$(sources_a))

# object files; replace .c extension with .o
objects_e:=$(patsubst %$(EXTENSION_C),%.o,$(sources_e))


all: exec

exec: $(objects) $(objects_ao) $(objects_e)
	gcc -o mjpeg645-test1.xo exc/mjpeg645_test1.o $(objects) $(objects_ao) $(LIBS)


# Build rules
%.o: %$(EXTENSION_C)
	gcc -c $(COMPILEFLAGS) $(INCLUDE) -o $@ $<

%.ao: %$(EXTENSION_A)
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o $@ $<

clean:
	rm -f *.o *.xo exc/*.o exc/*.xo


