-- =============================================================================
-- Db.sql (iPDC Data Structures for MySQL )
-- 
-- iPDC - Phasor Data Concentrator
--
-- Copyright (C) 2011 Nitesh Pandit
-- Copyright (C) 2011 Kedar V. Khandeparkar
--
-- This program is free software; you can redistribute it and/or
-- modify it under the terms of the GNU General Public License
-- as published by the Free Software Foundation; either version 2
-- of the License, or (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
--
-- Authors: 
--		Kedar V. Khandeparkar <kedar.khandeparkar@gmail.com>			
--		Nitesh Pandit <panditnitesh@gmail.com>				
--
-- =============================================================================


-- First execute the following from the command prompt to create database:
-- 	  mysql -uroot -p <"Db.sql"

-- The following statements are used to create a user with access to the database.
-- Be sure to change the username and password.
-- CREATE USER NewUser IDENTIFIED BY 'MyPassword';
-- GRANT SELECT, UPDATE, INSERT, DELETE ON iPDC.* TO NewUser;

SET default_storage_engine=INNODB;
CREATE DATABASE iPDC;
USE iPDC;


-- ============================================================================================
-- This creates Main Configuration Table,
-- This table stores MAIN CONFIGURATION details Like PDC_ID, SOC, TIME BASE, NO. OF PMU,
-- DATA Rate in iPDC Database

CREATE TABLE MAIN_CFG_TABLE(
	PDC_ID INT NOT NULL,
	SOC DECIMAL(10) NOT NULL,
	FRACSEC DECIMAL(10) NOT NULL,
	TIMEBASE DECIMAL(10) NOT NULL,
	NUM_OF_PMU INT NOT NULL,
	DATA_RATE INT DEFAULT 0,
	PRIMARY KEY (PDC_ID ASC)
);


-- ============================================================================================
-- This creates Sub Configuration Table,
-- This table stores Sub Configuration details Like PDC_ID, PMU_ID, SOC,Fraction of Second,
-- Station Name, No. of Analog channels, No. of Phasor channels in iPDC Database

CREATE TABLE SUB_CFG_TABLE(

	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	SOC DECIMAL(10) NOT NULL,
	FRACSEC DECIMAL(10) NOT NULL,
	STN VARCHAR(40) NOT NULL,
	PHNMR INT DEFAULT 0,
	ANNMR INT DEFAULT 0,
	DGNMR INT DEFAULT 0,
	FNOM INT DEFAULT 0,
	PRIMARY KEY (PMU_ID),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID)
);

-- ============================================================================================		
-- This creates Phasor channel information Table
-- This table stores details about phasor Configuration like PDC_ID, Phasor name, Phasor type,
-- Phasor units in iPDC Database

CREATE TABLE PHASOR (

	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	PHASOR_NAMES VARCHAR(32) NOT NULL,
	PHASOR_TYPE VARCHAR(3) NOT NULL,
	PHUNITS FLOAT(20,6) DEFAULT 1.0,
	PRIMARY KEY(PHASOR_NAMES,PDC_ID,PMU_ID),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)
);

-- ============================================================================================
-- This creates ANALOG channel information Table
-- This table stores details about Analog Configuration like PDC_ID, Analog name, Analog type,
-- Analog units in iPDC Database

CREATE TABLE ANALOG (
	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	ANALOG_NAMES VARCHAR(32) NOT NULL,
	ANALOG_TYPE VARCHAR(6) NOT NULL,
	ANUNITS FLOAT(20,6) DEFAULT 1.0,
	PRIMARY KEY(ANALOG_NAMES,PDC_ID,PMU_ID),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)

);


-- ============================================================================================
-- This creates Digital information Table
-- this table stores details about Digital channel Configuration like Digital Status word etc..
-- in iPDC Database

CREATE TABLE DIGITAL (

	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	DIGITAL_NAMES VARCHAR(32) NOT NULL,
	DIGITAL_WORD BIGINT NOT NULL,
	PRIMARY KEY(DIGITAL_NAMES,PDC_ID,PMU_ID),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)
);

-- ============================================================================================
-- This creates Phasor Data Table
-- this table stores measurements received from phasor data channel(s) in iPDC Database.

CREATE TABLE PHASOR_MEASUREMENTS (
	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	SOC DECIMAL(10) NOT NULL,
	FRACSEC DECIMAL(10) NOT NULL,
	PHASOR_NAME VARCHAR(32) NOT NULL,
	PHASOR_AMPLITUDE REAL NOT NULL,
	PHASOR_ANGLE REAL NOT NULL,
	CONSTRAINT FOREIGN KEY (PHASOR_NAME) REFERENCES PHASOR (PHASOR_NAMES),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)
);

-- ============================================================================================
-- This creates Analog Data Table in iPDC database,
-- this table stores measurements received from Analog data channel(s) in iPDC Database.

CREATE TABLE ANALOG_MEASUREMENTS (
	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	SOC DECIMAL(10) NOT NULL,
	FRACSEC DECIMAL(10) NOT NULL,
	ANALOG_NAME VARCHAR(32) NOT NULL,
	ANALOG_VALUE REAL NOT NULL,
	CONSTRAINT FOREIGN KEY (ANALOG_NAME) REFERENCES ANALOG (ANALOG_NAMES),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)

);

-- ============================================================================================
-- This creates "Digital Measurements" Table
-- this table stores measurements received from Digital data channel in iPDC Database.

CREATE TABLE DIGITAL_MEASUREMENTS (
	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	SOC DECIMAL(10) NOT NULL,
	FRACSEC DECIMAL(10) NOT NULL,
	DIGITAL_WORD INT  NOT NULL,
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)

);

-- ============================================================================================
-- This creates "Frequency Measurements" Table
-- This table stores Frequency measurements received from frequency data channel in iPDC 
-- Database.

CREATE TABLE FREQUENCY_MEASUREMENTS (
	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	SOC DECIMAL(10) NOT NULL,
	FRACSEC DECIMAL(10) NOT NULL,
	FREQ FLOAT (10, 8) NOT NULL,
	DFREQ FLOAT (10, 8) NOT NULL,
	PRIMARY KEY(SOC,FRACSEC,PDC_ID,PMU_ID),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)

);

-- ============================================================================================
-- This creates Received frame Time Table
-- This table stores REceived frame time log in iPDC Database.

CREATE TABLE RECEIVED_FRAME_TIME (
	PDC_ID INT NOT NULL,
	PMU_ID INT NOT NULL,
	FRAME_SOC DECIMAL(10) NOT NULL,
	FRAME_FRACSEC DECIMAL(10) NOT NULL,
	RECEIVE_SOC DECIMAL(10) NOT NULL,
	RECEIVE_FRACSEC DECIMAL(10) NOT NULL,
	TIME_DIFF DECIMAL(10) NOT NULL,
	PRIMARY KEY(FRAME_SOC,FRAME_FRACSEC,PDC_ID,PMU_ID),
	CONSTRAINT FOREIGN KEY (PDC_ID) REFERENCES MAIN_CFG_TABLE (PDC_ID),
	CONSTRAINT FOREIGN KEY (PMU_ID) REFERENCES SUB_CFG_TABLE (PMU_ID)

);
-- ============================================================================================
