#Makefile

# Created on: Sep 2, 2012
#     Author: gk

###################
BASE_INCLUDE=-I/usr/include
BASE_LIBS=-L/usr/lib -L/usr/local/lib

GTK_INCLUDE=-I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12
GTK_LIBS=-lgtk-x11-2.0 -lgobject-2.0 -lglib-2.0 -lcairo -lgdk-x11-2.0 -lgdk_pixbuf-2.0
	
################################
_gtk = gui.o

f690 = f690_gui.o

gtk : $(f690)
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o ./std1.ao ../f650/asm/std1.asm
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o ./graphics.ao ../f650/asm/graphics.asm
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o ./graphics2.ao ../f650/asm/graphics2.asm
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o ./graphics3.ao ../f650/asm/graphics3.asm
	
	gcc -o f690-gui.xo $(f690) $(BASE_LIBS) $(GTK_LIBS) \
	../f650/f650_graphics.o ../f650/f650_geometry1.o ../f650/f650_monospaced.o \
	./graphics.ao ./graphics2.ao ./graphics3.ao ./std1.ao
#	
	gcc -O3 -Wall -c gui.c -o gui.o $(BASE_INCLUDE) $(GTK_INCLUDE)
	gcc -o gtk.xo $(_gtk) $(BASE_LIBS) $(GTK_LIBS) \
	../f650/f650_graphics.o ../f650/f650_geometry1.o ../f650/f650_monospaced.o \
	./graphics.ao ./graphics2.ao ./graphics3.ao ./std1.ao
	
	
gui.o : gui.c
	gcc -O3 -Wall -c gui.c -o gui.o $(BASE_INCLUDE) $(GTK_INCLUDE)

f690_gui.o : f690_gui.c
	gcc -O3 -Wall -c f690_gui.c -o f690_gui.o $(BASE_INCLUDE) $(GTK_INCLUDE)

###################

all : gtk

.PHONY : clean
clean :
	rm -f $(_gtk) gtk.xo
