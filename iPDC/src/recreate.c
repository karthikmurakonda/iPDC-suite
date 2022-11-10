/* ----------------------------------------------------------------------------- 
 * recreate.c
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
 *		Nitesh Pandit <panditnitesh@gmail.com>
 *		Kedar V. Khandeparkar <kedar.khandeparkar@gmail.com>			
 *
 * ----------------------------------------------------------------------------- */


#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h> 
#include  <pthread.h>
#include  <sys/types.h>
#include  <arpa/inet.h>
#include  "parser.h"
#include  "global.h" 
#include  "connections.h"
#include  "recreate.h"
#include  "new_pmu_or_pdc.h"

/* ------------------------------------------------------------------------------------ */
/*                  Functions defined in recreate.c         	  			*/
/* ------------------------------------------------------------------------------------ */

/*	1.  recreate_cfg_objects()          	    				*/
/*	2.  void init_cfgparser()   	    	          			*/
/*	3.  void recreate_Connection_Table()					*/
/*	4.  void recreate_udp_connections(struct Lower_Layer_Details *t11) 	*/
/*	5.  void recreate_tcp_connections(struct Lower_Layer_Details *t12) 	*/

/* ------------------------------------------------------------------------------------ */



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
	pthread_mutex_init(&mutex_status_change, NULL);
	pthread_mutex_init(&mutex_Lower_Layer_Details, NULL);
	pthread_mutex_init(&mutex_Upper_Layer_Details, NULL);
	pthread_mutex_init(&mutex_on_TSB, NULL);

	CMDSYNC[0] = 0xaa;
	CMDSYNC[1] = 0x41;
	CMDSYNC[2] = '\0';

	CMDCFGSEND[0] = 0x00;
	CMDCFGSEND[1] = 0x05;
	CMDCFGSEND[2] = '\0'; 

	CMDDATASEND[0] = 0x00;
	CMDDATASEND[1] = 0x02;
	CMDDATASEND[2] = '\0'; 

	CMDDATAOFF[0] = 0x00;
	CMDDATAOFF[1] = 0x01;
	CMDDATAOFF[2] = '\0'; 

	DATASYNC[0] = 0xaa;
	DATASYNC[1] = 0x01;
	DATASYNC[2] = '\0';

	CFGSYNC[0] = 0xaa;
	CFGSYNC[1] = 0x31;
	CFGSYNC[2] = '\0';

	int yes =1;

	/* Create UDP socket for DB Server and bind to DBPORT */
	if ((DB_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {

		perror("socket");
		exit(1);

	} else {

		printf("DB Socket:Sucessfully created\n");

	} 	
	if (setsockopt(DB_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	bzero(&DB_Server_addr,sizeof(DB_Server_addr));
	DB_Server_addr.sin_family = AF_INET;            // host byte order
	DB_Server_addr.sin_port = htons(DBPORT);       // short, network byte order
	DB_Server_addr.sin_addr.s_addr = inet_addr(dbserver_ip);    // automatically fill with my IP
	memset(&(DB_Server_addr.sin_zero),'\0', 8);     // zero the rest of the struct

	unsigned char *line;
	unsigned int framesize;

	int tempi;
	char *rline = NULL, *d1;
	ssize_t read;
	size_t len = 0;

	/* Create CFG Objects from cfg.bin*/
	FILE *file = fopen (ipdcFilePath,"rb");

	if(file != NULL)
	{
		tempi = 1;

		while(tempi < 7)    //iPDCServer
		{
			read = getline(&rline, &len, file);
			tempi++;
		}

		if(read > 0)        //LowerDevices
		{
			d1 = strtok (rline," ");
			d1 = strtok (NULL,"\n");
			tempi = atoi(d1);

			while (tempi > 0)
			{
				read = getline(&rline, &len, file);
				tempi--;
			}
		}

		read = getline(&rline, &len, file);     //UpperDevices

		if(read > 0)
		{
			d1 = strtok (rline," ");
			d1 = strtok (NULL,"\n");
			tempi = atoi(d1);

			while (tempi > 0)
			{
				read = getline(&rline, &len, file);
				tempi--;
			}
		}

		read = getline(&rline, &len, file);     //SourcesCFG

		if(read > 0)
		{
			d1 = strtok (rline," ");
			d1 = strtok (NULL,"\n");
			tempi = atoi(d1);

			if (tempi > 0)
			{
				getline(&rline, &len, file);
				framesize = atoi(rline);

				line = malloc(framesize*sizeof(unsigned char));

				fread(line, sizeof(unsigned char), framesize, file);

				init_cfgparser(line);
				free(line);

				tempi--;
			}
		}
		fclose (file);
	}
	else
	{
		printf("iPDC Setup File is missing, iPDC exit.\n");
		exit(1);
	}
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  init_cfgparser():                                	     		*/
/* It is called by recreate_cfg_object() to read the file `cfg.bin`  		*/
/* and create cfg objects in the memory    					*/
/* ----------------------------------------------------------------------------	*/

void init_cfgparser(unsigned char st[]){ 

	unsigned char *s;
	unsigned int phn,ann,dgn,num_pmu;
	int i,j,dgchannels;
	struct cfg_frame *cfg;
	struct channel_names *cn;


	/******************** PARSING BEGINGS *******************/

	cfg = malloc(sizeof(struct cfg_frame));

	if(!cfg) {

		printf("Not enough memory for cfg\n");
	}

	s = st;

	/* Memory Allocation Begins - Allocate memory to framesize */
	cfg->framesize = malloc(3*sizeof(unsigned char));
	if(!cfg->framesize) {
		printf("Not enough memory for cfg->framesize\n");
	}

	// Allocate memory to idcode
	cfg->idcode = malloc(3*sizeof(unsigned char));
	if(!cfg->idcode) {
		printf("Not enough memory for cfg->framesize\n");
	}

	// Allocate memory to soc
	cfg->soc = malloc(5*sizeof(unsigned char));
	if(!cfg->soc) {
		printf("Not enough memory for cfg->soc\n");
	}

	// Allocate memory to fracsec
	cfg->fracsec = malloc(5*sizeof(unsigned char));
	if(!cfg->fracsec) {
		printf("Not enough memory for cfg->fracsec\n");
	}

	// Allocate memory to time_base
	cfg->time_base = malloc(5*sizeof(unsigned char));
	if(!cfg->time_base) {
		printf("Not enough memory for cfg->time_base\n");
	}

	// Allocate memory to num_pmu
	cfg->num_pmu = malloc(3*sizeof(unsigned char));
	if(!cfg->num_pmu) {
		printf("Not enough memory for cfg->num_pmu\n");
	}

	// Allocate memory to data_rate
	cfg->data_rate = malloc(3*sizeof(unsigned char));
	if(!cfg->data_rate) {
		printf("Not enough memory for cfg->data_rate\n");
	}

	// Skip the sync word
	s = s + 2;	

	// Separate the FRAME SIZE
	copy_cbyc (cfg->framesize,(unsigned char *)s,2);
	cfg->framesize[2]='\0';
	s = s + 2;

	//SEPARATE IDCODE
	copy_cbyc (cfg->idcode,(unsigned char *)s,2);
	cfg->idcode[2]='\0';
	int id = to_intconvertor(cfg->idcode);
	printf("ID Code %d\n",id);
	s = s + 2;

	//SEPARATE SOC
	copy_cbyc (cfg->soc,(unsigned char *)s,4);
	cfg->soc[4]='\0';
	s = s + 4;

	//SEPARATE FRACSEC
	copy_cbyc (cfg->fracsec,(unsigned char *)s,4);
	cfg->fracsec[4]='\0';
	s = s + 4;

	//SEPARATE TIMEBASE
	copy_cbyc (cfg->time_base,(unsigned char *)s,4);
	cfg->time_base[4]='\0';
	s = s + 4;

	//SEPARATE PMU NUM
	copy_cbyc (cfg->num_pmu,(unsigned char *)s,2);
	cfg->num_pmu[2]='\0';
	s = s + 2;	

	num_pmu = to_intconvertor(cfg->num_pmu);
	printf("Number of PMU's = %d.\n",num_pmu);

	// Allocate Memeory For Each PMU
	cfg->pmu = malloc(num_pmu* sizeof(struct for_each_pmu *));
	if(!cfg->pmu) {
		printf("Not enough memory pmu[][]\n");
		exit(1);
	}

	for (i = 0; i < num_pmu; i++) {

		cfg->pmu[i] = malloc(sizeof(struct for_each_pmu));
	}

	j = 0;

	///WHILE EACH PMU IS HANDLED
	while(j<num_pmu) {

		// Memory Allocation for stn
		cfg->pmu[j]->stn = malloc(17*sizeof(unsigned char));
		if(!cfg->pmu[j]->stn) {
			printf("Not enough memory cfg->pmu[j]->stn\n");
			exit(1);
		}

		// Memory Allocation for idcode
		cfg->pmu[j]->idcode = malloc(3*sizeof(unsigned char));
		if(!cfg->pmu[j]->idcode) {
			printf("Not enough memory cfg->pmu[j]->idcode\n");
			exit(1);
		}

		// Memory Allocation for format
		cfg->pmu[j]->data_format = malloc(3*sizeof(unsigned char));
		if(!cfg->pmu[j]->data_format) {
			printf("Not enough memory cfg->pmu[j]->data_format\n");
			exit(1);
		}

		// Memory Allocation for phnmr
		cfg->pmu[j]->phnmr = malloc(3*sizeof(unsigned char));
		if(!cfg->pmu[j]->phnmr) {
			printf("Not enough memory cfg->pmu[j]->phnmr\n");
			exit(1);
		}

		// Memory Allocation for annmr
		cfg->pmu[j]->annmr = malloc(3*sizeof(unsigned char));
		if(!cfg->pmu[j]->annmr) {
			printf("Not enough memory cfg->pmu[j]->annmr\n");
			exit(1);
		}

		// Memory Allocation for dgnmr
		cfg->pmu[j]->dgnmr = malloc(3*sizeof(unsigned char));
		if(!cfg->pmu[j]->dgnmr) {
			printf("Not enough memory cfg->pmu[j]->dgnmr\n");
			exit(1);
		}

		// Memory Allocation for fnom
		cfg->pmu[j]->fnom = malloc(3*sizeof(unsigned char));
		if(!cfg->pmu[j]->fnom) {
			printf("Not enough memory cfg->pmu[j]->fnom\n");
			exit(1);
		}

		// Memory Allocation for cfg_cnt
		cfg->pmu[j]->cfg_cnt = malloc(3*sizeof(unsigned char));
		if(!cfg->pmu[j]->cfg_cnt) {
			printf("Not enough memory cfg->pmu[j]->cfg_cnt\n");
			exit(1);
		}

		//SEPARATE STATION NAME
		copy_cbyc (cfg->pmu[j]->stn,(unsigned char *)s,16);
		cfg->pmu[j]->stn[16]='\0';
		s = s + 16;

		//SEPARATE IDCODE		
		copy_cbyc (cfg->pmu[j]->idcode,(unsigned char *)s,2);
		cfg->pmu[j]->idcode[2]='\0';
		s = s + 2;

		//SEPARATE DATA FORMAT		
		copy_cbyc (cfg->pmu[j]->data_format,(unsigned char *)s,2);
		cfg->pmu[j]->data_format[2]='\0';
		s = s + 2;	

		// USE fmt
		unsigned char hex = cfg->pmu[j]->data_format[1];
		hex <<= 4;

		// Extra field has been added to identify polar,rectangular,floating/fixed point	
		cfg->pmu[j]->fmt = malloc(sizeof(struct format));
		if((hex & 0x80) == 0x80) cfg->pmu[j]->fmt->freq = '1'; else cfg->pmu[j]->fmt->freq = '0';
		if((hex & 0x40) == 0x40 ) cfg->pmu[j]->fmt->analog = '1'; else cfg->pmu[j]->fmt->analog = '0';
		if((hex & 0x20) == 0x20) cfg->pmu[j]->fmt->phasor = '1'; else cfg->pmu[j]->fmt->phasor = '0';
		if((hex & 0x10) == 0x10) cfg->pmu[j]->fmt->polar =  '1'; else cfg->pmu[j]->fmt->polar = '0';

		//SEPARATE PHASORS	
		copy_cbyc (cfg->pmu[j]->phnmr,(unsigned char *)s,2);
		cfg->pmu[j]->phnmr[2]='\0';
		s = s + 2;
		phn = to_intconvertor(cfg->pmu[j]->phnmr);

		//SEPARATE ANALOGS			
		copy_cbyc (cfg->pmu[j]->annmr,(unsigned char *)s,2);
		cfg->pmu[j]->annmr[2]='\0';
		s = s + 2;
		ann = to_intconvertor(cfg->pmu[j]->annmr);

		//SEPARATE DIGITALS			
		copy_cbyc (cfg->pmu[j]->dgnmr,(unsigned char *)s,2);
		cfg->pmu[j]->dgnmr[2] = '\0';
		s = s + 2; 
		dgn = to_intconvertor(cfg->pmu[j]->dgnmr);
		printf("CFG consist Phasor = %d, Analogs = %d, and Digitals = %d.\n",phn,ann,dgn);

		cn = malloc(sizeof(struct channel_names));
		if(!cn) {
			printf("Not enough memory cn\n");
			exit(1);
		}
		cn->first =  NULL;

		////SEPARATE PHASOR NAMES 
		if(phn != 0){

			cn->phnames = malloc(phn*sizeof(unsigned char*));
			if(!cn->phnames) {
				printf("Not enough memory cfg->pmu[j]->cn->phnames[][]\n");
				exit(1);
			}

			for (i = 0; i < phn; i++) {

				cn->phnames[i] = malloc(17*sizeof(unsigned char));
			}

			i = 0;//Index for PHNAMES

			while(i<phn){
				copy_cbyc (cn->phnames[i],(unsigned char *)s,16);
				cn->phnames[i][16] = '\0';        
				printf("Phnames %s\n",cn->phnames[i]);
				s = s + 16;  
				i++;
			}
		} 

		//SEPARATE ANALOG NAMES
		if(ann!=0){
			cn->angnames = malloc(ann*sizeof(unsigned char*));
			if(!cn->angnames) {
				printf("Not enough memory cfg->pmu[j]->cn->phnames[][]\n");
				exit(1);
			}

			for (i = 0; i < ann; i++) {

				cn->angnames[i] = malloc(17*sizeof(unsigned char));
			}

			i=0;//Index for ANGNAMES

			while(i<ann){
				copy_cbyc (cn->angnames[i],(unsigned char *)s,16);
				cn->angnames[i][16]='\0';
				printf("ANGNAMES %s\n",cn->angnames[i]);
				s =s + 16;  
				i++;			
			}
		}

		i = 0; //Index for number of dgwords
		struct dgnames *q;

		while(i<dgn) {

			struct dgnames *temp1 = malloc(sizeof(struct dgnames));

			temp1->dgn = malloc(16*sizeof(unsigned char *));
			if(!temp1->dgn) {
				printf("Not enough memory temp1->dgn\n");
				exit(1);
			}

			for (i = 0; i < 16; i++) {

				temp1->dgn[i] = malloc(17*sizeof(unsigned char));
			}

			temp1->dg_next = NULL;

			for(dgchannels=0;dgchannels<16;dgchannels++){

				copy_cbyc (temp1->dgn[dgchannels],(unsigned char *)s,16);
				temp1->dgn[dgchannels][16]='\0';
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
		if(phn != 0){

			cfg->pmu[j]->phunit = malloc(phn*sizeof(unsigned char*));
			if(!cfg->pmu[j]->phunit) {
				printf("Not enough memory cfg->pmu[j]->phunit[][]\n");
				exit(1);
			}

			for (i = 0; i < phn; i++) {

				cfg->pmu[j]->phunit[i] = malloc(5*sizeof(unsigned char));
			}

			i = 0;

			while(i<phn){ //Separate the Phasor conversion factors

				copy_cbyc (cfg->pmu[j]->phunit[i],(unsigned char *)s,4);
				cfg->pmu[j]->phunit[i][4] = '\0';
				s = s + 4;
				i++;
			}
		}//if for PHASOR Factors ends

		//ANALOG FACTORS
		if(ann != 0){

			cfg->pmu[j]->anunit = malloc(ann*sizeof(unsigned char*));
			if(!cfg->pmu[j]->anunit) {
				printf("Not enough memory cfg->pmu[j]->anunit[][]\n");
				exit(1);
			}

			for (i = 0; i < ann; i++) {

				cfg->pmu[j]->anunit[i] = malloc(5*sizeof(unsigned char));

			}

			i = 0;

			while(i<ann){ //Separate the Phasor conversion factors
				copy_cbyc (cfg->pmu[j]->anunit[i],(unsigned char *)s,4);
				cfg->pmu[j]->anunit[i][4] = '\0';
				s = s + 4;
				i++;
			}

		} // if for ANALOG Factors ends

		if(dgn != 0){

			cfg->pmu[j]->dgunit = malloc(dgn*sizeof(unsigned char*));
			if(!cfg->pmu[j]->dgunit) {
				printf("Not enough memory cfg->pmu[j]->dgunit[][]\n");
				exit(1);
			}

			for (i = 0; i < dgn; i++) {
				cfg->pmu[j]->dgunit[i] = malloc(5*sizeof(unsigned char));
			}

			i = 0;

			while(i<dgn){ //Separate the Phasor conversion factors

				copy_cbyc (cfg->pmu[j]->dgunit[i],(unsigned char *)s,4);
				cfg->pmu[j]->dgunit[i][4] = '\0';
				s = s + 4;
				i++;
			}
		} //if for Digital Words Factors ends

		copy_cbyc (cfg->pmu[j]->fnom,(unsigned char *)s,2);
		cfg->pmu[j]->fnom[2]='\0';
		s = s + 2;

		copy_cbyc (cfg->pmu[j]->cfg_cnt,(unsigned char *)s,2);
		cfg->pmu[j]->cfg_cnt[2] = '\0';
		s = s + 2;	
		j++;
	}//While for PMU number ends

	copy_cbyc (cfg->data_rate,(unsigned char *)s,2);
	cfg->data_rate[2] = '\0';
	cfg->cfgnext = NULL;

	if (cfgfirst == NULL)  {
		cfgfirst = cfg;	
	} else {

		struct cfg_frame *t=cfgfirst;
		while(t->cfgnext != NULL){
			t = t->cfgnext;
		}
		t->cfgnext = cfg;
	}
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  recreate_Connection_Table():                   		 	*/
/* It re-creates objects by reading the file ipdcINFO.bin.*/
/* ----------------------------------------------------------------------------	*/

void recreate_Connection_Table() {

	char line[40],*ip,*port,*protocol,*id, *latitude, *longitude;
	char *rline = NULL, *d1;
	int err, tempi;
	FILE *f;
	ssize_t read;
	size_t len = 0;

	// A new thread is created for each TCP connection in 'detached' mode. Thus allowing any number of threads to be created. 
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/* In  the detached state, the thread resources are immediately freed when it terminates, but 
	   pthread_join(3) cannot be used to synchronize on the thread termination. */
	if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) { 

		perror(strerror(err));	                         
		exit(1);					

	}								

	/* Shed policy = SCHED_FIFO (realtime, first-in first-out) */
	if((err = pthread_attr_setschedpolicy(&attr,SCHED_FIFO))) { 

		perror(strerror(err));		     
		exit(1);
	}  

	f = fopen(ipdcFilePath,"rb");
	tempi = 1;

	while(tempi < 7)
	{
		read = getline(&rline, &len, f);
		tempi++;
	}

	if(read > 0)
	{
		d1 = strtok (rline," ");
		d1 = strtok (NULL,"\n");
		tempi = atoi(d1);

		if (tempi > 0)      //1 if
		{
			printf("####         CONNECTION TABLE OF SOURCE DEVICES        ####\n");
			printf("-----------------------------------------------------------------\n");
			printf("|   PMU ID	|  Lower IP 	|	Port	|     Protocol  |\n");
			printf("-----------------------------------------------------------------\n");

			while (tempi > 0)  //2 while
			{
				read = getline(&rline, &len, f);

				/* Extract the ip and protocol */
				id = strtok (rline,",");
				ip = strtok (NULL,",");
				port = strtok (NULL,",");
				protocol = strtok (NULL,",");
				latitude = strtok (NULL,",");
				longitude = strtok (NULL,"\n");
				protocol[3] = '\0';
				printf("|\t%d\t|%s\t|\t%d\t|\t%s\t|\n",atoi(id),ip,atoi(port),protocol);

				struct Lower_Layer_Details *temp_pmu;

				temp_pmu = malloc(sizeof(struct Lower_Layer_Details));
				if(!temp_pmu) {

					printf("Not enough memory temp_pmu\n");
					exit(1);
				}

				temp_pmu->pmuid = atoi(id);
				strcpy(temp_pmu->ip,ip);
				temp_pmu->port = atoi(port);
				strcpy(temp_pmu->protocol,protocol);
				temp_pmu->latitude = atof(latitude);
				temp_pmu->longitude = atof(longitude);
				temp_pmu->data_transmission_off = 0;
				temp_pmu->pmu_remove = 0;
				temp_pmu->request_cfg_frame = 0;
				temp_pmu->next = NULL;
				temp_pmu->prev = NULL;

				if(!strncasecmp(temp_pmu->protocol,"UDP",3)) {

					recreate_udp_connections(temp_pmu);
				}

				if(!strncasecmp(temp_pmu->protocol,"TCP",3)) {

					recreate_tcp_connections(temp_pmu);
				}

				memset(line,'\0',40);
				tempi--;
			}
			printf("-----------------------------------------------------------------\n");

		} else {

			printf("Source Devices Not Found.\n");
		}
	}

	read = getline(&rline, &len, f);

	if(read > 0)
	{
		d1 = strtok (rline," ");
		d1 = strtok (NULL,"\n");
		tempi = atoi(d1);

		if (tempi > 0)      //1 if
		{
			printf("####  CONNECTION TABLE OF DESTINATION DEVICES  ####\n");
			printf("-------------------------------------------------\n");
			printf("|  Upper IP 	|	Port	|     Protocol  |\n");
			printf("-------------------------------------------------\n");

			while (tempi > 0)  //2 while
			{
				read = getline(&rline, &len, f);

				/* Extract the ip and protocol */
				ip = strtok (rline,",");
				port = strtok (NULL,",");
				protocol = strtok (NULL,"\n");
				protocol[3] = '\0';
				printf("|%s\t|\t%d\t|\t%s\t|\n",ip,atoi(port),protocol);

				struct Upper_Layer_Details *temp_pdc;
				temp_pdc = malloc(sizeof(struct Upper_Layer_Details));
				if(!temp_pdc) {

					printf("Not enough memory temp_pmu\n");
					exit(1);
				}

				strcpy(temp_pdc->ip,ip);
				temp_pdc->port = atoi(port);
				strcpy(temp_pdc->protocol,protocol);

				bzero(&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr));
				temp_pdc->pdc_addr.sin_family = AF_INET;
				temp_pdc->pdc_addr.sin_addr.s_addr = inet_addr(temp_pdc->ip);
				temp_pdc->pdc_addr.sin_port = htons(temp_pdc->port);

				memset(&(temp_pdc->pdc_addr.sin_zero), '\0', 8);   // zero the rest of the struct
				temp_pdc->config_change = 0;
				temp_pdc->UL_upper_pdc_cfgsent = 0;
				temp_pdc->UL_data_transmission_off = 1;
				temp_pdc->address_set = 0;
				temp_pdc->tcpup = 1;

				if(ULfirst == NULL) {

					ULfirst = temp_pdc;
					temp_pdc->prev = NULL;

				} else {

					ULlast->next = temp_pdc;
					temp_pdc->prev = ULlast;
				}

				ULlast = temp_pdc;
				temp_pdc->next = NULL;
				memset(line,'\0',40);

				tempi--;
			}
			printf("-------------------------------------------------\n");
		} else {

			printf("Destination Devices Not Found.\n");
		}
	}
	fclose(f);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  recreate_udp_connections(struct Lower_Layer_Details *t11):         */
/* ----------------------------------------------------------------------------	*/

void recreate_udp_connections(struct Lower_Layer_Details *t11) {

	int err;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/* In  the detached state, the thread resources are immediately freed when it terminates, but 
	   pthread_join(3) cannot be used to synchronize on the thread termination. */
	if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) { 

		perror(strerror(err));	                         
		exit(1);					
	}								

	/* Shed policy = SCHED_FIFO (realtime, first-in first-out) */
	if((err = pthread_attr_setschedpolicy(&attr,SCHED_FIFO))) { 

		perror(strerror(err));		     
		exit(1);
	}  

	pthread_t t;

	if(pthread_create(&t,&attr,connect_pmu_udp,(void *)t11)) {

		perror(strerror(err));		     
		exit(1);
	}						
}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  recreate_tcp_connections(struct Lower_Layer_Details *t12):         */
/* ----------------------------------------------------------------------------	*/

void recreate_tcp_connections(struct Lower_Layer_Details *t12) {

	int err;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/* In  the detached state, the thread resources are immediately freed when it terminates, but 
	   pthread_join(3) cannot be used to synchronize on the thread termination. */
	if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) { 

		perror(strerror(err));	                         
		exit(1);					
	}

	/* Shed policy = SCHED_FIFO (realtime, first-in first-out) */
	if((err = pthread_attr_setschedpolicy(&attr,SCHED_FIFO))) { 

		perror(strerror(err));		     
		exit(1);
	}  

	pthread_t t;

	if(pthread_create(&t,&attr,connect_pmu_tcp,(void *)t12)) {			
		perror(strerror(err));		     
		exit(1);
	}										
}

/**************************************** End of File *******************************************************/
