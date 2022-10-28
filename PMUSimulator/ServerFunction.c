/* ----------------------------------------------------------------------------- 
 * ServerFunction.c
 * 
 * PMU Simulator - Phasor Measurement Unit Simulator
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
 *		Nitesh Pandit <panditnitesh@gmail.com>
 *		Kedar V. Khandeparkar <kedar.khandeparkar@gmail.com>			
 *
 * ----------------------------------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "function.h"
#include "ServerFunction.h"
#include "ShearedMemoryStructure.h"


/* -------------------------------------------------------------------------------------- */
/*                            Functions in ServerFunction.c                               */
/* -------------------------------------------------------------------------------------- */

/* ----------------------------------------- */
/*                                           */
/*   1. int   get_header_frame();            */
/*	2. void  frame_size();                  */
/*	3. void  generate_data_frame();		*/
/*	4. void* udp_send_data();			*/
/*	5. void* pmu_udp();					*/
/*	6. void* tcp_send_data(void * newfd);	*/
/*	7. void* new_pmu_tcp(void * nfd);		*/
/*	8. void* pmu_tcp();					*/
/*	9. void  start_server();				*/
/*   10.void  SIGUSR1_handler(int);          */
/*   11.void  SIGUSR2_handler(int);          */
/*                                           */
/* ----------------------------------------- */


/* ---------------------------------------------------------------- */
/*                         global variables                         */
/* ---------------------------------------------------------------- */

int df_pmu_id, df_fdf, df_af, df_pf, df_pn, df_phnmr, df_annmr, df_dgnmr;
int df_data_frm_size = 0, old_data_rate = 0, cfg_size, hdr_size=0;
int count = 0, pmuse=0, sc1 = 0, tcp_port, udp_port, tmp_wait = 1, df_fnom;
int UDP_sockfd, TCP_sockfd, TCP_sin_size, UDP_addr_len, PhasorType[50];
int udp_cfg_flag = 0, tcp_cfg_flag = 0, tcp_data_flag = 0, udp_data_flag = 0;
int err, errno, udp_data_trans_off = 1, tcp_data_trans_off = 1, stat_flag = 0;

int yes = 1; 	/* argument to setsockopt */
int df_data_rate = 0;
int fsecNum = 0, PhasorType[50];
long int df_soc, fsec = 0, curnt_soc = 0, prev_soc = 0,soc1,soc2;
long int send_thrd_id = 0;

/* Initialize the pthread_mutex for PDC Objects */
pthread_mutex_t mutex_pdc_object = PTHREAD_MUTEX_INITIALIZER;

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  get_header_frame():							               */
/* This function get the header frame from the PMU Setup File.                  */
/* ----------------------------------------------------------------------------	*/

