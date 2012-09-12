# Options
EXTENSION_C=.c
EXTENSION_MC=.mc
EXTENSION_A=.asm

ifeq (yes,$(DEBUG))
DEBUGFLAGS=-g -DDEBUG
endif

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
	gcc -o f650-brodge.xo exc/f650_brodge.o $(objects) $(objects_ao) $(LIBS)
	gcc -o f650-test.xo exc/f650_test.o $(objects) $(objects_ao) $(LIBS)
	gcc -o xgui650.xo exc/xgui650.o $(objects) $(objects_ao) $(LIBS)


# Build rules
%.o: %$(EXTENSION_C)
	gcc -c $(COMPILEFLAGS) $(INCLUDE) -o $@ $<

%.ao: %$(EXTENSION_A)
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o $@ $<

clean:
	rm -f *.o exc/*.o brodge/*.o layout/*.o asm/*.ao *.xo exc/*.xo


tmp:
	rm -rf f650-test.xo f650_test.o f650_graphics.o asm/graphics.ao
	yasm -f elf64 -a x86 -m amd64 -o asm/graphics.ao asm/graphics.asm
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o asm/graphics2.ao asm/graphics2.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/graphics3.ao asm/graphics3.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/geo1.ao asm/geo1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/math1.ao asm/math1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/trig1.ao asm/trig1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/poly1.ao asm/poly1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/math0.ao asm/math0.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/std1.ao asm/std1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/brodge.ao asm/brodge.asm
	gcc -c f650_graphics.c
	gcc -c f650_geometry1.c
	gcc -c f650_test.c
	gcc -c f650_monospaced.c
	gcc -o f650-test.xo f650_test.o f650_graphics.o f650_geometry1.o 	\
	f650_monospaced.o													\
	asm/math1.ao asm/trig1.ao asm/geo1.ao 								\
	asm/graphics.ao asm/graphics2.ao asm/graphics3.ao 					\
	asm/rdtsc64.ao 														\
	asm/poly1.ao 														\
	asm/math0.ao asm/std1.ao 											\
	-lm
#	
	gcc -c f650_brodge.c
	gcc -o f650-brodge.xo f650_brodge.o f650_graphics.o f650_geometry1.o 	\
	f650_monospaced.o													\
	asm/math1.ao asm/trig1.ao asm/geo1.ao 								\
	asm/graphics.ao asm/graphics2.ao asm/graphics3.ao 					\
	asm/rdtsc64.ao 														\
	asm/poly1.ao 														\
	asm/math0.ao asm/std1.ao 											\
	asm/brodge.ao														\
	-lm
	
