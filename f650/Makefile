# Options
EXTENSION_C=.c
EXTENSION_MC=.mc
EXTENSION_A=.asm

ifeq (yes,$(DEBUG))
DEBUGFLAGS=-g -DDEBUG
endif

COMPILEFLAGS=-g -DDEBUG


# Path
SRC_DIR=.
ASM_DIR=asm
EXC_DIR=exc

# Librarires
INCLUDE=
LIBS=-lm -lpthread -lX11 -lxcb -lXext

#
# source-files
sources_c:= $(wildcard ${SRC_DIR}/*$(EXTENSION_C))
sources_c+= $(wildcard ${SRC_DIR}/brodge/*$(EXTENSION_C))
sources_c+= $(wildcard ${SRC_DIR}/layout/*$(EXTENSION_C))
sources_c+=../f691/xgui691.c
sources_c+=../f640/f641/f641_v4l2.c
sources_c+=../f645/mjpeg645.c


# asm files
sources_a:=$(wildcard ${ASM_DIR}/*$(EXTENSION_A))
sources_a+=$(wildcard ../f645/*$(EXTENSION_A))

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
	make -C ../f645
#	gcc -o f650-brodge.xo exc/f650_brodge.o $(objects) $(objects_ao) $(LIBS)
	gcc -o f650-test.xo exc/f650_test.o $(objects) $(objects_ao) $(LIBS)
#	gcc -o xgui650-1t.xo exc/xgui650_test1.o $(objects) $(objects_ao) $(LIBS)
#	gcc -o xgui650-2t.xo exc/xgui650_test2.o $(objects) $(objects_ao) $(LIBS) $(COMPILEFLAGS)
	gcc -o xgui650-3t.xo exc/xgui650_test3.o $(objects) $(objects_ao) $(LIBS)


# Build rules
%.o: %$(EXTENSION_C)
	gcc -c $(COMPILEFLAGS) $(INCLUDE) -o $@ $<

%.ao: %$(EXTENSION_A)
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o $@ $<

clean:
	rm -f *.o exc/*.o brodge/*.o layout/*.o asm/*.ao *.xo exc/*.xo


