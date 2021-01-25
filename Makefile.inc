# Makefile.inc for yume3 program - jiw 15 Mar 2011
# $Id:$
#
SHELL = /bin/sh
INSTDIR = /usr/local/bin
MANDIR = /usr/share/man/man1
CC = gcc
CFLAGS = -O3 -Wall

# Set up short labels for yume's bin, doc, examples, & src dirs
B = $(YDIR)/bin
D = $(YDIR)/doc
E = $(YDIR)/examples
S = $(YDIR)/src

# Make labels to stand for files with parsing-data search structures,
# constants, and generated arrays, and for some dependencies
TRE  = $(S)/yume3-trec.h $(S)/yume3-treh.h
YUTI = $(S)/yume3-util.h $(S)/yume3-init.c $(TRE)

# Note, one may need to install GDK/GTK development files,
# via eg    sudo apt-get install build-essential libgtk-3-dev
LIBS = `pkg-config gtk+-3.0 --libs`
INCS = `pkg-config gtk+-3.0 --cflags`

OBJECTS=$(B)/yume3.o $(B)/yume3-p1.o $(B)/yume3-p2.o \
	$(B)/yume3-p3.o $(B)/yume3-util.o

PDFS=$(D)/yume.1.pdf $(D)/yume-examples.1.pdf

all:  $(B)/yume3-init $(B)/yume $(B)/yume3 $(HOME)/.yume-gtkrc $(PDFS)

init: $(B)/yume3-init $(TRE)

$(B)/yume3-init: $(S)/yume3-init.c
	$(CC) $(S)/yume3-init.c -O3 -Wall  -o $@

$(TRE): $(B)/yume3-init
	$(B)/yume3-init
	test -d src && mv yume3-tre*h src

$(D)/yume.1.pdf: $(D)/yume.1
	man -l -t $(D)/yume.1 > t.ps
	ps2pdf t.ps
	mv t.pdf $(D)/yume.1.pdf
	rm t.ps
	
$(D)/yume-examples.1.pdf: $(D)/yume-examples.1
	man -l -t $(D)/yume-examples.1 > t.ps
	ps2pdf t.ps
	mv t.pdf $(D)/yume-examples.1.pdf
	rm t.ps

# "install" hasn't been tested on non-linux systems
install: $(B)/yume
	install -s $(B)/yume  $(INSTDIR)/yume
	install -s $(B)/yume3 $(INSTDIR)/yume3
	install $(D)/yume.1 $(MANDIR)/yume.1 
	install $(D)/yume-examples.1 $(MANDIR)/yume-examples.1

man-page:
	install $(D)/yume.1 $(MANDIR)/yume.1 
	install $(D)/yume-examples.1 $(MANDIR)/yume-examples.1	

# Following item should be done as ordinary user
$(HOME)/.yume-gtkrc: $(E)/yume-gtkrc
	install $(E)/yume-gtkrc $(HOME)/.yume-gtkrc

$(B)/yume3: $(TRE) $(OBJECTS)
	$(CC) $(YOPTS) $(OBJECTS) -o $(B)/yume3 $(LIBS)

$(B)/yume3.o: $(S)/yume3.c $(S)/yume3.h $(S)/yume3-p1.h $(S)/yume3-p2.h $(S)/yume3-p3.h $(YUTI)
	 $(CC) $(CFLAGS) -c $(S)/yume3.c $(INCS) -o $(B)/yume3.o

$(B)/yume3-p1.o: $(S)/yume3-p1.c $(S)/yume3.h $(S)/yume3-p1.h $(S)/yume3-p2.h  $(YUTI)
	    $(CC) $(CFLAGS) -c $(S)/yume3-p1.c $(INCS) -o $(B)/yume3-p1.o

# In the  yume3-p2  section, uncomment just one of the $(CC) lines, to
# allow or disallow display of warnings about Gtk deprecated functions.
# (8 Gtk functions used in yume3-p2.c have been deprecated.)
$(B)/yume3-p2.o: $(S)/yume3-p2.c $(S)/yume3.h $(S)/yume3-p2.h $(S)/yume3-p3.h  $(YUTI)
	#$(CC) $(CFLAGS) -c $(S)/yume3-p2.c $(INCS) -o $(B)/yume3-p2.o
	$(CC) $(CFLAGS) -Wno-deprecated-declarations -c $(S)/yume3-p2.c $(INCS) -o $(B)/yume3-p2.o

$(B)/yume3-p3.o: $(S)/yume3-p3.c $(S)/yume3.h $(S)/yume3-p3.h $(YUTI) $(S)/yume3-util.h
	    $(CC) $(CFLAGS) -c $(S)/yume3-p3.c $(INCS) -o $(B)/yume3-p3.o

$(B)/yume3-util.o: $(S)/yume3-util.c $(S)/yume3.h $(YUTI)
	    $(CC) $(CFLAGS) -c $(S)/yume3-util.c $(INCS) -o $(B)/yume3-util.o

$(B)/yume: $(S)/yume3-pipr.c
	    $(CC) $(CFLAGS) $(S)/yume3-pipr.c -o $(B)/yume

clean: 
	rm -f $(B)/*.o
	rm -f $(B)/yume3-init
	rm -f $(S)/yume3-tre*

