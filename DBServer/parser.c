/* ----------------------------------------------------------------------------- 
 * parser.c
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


/* ---------------------------------------------------------------------------- */
/*                       Functions defined in parser.c          	    	*/
/* -----------------------------------------------------------------------------*/

/*                 1. void cfgparser(char [])           	    	      	*/
/*                 2. int remove_old_cfg(char[])              	    	      	*/
/*                 3. void cfginsert(struct cfg_frame *)       	    	      	*/
/*                 4. void dataparser(char[])          	        	      	*/
/*                 5. int check_statword(char stat[])               	      	*/
/*                 6. void hexTobin(char [])               	    	    	*/
/*                 7. void hexToAscii(char [])                	    	    	*/

/* ---------------------------------------------------------------------------- */


#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h> 
#include  <pthread.h>
#include  <math.h>
#include <float.h>
#include <assert.h>
#include  <mysql.h>
#include  <errno.h>
#include  "parser.h"
#include  "global.h" 
#include  "dallocate.h" 
#include  "connections.h"

const int PI = 3.14;

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  cfgparser():                                	     		*/
/* It creates configuration objects for the received configuration frames.	*/
/* Configuration frame is also written in the file `cfg.bin`.			*/
/* If the object is already present, it will replace in cfg_frame LL and	*/
/* also in the file `cfg.bin` by calling remove_old_cfg().			*/
/* ----------------------------------------------------------------------------	*/

