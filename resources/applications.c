/* ----------------------------------------------------------------------------- 
 * applications.c
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <math.h>       /* fabs */
#include "parser.h"
#include "global.h" 
#include "dallocate.h"
#include "applications.h"

/* ----------------------------------------------------------------------------		*/
/* FUNCTION  averageFrequency():                               	     				*/
/* Computes average of frequency from all PMUs 										*/
/* ----------------------------------------------------------------------------		*/

float averageFrequency(struct data_frame *df) {

	//struct data_frame *df = TSB[index].first_data_frame;
	struct cfg_frame *temp_cfg = cfgfirst;
	struct data_frame *temp_df;
	float tempFreq = 0,avgFrequency;
	int noOfPMU,i,j,count = 0;

	pthread_mutex_lock(&mutex_cfg);

	while(temp_cfg != NULL){
		
		noOfPMU = to_intconvertor(temp_cfg->num_pmu);	
		count += noOfPMU;
		temp_cfg = temp_cfg->cfgnext;	
	}
	
	for(i = 0;i<(count*10-100);i++) {
	
		temp_df = df;		
		temp_cfg = cfgfirst;
		//printf("count = %d\n",count);

		while(temp_cfg != NULL){
			
			noOfPMU = to_intconvertor(temp_cfg->num_pmu);	
		
			if(temp_df != NULL) {
				
				for(j = 0;j < noOfPMU; j++) {

					//printf("a\n");
					if(temp_cfg->pmu[j]->fmt->freq == '1')	{		  					
					
						tempFreq += decode_ieee_single(temp_df->dpmu[j]->freq); 					

					} else {					

						tempFreq += to_intconvertor(temp_df->dpmu[j]->freq); 					
					}
				}	
				temp_df = temp_df->dnext;
			}
			temp_cfg = temp_cfg->cfgnext;	
		}
	}
	pthread_mutex_unlock(&mutex_cfg);
	
	avgFrequency = tempFreq/count;	
	return avgFrequency;
}

/* ----------------------------------------------------------------------------		*/
/* FUNCTION  frequencyInstability():                           	     				*/
/* Computes weighted average of frequency from some PMUs 							*/
/* ----------------------------------------------------------------------------		*/

/*int frequencyInstability(struct data_frame *df) {

	//struct data_frame *df = TSB[index].first_data_frame;
	struct cfg_frame *temp_cfg = cfgfirst;

	float tempFreq = 0,tempFc=0,totalFreq =0,totalHi=0;
	float timeDiff = 20*1e-3;
	float deltaFreq = 0;
	int noOfPMUs = 0,totalPMUs=0,j;

	while(temp_cfg != NULL){

			totalPMUs += to_intconvertor(temp_cfg->num_pmu);	
			temp_cfg = temp_cfg->cfgnext;
	}	

	int generatorPMUs = (totalPMUs*ratio) + 2;  // Considered 1/5*noofPMUs+2 frequencies
	int x = 0,*Hi;
	Hi = randomGenerator(minHi,maxHi,generatorPMUs);
	
	temp_cfg = cfgfirst;

	while(temp_cfg != NULL) {
		
		noOfPMUs = to_intconvertor(temp_cfg->num_pmu);	 

		if(df != NULL) {
			
			for(j=0;j<noOfPMUs;j++) {		  					

				if(temp_cfg->pmu[j]->fmt->freq == '1') {					

					if(x == generatorPMUs)
						break;
					tempFreq = decode_ieee_single(df->dpmu[j]->freq); 
					totalFreq += Hi[x]*tempFreq;	// Sum of Hi*fi (numerator)
					totalHi += Hi[x];				// Sum of Hi (denominator)
					x++;					
				} else {

					if(x == generatorPMUs)
						break;					
					tempFreq = to_intconvertor(df->dpmu[j]->freq); 
					totalFreq += Hi[x]*tempFreq; // Sum of Hi*fi (numerator)
					totalHi += Hi[x];   		  // Sum of Hi (denominator)
					x++;			
				}			 
			} 					
			
			df = df->dnext;
		}
		temp_cfg = temp_cfg->cfgnext;		
	}

	free(Hi);

	if(fc == 0) {

		fc = totalFreq/totalHi;
		return 0;

	} else {

		tempFc = totalFreq/totalHi;
		deltaFreq = fabs(fc-tempFc)/timeDiff;
		/// Need to check deltaFreq < Threshold
		fc = tempFc;
		if(deltaFreq>delFThreshold)
			return 1;
		else return 0;
	}
}
*/