int get_header_frame()
{
	/* Local variables */
	int tempi;
	char *rline = NULL, *d1;
	ssize_t read;
	size_t len = 0;
	FILE *fp1;

	/* Open the PMU Setup File to read the header frame, if present in file? */
	fp1 = fopen (pmuFilePath,"rb");

	tempi = 1;

     /* Read all the unnecessary lines - PMUServer and CFG */
	while(tempi < 6)
	{
		read = getline(&rline, &len, fp1);

		if(read == 0)
			break;

		tempi++;
	}

	if(read > 0)
	{
		d1 = strtok (rline," ");
		d1 = strtok (NULL," ");
		d1 = strtok (NULL,"\n");
		tempi = atoi(d1);

		if(tempi > 0)
		{
			fread(hdrline, sizeof(unsigned char), tempi, fp1);
			fclose(fp1);
			return 1;
		}
	}
	else
	{
		printf("\nHeader Frame is not created by the PMU operator.\n");
	}

	return 0;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  frame_size():							                    */
/* Function To calculate the recent data frame size via reading cfg frm.        */
/* Also initializing some of global variables. 	                              */
/* ----------------------------------------------------------------------------	*/

void frame_size()
{
	/* Local variables */
	int format, i, j;
	int tempi,index=2;
	long int temp_li; 
	char filename[200];
	char *rline = NULL, *d1;
	ssize_t read;
	size_t len = 0;
	FILE *fp1;

	strcpy(filename, pmuFilePath);

	/* Open the PMU Setup File and read the last CFG frame */
	fp1 = fopen (filename,"rb");

	if (fp1 != NULL)			
	{
		tempi = 1;

          /* Read all the unnecessary lines - PMUServer only */
		while(tempi < 4)
		{
			read = getline(&rline, &len, fp1);

			if(read == 0)
				break;

			tempi++;
		}

		if(read > 0)
		{
			d1 = strtok (rline," ");
			d1 = strtok (NULL," ");
			tempi = atoi(d1);

			if (tempi == 1)
			{
				memset(cline,'\0',sizeof(cline));

				d1 = strtok (NULL,"\n");
				tempi = atoi(d1);

                    /* Copy the complete Configuration frame into an unsigned charactor array called cline */
				fread(cline, sizeof(unsigned char), tempi, fp1);
				fclose(fp1);

				/* Get the CFG size & store globally */
				df_temp[0] = cline[index++];
				df_temp[1] = cline[index++];
				cfg_size = c2i(df_temp);
				cline[cfg_size] = '\0';

				/* Get the PMU ID from CFG FRM & store globally */
				df_temp[0] = cline[index++];
				df_temp[1] = cline[index++];
				df_pmu_id = c2i(df_temp);
				index = index + 32;

				/* Get the FORMAT word from CFG FRM */
				df_temp[0] = cline[index++];
				df_temp[1] = cline[index++];
				format = c2i(df_temp);

				/* Initialize the format bits as in his appropriate global variable */
				if(format == 15)
				{
					df_fdf=1, df_af=1, df_pf=1, df_pn=1;
				}
				else if(format == 14)
				{
					df_fdf=1, df_af=1, df_pf=1, df_pn=0;
				}
				else if(format == 13)
				{
					df_fdf=1, df_af=1, df_pf=0, df_pn=1;
				}
				else if(format == 12)
				{
					df_fdf=1, df_af=1, df_pf=0, df_pn=0;
				}
				else if(format == 11)
				{
					df_fdf=1, df_af=0, df_pf=1, df_pn=1;
				}
				else if(format == 10)
				{
					df_fdf=1, df_af=0, df_pf=1, df_pn=0;
				}
				else if(format == 9)
				{
					df_fdf=1, df_af=0, df_pf=0, df_pn=1;
				}
				else if(format == 8)
				{
					df_fdf=1, df_af=0, df_pf=0, df_pn=0;
				}
				else if(format == 7)
				{
					df_fdf=0, df_af=1, df_pf=1, df_pn=1;
				}
				else if(format == 6)
				{
					df_fdf=0, df_af=1, df_pf=1, df_pn=0;
				}
				else if(format == 5)
				{
					df_fdf=0, df_af=1, df_pf=0, df_pn=1;
				}
				else if(format == 4)
				{
					df_fdf=0, df_af=1, df_pf=0, df_pn=0;
				}
				else if(format == 3)
				{
					df_fdf=0, df_af=0, df_pf=1, df_pn=1;
				}
				else if(format == 2)
				{
					df_fdf=0, df_af=0, df_pf=1, df_pn=0;
				}
				else if(format == 1)
				{
					df_fdf=0, df_af=0, df_pf=0, df_pn=1;
				}
				else
				{
					df_fdf=0, df_af=0, df_pf=0, df_pn=0;
				}

				/* Get the PHNMR from CFG FRM & store globally */
				df_temp[0] = cline[index++];
				df_temp[1] = cline[index++];
				df_phnmr = c2i(df_temp);

				/* Get the df_annmr from CFG FRM & store globally */
				df_temp[0] = cline[index++];
				df_temp[1] = cline[index++];
				df_annmr = c2i(df_temp);

				/* Get the df_dgnmr from CFG FRM & store globally */
				df_temp[0] = cline[index++];
				df_temp[1] = cline[index++];
				df_dgnmr = c2i(df_temp);

				/* To escape the some of fields in cfg frame */
				index = index + (16*df_phnmr) + (16*df_annmr) + (256*df_dgnmr);

				/* Extract the value of PHUNIT for each Phasor channel */
				for(i=0, j=0; i<(df_phnmr); i++, j++)
				{
					if(cline[index++] == 0)
						temp_pahsor_type[j] = 0;
					else
						temp_pahsor_type[j] = 1;

					temp_li =cline[index++];
					temp_li<<=8;
					temp_li |=cline[index++];
					temp_li<<=8;
					temp_li |=cline[index++];

					temp_PHUNIT_val[j] = temp_li;
				}

				/* Extract the value of ANUNIT for each Analog channel */
				for(i=0, j=0; i<(df_annmr); i++, j++)
				{
					temp_analog_type[j] = (int)cline[index++];

					temp_li =cline[index++];
					temp_li<<=8;
					temp_li |=cline[index++];
					temp_li<<=8;
					temp_li |=cline[index++];

					temp_ANUNIT_val[j] = temp_li;
				}
				index = index + (4*df_dgnmr); // for skiping 2 byte for DIGUNIT

				df_temp[0] = cline[index++];
				df_temp[1] = cline[index++];
				int temp_df_fnom = c2i(df_temp);
				if(temp_df_fnom == 0)
                         {
     					df_fnom = 60;
                              printf("Set Fnom = 60\n");                    
                         }
				else
                         {
     					df_fnom = 50;
                              printf("Set Fnom = 50\n");                    
                         }

				df_temp[0] = cline[cfg_size-4];
				df_temp[1] = cline[cfg_size-3];
				df_data_rate = c2i(df_temp);

				/* Calculate the data frame size */
				df_data_frm_size = 0;
				df_data_frm_size = df_data_frm_size + 18;	/* 18 Bytes or 36 char is sum of all static fields in data frame */

				/* Calculate 4/8 bytes for each PHNMR & store globally */
				if (df_pf == 0)
				{
					df_data_frm_size = df_data_frm_size + (4*df_phnmr);
				}
				else
				{
					df_data_frm_size = df_data_frm_size + (8*df_phnmr);
				}

				/* Calculate 2/4 bytes for each df_annmr & store globally */
				if (df_af == 0)
				{
					df_data_frm_size = df_data_frm_size + (2*df_annmr);
				}
				else
				{
					df_data_frm_size = df_data_frm_size + (4*df_annmr);
				}

				/* Calculate 2/4 bytes for both (FREQ + DFREQ) & store globally */
				if (df_fdf == 0)
				{
					df_data_frm_size = df_data_frm_size + 4;
				}
				else
				{
					df_data_frm_size = df_data_frm_size + 8;
				}

				/* Calculate 2 bytes for each DGNMR & store globally */
				df_data_frm_size = df_data_frm_size + (2*df_dgnmr);
				printf("PMU Server : Calculated data frame would be %d Bytes.\n", df_data_frm_size);
			}
		}
	} /* end of else of fopen*/

	else
		exit(1);
} /* end of function frame_size() */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  generate_data_frame():	               					*/
/* Function to generate the data frame. Based on the Configuration Frame 	     */
/* attributes.                          					               */
/* ----------------------------------------------------------------------------	*/

void generate_data_frame()
{
	/* local variables */
	int freqI, phasorI, analogI;
	int indx = 0, j, temp_i, freq, dfreq, dsw = 0, ka = 1;
	int analog[3] = {100, 1000, 10000}, rand_ph, rand_ang;
	long int freq_f, dfreq_f, analog_f;

	float freqF, phasorF, analogF;
	float phasor = 100.85, angle, result, TB;
     struct timespec *cal_timeSpec, *cal_timeSpec1;
     cal_timeSpec = malloc(sizeof(struct timespec));
     cal_timeSpec1 = malloc(sizeof(struct timespec));
    
     TB = powf(2,24);

	/* If configuration has changed then call the function "frame_size()" to read new CFG and
        reinitialize global variables & generate new Data frames with STAT word bit-10(CFG change bit)
        set to 1 till CFG request not received from connected PDC */

	memset(data_frm,'\0',sizeof(df_data_frm_size));

	/* Insert SYNC Word in data frame */
	data_frm[indx++] = 0xAA; 
	data_frm[indx++] = 0x01; 

	/* Insert data frame size in data frame */
	i2c(df_data_frm_size, df_temp);
	B_copy(data_frm, df_temp, indx, 2);
	indx = indx + 2;

	/* Insert PMU ID in data frame */
	i2c(df_pmu_id, df_temp);
	B_copy(data_frm, df_temp, indx, 2);
	indx = indx + 2;

	/* Insert SOC value in data frame */
     /* No PPS so have to manage by seeing local time */
     clock_gettime(CLOCK_REALTIME, cal_timeSpec);
	if (fsecNum >= df_data_rate)
	{	
		cal_timeSpec->tv_sec ++;
        fsecNum = 0;
	}
	df_soc = (long)cal_timeSpec->tv_sec;
	li2c(df_soc, df_temp_1);
	B_copy(data_frm, df_temp_1, indx, 4);
	indx = indx + 4;

	/* Insert Time Quality flag + fraction of second in data frame */
     fsec = roundf(fsecNum*TB/df_data_rate);
	li2c(fsec, df_temp_1);
	B_copy(data_frm, df_temp_1, indx, 4);
	indx = indx + 4;
	fsecNum += 1;   

	/* Insert STAT Word in data frame Default or Changed */
	time_t curnt_soc = time(NULL);
	if(pmuse == 0) 
	{
		prev_soc = curnt_soc;
	} 

	if((curnt_soc-prev_soc) > 1)
	{ 
		printf("\tSTAT word Changed due to PMU SYNC Error.");
		data_frm[indx++] = 0x20;
		data_frm[indx++] = 0x00;
	}
	else
	{
		/* If not insert default STAT Word: 0000 */
		data_frm[indx++] = 0x00;
		data_frm[indx++] = 0x00;
	}

	prev_soc = curnt_soc;
	pmuse = 1;

	/*----------------Auto Generated Data------------------*/	

	if(dataFileVar == 0)
	{
		/* Insert Fix point phasor values in data frame */
		if(df_pf == 0)		
		{
			/* For rendom phasor values */
			if(df_pn == 0)	/* Rectangular */
			{
				for(j=0; j<df_phnmr; j++)
				{
					rand_ph = rand() % 9 + 1;
					rand_ang = rand() % 29 + 1;
					angle = 120.89;

					if(ka == 1)
					{
						phasor = phasor + rand_ph;
						angle = angle + rand_ang;
						ka = 0;
					}
					else
					{
						phasor = phasor - rand_ph;
						angle = angle - rand_ang;
						ka = 1;
					}

					angle = (angle*3.1415)/180;
					result = cos(angle)*phasor;
					temp_i = 65535+result;
					i2c(temp_i, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;

					temp_i = sin(angle)*phasor;
					i2c(temp_i, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;
				}
			}
			else	/* Polar */
			{
				/* For rendom phasor values */
				for(j=0; j<df_phnmr; j++)
				{
					rand_ph = rand() % 9 + 1;
					rand_ang = rand() % 29 + 1;
					angle = 120.89;

					if(ka == 1)
					{
						phasor = phasor + rand_ph;
						angle = angle + rand_ang;
						ka = 0;
					}
					else
					{
						phasor = phasor - rand_ph;
						angle = angle - rand_ang;
						ka = 1;
					}

					angle = ((angle*3.1415)/180)*100000;
					temp_i = phasor*100000;
					i2c(temp_i, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;

					temp_i = angle;
					i2c(temp_i, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;
				}
			}
		}
		else	      /* Insert Floating point phasor values in data frame */
		{
			if(df_pn == 0)	/* Rectangular */
			{
				/* For rendom phasor values */
				for(j=0; j<df_phnmr; j++)
				{
					rand_ph = rand() % 9 + 1;
					rand_ang = rand() % 29 + 1;
					angle = 120.89;

					if(ka == 1)
					{
						phasor = phasor + rand_ph;
						angle = angle + rand_ang;
						ka = 0;
					}
					else
					{
						phasor = phasor - rand_ph;
						angle = angle - rand_ang;
						ka = 1;
					}

					angle = (angle*3.1415)/180;
					result = cos(angle)*phasor;
					result = 65535+result;
					f2c(result, df_temp_1);
					B_copy(data_frm, df_temp_1, indx, 4);
					indx = indx + 4;

					result = sin(angle)*phasor;
					f2c(result, df_temp_1);
					B_copy(data_frm, df_temp_1, indx, 4);
					indx = indx + 4;
				}
			}
			else	  /* Polar */
			{
				/* For rendom phasor values */
				for(j=0; j<df_phnmr; j++)
				{
					rand_ph = rand() % 9 + 1;
					rand_ang = rand() % 29 + 1;
					angle = 120.89;

					if(ka == 1)
					{
						phasor = phasor + rand_ph;
						angle = angle + rand_ang;
						ka = 0;
					}
					else
					{
						phasor = phasor - rand_ph;
						angle = angle - rand_ang;
						ka = 1;
					}

					angle = (angle*3.1415)/180;
					result = phasor;
					f2c(result, df_temp_1);
					B_copy(data_frm, df_temp_1, indx, 4);
					indx = indx + 4;

					result = angle;
					f2c(result, df_temp_1);
					B_copy(data_frm, df_temp_1, indx, 4);
					indx = indx + 4;
				}
			}
		}

		/* Insert Fix point Frequency & DFrequency values in data frame */
		if(df_fdf == 0)
		{
			/* For rendom values of FREQ & DFREQ */
			freq = (rand() % 5 + 1)*100;
			i2c(freq, df_temp);
			B_copy(data_frm, df_temp, indx, 2);
			indx = indx + 2;

			dfreq = 0;
			i2c(dfreq, df_temp);
			B_copy(data_frm, df_temp, indx, 2);
			indx = indx + 2;
		}
		else	      	/* Insert Floating point Frequency & DFrequency values in data frame */
		{
			/* For rendom values of FREQ & DFREQ */
			freqF = (rand() % 5 + 1)*100;
			f2c(freqF, df_temp_1);
			B_copy(data_frm, df_temp_1, indx, 4);
			indx = indx + 4;

			float dfreqF = 0;
			dfreqF = (rand() % 5 + 1)*0.00639;
			i2c(dfreqF, df_temp_1);
			B_copy(data_frm, df_temp_1, indx, 4);
			indx = indx + 4;
		}

		/* Insert Fix point Analog values in data frame */
		if(df_af == 0)
		{
			for(j=0, ka=0; ka<df_annmr; j++, ka++)
			{
				if (j == 3) j = 0;
				i2c(analog[j], df_temp);
				B_copy(data_frm, df_temp, indx, 2);
				indx = indx + 2;
			}
		}
		else      /* Insert Floating point Analog values in data frame */
		{
			for(j=0, ka=0; ka<df_annmr; j++, ka++)
			{
				if (j == 3) j = 0;
				analog_f = analog[j];
				li2c(analog_f, df_temp_1);
				B_copy(data_frm, df_temp_1, indx, 4);
				indx = indx + 4;
			}
		}
	}

	/*----------------Read measurements from file------------------*/	

	else
	{
		char *mData, *d1, *d2;
		int j;

		mData = measurement_Return ();
		d1 = strtok (mData,","); 

		/* Insert Fix point phasor values in data frame */
		if(df_pf == 0)		
		{
			for(j=0; j<df_phnmr; j++)
			{
				if(df_pn == 0)	/* Rectangular Values */
				{
					d1 = strtok (NULL,",\""); 
					phasorI = (atof(d1)*100000/temp_PHUNIT_val[j]);
					i2c(phasorI, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;

					d2 = strtok (NULL,",\""); 
					phasorI = (atof(d2)*100000/temp_PHUNIT_val[j]);
					i2c(phasorI, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;
				}
				else	/* Polar Values */
				{
					d1 = strtok (NULL,",\""); 
					phasorI = (atof(d1)*100000/temp_PHUNIT_val[j]);
					i2c(phasorI, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;

					d2 = strtok (NULL,",\""); 
					phasorI = (((atof(d2)*M_PI)/180)*10000);
					i2c(phasorI, df_temp);
					B_copy(data_frm, df_temp, indx, 2);
					indx = indx + 2;
				}
			}
		}
		else	      /* Insert Floating point phasor values in data frame */
		{
			for(j=0; j<df_phnmr; j++)
			{
				d1 = strtok (NULL,",\""); 
				phasorF = atof(d1);
				f2c(phasorF, df_temp_1);
				B_copy(data_frm, df_temp_1, indx, 4);
				indx = indx + 4;

				d2 = strtok (NULL,",\""); 
				phasorF = atof(d2);
				f2c(phasorF, df_temp_1);
				B_copy(data_frm, df_temp_1, indx, 4);
				indx = indx + 4;
			}
		}

		/* Insert Fix point Frequency & DFrequency values in data frame */
		if(df_fdf == 0)
		{
			/* For values of FREQ & DFREQ */
			d1 = strtok (NULL,",\""); 
			freqI = (atof(d1)-df_fnom)*1000;
			i2c(freqI, df_temp);
			B_copy(data_frm, df_temp, indx, 2);
			indx = indx + 2;

			d2 = strtok (NULL,",\""); 
			freqI = (atof(d2)*100);
			i2c(freqI, df_temp);
			B_copy(data_frm, df_temp, indx, 2);
			indx = indx + 2;
		}
		else	      	/* Insert Floating point Frequency & DFrequency values in data frame */
		{
			/* For values of FREQ & DFREQ */
			d1 = strtok (NULL,",\""); 
			freqF = atof(d1);
			f2c(freqF, df_temp_1);
			B_copy(data_frm, df_temp_1, indx, 4);
			indx = indx + 4;

			d2 = strtok (NULL,",\""); 
			freqF = atof(d2);
			f2c(freqF, df_temp_1);
			B_copy(data_frm, df_temp_1, indx, 4);
			indx = indx + 4;
		}

		/* Insert Fix point Analog values in data frame */
		if(df_af == 0)
		{
			for(j=0; j<df_annmr; j++)
			{
				d1 = strtok (NULL,",\"");
				analogI = (atof(d1)*1e5/temp_ANUNIT_val[j]);
				i2c(analogI, df_temp);
				B_copy(data_frm, df_temp, indx, 2);
				indx = indx + 2;
			}
		}
		else      /* Insert Floating point Analog values in data frame */
		{
			for(j=0; j<df_annmr; j++)
			{
				d2 = strtok (NULL,",\""); 
				analogF = (atof(d2));
				f2c(analogF, df_temp_1);
				B_copy(data_frm, df_temp_1, indx, 4);
				indx = indx + 4;
			}
		}
	} /* end of measurements from file */

	/* Insert Digital values in data frame */
	for(j=1; j<=df_dgnmr; j++)
	{
		i2c(dsw, df_temp);
		B_copy(data_frm, df_temp, indx, 2);
		indx = indx + 2;
	}

	/* Calculate and insert the Checksum value in data frame (till now) */
	df_chk = compute_CRC(data_frm,indx);

     /* Right checksum calculation */
	if (cfg_crc_error == 0)
	{
	     data_frm[indx++] = (df_chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte; */
	     data_frm[indx++] = (df_chk ) & ~(~0<<8);     	/* CHKSUM low byte;  */
	}
	else
	{
		printf("\nInvalid CheckSum in a sending Data Frame.\n");
	     data_frm[indx++] = (df_chk ) & ~(~0<<8);     	/* CHKSUM low byte;  */
	     data_frm[indx++] = (df_chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte; */
          cfg_crc_error = 0;
	}

} /* end of function generate_data_frame() */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  void* SEND_DATA       	               					*/
/* This function run by a seprate thread only for data transmission.            */
/* Function to generate and send the data frame periodically to client's   	*/
/* destination address or to PDC (client).                                      */
/* ----------------------------------------------------------------------------	*/

void* SEND_DATA()
{
     /* Wait till server will get Setup file path */
	while(df_data_rate == 0) usleep(1000);

     /* Calculate the waiting time during sending data frames */
	int data_waiting = 1e9/df_data_rate, i=0;
     struct PDC_Details *temp_pdc;
     send_thrd_id = pthread_self();

     struct timespec *cal_timeSpec, *cal_timeSpec1;
     cal_timeSpec = malloc(sizeof(struct timespec));
     cal_timeSpec1 = malloc(sizeof(struct timespec));

	clock_gettime(CLOCK_REALTIME, cal_timeSpec);

    	while(1)
    	{
        	clock_gettime(CLOCK_REALTIME, cal_timeSpec1);
        	clock_gettime(CLOCK_REALTIME, cal_timeSpec);

        	if (cal_timeSpec->tv_sec > cal_timeSpec1->tv_sec)
        	{
            		fsecNum = 1;
            		break;
        	}
    	}
	

	while(1)
	{
        	if (i != 0)
        	{
            		cal_timeSpec->tv_nsec += data_waiting;
        	}
        	else
        	{
            		cal_timeSpec->tv_nsec = data_waiting;
        	}
        	if ((cal_timeSpec->tv_nsec) >= 1e9)
        	{
            		cal_timeSpec->tv_sec++;
            		cal_timeSpec->tv_nsec-=1e9;
        	}

		/* Call the function generate_data_frame() to create a fresh new Data Frame */
		generate_data_frame();

		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, cal_timeSpec, cal_timeSpec1);

          temp_pdc = PDCfirst;

          pthread_mutex_lock(&mutex_pdc_object);

          while(temp_pdc != NULL)
          {
               if(!strncasecmp(temp_pdc->protocol, "UDP", 3) && (temp_pdc->data_transmission == 0))
               {
                    /* If STAT Word bits got changed by user */
                    if(temp_pdc->STAT_change != 0)     
                    {
                         switch (temp_pdc->STAT_change) 
                         {
                              case 1:   
		                         data_frm[14] = 0x04;     //CFG changed
		                         data_frm[15] = 0x00;
                                   break;
                              case 2:
		                         data_frm[14] = 0x80;
		                         data_frm[15] = 0x00;
                                   temp_pdc->STAT_change = 0;
                                   break;
                              case 3:
		                         data_frm[14] = 0x40;     //PMU error
		                         data_frm[15] = 0x00;
                                   break;
                              case 4:
		                         data_frm[14] = 0x10;
		                         data_frm[15] = 0x00;
                                   temp_pdc->STAT_change = 0;
                                   break;
                              case 5:
		                         data_frm[14] = 0x08;
		                         data_frm[15] = 0x00;
                                   temp_pdc->STAT_change = 0;
                                   break;
                         }
                    }

                   	/* UDP-Send the newly created data frame to connected PDC address */
                    if (sendto (temp_pdc->sockfd,data_frm, df_data_frm_size, 0,
                                 		(struct sockaddr *)&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr)) == -1) {

		               perror("sendto");
	               }
               }
               else if(!strncasecmp(temp_pdc->protocol, "TCP", 3) && (temp_pdc->data_transmission == 0))
               {    
				if(temp_pdc->tcpup == 1)
				{
		               /* TCP-Send the newly created data frame to connected PDC address */
				     if (send(temp_pdc->sockfd, data_frm, df_data_frm_size, 0) == -1) {

					          perror("sendto");
				          }
				}
               }
               temp_pdc = temp_pdc->next;
          }
          pthread_mutex_unlock(&mutex_pdc_object);

		i++;
        	clock_gettime(CLOCK_REALTIME, cal_timeSpec1);

	}    //while-2 ends here

} /* end of function udp_send_data() */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION void PDC_MATCH(int proto, int newfd):		                    */
/* This function will maintain the linked list of communicated PDC for          */
/* UDP and TCP PDC clients.                                                     */
/* ----------------------------------------------------------------------------	*/

void PDC_MATCH(int proto, int newfd)
{
     int flag = 1;
     struct PDC_Details *temp_pdc;

	pthread_mutex_lock(&mutex_pdc_object);

	if(PDCfirst != NULL)
	{
		temp_pdc = PDCfirst;

		while(temp_pdc != NULL ) {

		     if(!strncasecmp(temp_pdc->protocol,"UDP",3))
		    	{
				if(!strcmp(temp_pdc->ip,inet_ntoa(UDP_addr.sin_addr)))
                    {
					/* Only replace the new conn details with old? */
				     strcpy(temp_pdc->ip, inet_ntoa(UDP_addr.sin_addr));     // ip
				     strncpy(temp_pdc->protocol,"UDP",3); // protocol
				     temp_pdc->protocol[3] = '\0';
				     temp_pdc->port = ntohs(UDP_addr.sin_port);   //UDP_addr.sin_port

				     bzero(&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr));
				     temp_pdc->pdc_addr.sin_family = AF_INET;
				     temp_pdc->pdc_addr.sin_addr.s_addr =  inet_addr(temp_pdc->ip);
				     temp_pdc->pdc_addr.sin_port = htons(temp_pdc->port);
				     memset(&(temp_pdc->pdc_addr.sin_zero), '\0', 8);   // zero the rest of the struct
				     temp_pdc->sockfd = UDP_sockfd;
		               temp_pdc->cmd_received = 1;

		               flag = 0;
					break;
		     	}
			}
			temp_pdc = temp_pdc->next;
		}//while ends
	}//end of if

     if(flag)
     {
		temp_pdc = malloc(sizeof(struct PDC_Details));
		if(!temp_pdc) {

			printf("Not enough memory temp_pdc\n");
			exit(1);
		}

		if(proto == 0)
		{
		     strcpy(temp_pdc->ip, inet_ntoa(UDP_addr.sin_addr));     // ip
		     strncpy(temp_pdc->protocol,"UDP",3); // protocol
		     temp_pdc->protocol[3] = '\0';
		     temp_pdc->port = ntohs(UDP_addr.sin_port);   //UDP_addr.sin_port
               temp_pdc->sockfd = UDP_sockfd;
               temp_pdc->cmd_received = 1;   //received a cmd frame from pdc? only for UDP
		}
		else
		{
			printf("TCP new?\n");
		     strcpy(temp_pdc->ip, inet_ntoa(TCP_addr.sin_addr));     // ip
		     strncpy(temp_pdc->protocol,"TCP",3); // protocol
		     temp_pdc->protocol[3] = '\0';
		     temp_pdc->port = ntohs(TCP_addr.sin_port);   //UDP_addr.sin_port
               temp_pdc->sockfd = newfd; //new_sockfd
		     temp_pdc->tcpup = 0;
		}
               bzero(&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr));
               temp_pdc->pdc_addr.sin_family = AF_INET;
               temp_pdc->pdc_addr.sin_addr.s_addr =  inet_addr(temp_pdc->ip);
               temp_pdc->pdc_addr.sin_port = htons(temp_pdc->port);
               memset(&(temp_pdc->pdc_addr.sin_zero), '\0', 8);   // zero the rest of the struct
               temp_pdc->STAT_change = 0;  //no change
               temp_pdc->pmu_cfgsent = 0;     //not sent
               temp_pdc->data_transmission = 1;   //off
               temp_pdc->address_set = 0;
          
               if(PDCfirst == NULL) {

                    PDCfirst = temp_pdc;
                    temp_pdc->prev = NULL;

               } else {

                    PDClast->next = temp_pdc;
                    temp_pdc->prev = PDClast;
               }

               PDClast = temp_pdc;
               temp_pdc->next = NULL;
     }	

	pthread_mutex_unlock(&mutex_pdc_object);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION void* UDP_PMU():							                    */
/* This is a UDP Server of PMU and it will continuously on listening mode.      */
/* Function for receives frames from authentic PDC & reply back the 	     	*/
/* requested frame (if available) to PDC.					               */
/* ----------------------------------------------------------------------------	*/

void* UDP_PMU()
{
	/* local variables */
	unsigned char c;
	int n, ind;
	char udp_command[18],filename1[200];
	FILE *fp1;
     struct PDC_Details *temp_pdc;

    /* Apply 1 ms delay if required to allow the other thread to complete its
     * work
     */
	while(strlen(pmuFilePath) == 0) usleep(1000);
	
    strcpy(filename1, pmuFilePath);

     /* This while is always in listening mode to receiving frames from PDC and their respective reply */
	while(1)	
	{
		ind = 2;
		memset(udp_command,'\0',18);

          /* UDP data Received */
		if ((numbytes = recvfrom(UDP_sockfd, udp_command, 18, 0, (struct sockaddr *)&UDP_addr, (socklen_t *)&UDP_addr_len)) == -1)
		{ 
			perror("recvfrom");
			exit(1);
		}
		else		/* New datagram has been received */
		{
               PDC_MATCH(0, 0);

			c = udp_command[1];
			c <<= 1;
			c >>= 5;

			if(c  == 0x04) 		/* Check if it is a command frame from PDC */ 
			{	
				c = udp_command[15];

				if((c & 0x05) == 0x05)		/* Command frame for Configuration Frame from PDC */
				{ 				
					printf("\nCommand Frame for Configuration Frame-2 is received fron PDC.\n"); 
					fp1 = fopen (filename1,"rb");

					if (fp1 == NULL)
					{
						perror (filename1); 
						printf("\nPMU IS NOT Configured!\n");
						exit(1);
					}
					else 
					{ 
						fclose(fp1);

						/* Get the CFG size & store in global variable */
						df_temp[0] = cline[ind++];
						df_temp[1] = cline[ind];
						cfg_size = c2i(df_temp);
						cline[cfg_size] = '\0';

                              temp_pdc = PDCfirst;

                              while(temp_pdc != NULL )
                              {
                                   if(temp_pdc->cmd_received == 1)
                                   {
                                        pthread_mutex_lock(&mutex_pdc_object);

               					/* Send Configuration frame to PDC Device */
                                        if (sendto(temp_pdc->sockfd,cline, cfg_size, 0,(struct sockaddr *)&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr)) == -1) {
                                             perror("sendto");
                                        }
                                        temp_pdc->STAT_change = 0;
                                        temp_pdc->pmu_cfgsent = 1;
                                        temp_pdc->cmd_received = 0;

                                        pthread_mutex_unlock(&mutex_pdc_object);     
                                        printf("\nPMU CFG-2 frame [of %d Bytes] is sent to the PDC.\n", cfg_size);
                                        break;
                                   }
                                   temp_pdc = temp_pdc->next;
                              }
					} 
				}
				else if((c & 0x03) == 0x03)		/* Command frame for Header frame request from PDC */
				{ 				
					printf("\nCommand Frame for Header frame is received from PDC.\n"); 
					fp1 = fopen (filename1,"rb");

					if (fp1 == NULL)
					{
						printf("\nHeader Frame is not present in PMU Setup File.\n");
						exit(1);
					}
					else 
					{ 
						fclose(fp1);

						if(get_header_frame() == 1)
						{
							/* Get the CFG size & store in global variable */
							df_temp[0] = hdrline[2];
							df_temp[1] = hdrline[3];
							hdr_size = c2i(df_temp);
							hdrline[hdr_size] = '\0';

                                   temp_pdc = PDCfirst;

                                   while(temp_pdc != NULL )
                                   {
          	                         pthread_mutex_lock(&mutex_pdc_object);

                                        if(temp_pdc->cmd_received == 1)
                                        {
                                             if (sendto(temp_pdc->sockfd,hdrline, hdr_size, 0,(struct sockaddr *)&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr)) == -1) {
								               perror("sendto");
                                             }

                                             temp_pdc->cmd_received = 0;
                                             pthread_mutex_unlock(&mutex_pdc_object);     

     								printf("\nPMU Header Frame is sent to the PDC.\n");
                                             break;
                                        }
	                              	     temp_pdc = temp_pdc->next;
                                   }
						}
					} 
				}
				else if((c & 0x01) == 0x01)		/* Command frame for Turn off transmission request from PDC */
				{ 
					printf("\nCommand Frame for Turn OFF data received from PDC.\n");

                         temp_pdc = PDCfirst;

                         while(temp_pdc != NULL ) 
                         {
                              if(temp_pdc->cmd_received == 1)
                              {
                                   pthread_mutex_lock(&mutex_pdc_object);     

							if(temp_pdc->data_transmission == 1)
						     	printf("Data Transmission is already in OFF mode for PDC.\n");
							else
							{
                                       	temp_pdc->data_transmission = 1;
                                       	temp_pdc->cmd_received = 0;
                                        pthread_mutex_unlock(&mutex_pdc_object);     

								printf("Data Transmission Started for PDC.\n");
								break;
							}
                              }
                              temp_pdc = temp_pdc->next;
                         }
				}
				else if((c & 0x02) == 0x02)		/* Command frame for Turn ON transmission request from PDC */
				{ 				
					printf("\nCommand Frame for Turn ON data received from PDC.\n");

                         temp_pdc = PDCfirst;

                         while(temp_pdc != NULL ) 
                         {
                              if(temp_pdc->cmd_received == 1)
                              {
                                   pthread_mutex_lock(&mutex_pdc_object);     

                                   if(temp_pdc->data_transmission == 0)
								printf("Data Transmission is already in ON mode for PDC.\n");
							else
							{
								if(temp_pdc->pmu_cfgsent == 1)
								{
									printf("Turn ON Data Transmission for PDC.\n");
						               temp_pdc->data_transmission = 0;
								}
								else
									printf("Data Transmission can't be turn on for PDC. As CMD frame has not received for CFG?\n");
                                       	temp_pdc->cmd_received = 0;
                                        pthread_mutex_unlock(&mutex_pdc_object);     
								break;
							}
                              }
                              temp_pdc = temp_pdc->next;
                         }
                    } 
				else if((c & 0x04) == 0x04)		 /* Command frame for Configuration frame-1 request from PDC */
				{
					printf("\nCommand Frame for CFG Frame-1 is received fron PDC.\n"); 
					fp1 = fopen (filename1,"rb");

					if (fp1 == NULL)
					{
						printf("\nConfiguration Frame-1 is not present in PMU Setup File.\n");
					}
					else 
					{ 
						fclose(fp1);

						/* Get the CFG size & store in global variable */
						df_temp[0] = cline[ind++];
						df_temp[1] = cline[ind++];
						cfg_size = c2i(df_temp);
						cline[cfg_size] = '\0';

                              temp_pdc = PDCfirst;

                              while(temp_pdc != NULL ) 
                              {
                                   if(temp_pdc->cmd_received == 1)
                                   {
                                        pthread_mutex_lock(&mutex_pdc_object);

                                        if (sendto(temp_pdc->sockfd,cline, cfg_size, 0,(struct sockaddr *)&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr)) == -1) {
							          perror("sendto");
						          }

                                        temp_pdc->cmd_received = 0;
                                      	pthread_mutex_unlock(&mutex_pdc_object);
                                        printf("\nPMU CFG-1 frame [of %d Bytes] is sent to the PDC.\n", cfg_size);
		     					break;
                                   }
                                   temp_pdc = temp_pdc->next;
                              }
                         }
                    }

			} /* end of processing with received Command frame */

			else		/* If it is other than command frame */				
			{ 
				printf("\nReceived Frame is not a command frame!\n");						
				continue;				
			}
		} /* end of if-else-if */
	} /* end of while */
} /* end of pmu_udp(); */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION void* TCP_CONNECTIONS(void * temp_pdc):					     */
/* This is a TCP Server of PMU and it will continuously on listening mode.      */
/* Function for receives frames from authentic PDC & reply back the 	     	*/
/* requested frame (if available) to PDC. For each and every new connection     */
/* acceptance new thread will create with this function and handle all          */
/* type of frame requeste from communicating PDC.                               */
/* ----------------------------------------------------------------------------	*/

void* TCP_CONNECTIONS(void * temp_pdc)
{
	/* local variables */
	unsigned char c;
	int n,sin_size,ind;
	char tcp_command[19], filename1[200];
	FILE *fp1;

	struct PDC_Details *single_pdc_node = (struct PDC_Details *) temp_pdc;
	int new_fd = single_pdc_node->sockfd;
	single_pdc_node->thread_id = pthread_self();

	while(pmuFilePath == NULL) usleep(1000);
	strcpy(filename1, pmuFilePath);

     /* This will wait until CFG has not been set by user. */
	while(1)
 	{
		ind = 2;
		memset(tcp_command,19,0);	

          /* TCP data Received For new_fd */
		int bytes_read = recv(new_fd,tcp_command,18,0);

		if(bytes_read == -1) 
          {
			perror("recv");
			single_pdc_node->tcpup = 0;	
			remove_tcp_node(single_pdc_node);  //remove the node in pdc_list?
			pthread_exit(NULL);

		} 
          else if(bytes_read == 0)
          {
			printf("The PDC Client close the connection!\n");
			single_pdc_node->tcpup = 0;
			remove_tcp_node(single_pdc_node);  //remove the node in pdc_list?
			pthread_exit(NULL);

		} 
          else		/* New dat has been received */
		{
			c = tcp_command[1];
			c <<= 1;
			c >>= 5;

			if(c  == 0x04) 		/* Check if it is a command frame from PDC */ 
			{	
				c = tcp_command[15];

				if((c & 0x05) == 0x05)		/* Command frame for Configuration Frame-2 request from PDC */
				{ 
					printf("\nCommand Frame for Configuration Frame-2 is received fron PDC.\n"); 
					fp1 = fopen (filename1,"rb");

					if (fp1 == NULL)
					{
						perror (filename1); 
						printf("\nPMU IS NOT Configured!\n");
						exit(1);
					}
					else 
					{ 
						fclose(fp1);

						/* Get the CFG size & store in global variable */
						df_temp[0] = cline[ind++];
						df_temp[1] = cline[ind];
						cfg_size = c2i(df_temp);

						/* Send Configuration frame to PDC Device */
	                         pthread_mutex_lock(&mutex_pdc_object);

				          if (send(new_fd,cline, cfg_size, 0) == -1)
				          {
					          perror("sendto");
				          }
                              single_pdc_node->STAT_change = 0;
                              single_pdc_node->pmu_cfgsent = 1;

                            	pthread_mutex_unlock(&mutex_pdc_object);     

				     	printf("\nPMU CFG-2 frame [of %d Bytes] is sent to PDC.\n", cfg_size);
					} 
				}
				else if((c & 0x03) == 0x03)		/* Command frame for Header frame request from PDC */
				{
					printf("\nCommand Frame for Header frame is received from PDC.\n"); 
					fp1 = fopen(filename1,"rb");

					if (fp1 == NULL)
					{
						printf("\nHeader Frame is not present in PMU Setup File.\n");
						exit(1);
					}
					else 
					{ 
						fclose(fp1);

						if(get_header_frame() == 1)
						{
							/* Get the CFG size & store in global variable */
							df_temp[0] = hdrline[2];
							df_temp[1] = hdrline[3];
							hdr_size = c2i(df_temp);
							hdrline[hdr_size] = '\0';

							/* Send Header frame to PDC Device */
					          if (send(new_fd,hdrline, hdr_size, 0) == -1)
							{
								perror("sendto");
							}
							printf("\nPMU Header Frame is sent to PDC.\n");
						}
					} 
				}
				else if((c & 0x01) == 0x01)		/* Command frame for Turn off transmission request from PDC */
				{
					printf("\nCommand Frame for Turn OFF data received from PDC.\n");

                         pthread_mutex_lock(&mutex_pdc_object);     

					if(single_pdc_node->data_transmission == 1)
						printf("Data Transmission is already in OFF mode for PDC.\n");
					else
					{
						printf("Turn ON Data Transmission for PDC.\n");
                             	single_pdc_node->data_transmission = 1;
					}
                         pthread_mutex_unlock(&mutex_pdc_object);     
				}
				else if((c & 0x02) == 0x02)		/* Command frame for Turn ON transmission request from PDC */
				{ 				
					printf("\nRequest received for data transmission ON.\n"); 

					/* Send data frames if and Only if cfg is sent to PDC */
                         pthread_mutex_lock(&mutex_pdc_object);     

					if(single_pdc_node->data_transmission == 0)
						printf("Data Transmission is already in ON mode for PDC.\n");
					else
					{
						if(single_pdc_node->pmu_cfgsent == 1)
						{
                             		single_pdc_node->data_transmission = 0;
							single_pdc_node->tcpup = 1;
							printf("Turn ON Data Transmission for PDC.\n");
						}
						else
							printf("Data Transmission can't be turn on for PDC. As CMD frame has not received for CFG?\n");
					}
                         pthread_mutex_unlock(&mutex_pdc_object);     
				} 
				else if((c & 0x04) == 0x04)		/* Command frame for Configuration frame-1 request from PDC */
				{
					printf("\nCommand Frame for CFG Frame-1 is received fron PDC.\n");
					fp1 = fopen (filename1,"rb");

					if (fp1 == NULL)
					{
						printf("\nConfiguration Frame-1 is not present in PMU Setup File.\n");
					}
					else 
					{ 
						fclose(fp1);

						/* Get the CFG size & store in global variable */
						df_temp[0] = cline[ind++];
						df_temp[1] = cline[ind++];
						cfg_size = c2i(df_temp);

						if (send(new_fd,cline, cfg_size, 0)== -1)
						{
							perror("sendto");
						}
						printf("\nPMU CFG-1 frame [of %d Bytes] is sent to PDC.\n", cfg_size);
					} 
				}
			} /* end of processing with received Command frame */

			else		/* If it is other than command frame */				
			{ 
				printf("\nReceived Frame is not a command frame!\n");						
				continue;				
			}

		} /* end of processing with received Command frame */

	} /*end of While */

	close(new_fd);
	pthread_exit(NULL);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_tcp_node(void * node);	     		                    */
/* This function will remove the connection nodes from PDC linked list,         */
/* based on other-end connection lost.                                          */
/* ----------------------------------------------------------------------------	*/

void remove_tcp_node(void * node)
{
	struct PDC_Details *pdc_node = (struct PDC_Details *) node;

	if(PDCfirst == NULL) 
	{
		printf("No connected-PDC Present?\n");
	} 
	else 
	{
		struct PDC_Details *temp_pdc = PDCfirst;
		pthread_mutex_lock(&mutex_pdc_object);     

		while(temp_pdc != NULL)
		{
			if((!strcmp(temp_pdc->ip,pdc_node->ip)) && 
					(!strncasecmp(temp_pdc->protocol,pdc_node->protocol,3)) && (temp_pdc->port == pdc_node->port)) 
			{
				if(temp_pdc->prev == NULL)  {

					PDCfirst = temp_pdc->next;
					if(PDCfirst != NULL) PDCfirst->prev = NULL;		

				} else {

					temp_pdc->prev->next = temp_pdc->next;
				}

				if(temp_pdc->next == NULL) {

					PDClast = temp_pdc->prev;

				} else {
					if(temp_pdc->prev != NULL)
						temp_pdc->prev->next = temp_pdc->next;
				}

				break;
			}
			else
				temp_pdc = temp_pdc->next;
		}
	}
	pthread_mutex_unlock(&mutex_pdc_object);     
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  TCP_PMU();                 	     		                    */
/* This function will call by the thread for TCP communication for PMU Server.  */
/* It will accept new connections from PDC-clients and create thread for every  */
/* PDC via function call of TCP_CONNECTIONS.                                    */
/* ----------------------------------------------------------------------------	*/

void* TCP_PMU()
{
	int err;
	int sin_size,new_fd,pdc_flag = 0;

	// A new thread is created for each TCP connection in 'detached' mode. Thus allowing any number of threads to be created. 
	pthread_attr_t attr;
	pthread_attr_init(&attr);

     /* In  the detached state, the thread resources are immediately freed when it terminates, but on the thread termination. */
	if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) { 

		perror(strerror(err));   /* pthread_join(3) cannot be used to synchronize */
		exit(1);	       
	}

     /* Shed policy = SCHED_FIFO (realtime, first-in first-out) */
	if((err = pthread_attr_setschedpolicy(&attr,SCHED_FIFO))) { 

		perror(strerror(err));		     
		exit(1);
	}  


	while (1) {

		sin_size = sizeof(struct sockaddr_in);

		if (((new_fd = accept(TCP_sockfd, (struct sockaddr *)&TCP_addr, (socklen_t *)&sin_size)) == -1)) 
          {
			perror("accept");

		} 
          else  /* New TCP connection has been received*/ 
          { 

			/* PDC is authentic */
		     printf("\nPMU server: got connection from %s, & on Port = %d.\n",inet_ntoa(TCP_addr.sin_addr), ntohs(TCP_addr.sin_port)); 

               /* Add the new TCP connection details to PDC linked list */
               PDC_MATCH(1, new_fd);

			struct PDC_Details *temp_pdc = PDCfirst;

			while(temp_pdc != NULL ) 
               {
				if((!strcmp(temp_pdc->ip,inet_ntoa(TCP_addr.sin_addr))) && 
						(!strncasecmp(temp_pdc->protocol,"TCP",3)) && (temp_pdc->port == ntohs(TCP_addr.sin_port))) {

					pdc_flag = 1;		
					break;

				} else {

					temp_pdc = temp_pdc->next;
				}									
			}  			
									
			if(pdc_flag) 
               {
				pthread_t t;

				/* Creates a new thread for each TCP connection. */
				if((err = pthread_create(&t,&attr,TCP_CONNECTIONS,(void *)temp_pdc))) {

					perror(strerror(err));		     
					exit(1);
				}				

			} 
               else 
               { 
				printf("Request from %s TCP which is un-authentic\n",inet_ntoa(TCP_addr.sin_addr));			
			}	
		} // main if ends	

	} // While ends
	pthread_attr_destroy(&attr);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  start_server():							                    */
/* Function for Start PMU Server as per user given Ports.                       */
/* ----------------------------------------------------------------------------	*/

void start_server()
{
	/* Initialy create the shared memory ID */
	int ShmID, err;
     char *ptr1;

     dataFileVar = 0; 
     cfg_crc_error = 0;

	p1.pid = getpid();

	key_t MyKey;

	if (signal(SIGUSR1, SIGUSR1_handler) == SIG_ERR) 
	{
		printf("SIGUSR-1 install error\n");
		exit(1);
	}

	if (signal(SIGUSR2, SIGUSR2_handler) == SIG_ERR) 
	{
		printf("SIGUSR-2 install error\n");
		exit(1);
	}

	MyKey   = 12345;                     /* obtain the shared memory */
	ShmID   = shmget(MyKey, sizeof(struct P_id), IPC_CREAT | 0666);
	ShmPTR  = (struct P_id *) shmat(ShmID, NULL, 0);
	*ShmPTR = p1;                /* save my pid there             */

     /* This will wait until Port and Protocol have not been set by user.  */
	while(tmp_wait)
	{
		usleep(1000);
	}

	fp_DataFile = NULL;

     /* Get the user's name for storing the PMU Setup File */
	ptr1 = getenv ("HOME");

	if (ptr1 == NULL)
	{
		printf("user not found\n");
		exit(1);
	}
	strcat(pmuFolderPath, ptr1);
	strcat(pmuFolderPath, "/iPDC/PMU");

	printf("\n\t\t|-------------------------------------------------------|\n");      
	printf("\t\t|\t\tPMU Simulator SERVER\t\t\t|\n");      
	printf("\t\t|-------------------------------------------------------|\n");      

	/* Create UDP socket and bind to port */
	if ((UDP_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {

		perror("socket");
		exit(1);

	} else {

		printf("\nUDP Socket : Sucessfully Created\n");

	} 	

	if (setsockopt(UDP_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	UDP_my_addr.sin_family = AF_INET;            // host byte order
	UDP_my_addr.sin_port = htons(udp_port);       // short, network byte order
	UDP_my_addr.sin_addr.s_addr = INADDR_ANY;    // automatically fill with my IP
	memset(&(UDP_my_addr.sin_zero),'\0', 8);     // zero the rest of the struct

	if (bind(UDP_sockfd, (struct sockaddr *)&UDP_my_addr,
			sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	} else {

		printf("UDP Socket Bind : Sucessfull\n");
	} 

	/* UDP created socket and is litening for connections */
	printf("PMU UDP SERVER Listening on port: %d\n\n",udp_port);

	/* Create TCP socket and bind and listen on port */
	if ((TCP_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	} else {

		printf("TCP Socket : Sucessfully created\n");
	}

	if (setsockopt(TCP_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	TCP_my_addr.sin_family = AF_INET;          // host byte order
	TCP_my_addr.sin_port = htons(tcp_port);     // short, network byte order
	TCP_my_addr.sin_addr.s_addr = INADDR_ANY;  // automatically fill with my IP
	memset(&(TCP_my_addr.sin_zero), '\0', 8);  // zero the rest of the struct

	if (bind(TCP_sockfd, (struct sockaddr *)&TCP_my_addr, sizeof(struct sockaddr))
			== -1) {
		perror("bind");
		exit(1);

	} else {

		printf("TCP Socket Bind : Sucessfull\n");
	}

	if (listen(TCP_sockfd, BACKLOG) == -1) {

		perror("listen");
		exit(1);

	} else {

		printf("TCP Listen : Sucessfull\n");
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	/* TCP created socket and is litening for connections */
	printf("PMU TCP SERVER Listening on port: %d\n",tcp_port);

	TCP_sin_size = sizeof(struct sockaddr_in);
	UDP_addr_len = sizeof(struct sockaddr);

	/* Threads are created for UDP and TCP to listen on ports given by user */
	if((err = pthread_create(&UDP_thread,NULL,UDP_PMU,NULL))) { 

		perror(strerror(err));
		exit(1);	
	}

	if((err = pthread_create(&TCP_thread,NULL,TCP_PMU,NULL))) {

		perror(strerror(err));
		exit(1);	
	}    

	pthread_join(UDP_thread, NULL);
	pthread_join(TCP_thread, NULL);

	close(UDP_sockfd);	
	close(TCP_sockfd);

} /* end of start_server() */


/* ------------------------------------------------------------------ */
/* FUNCTION  SIGUSR1_handler(int sig):                                */
/* SIGUSR1 signal handler will give the user's choice regarding data  */
/* data source. Give also the file path when data measurements from   */
/* file.                                                              */
/* ------------------------------------------------------------------ */

void  SIGUSR1_handler(int sig)
{
	signal(sig, SIG_IGN);
	printf("PMU Server SIGUSR-1 Received.\n");

	if(ShmPTR->dataFileVar == 1)
	{
		fp_DataFile = fopen (ShmPTR->filePath, "r");

		if (fp_DataFile == NULL)
		{
			perror (ShmPTR->filePath);
		}
		else
		{
			dataFileVar = ShmPTR->dataFileVar;
		}
	}
	else if(ShmPTR->dataFileVar == 0)
	{
		dataFileVar = ShmPTR->dataFileVar;

		if(fp_DataFile != NULL)			
			fclose(fp_DataFile);
	}
	else if(ShmPTR->dataFileVar == 2)  /* When sends a complete setup file path */
	{
		strcpy(pmuFilePath, ShmPTR->cfgPath);

		/* Call the function frame_size() to initialized all globals as Configuration frame  */
		frame_size();

          /* Create the SEND_DATA thread for sending Data */
	     if((err = pthread_create(&DATA_thread,NULL,SEND_DATA,NULL))) {

		     perror(strerror(err));
		     exit(1);	
	     }
	}

	signal(sig, SIGUSR1_handler);
}


/* ------------------------------------------------------------------ */
/* FUNCTION  SIGUSR2_handler(int sig):                                */
/* SIGUSR2 signal handler will give the user's choice regarding data  */
/* STAT Word change. Gives what kind of error has been introduced and */
/* which bit should be change from 0 to 1 in data frame's STAT Word.  */
/* ------------------------------------------------------------------ */

void  SIGUSR2_handler(int sig)
{
	signal(sig, SIG_IGN);
	printf("PMU Server SIGUSR-2 Received.\n");

	if(ShmPTR->cfg_bit_change_info == 0)	
	{
		udp_port = ShmPTR->UdpPort;
		tcp_port = ShmPTR->TcpPort;
		tmp_wait = 0;
	}
	else if(ShmPTR->cfg_bit_change_info == 6)	/* for CheckSum Error */
	{
		cfg_crc_error = 1; 
		printf("Invalid CheckSum!\n");
	}
	else
     {
          struct PDC_Details *temp_pdc = PDCfirst;

	     pthread_mutex_lock(&mutex_pdc_object);

          while(temp_pdc != NULL ) 
          {
               if(ShmPTR->cfg_bit_change_info == 1)	/* for configuration change bit */
	          {
                    temp_pdc->STAT_change = 1;
		          printf("STAT - Configuration changed!\n");
	          }

	          else if(ShmPTR->cfg_bit_change_info == 2)	/* for invalid data bit */
	          {
                    temp_pdc->STAT_change = 2;
		          printf("STAT - Invalid data!\n");

	          }
	          else if(ShmPTR->cfg_bit_change_info == 3)	/* for PMU error bit */
	          {
                    temp_pdc->STAT_change = 3;
		          printf("STAT - PMU error!\n");
	          }
	          else if(ShmPTR->cfg_bit_change_info == 4)	/* for data sorting bit */
	          {
                    temp_pdc->STAT_change = 4;
		          printf("STAT - Data Sorting!\n");
	          }
	          else if(ShmPTR->cfg_bit_change_info == 5)	/* for PMU trigger bit */
	          {
                    temp_pdc->STAT_change = 5;
		          printf("STAT - PMU Trigger!\n");
	          }

			temp_pdc = temp_pdc->next;
          }

          if(ShmPTR->cfg_bit_change_info == 1)
          {
               /* As configuration has been changed, fill the global variables with new values for Data and CFG frames */
               frame_size();

               /* Needs to cancle the existing thread for data sending and create new one */
               int n = pthread_cancel(send_thrd_id);

			if (n == 0)
			{
	               if((err = pthread_create(&DATA_thread,NULL,SEND_DATA,NULL))) 
                    {
		               perror(strerror(err));
		               exit(1);	
	               }
			     printf("Now PMU sending Data Frames according to new configuration.");
	               //pthread_join(DATA_thread, NULL);
			}
               else
			     printf("PMU unable to send Data Frames according to new configuration??");
          }

	     pthread_mutex_unlock(&mutex_pdc_object);
     }
	signal(sig, SIGUSR2_handler);
}

/**************************************** End of File *******************************************************/