void cfgparser(unsigned char st[]){ 

	FILE *fp;
	unsigned char *s;
	int match = 0,dgchannels,i,j, phtype, antype;
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

	fp = fopen("cfg.bin","ab"); //Store configuration in a file     

	if (fp == NULL)
		printf("File doesn't exist\n");

	cfg = malloc(sizeof(struct cfg_frame));
	if(!cfg) {
		printf("No enough memory for cfg\n");
	}

	printf("Inside cfgparser()\n");
	s = st;

	//Copy sync word to file
	copy_cbyc (sync,(unsigned char *)s,2);
	sync[2] = '\0';
	s = s + 2;

	// Separate the FRAME SIZE
	copy_cbyc (frame_size,(unsigned char *)s,2);
	frame_size[2]='\0';
	cfg->framesize = to_intconvertor(frame_size);
	printf("FRAME SIZE %d\n",cfg->framesize);
	s = s + 2;

	size_t result;

	result = fwrite(st, sizeof(unsigned char),cfg->framesize, fp);
	printf("No of bytes written %ld, into the cfg file.\n",(long int)result);

	unsigned long fileLen;
	fseek(fp, 0, SEEK_END);
	fileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("FILE LENGTH %ld\n",fileLen);


	//SEPARATE IDCODE
	copy_cbyc (idcode_h,(unsigned char *)s,2);
	idcode_h[2] = '\0';
	cfg->idcode = to_intconvertor(idcode_h);
	printf("ID Code %d\n",cfg->idcode);
	s = s + 2;

	//SEPARATE SOC	
	copy_cbyc (soc,(unsigned char *)s,4);
	soc[4] = '\0';
	cfg->soc = to_long_int_convertor(soc);	 			
	printf("SOC %ld\n",cfg->soc);
	s =s + 4;

	//SEPARATE FRACSEC	
	copy_cbyc (fracsec,(unsigned char *)s,4);
	fracsec[4] = '\0';
	cfg->fracsec = to_long_int_convertor(fracsec);	 
	printf("FracSec %ld\n",cfg->fracsec);
	s = s + 4;

	//SEPARATE TIMEBASE
	copy_cbyc (time_base,(unsigned char *)s,4);
	time_base[4]='\0';
	cfg->time_base = to_long_int_convertor(time_base);			
	printf("Time Base %ld\n",cfg->time_base);
	s = s + 4;

	//SEPARATE PMU NUM
	copy_cbyc (num_pmu,(unsigned char *)s,2);
	num_pmu[2] = '\0';	
	cfg->num_pmu = to_intconvertor(num_pmu);		
	printf("Number of PMU's %d\n",cfg->num_pmu);
	s = s + 2;

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

		//SEPARATE IDCODE		
		copy_cbyc (idcode_l,(unsigned char *)s,2);
		idcode_l[2]='\0';
		cfg->pmu[j]->idcode = to_intconvertor(idcode_l);
		printf("ID Code %d\n",cfg->pmu[j]->idcode);
		s = s + 2;

		//SEPARATE DATA FORMAT		
		copy_cbyc ((unsigned char *)cfg->pmu[j]->data_format,(unsigned char *)s,2);
		cfg->pmu[j]->data_format[2]='\0';
		//printf("PMU format %s\n",cfg->pmu[j]->data_format);
		s = s + 2;

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

		//SEPARATE ANALOGS			
		copy_cbyc (annmr,(unsigned char *)s,2);
		annmr[2]='\0';
		cfg->pmu[j]->annmr = to_intconvertor(annmr);
		printf("Analogs %d\n",cfg->pmu[j]->annmr);
		s = s + 2;

		//SEPARATE DIGITALS			
		copy_cbyc (dgnmr,(unsigned char *)s,2);
		dgnmr[2]='\0';
		cfg->pmu[j]->dgnmr = to_intconvertor(dgnmr);
		printf("Digitals %d\n",cfg->pmu[j]->dgnmr);
		s = s + 2; 

		cn = malloc(sizeof(struct channel_names));
		cn->first = NULL;

		////SEPARATE PHASOR NAMES 
		if(cfg->pmu[j]->phnmr != 0){
			cn->phtypes = malloc((cfg->pmu[j]->phnmr) * sizeof(unsigned char*));
			cn->phnames = malloc((cfg->pmu[j]->phnmr) * sizeof(unsigned char*));
			if(!cn->phnames) {
				printf("Not enough memory cfg->pmu[j]->cn->phnames[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->phnmr; i++) {

				cn->phnames[i] = malloc(17*sizeof(unsigned char));
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
				i++;
			}	
		} 		

		//SEPARATE ANALOG NAMES
		if(cfg->pmu[j]->annmr != 0){
			cn->antypes =  malloc((cfg->pmu[j]->annmr)*sizeof(unsigned char*));
			cn->angnames = malloc((cfg->pmu[j]->annmr)*sizeof(unsigned char*));
			if(!cn->angnames) {

				printf("Not enough memory cfg->pmu[j]->cn->phnames[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->annmr; i++) {

				cn->angnames[i] = malloc(17*sizeof(unsigned char));
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
				i++;
			}
		}


		if(cfg->pmu[j]->dgnmr != 0){

			cfg->pmu[j]->dgunit = malloc(cfg->pmu[j]->dgnmr*sizeof(unsigned char*));
			if(!cfg->pmu[j]->dgunit) {

				printf("Not enough memory cfg->pmu[j]->dgunit[][]\n");
				exit(1);
			}

			for (i = 0; i < cfg->pmu[j]->dgnmr; i++) {

				cfg->pmu[j]->dgunit[i] = malloc(5);
			}
		}

        i = 0; 
        int di;//Index for number of dgwords

        struct dgnames *q;

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
				phtype = to_intconvertor1(buf);
				copy_cbyc (buf,(unsigned char *)s,3);
				buf[3] = '\0';
				s = s + 3;
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

		///DIGITAL FACTORS
		if(cfg->pmu[j]->dgnmr != 0){

			i = 0;
			while(i < cfg->pmu[j]->dgnmr ){ //Separate the Digital conversion factors

				copy_cbyc(cfg->pmu[j]->dgunit[i],(unsigned char *)s,4);
				cfg->pmu[j]->dgunit[i][4] = '\0';
				printf("DGWORD %s\n",cfg->pmu[j]->dgunit[i]); 
				s += 4;
				i++;
			}
		} //if for Digital Words FActtors ends

		copy_cbyc (fnom,(unsigned char *)s,2);
		fnom[2]='\0';
		cfg->pmu[j]->fnom = to_intconvertor(fnom);
		printf("FREQUENCY %d\n",cfg->pmu[j]->fnom);
		s = s + 2;

		copy_cbyc (cfg_cnt,(unsigned char *)s,2);
		cfg_cnt[2] = '\0';
		cfg->pmu[j]->cfg_cnt = to_intconvertor(cfg_cnt);
		printf("CFG CHANGE COUNT %d\n",cfg->pmu[j]->cfg_cnt);
		s = s + 2;
		j++; 
	}//While for PMU number ends

	copy_cbyc (data_rate,(unsigned char *)s,2);
	data_rate[2] = '\0';
	cfg->data_rate = to_intconvertor(data_rate);
	printf("Data Rate %d\n",cfg->data_rate);
	s += 2;
	cfg->cfgnext = NULL;

	// Adjust the configuration object pointers

	// Lock the mutex_cfg    
	pthread_mutex_lock(&mutex_cfg);

	// Index is kept to replace the cfgfirst if it matches
	int index = 0; 

	if (cfgfirst == NULL)  { // Main if
		printf("1\n");
		cfgfirst = cfg;	
		fclose(fp);    

	} else {

		struct cfg_frame *temp_cfg = cfgfirst,*tprev_cfg;
		tprev_cfg = temp_cfg;

		//Check if the configuration frame already exists
		while(temp_cfg!=NULL){

			if(cfg->idcode == temp_cfg->idcode) {

				printf("CFG PRESENT NEED TO REPLACE\n");
				match = 1;
				break;	

			} else {

				index++;
				tprev_cfg = temp_cfg;
				temp_cfg = temp_cfg->cfgnext;

			}
		}// While ends

		if(match) {

			if(!index) {

				// Replace the cfgfirst
				cfg->cfgnext = cfgfirst->cfgnext;
				free_cfgframe_object(cfgfirst);				
				cfgfirst = cfg;

				// Get the new value of the CFG frame
				fclose(fp);
				remove_old_cfg(cfg->idcode,st);      

			} else {

				// Replace in between cfg
				tprev_cfg->cfgnext = cfg;
				cfg->cfgnext = temp_cfg->cfgnext;
				free_cfgframe_object(temp_cfg);
				fclose(fp);
				remove_old_cfg(cfg->idcode,st);      
			}

		} else { // No match and not first cfg 

			tprev_cfg->cfgnext = cfg;
			fclose(fp);
		}   

	} //Main if

	cfginsert(cfg); // DATABASE INSERTION
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


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_old_cfg():              	     				*/
/* It removes the old configuration frame from the file `cfg.bin` and replaces 	*/
/* it with new configuration frame at same position in the file. 	  	*/
/* ----------------------------------------------------------------------------	*/

void remove_old_cfg(int idcode,unsigned char frame[]) { // Begins

	FILE *file,*newfile;
	int result;
	unsigned int idCODE,framesize;
	unsigned char *s,id[3],*line,frame_len[2];
	unsigned long fileLen;

	file = fopen("cfg.bin","rb");
	newfile = fopen("ncfg.bin","wb");
	if (file != NULL) {

		//Get file length
		fseek(file, 0, SEEK_END);
		fileLen = ftell(file);
		fseek(file, 0, SEEK_SET);
		printf("BEFORE REMOVAL OF OLDFRAME FILE LEN %ld\n",fileLen);

		while (fileLen != 0) /* Till the EOF */{

			fseek (file,2 ,SEEK_CUR);
			fread(frame_len, sizeof(unsigned char),2, file);
			fseek (file,-4 ,SEEK_CUR);

			framesize = to_intconvertor(frame_len);
			line = malloc(framesize*sizeof(unsigned char));
			memset(line,'\0',sizeof(line));
			fread(line, sizeof(unsigned char),framesize,file);				
			s = line;
			s += 4;
			//match IDCODE in cfg.bin file
			copy_cbyc (id,s,2);
			id[2] = '\0';	
			idCODE = to_intconvertor(id);

			if(idCODE == idcode) {

				printf("MATCH the new cfg with old cfg in file cfg.bin?\n");
				break;

			} else {

				//Place rest of lines in the new file			
				fwrite(line, sizeof(unsigned char),framesize,newfile);
				free(line);
				fileLen -= framesize;
			}        
		}//While ends

		// The new cfg is copied in the ncfg.bin file 			
		unsigned int len;
		unsigned char *p = frame;			
		p += 2;
		copy_cbyc (frame_len,p,2);
		len = to_intconvertor(frame_len);
		fwrite(frame, sizeof(unsigned char),len,newfile);

		// If cfg.bin file still contains data copy it to ncfg.bin
		while (fileLen != 0) /* Till the EOF */{

			fseek (file,2 ,SEEK_CUR);
			fread(frame_len, sizeof(unsigned char),2, file);
			fseek (file,-4 ,SEEK_CUR);

			framesize = to_intconvertor(frame_len);
			line = malloc(framesize*sizeof(unsigned char));
			memset(line,'\0',sizeof(line));
			fread(line, sizeof(unsigned char),framesize,file);				

			if(!ncmp_cbyc(line,frame,framesize)) {                  		   

				//This skips the last line of the file that contains already added cfg
				//hence we dont copy this line to ncfg.bin
				break;

			} else {	

				fwrite(frame, sizeof(unsigned char),framesize,newfile);
				free(line);						
				fileLen -= framesize;	  				
			}	

		}	

		//File renaming 
		fclose (file);
		fclose(newfile);

		if( remove( "cfg.bin" ) != 0 )
			perror( "Error deleting file" );
		result= rename("ncfg.bin","cfg.bin");
		if ( result == 0 )
			fputs ("File successfully renamed",stdout);
		else
			perror( "Error renaming file" );

	} else {

		perror ("cfg.bin"); /* why didn't the file open? */

	}
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  cfginsert():              	     					*/
/* It inserts/updates the configuration frames in the configuration tables.	*/
/* ----------------------------------------------------------------------------	*/

void cfginsert(struct cfg_frame *cfg){

	int i,j,k;
	char *cmd,*cmd2;
	MYSQL_RES *res1,*res2,*res3,*res4;

	printf("INSIDE CFG INSERT\n");

	cmd = malloc(1000);
	cmd2 = malloc(500);
	memset(cmd,'\0',1000);	//For update queries
	memset(cmd2,'\0',500);	//For insert queries

	pthread_mutex_lock(&mutex_MYSQL_CONN_ON_CFG);

	sprintf(cmd2, "SELECT * FROM MAIN_CFG_TABLE WHERE PDC_ID = %d",cfg->idcode);


	if (mysql_query(conn_cfg,cmd2)) {

		fprintf(stderr, "%s\n", mysql_error(conn_cfg));
		exit(1);
	}

	res1 = mysql_use_result(conn_cfg);
	if(mysql_fetch_row(res1)== NULL) {

		mysql_free_result(res1);
		printf("No Entry Make Insert into table\n");
		memset(cmd,'\0',1000);
		sprintf(cmd, "INSERT INTO MAIN_CFG_TABLE VALUES(%d,%ld,%ld,%ld,%d,%d)",cfg->idcode,cfg->soc,cfg->fracsec,
				cfg->time_base,cfg->num_pmu,cfg->data_rate);
		if (mysql_query(conn_cfg, cmd)) {

			fprintf(stderr, "%s\n", mysql_error(conn_cfg));
			exit(1);
		}
		printf("%s\n",cmd);

		j = 0;
		while(j < cfg->num_pmu){

			memset(cmd,'\0',1000);

			sprintf(cmd,"INSERT INTO SUB_CFG_TABLE(PDC_ID,PMU_ID,SOC,FRACSEC,STN,PHNMR,ANNMR,DGNMR,FNOM) VALUES(%d,%d,%ld,%ld,\"%s\",%d,%d,%d,%d)",cfg->idcode,cfg->pmu[j]->idcode,cfg->soc,cfg->fracsec,cfg->pmu[j]->stn,cfg->pmu[j]->phnmr,cfg->pmu[j]->annmr,cfg->pmu[j]->dgnmr,(cfg->pmu[j]->fnom > 0) ? 50 : 60);

			if (mysql_query(conn_cfg, cmd)) {

				fprintf(stderr, "%s\n", mysql_error(conn_cfg));
				exit(1);
			}
			printf("%s\n",cmd);

			if(cfg->pmu[j]->phnmr != 0){
				for(i = 0;i<cfg->pmu[j]->phnmr;i++) {

					memset(cmd,'\0',1000);

					sprintf(cmd,"INSERT INTO PHASOR(PDC_ID,PMU_ID,PHASOR_NAMES,PHASOR_TYPE,PHUNITS) VALUES (%d,%d,\"%s\",\"%s\",%f)",cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->phnames[i],cfg->pmu[j]->cnext->phtypes[i],*cfg->pmu[j]->phunit[i]);

					if (mysql_query(conn_cfg, cmd)) {

						fprintf(stderr, "%s\n", mysql_error(conn_cfg));
						exit(1);
					}
                    			printf("%s\n",cmd);
				}
			}	

			if(cfg->pmu[j]->annmr != 0){
				for(i = 0;i<cfg->pmu[j]->annmr;i++) {

					memset(cmd,'\0',1000);

					sprintf(cmd,"INSERT INTO ANALOG(PDC_ID,PMU_ID,ANALOG_NAMES,ANALOG_TYPE,ANUNITS) VALUES(%d,%d,\"%s\",\"%s\",%f)",cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->angnames[i],cfg->pmu[j]->cnext->antypes[i],*cfg->pmu[j]->anunit[i]);  						

					if (mysql_query(conn_cfg, cmd)) {

						fprintf(stderr, "%s\n", mysql_error(conn_cfg));
						exit(1);
					}
					printf("%s\n",cmd);
				}
			}

			if(cfg->pmu[j]->dgnmr != 0){

				unsigned long int dunit;
				struct dgnames *temp_dnames = cfg->pmu[j]->cnext->first;
				for(i = 0;i<cfg->pmu[j]->dgnmr;i++) {

					for(k = 0;k<16;k++) {

						dunit = to_long_int_convertor(cfg->pmu[j]->dgunit[i]);
						memset(cmd,'\0',1000);

						sprintf(cmd,"INSERT INTO DIGITAL(PDC_ID,PMU_ID,DIGITAL_NAMES,DIGITAL_WORD) VALUES(%d,%d,\"%s\",%u)",cfg->idcode,cfg->pmu[j]->idcode,temp_dnames->dgn[k],(unsigned int)dunit);

						if (mysql_query(conn_cfg, cmd)) {

							fprintf(stderr, "%s\n", mysql_error(conn_cfg));
							exit(1);
						}
						printf("%s\n",cmd);
					}

					temp_dnames = temp_dnames->dg_next;
				}
			}

			j++;	
		}

	} else {

		// update
		mysql_free_result(res1);

		printf("Update the existing entry in table\n");
		memset(cmd,'\0',1000);

		sprintf(cmd, "UPDATE MAIN_CFG_TABLE SET SOC = %ld,FRACSEC = %ld,TIMEBASE = %ld,NUM_OF_PMU = %d,DATA_RATE = %d WHERE PDC_ID = %d",cfg->soc,cfg->fracsec,cfg->time_base,cfg->num_pmu,cfg->data_rate,cfg->idcode);

		if (mysql_query(conn_cfg,cmd)) {

			fprintf(stderr, "%s\n", mysql_error(conn_cfg));
			exit(1);
		}
		mysql_query(conn_cfg, "COMMIT");
		printf("%s\n",cmd);

		int j = 0;
		while(j < cfg->num_pmu){

			memset(cmd2,'\0',500);

			sprintf(cmd2, "SELECT * FROM SUB_CFG_TABLE WHERE PDC_ID = %d AND PMU_ID = %d AND STN = \"%s\"",cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->stn);

			if (mysql_query(conn_cfg, cmd2)) {

				fprintf(stderr, "%s\n", mysql_error(conn_cfg));
				exit(1);
			}
			res2 = mysql_use_result(conn_cfg);
			printf("%s\n",cmd2);

			if(mysql_fetch_row(res2)== NULL) {

				mysql_free_result(res2);
				memset(cmd,'\0',1000);

				sprintf(cmd,"INSERT INTO SUB_CFG_TABLE(PDC_ID,PMU_ID,SOC,FRACSEC,STN,PHNMR,ANNMR,DGNMR,FNOM) VALUES(%d,%d,%ld,%ld,\"%s\",%d,%d,%d,%d)",cfg->idcode,cfg->pmu[j]->idcode,cfg->soc,cfg->fracsec,cfg->pmu[j]->stn,cfg->pmu[j]->phnmr,cfg->pmu[j]->annmr,cfg->pmu[j]->dgnmr,(cfg->pmu[j]->fnom > 0) ? 50 : 60);

				if (mysql_query(conn_cfg, cmd)) {

					fprintf(stderr, "%s\n", mysql_error(conn_cfg));
					exit(1);
				}
				printf("%s\n",cmd);

			} else {

				mysql_free_result(res2);
				memset(cmd,'\0',1000);

				sprintf(cmd, "UPDATE SUB_CFG_TABLE SET PHNMR = %d,ANNMR = %d,DGNMR = %d,FNOM = %d WHERE PDC_ID = %d AND PMU_ID = %d AND STN = \"%s\"",cfg->pmu[j]->phnmr,cfg->pmu[j]->annmr,cfg->pmu[j]->dgnmr,(cfg->pmu[j]->fnom > 0) ? 50 : 60,cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->stn);

				if (mysql_query(conn_cfg, cmd)) {

					fprintf(stderr, "%s\n", mysql_error(conn_cfg));
					exit(1);
				}

				mysql_query(conn_cfg, "COMMIT");
				printf("%s\n",cmd);
			}

			if(cfg->pmu[j]->phnmr !=0){
				for(i = 0;i<cfg->pmu[j]->phnmr;i++) {

					memset(cmd2,'\0',500);

					sprintf(cmd2, "SELECT * FROM PHASOR WHERE PDC_ID = %d AND PMU_ID = %d AND PHASOR_NAMES = \"%s\"",cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->phnames[i]);

					if (mysql_query(conn_cfg, cmd2)) {

						fprintf(stderr, "%s\n", mysql_error(conn_cfg));
						exit(1);
					}
					printf("%s\n",cmd2);

					res3 = mysql_use_result(conn_cfg);
					if(mysql_fetch_row(res3)== NULL) {

						mysql_free_result(res3);		
						memset(cmd,'\0',1000);

                        sprintf(cmd,"INSERT INTO PHASOR(PDC_ID,PMU_ID,PHASOR_NAMES,PHASOR_TYPE,PHUNITS) VALUES (%d,%d,\"%s\",\"%s\",%f)",cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->phnames[i],cfg->pmu[j]->cnext->phtypes[i],*cfg->pmu[j]->phunit[i]);

						if (mysql_query(conn_cfg, cmd)) {

							fprintf(stderr, "%s\n", mysql_error(conn_cfg));
							exit(1);
						}
						printf("%s\n",cmd);

					} else {

						mysql_free_result(res3);
						memset(cmd,'\0',1000);
						sprintf(cmd,"UPDATE PHASOR SET PHUNITS = %f, PHASOR_TYPE = \"%s\" WHERE PDC_ID = %d AND PMU_ID = %d AND PHASOR_NAMES = \"%s\"",*cfg->pmu[j]->phunit[i],cfg->pmu[j]->cnext->phtypes[i],cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->phnames[i]);  	

						if (mysql_query(conn_cfg, cmd)) {

							fprintf(stderr, "%s\n", mysql_error(conn_cfg));
							exit(1);
						}
						mysql_query(conn_cfg, "COMMIT");		
						printf("%s\n",cmd);

					}
				}
			} // Phasors

			if(cfg->pmu[j]->annmr !=0){

				for(i = 0;i<cfg->pmu[j]->annmr;i++) {

					memset(cmd2,'\0',500);

					sprintf(cmd2, "SELECT * FROM ANALOG WHERE PDC_ID = %d AND PMU_ID = %d AND ANALOG_NAMES = \"%s\"",cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->angnames[i]);

					if (mysql_query(conn_cfg, cmd2)) {

						fprintf(stderr, "%s\n", mysql_error(conn_cfg));
						exit(1);
					}
					printf("%s\n",cmd2);

					res4 = mysql_use_result(conn_cfg);
					if(mysql_fetch_row(res4)== NULL) {

						mysql_free_result(res4);
						memset(cmd,'\0',1000);

                        sprintf(cmd,"INSERT INTO ANALOG(PDC_ID,PMU_ID,ANALOG_NAMES,ANALOG_TYPE,ANUNITS) VALUES(%d,%d,\"%s\",\"%s\",%f)",cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->angnames[i],cfg->pmu[j]->cnext->antypes[i],*cfg->pmu[j]->anunit[i]);

						if (mysql_query(conn_cfg, cmd)) {

							fprintf(stderr, "%s\n", mysql_error(conn_cfg));
							exit(1);
						}
						printf("%s\n",cmd);

					} else {

						mysql_free_result(res4);
						memset(cmd,'\0',1000);

						sprintf(cmd,"UPDATE ANALOG SET ANUNITS = %f, ANALOG_TYPE = \"%s\" WHERE PDC_ID = %d AND PMU_ID = %d AND ANALOG_NAMES = \"%s\"",*cfg->pmu[j]->anunit[i],cfg->pmu[j]->cnext->antypes[i],cfg->idcode,cfg->pmu[j]->idcode,cfg->pmu[j]->cnext->angnames[i]);

						if (mysql_query(conn_cfg, cmd)) {

							fprintf(stderr, "%s\n", mysql_error(conn_cfg));
							exit(1);
						}
						mysql_query(conn_cfg, "COMMIT");
						printf("%s\n",cmd);
					}
				}
			} // Analog	
			j++;				

		} // while ends

	} // Update else ends

	pthread_mutex_unlock(&mutex_MYSQL_CONN_ON_CFG);
	free(cmd);
	free(cmd2);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  dataparser():                                	     		*/
/* Parses the data frames. It searches for configuration objects that matches   */ 
/* with the IDCODE and then inserts into data tables. 				*/
/* ----------------------------------------------------------------------------	*/

int dataparser(unsigned char data[]) { 

	struct cfg_frame *temp_cfg;
	int match = 0,i,j = 0;
	int stat_status,config_change = 0;
	unsigned int t_id,num_pmu,phnmr,annmr,dgnmr;
	float fp_r,fp_i,fp_real,fp_imaginary,fp_analogs;
	long int f_r,f_i,f_analogs,f_freq,f_dfreq,l_soc,l_fracsec;
	short int s_analogs, s_freq, s_dfreq;
	float fp_freq,fp_dfreq; 

	unsigned char *sync,*framesize,*idcode,*soc,*fracsec,*timequality,*stat,*phasors,*analogs,*digital,*freq,*dfreq,*d;
	unsigned char *fp_left,*fp_right;
	unsigned char *f_left,*f_right;
	char *cmd;

	cmd = malloc(500);
	sync = malloc(3*sizeof(unsigned char));
	framesize = malloc(3*sizeof(unsigned char));
	idcode = malloc(3*sizeof(unsigned char));
	soc = malloc(5*sizeof(unsigned char));
	fracsec  = malloc(5*sizeof(unsigned char));
	timequality  = malloc(2*sizeof(unsigned char));
	stat = malloc(3*sizeof(unsigned char));
	phasors = malloc(9*sizeof(unsigned char));
	analogs = malloc(5*sizeof(unsigned char));
	digital = malloc(3*sizeof(unsigned char));
	freq = malloc(5*sizeof(unsigned char));
	dfreq = malloc(5*sizeof(unsigned char));

	memset(cmd,'\0',500);
	memset(sync,'\0',3);
	memset(framesize,'\0',3);
	memset(idcode,'\0',3);
	memset(soc,'\0',5);
	memset(fracsec,'\0',5);
	memset(timequality,'\0',2);
	memset(stat,'\0',3);
	memset(phasors,'\0',9);
	memset(analogs,'\0',5);
	memset(digital,'\0',3);
	memset(freq,'\0',5);
	memset(dfreq,'\0',5);

	fp_left = malloc(5);
	fp_right = malloc(5);
	f_left = malloc(3);
	f_right = malloc(3);

	memset(fp_left,'\0',5);
	memset(fp_right,'\0',5);
	memset(f_left,'\0',3);
	memset(f_right,'\0',3);

	d = data;

	//Skip SYN 
	d += 2;

	//SEPARATE FRAMESIZE
	copy_cbyc (framesize,d,2);
	framesize[2] = '\0';
	d += 2;

	//SEPARATE IDCODE
	copy_cbyc (idcode,d,2);
	idcode[2] ='\0';
	d += 2;

	pthread_mutex_lock(&mutex_cfg);
	// Check for the IDCODE in Configuration Frame
	temp_cfg = cfgfirst;
	t_id = to_intconvertor(idcode);
	printf("ID Code %d\n",t_id);	

	while(temp_cfg != NULL){

		if(t_id == temp_cfg->idcode) {

			match = 1;
			break;	

		} else {

			temp_cfg = temp_cfg->cfgnext;

		}
	}
	pthread_mutex_unlock(&mutex_cfg);

	pthread_mutex_lock(&mutex_MYSQL_CONN_ON_DATA);
	
	if(match){	// idcode matches with cfg idcode

		printf("Inside DATAPARSER, data frame and matched with CFG.\n");

		// Allocate Memeory For Each PMU
		num_pmu = temp_cfg->num_pmu; 	

		//Copy SOC						
		copy_cbyc (soc,d,4);
		soc[4] = '\0';
		l_soc = to_long_int_convertor(soc);			
		d += 4;

		//Copy FRACSEC 
        //First seprate the first Byte of Time Quality Flags
		copy_cbyc (timequality,d,1);
		timequality[1] = '\0';
		d += 1;

          //First seprate the next 3-Byte of Actual Fraction of Seconds
        copy_cbyc (fracsec,d,3);
        fracsec[3] = '\0';
        l_fracsec = to_long_int_convertor1(fracsec);
        l_fracsec = roundf((l_fracsec*1e6)/(temp_cfg->time_base));
        d += 3;

		// Separate the data for each PMU	    	
		while(j < num_pmu) {		  					

			copy_cbyc (stat,d,2);
			stat[2] = '\0';
			d += 2;	

			// Check Stat Word for each data block 
			stat_status = check_statword(stat);


			// If the data has not arrived
			if(stat_status == 16) {

				memset(stat,'\0',3);
				j++;
				continue;

			} else if((stat_status == 14)||(stat_status == 10)) {

				memset(stat,'\0',3);
				config_change = stat_status;
				j++;
				continue;
			}

			// Extract PHNMR, DGNMR, ANNMR
			phnmr = temp_cfg->pmu[j]->phnmr;	
			annmr = temp_cfg->pmu[j]->annmr;
			dgnmr = temp_cfg->pmu[j]->dgnmr;

			pthread_mutex_lock(&mutex_phasor_buffer);

			//Phasors 
			if(phnmr != 0) {

				if(temp_cfg->pmu[j]->fmt->phasor == 1) { // Floating

					for(i = 0;i<phnmr;i++){	

						memset(fp_left,'\0',5);
						memset(fp_right,'\0',5);
						copy_cbyc (fp_left,d,4);
						fp_left[4] = '\0';
						d += 4;

						copy_cbyc(fp_right,d,4);
						fp_right[4] = '\0';   
						d += 4;

						fp_r = decode_ieee_single(fp_left);
						fp_i = decode_ieee_single(fp_right);

                        if(temp_cfg->pmu[j]->fmt->polar == 1) { // POLAR

                            /*fp_real = fp_r*cos(f_i);
                              fp_imaginary = fp_r*sin(f_i);

                              Commented by Gopal on 8th Aug 2012.
                              We want to store polar values in the table */

                            fp_real = fp_r;
                            fp_imaginary = fp_i;
                        }
                        else // RECTANGULAR 
                        {
                            fp_real = hypotf(fp_r,fp_i);
                            fp_imaginary = atan2f(fp_i, fp_r);
                        }

                        memset(cmd,'\0',500);
                        sprintf(cmd," %d,%d,%ld,%ld,\"%s\",%f,%f\n",temp_cfg->idcode,temp_cfg->pmu[j]->idcode,l_soc,l_fracsec,temp_cfg->pmu[j]->cnext->phnames[i],fp_real,fp_imaginary);

                        dataCollectInBuffer(cmd, phasorBuff, 1);
                    }
                } 
                else { // Fixed point

                    for(i = 0;i < phnmr; i++){	

                        memset(f_left,'\0',3);
                        memset(f_right,'\0',3);
                        copy_cbyc (f_left,d,2);
                        f_left[2] = '\0';
                        d += 2;

                        copy_cbyc(f_right,d,2);
                        f_right[2] = '\0';   
                        d += 2;

                        f_r = to_intconvertor(f_left);
                        f_i = to_intconvertor(f_right);

                        if(temp_cfg->pmu[j]->fmt->polar == 1) { // POLAR

                            fp_real = *temp_cfg->pmu[j]->phunit[i] *f_r;
                            fp_imaginary = f_i*1e-4; // Angle is in 10^4 radians
                        }
                        else // RACTANGULAR
                        {
                            fp_r = *temp_cfg->pmu[j]->phunit[i] *f_r;
                            fp_i = *temp_cfg->pmu[j]->phunit[i] *f_i;

                            fp_real = hypotf(fp_r,fp_i);
                            fp_imaginary = atan2f(fp_i, fp_r);
                        }

                        memset(cmd,'\0',500);
                        sprintf(cmd," %d,%d,%ld,%ld,\"%s\",%f,%f\n",temp_cfg->idcode,temp_cfg->pmu[j]->idcode,l_soc,l_fracsec,temp_cfg->pmu[j]->cnext->phnames[i],fp_real,fp_imaginary);

                        dataCollectInBuffer(cmd, phasorBuff,1);
                    }
                } 
			}// Phasors Insertion ends

			//Freq 
			if(temp_cfg->pmu[j]->fmt->freq == 1) { // FLOATING

				memset(freq,'\0',5);
				copy_cbyc (freq,d,4);
				freq[4] = '\0';
				d += 4;					 

				memset(dfreq,'\0',5);
				copy_cbyc (dfreq,d,4);
				dfreq[4] = '\0';
				d += 4;					 

				fp_freq = decode_ieee_single(freq);
				fp_dfreq = decode_ieee_single(dfreq);

			} else { // FIXED

				memset(freq,'\0',5);
				copy_cbyc (freq,d,2);
				freq[2] = '\0';
				d += 2;					 

				memset(dfreq,'\0',5);
				copy_cbyc (dfreq,d,2);
				dfreq[2] = '\0';
				d += 2;	
				s_freq = to_intconvertor(freq);
				s_dfreq = to_intconvertor(dfreq);

                		fp_freq = s_freq*1e-3; // freq is in mHz deviation from nominal
                		if (temp_cfg->pmu[j]->fnom == 0)
                    			fp_freq = 60 + fp_freq;
                		else
                    			fp_freq = 50 + fp_freq;
                		fp_dfreq = s_dfreq*1e-2; // dfreq is 100 times hz/sec 
            		}

            		memset(cmd,'\0',500);
            		sprintf(cmd," %d,%d,%ld,%ld,%f,%f\n",temp_cfg->idcode,temp_cfg->pmu[j]->idcode,l_soc,l_fracsec,fp_freq,fp_dfreq);
            
			dataCollectInBuffer(cmd, frequencyBuff,2); // Freq Insert Ends
									
			//Analogs
			if(annmr != 0) {

				if(temp_cfg->pmu[j]->fmt->analog == 1) { // FLOATING

					for(i = 0; i < annmr; i++){					

						memset(analogs,'\0',5);
						copy_cbyc(analogs,d,4);
						d += 4;
						analogs[4] = '\0';

						fp_analogs = decode_ieee_single(analogs);
                        			fp_analogs =  *temp_cfg->pmu[j]->anunit[i]*fp_analogs;;
                        			memset(cmd,'\0',500);

                        			sprintf(cmd," %d,%d,%ld,%ld,\"%s\",%f\n",temp_cfg->idcode,temp_cfg->pmu[j]->idcode,l_soc,l_fracsec,temp_cfg->pmu[j]->cnext->angnames[i],fp_analogs);

                        			dataCollectInBuffer(cmd, analogBuff,3);
                    			}
					
				} else { // FIXED

					for(i = 0; i < annmr; i++){					

                        			memset(analogs,'\0',5);
                        			copy_cbyc (analogs,d,2);
                        			d += 2;
			
						analogs[2] = '\0';
						s_analogs = to_intconvertor(analogs);
						fp_analogs = *temp_cfg->pmu[j]->anunit[i]*s_analogs ;
					
                        			memset(cmd,'\0',500);
						sprintf(cmd," %d,%d,%ld,%ld,\"%s\",%f\n",temp_cfg->idcode,temp_cfg->pmu[j]->idcode,l_soc,l_fracsec,temp_cfg->pmu[j]->cnext->angnames[i],fp_analogs);

                        			dataCollectInBuffer(cmd, analogBuff,3);
					}
				}
			} // Insertion for Analog done here.

			// Digital
			if(dgnmr != 0) {

				unsigned int dgword;

				for(i = 0; i<dgnmr; i++) {

					memset(digital,'\0',3);
					copy_cbyc (digital,d,2);
					d += 2;
					digital[2] = '\0';
					dgword = to_intconvertor(digital);							

                    			memset(cmd,'\0',500);
                    			sprintf(cmd," %d,%d,%ld,%ld,%u\n",temp_cfg->idcode,temp_cfg->pmu[j]->idcode,l_soc,l_fracsec,dgword);

                    			dataCollectInBuffer(cmd, digitalBuff,4);
				}
			} // Insertion for Digital done here.

               struct timeval tv;
               long local_soc, local_fsec,ms_diff,s_diff;

               /* Obtain the time of day, and convert it to a tm struct. */
               gettimeofday (&tv, NULL);
      
               local_soc = tv.tv_sec;
               local_fsec = tv.tv_usec;
               s_diff = (tv.tv_sec - l_soc);
               ms_diff = (tv.tv_usec - l_fracsec);

	       // Formula to calculate the exact delay in micro between data frame inside-time and
	       // system receive time at which that data frame received.
               ms_diff = ((s_diff == 0) ? ((ms_diff > 0) ? ms_diff : -1*ms_diff) : ((s_diff == 1) ? (1000000-l_fracsec+tv.tv_usec) : ((1000000*(s_diff-1))+(1000000-l_fracsec+tv.tv_usec))));

               memset(cmd,'\0',500);
               sprintf(cmd," %d,%d,%ld,%ld,%ld,%ld,%ld\n",temp_cfg->idcode,temp_cfg->pmu[j]->idcode,l_soc,l_fracsec,local_soc,local_fsec,ms_diff);
               dataCollectInBuffer(cmd, delayBuff,5);
	
		 	pthread_mutex_unlock(&mutex_phasor_buffer);
			j++;
		} //While ends 

	} else {

		printf("NO CFG for data frames\n");	
	}  

	pthread_mutex_unlock(&mutex_MYSQL_CONN_ON_DATA);	

	free(cmd);
	free(sync);
	free(framesize);
	free(idcode);
	free(soc);
	free(fracsec);
	free(timequality);
	free(stat);
	free(phasors);
	free(analogs);
	free(digital);
	free(freq);
	free(dfreq);

	free(fp_left);
	free(fp_right);
	free(f_left);
	free(f_right);

	if((config_change == 14) ||(config_change == 10)) 
		return config_change;
	else return stat_status;
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  check_statword():                                	     		*/
/* Check the STAT word of the data frames for any change in the data block.	*/
/* Some of the prime errors are handled.				 	*/
/* ----------------------------------------------------------------------------	*/

int check_statword(unsigned char stat[]) { 

	int ret = 0;

	if(stat[0] == 0x0f) {

		// DEVELOPERS has used these bits as an indication for PMU data that has not arrived/missing data frames
		ret = 16;
		return ret;

	} else if ((stat[0] & 0x04) == 0x04) {

		printf("Configuration Change error\n");
		ret = 10;
		return ret;

	} else if ((stat[0] & 0x40) == 0x40) {

		printf("PMU error including configuration error\n");
		ret = 14;
		return ret;

	} else if((stat[0] & 0x80) == 0x80) {

		printf("Data invalid\n");
		ret = 15;
		return ret;

	} else if ((stat[0] & 0x20) == 0x20) {

		printf("PMU Sync error\n");
		ret = 13;
		return ret;

	} else if ((stat[0] & 0x10) == 0x10) {

		printf("Data sorting error\n");
		ret = 12;
		return ret;

	} else if ((stat[0] & 0x08) == 0x08) {

		printf("PMU Trigger error\n");
		ret = 11;
		return ret;

	} 

	printf("Return from STAT Word checking %d.\n",ret);
	return ret;
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  to_intconvertor():                                	     		*/
/* ----------------------------------------------------------------------------	*/

unsigned int to_intconvertor(unsigned char array[]) {

	unsigned int n;
	n = array[0];
	n <<= 8;
	n |= array[1];
	return n;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  to_intconvertor1():                                	     		*/
/* Written by Gopal to convert just 1 byte to int                               */
/* ----------------------------------------------------------------------------	*/

unsigned int to_intconvertor1(unsigned char array[]) {

	unsigned int n;
	n = array[0];
	return n;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  to_long_int_convertor():                                	     	*/
/* ----------------------------------------------------------------------------	*/

unsigned long int to_long_int_convertor(unsigned char array[]) {

	unsigned long int n;
	n = array[0];
	n <<= 8;
	n |= array[1];
	n <<= 8;
	n |= array[2];
	n <<= 8;
	n |= array[3];
	return n;

}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  to_long_int_convertor1():                                	     	*/
/* Written by Gopal to convert just 3 bytes to long int                         */
/* ----------------------------------------------------------------------------	*/

unsigned long int to_long_int_convertor1(unsigned char array[]) {

	unsigned long int n;
	n = array[0];
	n <<= 8;
	n |= array[1];
	n <<= 8;
	n |= array[2];
	return n;

}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION decode_ieee_single():                                	     	*/
/* ----------------------------------------------------------------------------	*/

float decode_ieee_single(const void *v) {

	const unsigned char *data = v;
	int s, e;
	unsigned long src;
	long f;
	float value;

	src = ((unsigned long)data[0] << 24) |
			((unsigned long)data[1] << 16) |
			((unsigned long)data[2] << 8) |
			((unsigned long)data[3]);

	s = (src & 0x80000000UL) >> 31;
	e = (src & 0x7F800000UL) >> 23;
	f = (src & 0x007FFFFFUL);

	if (e == 255 && f != 0) {
		/* NaN (Not a Number) */
		value = DBL_MAX;

	} else if (e == 255 && f == 0 && s == 1) {
		/* Negative infinity */
		value = -DBL_MAX;
	} else if (e == 255 && f == 0 && s == 0) {
		/* Positive infinity */
		value = DBL_MAX;
	} else if (e > 0 && e < 255) {
		/* Normal number */
		f += 0x00800000UL;
		if (s) f = -f;
		value = ldexp(f, e - 150);
	} else if (e == 0 && f != 0) {
		/* Denormal number */
		if (s) f = -f;
		value = ldexp(f, -149);
	} else if (e == 0 && f == 0 && s == 1) {
		/* Negative zero */
		value = 0;
	} else if (e == 0 && f == 0 && s == 0) {
		/* Positive zero */
		value = 0;
	} else {
		/* Never happens */
		printf("s = %d, e = %d, f = %lu\n", s, e, f);
		assert(!"Woops, unhandled case in decode_ieee_single()");
	}

	return value;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION copy_cbyc():                                	     		*/
/* ----------------------------------------------------------------------------	*/

void copy_cbyc(unsigned char dst[],unsigned char *s,int size) {

	int i;
	for(i = 0; i< size; i++)
		dst[i] = *(s + i);	
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION ncmp_cbyc():                                	     		*/
/* ----------------------------------------------------------------------------	*/

int ncmp_cbyc(unsigned char dst[],unsigned char src[],int size) {

	int i,flag = 0;
	for(i = 0; i< size; i++)	{

		if(dst[i] != src[i]) {

			flag = 1;
			break; 	
		}	
	}		
	return flag;
}



void dataCollectInBuffer(char *cmd, char *buffer, int type)
{

    int err;
    if((BUFF_LEN - strlen(buffer)) < strlen(cmd)) {

        char *ttt;
        ttt = malloc((BUFF_LEN+1)*sizeof(unsigned char));
        memset(ttt,'\0',BUFF_LEN+1);
        strncpy(ttt,buffer,BUFF_LEN+1);
        memset(buffer,'\0',BUFF_LEN);
        strncpy(buffer,cmd,strlen(cmd));
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_t thread1;


        if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) { // In  the detached state, the thread resources are
            // immediately freed when it terminates, but 
            perror(strerror(err));                                          // pthread_join(3) cannot be used to synchronize
            exit(1);                                                        //  on the thread termination.         
        }

        //int er;
        struct DataBuff *buf=malloc(sizeof(struct DataBuff));
        buf->data = ttt;
        buf->type = type;	
        if((err = pthread_create(&thread1,&attr,dataWriteInFile,(void *)buf))) {

            perror(strerror(err));
            exit(1);
        }										

    } else {
        strcat(buffer,cmd);
    }
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  dataWriteInFile():                                	     		*/
/* 								 		*/
/* ----------------------------------------------------------------------------	*/

void* dataWriteInFile(void * temp_buff) {

	FILE *p,*f,*a,*d,*e;

	struct DataBuff *buf = (struct DataBuff*)temp_buff;
	char *phasorFile = "PHASOR_MEASUREMENTS.txt";
	char *freqFile = "FREQUENCY_MEASUREMENTS.txt";
	char *analogFile = "ANALOG_MEASUREMENTS.txt";
	char *digitalFile = "DIGITAL_MEASUREMENTS.txt";
	char *frameDelay = "RECEIVED_FRAME_TIME.txt";

	if(buf->type == 1) {
	     
		p = fopen(phasorFile,"w");
		fprintf(p,buf->data);
		fclose(p);

		f = fopen(freqFile,"w");
		fprintf(f,frequencyBuff);
		memset(frequencyBuff,'\0',BUFF_LEN);
		fclose(f);

		a = fopen(analogFile,"w");
		fprintf(a,analogBuff);
		memset(analogBuff,'\0',BUFF_LEN);
		fclose(a);

		d = fopen(digitalFile,"w");
		fprintf(d,digitalBuff);
		memset(digitalBuff,'\0',BUFF_LEN);
		fclose(d);

		e = fopen(frameDelay,"w");
		fprintf(e,delayBuff);
		memset(delayBuff,'\0',BUFF_LEN);
		fclose(e);

	} else if(buf->type == 2) {

		f = fopen(freqFile,"w");
		fprintf(f,buf->data);
		fclose(f);
		
		p = fopen(phasorFile,"w");
		fprintf(p,phasorBuff);
		memset(phasorBuff,'\0',BUFF_LEN);
		fclose(p);

		a = fopen(analogFile,"w");
		fprintf(a,analogBuff);
		memset(analogBuff,'\0',BUFF_LEN);
		fclose(a);

		d = fopen(digitalFile,"w");
		fprintf(d,digitalBuff);
		memset(digitalBuff,'\0',BUFF_LEN);
		fclose(d);

		e = fopen(frameDelay,"w");
		fprintf(e,delayBuff);
		memset(delayBuff,'\0',BUFF_LEN);
		fclose(e);

	} else if(buf->type == 3) {

		a = fopen(analogFile,"w");
		fprintf(a,buf->data);
		fclose(a);

		p = fopen(phasorFile,"w");
		fprintf(p,phasorBuff);
		memset(phasorBuff,'\0',BUFF_LEN);
		fclose(p);

		f = fopen(freqFile,"w");
		fprintf(f,frequencyBuff);
		memset(frequencyBuff,'\0',BUFF_LEN);
		fclose(f);

		d = fopen(digitalFile,"w");
		fprintf(d,digitalBuff);
		memset(digitalBuff,'\0',BUFF_LEN);
		fclose(d);

		e = fopen(frameDelay,"w");
		fprintf(e,delayBuff);
		memset(delayBuff,'\0',BUFF_LEN);
		fclose(e);

	} else if(buf->type == 4) {

		d = fopen(digitalFile,"w");
		fprintf(d,buf->data);
		fclose(d);

		p = fopen(phasorFile,"w");
		fprintf(p,phasorBuff);
		memset(phasorBuff,'\0',BUFF_LEN);
		fclose(p);

		f = fopen(freqFile,"w");
		fprintf(f,frequencyBuff);
		memset(frequencyBuff,'\0',BUFF_LEN);
		fclose(f);

		a = fopen(analogFile,"w");
		fprintf(d,analogBuff);
		memset(analogBuff,'\0',BUFF_LEN);
		fclose(d);

		e = fopen(frameDelay,"w");
		fprintf(e,delayBuff);
		memset(delayBuff,'\0',BUFF_LEN);
		fclose(e);

	} else if(buf->type == 5) {

		d = fopen(digitalFile,"w");
		fprintf(d,buf->data);
		fclose(d);

		p = fopen(phasorFile,"w");
		fprintf(p,phasorBuff);
		memset(phasorBuff,'\0',BUFF_LEN);
		fclose(p);

		f = fopen(freqFile,"w");
		fprintf(f,frequencyBuff);
		memset(frequencyBuff,'\0',BUFF_LEN);
		fclose(f);

		a = fopen(analogFile,"w");
		fprintf(d,analogBuff);
		memset(analogBuff,'\0',BUFF_LEN);
		fclose(d);

		e = fopen(frameDelay,"w");
		fprintf(e,delayBuff);
		memset(delayBuff,'\0',BUFF_LEN);
		fclose(e);
	}
	
    system("mysqlimport --local --fields-terminated-by=\, --fields-enclosed-by=\\\" -uroot -proot iPDC PHASOR_MEASUREMENTS.txt");
    system("mysqlimport --local --fields-terminated-by=\, --fields-enclosed-by=\\\" -uroot -proot iPDC FREQUENCY_MEASUREMENTS.txt");
    system("mysqlimport --local --fields-terminated-by=\, --fields-enclosed-by=\\\" -uroot -proot iPDC ANALOG_MEASUREMENTS.txt");
    system("mysqlimport --local --fields-terminated-by=\, --fields-enclosed-by=\\\" -uroot -proot iPDC DIGITAL_MEASUREMENTS.txt");
    system("mysqlimport --local --fields-terminated-by=\, --fields-enclosed-by=\\\" -uroot -proot iPDC RECEIVED_FRAME_TIME.txt");

//    	system("./upDateTable.sh");
	printf("wrote to database\n");

	free((unsigned char*)temp_buff);

	pthread_exit(NULL); /* Exit the thread once the task is done. */

}

/**************************************** End of File *******************************************************/
