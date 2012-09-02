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

gtk : $(_gtk)
	gcc -o gtk  $(_gtk) $(BASE_LIBS) $(GTK_LIBS)
	
gui.o : gui.c
	gcc -O3 -Wall -c gui.c -o gui.o $(BASE_INCLUDE) $(GTK_INCLUDE)

###################

all : gtk

.PHONY : clean
clean :
	rm $(_gtk) gtk
