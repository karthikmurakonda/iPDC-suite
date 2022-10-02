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
LDLIBS=`pkg-config --cflags --libs gtk+-3.0 osmgpsmap-1.0`
CFLAGS=-g -I./inc -fcommon `pkg-config --cflags --libs gtk+-3.0 osmgpsmap-1.0`

OBJECTS =  ipdc.c recreate.o connections.o new_pmu_or_pdc.o parser.o dallocate.o align_sort.o ipdcGui.o attack_detection.o data_vis.o utility_tools.o Attack_detect.o

CLEAN_OBJECTS =  recreate.o connections.o new_pmu_or_pdc.o parser.o dallocate.o align_sort.o ipdcGui.o ~iPDC.xml

PROGRAM = iPDC

COMPILE = $(CC) 

$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDLIBS) $(LIBS) -o $@

%.o: %.c
	$(COMPILE) $(CFLAGS) -c $<

install: $(PROGRAM)
	gtk-builder-convert iPDC.glade iPDC.xml
	mkdir -p $(CREATE_DIR)iPDC/$
	cp -f $(FILE1) $(CREATE_DIR)iPDC/$(FILE1)
	cp -f images/$(FILE2) $(CREATE_DIR)iPDC/$(FILE2)
	cp -f images/$(FILE3) $(CREATE_DIR)iPDC/$(FILE3)
	cp -f images/$(FILE4) $(CREATE_DIR)iPDC/$(FILE4)
	cp -f images/$(FILE5) $(CREATE_DIR)iPDC/$(FILE5)
	cp -f images/$(FILE6) $(DESKTOP_DIR)/$(FILE6)
	cp $(PROGRAM) $(INSTALL_PREFIX)bin/$(PROGRAM)
#	rm -f $(CLEAN_OBJECTS) $(PROGRAM)

uninstall:
	rm -f $(INSTALL_PREFIX)bin/$(PROGRAM)
	rm -rf $(CREATE_DIR)iPDC$
	rm -rf $(DESKTOP_DIR)/$(FILE6)
	rm -f $(CLEAN_OBJECTS) $(PROGRAM)

clean:
	rm -f $(CLEAN_OBJECTS) $(PROGRAM)