/* ----------------------------------------------------------------------------		*/
/* FUNCTION  angleInstability():                           	     					*/
/* Computes angular deviations from center of inertia 								*/
/* ----------------------------------------------------------------------------		*/

/*int angleInstability(struct data_frame *df) {

	//struct data_frame *df = TSB[index].first_data_frame
	struct cfg_frame *temp_cfg = cfgfirst;
	float totalPi,totalTheta,localAreaPi,*localAreaTheta;
	int totalPMUs,noOfPMU,noOfAreas,noOfPMUsInOneArea,tempTotalPMUs,*localAreaPowerSchedule,*individualPowerSchedule;
	
	noOfPMUsInOneArea = 5;
	totalPi= totalTheta =localAreaPi = 0;

	while(temp_cfg != NULL){

			totalPMUs += to_intconvertor(temp_cfg->num_pmu);	
			temp_cfg = temp_cfg->cfgnext;
	}	

	tempTotalPMUs = totalPMUs;
	float tt;
	tt = (float)(totalPMUs*ratio)+2; // Number of areas eg 200*1/5+2 = 42
	noOfAreas = nearbyint(tt/noOfPMUsInOneArea); // 42/5 = 8.4
	int tt1 = (int)(noOfAreas);				// 8
	int rem = nearbyint(tt%noOfPMUsInOneArea);
	if(fabs(tt-tt1) >0) {	//8.4 - 8 =0.4

		noOfAreas++;		// noOfAreas = 9
	}

	localAreaTheta = malloc((noOfAreas + 1)*sizeof(float));	
	localAreaPowerSchedule = malloc((noOfAreas + 1)*sizeof(int));		
	individualPowerSchedule = randomGenerator(minPi,maxPi,tt);

	temp_cfg = cfgfirst;

	float tempAreaAngle,tempAreaPower;
	unsigned int phnmr,noOfPMUs,pType;
	int x,y,z,i,j; // x to track no of areas and y to track no of generators in those areas z for individual generator schedule
	i = j = x = y = z = 0; 
	tempAreaPower = tempAreaAngle = 0;
	unsigned char *d,*fp_left,*fp_right,*fx_left,*fx_right,*phasorType,polar;

	fp_left = malloc(5*sizeof(unsigned char));
	fp_right = malloc(5*sizeof(unsigned char));
	fx_left = malloc(3*sizeof(unsigned char));
	fx_right = malloc(3*sizeof(unsigned char));
	phasorType = malloc(1*sizeof(unsigned char));

	while(temp_cfg != NULL) { // Main Loop starts
	
		if(df != NULL) {
			
			noOfPMUs = to_intconvertor(temp_cfg->num_pmu);	
			
			for(j = 0;j < noOfPMUs;j++) {

				phnmr = to_intconvertor(temp_cfg->pmu[j]->phnmr);	

				if(phnmr != 0) {
					
					if(temp_cfg->pmu[j]->fmt->phasor == 1) { // Floating	
						
						for(i = 0;i<phnmr;i++) {		
												
							copy_cbyc (phasorType,(unsigned char *)temp_cfg->pmu[j]->phunit[i],1);
							pType = to_intconvertor1(phasorType);

							if((pType == 0)&&(i%3 ==0)) { // pTYpe = 0 indicates V and only phase I is chosen with the assumption that 3 phase data is sent

								d = df->dpmu[j]->phasors[i];	
								polar = temp_cfg->pmu[j]->fmt->polar;	
								tempAreaAngle += (getVoltagePhasorAngle(1,fp_left,fp_right,d,polar,temp_cfg,i,j)) * individualPowerSchedule[z]; // 1 -> floating pt								
								tempAreaPower += individualPowerSchedule[z];
								z++;	
								y++;								

								if(y == rem && (x == noOfAreas - 1)) {
									
									localAreaTheta[x] = tempAreaAngle/tempAreaPower;									
									x++;
									break;

								} else if(y == noOfPMUsInOneArea) {

									localAreaTheta[x] = tempAreaAngle/tempAreaPower;
									y = 0;
									x++;
								}								
							}								
						}					
						
					} else { // fixed point

						for(i = 0;i < phnmr; i++){	

							copy_cbyc (phasorType,(unsigned char *)temp_cfg->pmu[j]->phunit[i],1);
							pType = to_intconvertor1(phasorType);
							
							if((pType == 0)&&(i%3 ==0)) {

		                        d = df->dpmu[j]->phasors[i];	
								polar = temp_cfg->pmu[j]->fmt->polar;	
								tempAreaAngle += getVoltagePhasorAngle(0,fp_left,fp_right,d,polar,temp_cfg,i,j)* individualPowerSchedule[z]; // 0 -> fixed pt								
								tempAreaPower += individualPowerSchedule[z];
								z++;	
								y++;								

								if(y == rem && (x == noOfAreas - 1)) {
									
									localAreaTheta[x] = tempAreaAngle/tempAreaPower;									
									localAreaPowerSchedule[x] = tempAreaPower;
									x++;
									break;

								} else if(y == noOfPMUsInOneArea) {

									localAreaTheta[x] = tempAreaAngle/tempAreaPower;
									localAreaPowerSchedule[x] = tempAreaPower;
									y = 0;
									x++;
								}
							}	                        
						}
						
					}											
				}
			} // if df ends	
			df = df->dnext;	
		}
		temp_cfg = temp_cfg->cfgnext;
	} // Main loop ends

	float centerAngle,totalPower;
	centerAngle = totalPower = 0;

	for (i = 0;i<noOfAreas;i++) {
	
		centerAngle += localAreaTheta[i]*localAreaPowerSchedule[i];
		totalPower += localAreaPowerSchedule[i];
	}
	centerAngle = centerAngle/totalPower;

	char *violation = malloc(noOfAreas*sizeof(char));
	for (i = 0;i<noOfAreas;i++) 
	{
		if(fabs(centerAngle - localAreaTheta[i]) > angleThreshold)
			violation[i] = '1';
		else
			violation[i] = '0';
	}

	free(violation);
	free(localAreaTheta);	
	free(localAreaPowerSchedule);
	free(individualPowerSchedule);
	free(fp_right);
	free(fp_left);
	free(fx_right);
	free(fx_left);
}
*/

