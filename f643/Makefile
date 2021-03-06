# Makefile (from libebml Makefile)
# The library is built without debug information. If you want
# debug information to be included then compile with
# 'make DEBUG=yes'.
#

prefix=/usr/local
libdir=$(prefix)/lib
includedir=$(prefix)/include

# Programs
INSTALL = install
INSTALL_OPTS = -m 644
INSTALL_OPTS_LIB = -m 644
INSTALL_DIR_OPTS = -m 755

# Options
EXTENSION=.c

ifeq (yes,$(DEBUG))
DEBUGFLAGS=-g -DDEBUG
endif

CWD=$(shell pwd)

SRC_DIR=$(CWD)/f643
INCLUDE_DIR=$(CWD)/f643

# Librarires
INCLUDE=-I/usr/local/include/lame -I/usr/local/include/x264 -I/usr/local/include/mp4v2 -I/usr/local/include/libswscale
LIBS=-lmp3lame -lx264 -lmp4v2 -lswscale

# Names
EXEC=f643-test.xo
LIBRARY=libf643.a
LIBRARY_SO=libf643.so
LIBRARY_SO_VER=libf643.so.0.1

# source-files
sources:=$(wildcard ${SRC_DIR}*$(EXTENSION))

# header files; replace .c extension with .h
headers:=$(patsubst %$(EXTENSION),%.h,$(sources))

# object files; replace .c extension with .o
objects:=$(patsubst %$(EXTENSION),%.o,$(sources))
objects_so:=$(patsubst %$(EXTENSION),%.lo,$(sources))

WARNINGFLAGS=-Wall
# -Wextra -Wno-unknown-pragmas -ansi -Wshadow
COMPILEFLAGS=$(WARNINGFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDE)
DEPENDFLAGS  = $(CXXFLAGS) $(INCLUDE)


all: $(EXEC)

staticlib: $(LIBRARY)

sharedlib: $(LIBRARY_SO)

lib:
	@echo "Use the 'staticlib', 'sharedlib' or 'all' targets."
	@false

# Build rules
%.o: %$(EXTENSION)
	gcc -c $(COMPILEFLAGS) $(INCLUDE) -o $@ $<

%.lo: %$(EXTENSION)
	gcc -c $(COMPILEFLAGS) $(INCLUDE) -fPIC -o $@ $<

$(EXEC): $(objects)
	@echo "Sources : $(sources)"
	@echo "Objects : $(objects)"
	gcc -o $(EXEC) $(objects) $(LIBS)
	
$(LIBRARY): $(objects)
	ar rcvu $@ $(objects)
	ranlib $@

$(LIBRARY_SO): $(objects_so)
	gcc -shared -Wl,-soname,$(LIBRARY_SO_VER) -o $(LIBRARY_SO_VER) $(objects_so)
	rm -f $(LIBRARY_SO)
	ln -s $(LIBRARY_SO_VER) $(LIBRARY_SO)

clean:
	rm -f $(objects) $(objects_so)
	rm -f $(LIBRARY)
	rm -f $(LIBRARY_SO)
	rm -f $(LIBRARY_SO_VER)
	rm -f CORE

distclean dist-clean: clean
	rm -f .depend

install: install_staticlib install_sharedlib install_headers

install_headers:
	$(INSTALL) $(INSTALL_DIR_OPTS) -d $(includedir)
	for i in $(INCLUDE_DIR)/*.h; do \
		$(INSTALL) $(INSTALL_OPTS) $$i $(includedir) ; \
	done
	$(INSTALL) $(INSTALL_DIR_OPTS) -d $(includedir)/c
	for i in $(INCLUDE_DIR)/c/*.h; do \
		$(INSTALL) $(INSTALL_OPTS) $$i $(includedir)/c ; \
	done

install_staticlib: $(LIBRARY)
	$(INSTALL) $(INSTALL_DIR_OPTS) -d $(libdir)
	$(INSTALL) $(INSTALL_OPTS_LIB) $(LIBRARY) $(libdir)

install_sharedlib: $(LIBRARY_SO)
	$(INSTALL) $(INSTALL_DIR_OPTS) -d $(libdir)
	$(INSTALL) $(INSTALL_OPTS_LIB) $(LIBRARY_SO_VER) $(libdir)
	ln -fs $(LIBRARY_SO_VER) $(libdir)/$(LIBRARY_SO)


ifneq ($(wildcard .depend),)
include .depend
endif

# DO NOT DELETE

