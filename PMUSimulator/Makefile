 # ----------------------------------------------------------------------------- 
 # Makefile for building PMU Simulator

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

FILE1 = pmu.xml
FILE2 = PmuLogo.png
FILE3 = PSLab.PNG
FILE4 = PmuLogo1.png
FILE5 = logo.png
FILE6 = PMU.desktop
#FILE7 = DataDir

CC = gcc
LIBS = -lpthread -lm -lrt
LDLIBS=`pkg-config --cflags --libs gtk+-2.0`
CFLAGS= -fcommon `pkg-config --cflags --cflags gtk+-2.0`

SOURCES = \
	pmu.c \
	PmuGui.c PmuGui.h \
	function.c function.h \
	ServerFunction.c ServerFunction.h \
	CfgFunction.c CfgFunction.h \
	CfgGuiFunctions.c CfgGuiFunctions.h \

OBJECTS =  pmu.c PmuGui.o function.o ServerFunction.o CfgFunction.o CfgGuiFunctions.o 
CLEAN_OBJECTS = PmuGui.o function.o ServerFunction.o CfgFunction.o CfgGuiFunctions.o pmu.xml

PROGRAM = PMU

COMPILE = $(CC) 

$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDLIBS) $(LIBS) -o $@

%.o: %.c
	$(COMPILE) $(CFLAGS) -c $<

install: $(PROGRAM)
	gtk-builder-convert pmu.glade pmu.xml
	mkdir -p $(CREATE_DIR)PMU/$
	cp -f $(FILE1) $(CREATE_DIR)PMU/$(FILE1)
	cp -f images/$(FILE2) $(CREATE_DIR)PMU/$(FILE2)
	cp -f images/$(FILE3) $(CREATE_DIR)PMU/$(FILE3)
	cp -f images/$(FILE4) $(CREATE_DIR)PMU/$(FILE4)
	cp -f images/$(FILE5) $(CREATE_DIR)PMU/$(FILE5)
	cp -f images/$(FILE6) $(DESKTOP_DIR)/$(FILE6)
	cp $(PROGRAM) $(INSTALL_PREFIX)bin/$(PROGRAM)
	mkdir -p $(HOME)/iPDC/DataDir
	mkdir -p $(HOME)/iPDC/DataDir/50Hz_measurement_files
	mkdir -p $(HOME)/iPDC/DataDir/60Hz_measurement_files
	cp -f -r ./DataDir/50Hz_measurement_files/* $(HOME)/iPDC/DataDir/50Hz_measurement_files	#-i -n for Debian based Linux 
	cp -f -r ./DataDir/60Hz_measurement_files/* $(HOME)/iPDC/DataDir/60Hz_measurement_files
#	chmod 777 $(HOME)/iPDC/DataDir
#	rm -f $(CLEAN_OBJECTS) $(PROGRAM)

uninstall:
	rm -f $(INSTALL_PREFIX)bin/$(PROGRAM)
	rm -rf $(CREATE_DIR)PMU$
	rm -rf $(DESKTOP_DIR)/$(FILE6)
	rm -f $(CLEAN_OBJECTS) $(PROGRAM)

clean:
	rm -f $(CLEAN_OBJECTS) $(PROGRAM)