/* ----------------------------------------------------------------------------		*/
/* FUNCTION  voltagePhasorAggregation():                          	     					*/
/* Computes voltage phasor phasorAggregation 										*/
/* ----------------------------------------------------------------------------		*/

/*float voltagePhasorAggregation(struct data_frame *df) {

	//struct data_frame *df = TSB[index].first_data_frame;
	struct cfg_frame *temp_cfg = cfgfirst;

	int i,j,noOfPMUs,totalPMUs;
	totalPMUs = i = j = 0;

	while(temp_cfg != NULL){

			totalPMUs += to_intconvertor(temp_cfg->num_pmu);	
			temp_cfg = temp_cfg->cfgnext;
	}

	int generatorPMUs = (totalPMUs*ratio) + 4;
	temp_cfg = cfgfirst;

	unsigned int phnmr;
	char polar;
	float avgVoltageMagnitude = 0;
	unsigned char *d;
	int x = 0;

	unsigned char *fp_left,*fp_right,*fx_left,*fx_right,*phasorType;
	fp_left = malloc(5*sizeof(unsigned char));
	fp_right = malloc(5*sizeof(unsigned char));
	fx_left = malloc(3*sizeof(unsigned char));
	fx_right = malloc(3*sizeof(unsigned char));
	phasorType = malloc(1*sizeof(unsigned char));

	int pType;

	while(temp_cfg != NULL){ // Main Loop starts
	
		if(df != NULL) {
			
			noOfPMUs = to_intconvertor(temp_cfg->num_pmu);	
			
			for(j=0;j<noOfPMUs;j++) {
				phnmr = to_intconvertor(temp_cfg->pmu[j]->phnmr);	
				if(phnmr != 0) {
					if(temp_cfg->pmu[j]->fmt->phasor == 1) { // Floating	

						for(i = 0;i<phnmr;i++) {		

							copy_cbyc (phasorType,(unsigned char *)temp_cfg->pmu[j]->phunit[i],1);
							pType = to_intconvertor1(phasorType);
							if((pType == 0) && (i%3 == 0)) {

								d = df->dpmu[j]->phasors[i];	
								polar = temp_cfg->pmu[j]->fmt->polar;	
								avgVoltageMagnitude += getPhasorVolageMagnitude(1,fp_left,fp_right,d,polar,temp_cfg,i,j); // 1 -> floating pt
							}	
							
						}
						x++;
						if(x == generatorPMUs)
							break; 
					} else { // fixed point

						for(i = 0;i < phnmr; i++){	

							copy_cbyc (phasorType,(unsigned char *)temp_cfg->pmu[j]->phunit[i],1);
							pType = to_intconvertor1(phasorType);
							if((pType == 0) && (i%3 == 0)) {

		                        d = df->dpmu[j]->phasors[i];	
								polar = temp_cfg->pmu[j]->fmt->polar;	
								avgVoltageMagnitude += getPhasorVolageMagnitude(0,fp_left,fp_right,d,polar,temp_cfg,i,j); // 0 -> fixed pt
							}	                        
						}
						x++;
						if(x == generatorPMUs)
							break; 
					}											
				}
			}	
			df = df->dnext;	
		}
		temp_cfg = temp_cfg->cfgnext;
	} // Main loop ends

	free(fp_right);
	free(fp_left);
	free(fx_right);
	free(fx_left);

	return avgVoltageMagnitude;
}
*/

