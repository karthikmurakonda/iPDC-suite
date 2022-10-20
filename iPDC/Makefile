 # ----------------------------------------------------------------------------- 
 # Makefile for building iPDC

 # iPDC - Phasor Data Concentrator
 #
 # Copyright (C) 2011 Nitesh Pandit
 # Copyright (C) 2011 Kedar V. Khandeparkar

 # This program is free software; you can redistribute it and/or
 # modify it under the terms of the GNU General Public License
 # as published by the Free Software Foundation; either version 2
 # of the License, or (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 #
 # Authors: 
 #		Nitesh Pandit <panditnitesh@gmail.com>
 #		Kedar V. Khandeparkar <kedar.khandeparkar@gmail.com>			
 #
 # ----------------------------------------------------------------------------- 


SHELL = /bin/sh
prefix = /usr/local
includedir = /usr/src/include

INSTALL = /usr/bin/install -c
INSTALL_PREFIX = /usr/local/
CREATE_DIR = /usr/local/share/
DESKTOP_DIR = /usr/share/applications

FILE1 = iPDC.xml
FILE2 = ipdcLogo.png
FILE3 = PSLab.PNG
FILE4 = ipdcLogo1.png
FILE5 = logo.png
FILE6 = iPDC.desktop


CC = gcc
LIBS = -lpthread
LDLIBS=`pkg-config --cflags --libs gtk+-3.0 osmgpsmap-1.0 gee-0.8` lib/liblivechart.so lib/liblivechart_static.a -Wl,--rpath=./lib
CFLAGS=-g -I./inc -I./lib -fcommon `pkg-config --cflags --libs gtk+-3.0 osmgpsmap-1.0 gee-0.8`

PROGRAM = iPDC
SOURCE_FILES = $(wildcard src/*.c)
OBJECT_FILES = $(patsubst src/%.c, bin/%.o, $(SOURCE_FILES))
CLEAN_OBJECTS = $(wildcard bin/*.o) $(PROGRAM)

all: $(PROGRAM)

$(PROGRAM): $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) -o $(PROGRAM) $(LDLIBS) 

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


install: $(PROGRAM)
	gtk-builder-convert assets/iPDC.glade assets/iPDC.xml
	mkdir -p $(CREATE_DIR)iPDC/$
	cp -f assets/$(FILE1) $(CREATE_DIR)iPDC/$(FILE1)
	cp -f assets/$(FILE2) $(CREATE_DIR)iPDC/$(FILE2)
	cp -f assets/$(FILE3) $(CREATE_DIR)iPDC/$(FILE3)
	cp -f assets/$(FILE4) $(CREATE_DIR)iPDC/$(FILE4)
	cp -f assets/$(FILE5) $(CREATE_DIR)iPDC/$(FILE5)
	cp -f assets/$(FILE6) $(DESKTOP_DIR)/$(FILE6)
	cp $(PROGRAM) $(INSTALL_PREFIX)bin/$(PROGRAM)

uninstall:
	rm -f $(INSTALL_PREFIX)bin/$(PROGRAM)
	rm -rf $(CREATE_DIR)iPDC$
	rm -rf $(DESKTOP_DIR)/$(FILE6)
	rm -f $(CLEAN_OBJECTS) $(PROGRAM)

clean:
	rm -f $(CLEAN_OBJECTS) $(PROGRAM)
