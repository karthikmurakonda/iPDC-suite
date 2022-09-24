/* ----------------------------------------------------------------------------- 
 * applications.h
 * 
 * iPDC - Phasor Data Concentrator
 *
 * Copyright (C) 2011-2012 Nitesh Pandit
 * Copyright (C) 2011-2012 Kedar V. Khandeparkar
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Authors: 
 *		Kedar V. Khandeparkar <kedar.khandeparkar@gmail.com>			
 *
 * ----------------------------------------------------------------------------- */

int minHi = 3,maxHi = 5;
int minPi = 100,maxPi = 500;
float fc = 0;
float delFThreshold = 0.3;
int angleThreshold = 20;
float ratio = 1/5;
float *delta,*slipFreuency;
char OOSTD = '0';
//delta = 0;
//slipFreuency = 0;


float averageFrequency(struct data_frame *df);
int frequencyInstability(struct data_frame *df);
int angleInstability(struct data_frame *df);
float voltagePhasorAggregation(struct data_frame *df);
float outOfStepDetection(struct data_frame *df);
int * randomGenerator(int max,int min,int num);
float getVoltagePhasorAngle(int format,char left[],char right[],unsigned char *d,char polar,struct cfg_frame *temp_cfg,int i,int j);
float getPhasorVolageMagnitude(int format,char left[],char right[],unsigned char *d,char polar,struct cfg_frame *temp_cfg,int i,int j);