/* ----------------------------------------------------------------------------		*/
/* FUNCTION  randomGenerator():                           	     					*/
/* num random numbers array generated between min to max 							*/
/* ----------------------------------------------------------------------------		*/

int * randomGenerator(int max,int min,int num) {

	int c =0,n,*coeff;
	coeff = malloc((num+1)*sizeof(int));

	while(1) {

    	n = rand()%max + min;

    	if(n>=min && n<= max) {
    		coeff[c] = n;    		
    		c++;

    		if(c == num)
    		break;
    	}   
  	}	
 	return coeff; 	  
}

/* ----------------------------------------------------------------------------		*/
/* FUNCTION  getVoltagePhasorAngle():                           	     					*/
/* get the phase angle 																*/
/* ----------------------------------------------------------------------------		*/

/*float getVoltagePhasorAngle(int format,char left[],char right[],unsigned char *d,char polar,struct cfg_frame *temp_cfg,int i,int j) {

	float fp_r,fp_i,fp_angle;
	unsigned int f_r,f_i;

	
	if(format == 1) { // format indicates fixed or floating point

		memset(left,'\0',5);
		memset(right,'\0',5);
		//d = df->dpmu[j]->phasors[i];						

		copy_cbyc (left,d,4);
		left[4] = '\0';
		d += 4;

		copy_cbyc(right,d,4);
		right[4] = '\0';   
		d += 4;

		fp_r = decode_ieee_single(left);
		fp_i = decode_ieee_single(right);

		if(polar == '1') { // POLAR
					
			fp_angle = fp_i;
		} else { // rectangular

			fp_angle = atan2f(fp_i, fp_r);
		}

	} else {


        memset(left,'\0',3);
        memset(right,'\0',3);

		copy_cbyc (left,d,2);
        left[2] = '\0';
        d += 2;

        copy_cbyc(right,d,2);
        right[2] = '\0';   
        d += 2;

        f_r = to_intconvertor(left);
        f_i = to_intconvertor(right);

        if(polar == '1') { // POLAR
              
            fp_angle = f_i*1e-4; // Angle is in 10^4 radians
        } else // RECTANGULAR 
        {
        	unsigned char *s = (unsigned char *)temp_cfg->pmu[j]->phunit[i];
        	s++;

        	unsigned char *buf = malloc(4*sizeof(unsigned char));        	
        	copy_cbyc (buf,(unsigned char *)s,3);
			buf[3] = '\0';
			unsigned long int l_phunit;
			l_phunit = to_long_int_convertor1(buf);
			l_phunit = l_phunit * 1e-5;

         	fp_r = l_phunit *f_r;
            fp_i = l_phunit *f_i;
                            
            fp_angle = atan2f(fp_i, fp_r);
            free(buf);
       	}

	}	
	return fp_angle;
}
*/
/* ----------------------------------------------------------------------------		*/
/* FUNCTION  getPhasorVolageMagnitude():                           	     			*/
/* get the phasor volage magnitude 													*/
/* ----------------------------------------------------------------------------		*/

/*float getPhasorVolageMagnitude(int format,char left[],char right[],unsigned char *d,char polar,struct cfg_frame *temp_cfg,int i,int j) {

	float fp_r,fp_i,fp_magnitude;
	unsigned int f_r,f_i;
	
	if(format == 1) { // format indicates fixed or floating point

		memset(left,'\0',5);
		memset(right,'\0',5);
		//d = df->dpmu[j]->phasors[i];						

		copy_cbyc (left,d,4);
		left[4] = '\0';
		d += 4;

		copy_cbyc(right,d,4);
		right[4] = '\0';   
		d += 4;

		fp_r = decode_ieee_single(left);
		fp_i = decode_ieee_single(right);

		if(polar == '1') { // POLAR
					
			fp_magnitude = fp_r;
		} else { // rectangular

			fp_magnitude = sqrt(fp_i*fp_i + fp_r*fp_r);
		}

	} else {


        memset(left,'\0',3);
        memset(right,'\0',3);

		copy_cbyc (left,d,2);
        left[2] = '\0';
        d += 2;

        copy_cbyc(right,d,2);
        right[2] = '\0';   
        d += 2;

        f_r = to_intconvertor(left);
        f_i = to_intconvertor(right);

	    unsigned char *s = (unsigned char *)temp_cfg->pmu[j]->phunit[i];
        s++;

        unsigned char *buf = malloc(4*sizeof(unsigned char));        	
        copy_cbyc (buf,(unsigned char *)s,3);
		buf[3] = '\0';
		unsigned long int l_phunit;
		l_phunit = to_long_int_convertor1(buf);
		l_phunit = l_phunit * 1e-5;
		free(buf);
        
        if(polar == '1') { // POLAR
              
            fp_magnitude = f_r*l_phunit; 

        } else // RECTANGULAR 
        {        	
         	fp_r = l_phunit *f_r;
            fp_i = l_phunit *f_i;
                            
            fp_magnitude = sqrt(fp_i*fp_i + fp_r*fp_r);           
       	}
	}	
	return fp_magnitude;
}
*/
/* ----------------------------------------------------------------------------		*/
/* FUNCTION  outOfStepDetection(): 		                          	     			*/
/* angle deviations 																*/
/* ----------------------------------------------------------------------------		*/

