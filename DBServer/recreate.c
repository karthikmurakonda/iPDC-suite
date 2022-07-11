/* ----------------------------------------------------------------------------- 
 * recreate.c
 * 
 * iPDC - Phasor Data Concentrator
 *
 * Copyright (C) 2011 Nitesh Pandit
 * Copyright (C) 2011 Kedar V. Khandeparkar
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


#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h> 
#include  <pthread.h>
#include  "parser.h"
#include  "global.h" 
#include  "recreate.h"


/* ---------------------------------------------------------------------*/
/*                  Functions defined in recreate.c         	  	*/
/* ---------------------------------------------------------------------*/

/*                 1.  recreate_cfg_objects()          	    		*/
/*                 2.  void init_cfgparser()   	    	          	*/

/* -------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------	*/
/* FUNCTION  recreate_cfg_objects():                                	     	*/
/* It re-creates Configuration Objects from file cfg.bin if the ./server 	*/
/* program is stopped abruptly. It internally calls init_cfgparser()     	*/
/* ----------------------------------------------------------------------------	*/

void recreate_cfg_objects(){ 

	/* ---------------------------------------------------------------- */
	/*        Initialize Global Mutex Variable from global.h            */
	/* ---------------------------------------------------------------- */

	pthread_mutex_init(&mutex_file, NULL);
	pthread_mutex_init(&mutex_cfg, NULL);
	pthread_mutex_init(&mutex_MYSQL_CONN_ON_DATA, NULL);
	pthread_mutex_init(&mutex_MYSQL_CONN_ON_CFG, NULL);
	pthread_mutex_init(&mutex_phasor_buffer,NULL);
	
	BUFF_LEN = 25000;
	phasorBuff = malloc(BUFF_LEN*sizeof(unsigned char));
	frequencyBuff = malloc(BUFF_LEN*sizeof(unsigned char));
	analogBuff = malloc(BUFF_LEN*sizeof(unsigned char));
	digitalBuff = malloc(BUFF_LEN*sizeof(unsigned char));
	delayBuff = malloc(BUFF_LEN*sizeof(unsigned char));

    	memset(phasorBuff,'\0',BUFF_LEN);
    	memset(frequencyBuff,'\0',BUFF_LEN);
    	memset(analogBuff,'\0',BUFF_LEN);
    	memset(digitalBuff,'\0',BUFF_LEN);
    	memset(delayBuff,'\0',BUFF_LEN);

	static const char filename[] = "cfg.bin";	 

	FILE *file = fopen (filename,"rb");
	unsigned char *line,frame_len[2];
	unsigned int framesize;
	unsigned long fileLen;

	if (file != NULL) {

		//Get file length
		fseek(file, 0, SEEK_END);
		fileLen = ftell(file);
		fseek(file, 0, SEEK_SET);
		printf("FILE LEN LEFT %ld\n",fileLen);
		printf("Creating objects for entries in cfg.bin\n");

		while (fileLen != 0) /* Till the EOF */{

			fseek (file,2 ,SEEK_CUR);
			fread(frame_len, sizeof(unsigned char),2, file);
			fseek (file,-4 ,SEEK_CUR);

			framesize = to_intconvertor(frame_len);
			line = malloc(framesize*sizeof(unsigned char));
			memset(line,'\0',sizeof(line));
			fread(line, sizeof(unsigned char),framesize,file);				
			init_cfgparser(line);
			free(line);
			fileLen -= framesize;
		}
		fclose (file);

	} else {
		perror (filename); /* why didn't the file open? */
	}
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  init_cfgparser():                                	     		*/
/* It is called by recreate_cfg_object() to read the file `cfg.bin`  		*/
/* and create cfg objects in the memory    					*/
/* ----------------------------------------------------------------------------	*/

void init_cfgparser(unsigned char st[]){ 

	unsigned char *s;
	int cfglen = 0;
	int i,j,k,dgchannels, phtype, antype;
	struct cfg_frame *cfg;
	struct channel_names *cn;
	unsigned long int l_phunit,l_anunit;

	unsigned char *sync,*frame_size,*idcode_h,*soc,*fracsec,*time_base,*num_pmu,*stn,*idcode_l,*format,*phnmr,*annmr,*dgnmr;
	unsigned char *fnom,*cfg_cnt,*data_rate,*buf;

	sync = malloc(3*sizeof(unsigned char));
	frame_size = malloc(3*sizeof(unsigned char));
	idcode_h= malloc(3*sizeof(unsigned char));
	soc = malloc(5*sizeof(unsigned char));
	fracsec = malloc(5*sizeof(unsigned char));
	time_base = malloc(5*sizeof(unsigned char));
	num_pmu = malloc(3*sizeof(unsigned char));
	stn = malloc(17*sizeof(unsigned char));
	idcode_l = malloc(3*sizeof(unsigned char));
	format = malloc(5*sizeof(unsigned char));
	phnmr = malloc(3*sizeof(unsigned char));
	annmr = malloc(3*sizeof(unsigned char));
	dgnmr = malloc(3*sizeof(unsigned char));

	fnom = malloc(3*sizeof(unsigned char));
	cfg_cnt = malloc(3*sizeof(unsigned char));
	data_rate = malloc(3*sizeof(unsigned char));
	buf = malloc(9*sizeof(unsigned char));

	memset(sync,'\0',3);
	memset(frame_size,'\0',3);
	memset(idcode_h,'\0',3);
	memset(soc,'\0',3);
	memset(fracsec,'\0',5);
	memset(time_base,'\0',5);
	memset(num_pmu,'\0',3);
	memset(stn,'\0',17);
	memset(idcode_l,'\0',3);
	memset(format,'\0',3);
	memset(phnmr,'\0',3);
	memset(annmr,'\0',3);
	memset(dgnmr,'\0',3);

	memset(fnom,'\0',3);
	memset(cfg_cnt ,'\0',3);
	memset(data_rate,'\0',3);
	memset(buf,'\0',9);


	/******************** PARSING BEGINGS *******************/

	pthread_mutex_lock(&mutex_file);

	cfg = malloc(sizeof(struct cfg_frame));
	if(!cfg) {
		printf("No enough memory for cfg\n");
	}

	printf("Inside INIT cfgparser()\n");
	s = st;

	//Copy sync word to file
	copy_cbyc (sync,s,2);
	sync[2] = '\0';
	s = s + 2;

	// Separate the FRAME SIZE
	copy_cbyc (frame_size,(unsigned char *)s,2);
	frame_size[2]='\0';
	cfg->framesize = to_intconvertor(frame_size);
	printf("FRAME SIZE %d\n",cfg->framesize);
	s = s + 2;
	cfglen += 2;

	//SEPARATE IDCODE
	copy_cbyc (idcode_h,(unsigned char *)s,2);
	idcode_h[2] = '\0';
	cfg->idcode = to_intconvertor(idcode_h);
	printf("ID Code %d\n",cfg->idcode);
	s = s + 2;
	cfglen += 2;

	//SEPARATE SOC	
	copy_cbyc (soc,(unsigned char *)s,4);
	soc[4] = '\0';
	sscanf((unsigned int *)soc,"%x", (unsigned int *)&cfg->soc);			
	printf("SOC %ld\n",cfg->soc);
	s =s + 4;
	cfglen += 4;

	//SEPARATE FRACSEC	
	copy_cbyc (fracsec,(unsigned char *)s,4);
	fracsec[4] = '\0';
	cfg->fracsec = to_long_int_convertor(fracsec);	 
	printf("FracSec %ld\n",cfg->fracsec);
	s = s + 4;
	cfglen += 4;

	//SEPARATE TIMEBASE
	copy_cbyc (time_base,(unsigned char *)s,4);
	time_base[4]='\0';
	cfg->time_base = to_long_int_convertor(time_base);			
	printf("Time Base %ld\n",cfg->time_base);
	s = s + 4;
	cfglen += 4;

	//SEPARATE PMU NUM
	copy_cbyc (num_pmu,(unsigned char *)s,2);
	num_pmu[2] = '\0';	
	cfg->num_pmu = to_intconvertor(num_pmu);		
	printf("Number of PMU's %d\n",cfg->num_pmu);
	s = s + 2;
	cfglen += 2;

	// Allocate Memeory For Each PMU
	cfg->pmu = malloc(cfg->num_pmu* sizeof(struct for_each_pmu *));
	if(!cfg->pmu) {
		printf("Not enough memory pmu[][]\n");
		exit(1);
	}

	for (i = 0; i < cfg->num_pmu; i++) {
		cfg->pmu[i] = malloc(sizeof(struct for_each_pmu));
	}

	j = 0;

	///WHILE EACH PMU IS HANDLED
	while(j<cfg->num_pmu) {

		//SEPARATE STATION NAME
		memset(cfg->pmu[j]->stn,'\0',17);
		copy_cbyc (cfg->pmu[j]->stn,(unsigned char *)s,16);
		cfg->pmu[j]->stn[16] = '\0';

		printf("STATION NAME %s\n",cfg->pmu[j]->stn);				
		s = s + 16;
		cfglen += 16;

		//SEPARATE IDCODE		
		copy_cbyc (idcode_l,(unsigned char *)s,2);
		idcode_l[2]='\0';
		cfg->pmu[j]->idcode = to_intconvertor(idcode_l);
		printf("ID Code %d\n",cfg->pmu[j]->idcode);
		s = s + 2;
		cfglen += 2;

		//SEPARATE DATA FORMAT		
		copy_cbyc ((unsigned char *)cfg->pmu[j]->data_format,(unsigned char *)s,2);
		cfg->pmu[j]->data_format[2]='\0';
		s = s + 2;
		cfglen += 2;

		unsigned char hex = cfg->pmu[j]->data_format[1];
		hex <<= 4;

		// Extra field has been added to identify polar,rectangular,floating/fixed point	
		cfg->pmu[j]->fmt = malloc(sizeof(struct format));
		if((hex & 0x80) == 0x80) cfg->pmu[j]->fmt->freq = 1; else cfg->pmu[j]->fmt->freq = 0;
		if((hex & 0x40) == 0x40 ) cfg->pmu[j]->fmt->analog = 1; else cfg->pmu[j]->fmt->analog = 0;
		if((hex & 0x20) == 0x20) cfg->pmu[j]->fmt->phasor = 1; else cfg->pmu[j]->fmt->phasor = 0;
		if((hex & 0x10) == 0x10) cfg->pmu[j]->fmt->polar =  1; else cfg->pmu[j]->fmt->polar = 0;

		//SEPARATE PHASORS	
		copy_cbyc (phnmr,(unsigned char *)s,2);
		phnmr[2]='\0';
		cfg->pmu[j]->phnmr = to_intconvertor(phnmr);
		printf("Phasors %d\n",cfg->pmu[j]->phnmr);
		s = s + 2;
		cfglen += 2;

		//SEPARATE ANALOGS			
		copy_cbyc (annmr,(unsigned char *)s,2);
		annmr[2]='\0';
		cfg->pmu[j]->annmr = to_intconvertor(annmr);
		printf("Analogs %d\n",cfg->pmu[j]->annmr);
		s = s + 2;
		cfglen += 2;

		//SEPARATE DIGITALS			
		copy_cbyc (dgnmr,(unsigned char *)s,2);
		dgnmr[2]='\0';
		cfg->pmu[j]->dgnmr = to_intconvertor(dgnmr);
		printf("Digitals %d\n",cfg->pmu[j]->dgnmr);
		s = s + 2; 
		cfglen += 2;

		cn = malloc(sizeof(struct channel_names));
		cn->first = NULL;

		////SEPARATE PHASOR NAMES 
		if(cfg->pmu[j]->phnmr != 0){
			cn->phtypes = malloc((cfg->pmu[j]->phnmr) * sizeof(unsigned char*));
			cn->phnames = malloc((cfg->pmu[j]->phnmr) * sizeof(char*));
			if(!cn->phnames) {
				printf("Not enough memory cfg->pmu[j]->cn->phnames[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->phnmr; i++) {

				cn->phnames[i] = malloc(17*sizeof(char));
				memset(cn->phnames[i],'\0',17);
				cn->phtypes[i] = malloc(2*sizeof(unsigned char));
				memset(cn->phtypes[i],'\0',2);

			}

			cfg->pmu[j]->phunit = malloc(cfg->pmu[j]->phnmr*sizeof(float*));
			if(!cfg->pmu[j]->phunit) {
				printf("Not enough memory cfg.pmu[j]->phunit[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->phnmr; i++) {
				cfg->pmu[j]->phunit[i] = malloc(sizeof(float));
			}


			i = 0;//Index for PHNAMES
			while(i<cfg->pmu[j]->phnmr){

				copy_cbyc (cn->phnames[i],(unsigned char *)s,16);
				cn->phnames[i][16] = '\0';
				printf("Phnames %s\n",cn->phnames[i]);
				s = s + 16;
				cfglen += 16;
				i++;
			}	
		} 		

		//SEPARATE ANALOG NAMES
		if(cfg->pmu[j]->annmr != 0){
			cn->antypes =  malloc((cfg->pmu[j]->annmr)*sizeof(unsigned char*));
			cn->angnames = malloc((cfg->pmu[j]->annmr)*sizeof(char*));
			if(!cn->angnames) {

				printf("Not enough memory cfg->pmu[j]->cn->phnames[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->annmr; i++) {

				cn->angnames[i] = malloc(17*sizeof(char));
				memset(cn->angnames[i],'\0',17);
				cn->antypes[i] = malloc(5*sizeof(unsigned char));
				memset(cn->antypes[i],'\0',5);
			}

			cfg->pmu[j]->anunit = malloc(cfg->pmu[j]->annmr*sizeof(float*));
			if(!cfg->pmu[j]->anunit) {
				printf("Not enough memory cfg.pmu[j]->anunit[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->annmr; i++) {
				cfg->pmu[j]->anunit[i] = malloc(sizeof(float));
			}

			i = 0;//Index for ANGNAMES

			while(i<cfg->pmu[j]->annmr){
				copy_cbyc (cn->angnames[i],(unsigned char *)s,16);
				cn->angnames[i][16]='\0';
				printf("ANGNAMES %s\n",cn->angnames[i]);
				s = s + 16; 
				cfglen += 16;
				i++;
			}
		}


		if(cfg->pmu[j]->dgnmr != 0){

			cfg->pmu[j]->dgunit = malloc(cfg->pmu[j]->dgnmr*sizeof(char*));
			if(!cfg->pmu[j]->dgunit) {

				printf("Not enough memory cfg->pmu[j]->dgunit[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->dgnmr; i++) {

				cfg->pmu[j]->dgunit[i] = malloc(5);
			}
		}

          	int di;	//Index for number of dgwords
		struct dgnames *q;
		i = 0; 

		while(i < cfg->pmu[j]->dgnmr) {

			struct dgnames *temp1 = malloc(sizeof(struct dgnames));
			temp1->dgn = malloc(16*sizeof(unsigned char *));	
			if(!temp1->dgn) {

				printf("Not enough memory temp1->dgn\n");
				exit(1);
			}

			for (di = 0; di < 16; di++) {

				temp1->dgn[di] = malloc(17*sizeof(unsigned char));

			}

			temp1->dg_next = NULL;

			for(dgchannels = 0;dgchannels < 16;dgchannels++){

				memset(temp1->dgn[dgchannels],'\0',16);
				copy_cbyc (temp1->dgn[dgchannels],(unsigned char *)s,16);
				temp1->dgn[dgchannels][16] = '\0';
				s += 16;
				cfglen += 16;
				k = 0;  
				printf("%s\n",temp1->dgn[dgchannels]);
			}

			if(cn->first == NULL){
				cn->first = q = temp1;

			} else {

				while(q->dg_next!=NULL){
					q = q->dg_next;
				}
				q->dg_next = temp1;			       
			}  

			i++;  
		} //DGWORD WHILE ENDS

		cfg->pmu[j]->cnext = cn;//Assign to pointers

		///PHASOR FACTORS
		if(cfg->pmu[j]->phnmr != 0){

			i = 0;
			while(i < cfg->pmu[j]->phnmr){ //Separate the Phasor conversion factors

				memset(buf,'\0',9);
				copy_cbyc (buf,(unsigned char *)s,1);
				buf[1] = '\0';
				s = s + 1;
				cfglen += 1;
				phtype = to_intconvertor1(buf);
				copy_cbyc (buf,(unsigned char *)s,3);
				buf[3] = '\0';
				s = s + 3;
				cfglen += 3;
				l_phunit = to_long_int_convertor1(buf);

				if (phtype == 0)
                		{
                    			cfg->pmu[j]->cnext->phtypes[i] = "V";

                    			if (cfg->pmu[j]->fmt->phasor == 0)
                    			{
                        			*cfg->pmu[j]->phunit[i] = l_phunit * 1e-5;
                    			}
                    			else
                    			{
                        			*cfg->pmu[j]->phunit[i] = 1;
                    			}
                		}
                		else if (phtype == 1)
                		{
                    			cfg->pmu[j]->cnext->phtypes[i] = "I";

                    			if (cfg->pmu[j]->fmt->phasor == 0)
                    			{
                        			*cfg->pmu[j]->phunit[i] = l_phunit * 1e-5;
                    			}
                    			else
                    			{
                        			*cfg->pmu[j]->phunit[i] = 1;
                    			}
                		}
                		else
                		{
                    			perror("Invalid first byte in PHUNIT - probably error in PMU");
                		}

				printf("Phasor Factor %d = %f\n",i,*cfg->pmu[j]->phunit[i]);
				i++;
			}
		}//if for PHASOR Factors ends

		//ANALOG FACTORS
		if(cfg->pmu[j]->annmr != 0){

			i=0;
			while(i<cfg->pmu[j]->annmr){ //Separate the Phasor conversion factors

				memset(buf,'\0',9);
				copy_cbyc (buf,(unsigned char *)s,1);
				buf[1] = '\0';
                		s = s + 1;
				cfglen += 1;

                		antype = to_intconvertor1(buf);
                		switch(antype)
                		{
                    			case 0 : cfg->pmu[j]->cnext->antypes[i] = "POW";
                    			break;
                    			case 1 : cfg->pmu[j]->cnext->antypes[i] = "RMS";
                    			break;
                    			case 2 : cfg->pmu[j]->cnext->antypes[i] = "PEAK";
                    			break;
                    			default: perror("Invalid first byte in ANUNIT?");
                    			break;
                		}
				copy_cbyc (buf,(unsigned char *)s,3);
				buf[3] = '\0';
				s = s + 3;
				cfglen += 3;
				l_anunit = to_long_int_convertor1(buf);

                    		if (cfg->pmu[j]->fmt->analog == 0)
                    		{
                        		*cfg->pmu[j]->anunit[i] = l_anunit * 1e-5; // Assumed factor of 1e5 in iPDC
                    		}
                    		else
                    		{
                        		*cfg->pmu[j]->anunit[i] = 1;
                    		}

				printf("Analog Factor %d = %f\n",i,*cfg->pmu[j]->anunit[i]);
				i++;
			}

		} // if for ANALOG FActtors ends

		///INCOMPLETE  
		if(cfg->pmu[j]->dgnmr != 0){

			i = 0;
			while(i < cfg->pmu[j]->dgnmr ){ //Separate the Phasor conversion factors

				copy_cbyc(cfg->pmu[j]->dgunit[i],s,4);
				cfg->pmu[j]->dgunit[i][4] = '\0';
				printf("DGWORD %s\n",cfg->pmu[j]->dgunit[i]); 
				s += 4;
				cfglen += 4;
				i++;
			}
		} //if for Digital Words FActtors ends

		copy_cbyc (fnom,(unsigned char *)s,2);
		fnom[2]='\0';
		cfg->pmu[j]->fnom = to_intconvertor(fnom);
		printf("FREQUENCY %d\n",cfg->pmu[j]->fnom);
		s = s + 2;
		cfglen += 2;

		copy_cbyc (cfg_cnt,(unsigned char *)s,2);
		cfg_cnt[2] = '\0';
		cfg->pmu[j]->cfg_cnt = to_intconvertor(cfg_cnt);
		printf("CFG CHANGE COUNT %d\n",cfg->pmu[j]->cfg_cnt);
		s = s + 2;
		cfglen += 2;
		j++; 
	}//While for PMU number ends

	copy_cbyc (data_rate,(unsigned char *)s,2);
	data_rate[2] = '\0';
	cfg->data_rate = to_intconvertor(data_rate);
	printf("Data Rate %d\n",cfg->data_rate);
	s += 2;
	cfglen += 2;
	cfg->cfgnext = NULL;

	// Adjust the configuration object pointers
	// Lock the mutex_cfg    
	pthread_mutex_lock(&mutex_cfg);

	if (cfgfirst == NULL)  { // Main if

		cfgfirst = cfg;	

	} else {

		struct cfg_frame *temp_cfg = cfgfirst;

		//Check if the configuration frame already exists
		while(temp_cfg->cfgnext != NULL){

			temp_cfg = temp_cfg->cfgnext;

		}// While ends
		temp_cfg->cfgnext = cfg;

	} //Main if

	pthread_mutex_unlock(&mutex_cfg);
	pthread_mutex_unlock(&mutex_file);

	free(sync);
	free(frame_size);
	free(idcode_h);
	free(soc);
	free(fracsec);
	free(time_base);
	free(num_pmu);
	free(stn);
	free(idcode_l);
	free(format);
	free(phnmr);
	free(annmr);
	free(dgnmr);
	free(fnom);
	free(cfg_cnt);
	free(data_rate);
	free(buf);
}

/**************************************** End of File *******************************************************/ 