/*float outOfStepDetection(struct data_frame *df) {

	//struct data_frame *df = TSB[index].first_data_frame;
	struct cfg_frame *temp_cfg = cfgfirst;

	int i,j,noOfPMUs,totalPMUs;
	totalPMUs = i = j = 0;

	while(temp_cfg != NULL){

			totalPMUs += to_intconvertor(temp_cfg->num_pmu);	
			temp_cfg = temp_cfg->cfgnext;
	}

	int generatorPMUs = (totalPMUs*ratio);
	if(generatorPMUs%2 == 1) 
		generatorPMUs +=1;
	
	temp_cfg = cfgfirst;

	if(OOSTD == '0') {

		delta = malloc((generatorPMUs+1)*sizeof(float));		
		if(delta == NULL) 
			printf("no memory for delta\n");	
		
		slipFreuency = malloc((generatorPMUs+1)*sizeof(float));			
		if(slipFreuency == NULL)
			printf("no memory for slipFreuency\n");
		OOSTD = '1';
	}

	unsigned int phnmr;
	char polar;
	float phasorAngle = 0;
	unsigned char *d;
	int x = 0;

	unsigned char *fp_left,*fp_right,*fx_left,*fx_right,*phasorType;
	fp_left = malloc(5*sizeof(unsigned char));
	fp_right = malloc(5*sizeof(unsigned char));
	fx_left = malloc(3*sizeof(unsigned char));
	fx_right = malloc(3*sizeof(unsigned char));
	phasorType = malloc(1*sizeof(unsigned char));

	int pType;

	while(temp_cfg != NULL){ // Main Loop starts
	
		if(df != NULL) {
			
			noOfPMUs = to_intconvertor(temp_cfg->num_pmu);	
			
			for(j=0;j<noOfPMUs;j++) {
				phnmr = to_intconvertor(temp_cfg->pmu[j]->phnmr);	
				if(phnmr != 0) {
					if(temp_cfg->pmu[j]->fmt->phasor == 1) { // Floating	

						for(i = 0;i<phnmr;i++) {		

							copy_cbyc (phasorType,(unsigned char *)temp_cfg->pmu[j]->phunit[i],1);
							pType = to_intconvertor1(phasorType);
							if((pType == 0) && (i%3 == 0)) {

								d = df->dpmu[j]->phasors[i];	
								polar = temp_cfg->pmu[j]->fmt->polar;	
								phasorAngle = getVoltagePhasorAngle(1,fp_left,fp_right,d,polar,temp_cfg,i,j); // 1 -> floating pt
							}	
							
						}
						x++;
						if(x == generatorPMUs)
							break; 
					} else { // fixed point

						for(i = 0;i < phnmr; i++){	

							copy_cbyc (phasorType,(unsigned char *)temp_cfg->pmu[j]->phunit[i],1);
							pType = to_intconvertor1(phasorType);
							if((pType == 0) && (i%3 == 0)) {

		                        d = df->dpmu[j]->phasors[i];	
								polar = temp_cfg->pmu[j]->fmt->polar;	
								phasorAngle = getVoltagePhasorAngle(0,fp_left,fp_right,d,polar,temp_cfg,i,j); // 0 -> fixed pt
							}	                        
						}
						x++;
						if(x == generatorPMUs)
							break; 
					}											
				}
			}	
			df = df->dnext;	
		}
		temp_cfg = temp_cfg->cfgnext;
	} // Main loop ends

	free(fp_right);
	free(fp_left);
	free(fx_right);
	free(fx_left);

	return 0;
}*/
