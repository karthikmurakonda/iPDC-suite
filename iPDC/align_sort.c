/* ----------------------------------------------------------------------------- 
 * align_sort.c
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <pthread.h>
#include "parser.h"
#include "global.h" 
#include "align_sort.h" 
#include "connections.h"
#include "dallocate.h"


/* ------------------------------------------------------------------------------------	*/
/*				Functions in align_sort.c				*/
/* ------------------------------------------------------------------------------------	*/

/*	1.  void time_align(struct data_frame *df)          	    	*/
/*	2.  void assign_df_to_TSB(struct data_frame *df,int index) 	*/
/*	3.  void dispatch(int index)					*/
/*	4.  void sort_data_inside_TSB(int index)			*/
/*	5.  void clear_TSB(int index)					*/
/*	6.  void create_dataframe(int index)				*/
/*	7.  void create_cfgframe()					*/

/* -----------------------------------------------------------------------------*/


int i;
int front = -1;
int rear = -1;
pthread_mutex_t mutex_on_TSB = PTHREAD_MUTEX_INITIALIZER;


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  time_align():                                	     		*/
/* It searches for the correct TSB[index] where data frame df is to be 		*/
/* assigned. If the df has soc and fracsec which is older then soc and fracsec  */
/* of TSB[first] then we discard the data frame					*/
/* ----------------------------------------------------------------------------	*/

void time_align(struct data_frame *df) {

	int flag = 0;

	pthread_mutex_lock(&mutex_on_TSB);
	if(front == -1) { // TSB is used for the first time

		front = rear = 0;
		assign_df_to_TSB(df,front);	
		pthread_mutex_unlock(&mutex_on_TSB);
		return;	

	} else {

		unsigned long int df_soc,df_fracsec,tsb_soc,tsb_fracsec;

		df_soc = to_long_int_convertor(df->soc);
		df_fracsec = to_long_int_convertor(df->fracsec);
		tsb_soc = to_long_int_convertor((unsigned char *)TSB[front].soc);
		tsb_fracsec = to_long_int_convertor((unsigned char *)TSB[front].fracsec);

		if((df_soc < tsb_soc) || ((df_soc == tsb_soc) && (df_fracsec < tsb_fracsec))) {

			pthread_mutex_unlock(&mutex_on_TSB);
			return;	
		}

		if(rear >= front) {
			for(i = rear; i>= front; i--) {

				if(!ncmp_cbyc ((unsigned char *)TSB[i].soc,df->soc,4)) {

					if(!ncmp_cbyc ((unsigned char *)TSB[i].fracsec,df->fracsec,3)) { /* 3 bytes is actual fraction of seconds and 1 byte is Time quality  */ 

						flag = 1;
						break;

					} 

				} else {

					continue;

				} //if for soc ends
			}// for ends					
		} else {

			for(i = rear; i< front; i++) {

				if(!ncmp_cbyc ((unsigned char *)TSB[i].soc,df->soc,4)) {

					if(!ncmp_cbyc ((unsigned char *)TSB[i].fracsec,df->fracsec,3)) { /* 3 bytes is actual fraction of seconds and 1 byte is Time quality  */ 

						flag = 1;
						break;
					} 

				} else {

					continue;

				} //if for soc ends
			}// for ends					
		}

	} // if other than the front = -1

	if(flag) {

		//Assign to existing TSB
		assign_df_to_TSB(df,i);				

	} else {

		rear = (rear + 1) % MAXTSB;

		if(front == rear) { // All TSB are full. Dispatch the TSB[rear] and use it to assign new df

			dispatch(rear);	
			front = (front + 1) % MAXTSB;
			assign_df_to_TSB(df,rear);

		} else {

			assign_df_to_TSB(df,rear);					

		}
	}
	pthread_mutex_unlock(&mutex_on_TSB);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  assign_df_to_TSB():                               	     		*/
/* It assigns the arrived data frame df to TSB[index]				*/
/* ----------------------------------------------------------------------------	*/

void assign_df_to_TSB(struct data_frame *df,int index) { 

	/* Check if the TSB is used for the first time. If so we need to 
	   allocate memory to its member variables */
	if(TSB[index].soc == NULL) { // 1 if

		struct  cfg_frame *temp_cfg = cfgfirst;

		TSB[index].soc = malloc(5);
		TSB[index].fracsec = malloc(5);

		memset(TSB[index].soc,'\0',5);
		memset(TSB[index].fracsec,'\0',5);

		copy_cbyc((unsigned char *)TSB[index].soc,df->soc,4);
		copy_cbyc((unsigned char *)TSB[index].fracsec,df->fracsec,4);

		TSB[index].first_data_frame = df; /* Assign df to the 'first_data_frame' in the data frame linked list of TSB[index] */

		/* Now we need to store the pmu/pdc id in the pmupdc_id_list that would be required while sorting */
		struct pmupdc_id_list *temp_pmuid;
		while(temp_cfg != NULL) {

			/* Create a node of the type 'pmupdc_id_list' and copy the pmu/pde id from the cfg to it */
			struct pmupdc_id_list *pmuid = malloc(sizeof(struct pmupdc_id_list));
			pmuid->idcode = malloc(3);
			memset(pmuid->idcode,'\0',3);
			copy_cbyc((unsigned char *)pmuid->idcode,temp_cfg->idcode,2);
			pmuid->num_pmu = to_intconvertor(temp_cfg->num_pmu);
			pmuid->nextid = NULL;

			if(TSB[index].idlist == NULL) { /* Assign the pmuid to the idlist as it is the first id in the list */

				TSB[index].idlist = temp_pmuid = pmuid;

			} else {

				temp_pmuid->nextid = pmuid;
				temp_pmuid = pmuid;

			}			

			temp_cfg = temp_cfg->cfgnext;				
		} // while ends . A pmu/pdc id list is created for the TSB[index]

	} else { // 1 if else 

		struct  cfg_frame *temp_cfg = cfgfirst;
		if(TSB[index].first_data_frame == NULL) { // 2 if 

			/* After TSB[index] is cleared this is the first data frame for it. 
			The memory for the member variables of TSB[index] has already 
			been allocated. Hence after dispatch() and clear_TSB() operation
			this TSB is to be assigned the data_frame for the first time. */

			copy_cbyc((unsigned char *)TSB[index].soc,df->soc,4);
			copy_cbyc((unsigned char *)TSB[index].fracsec,df->fracsec,4);

			TSB[index].first_data_frame = df;     /* Assign df to the 'first_data_frame' in the data frame linked list of TSB[index] */

			/* Now we need to store the pmu/pdc id in the pmupdc_id_list 
			that would be required while sorting */
			struct pmupdc_id_list *temp_pmuid;
			while(temp_cfg != NULL) {

				/* Create a node of the type 'pmupdc_id_list' and 
				copy the pmu/pde id from the cfg to it */

				struct pmupdc_id_list *pmuid = malloc(sizeof(struct pmupdc_id_list));
				pmuid->idcode = malloc(3);
				memset(pmuid->idcode,'\0',3);
				copy_cbyc((unsigned char *)pmuid->idcode,temp_cfg->idcode,2);
				pmuid->num_pmu = to_intconvertor(temp_cfg->num_pmu);	
				pmuid->nextid = NULL;

				if(TSB[index].idlist == NULL) { /* Assign the pmuid to the idlist as it is the first id in the list */

					TSB[index].idlist = temp_pmuid = pmuid;

				} else {

					temp_pmuid->nextid = pmuid;
					temp_pmuid = pmuid;

				}			

				temp_cfg = temp_cfg->cfgnext;				
			} // while ends

		} else { // 2 if else

			/* Traverse the data frames of TSB[index] and assign the df to 'dnext' of 
			the last element in the data frame LL.*/
			struct data_frame *temp_df,*check_df;

			/* Need to check if df with same idcode and soc is already assigned to 
			   the TSB[index] */
			check_df = TSB[index].first_data_frame;
			while(check_df != NULL) {

				if(!ncmp_cbyc(check_df->idcode,df->idcode,2)) {

					free_dataframe_object(df);
					return;					

				} else {

					check_df = check_df->dnext;

				}							
			}

			temp_df = TSB[index].first_data_frame;
			while(temp_df->dnext != NULL) {

				temp_df = temp_df->dnext;			

			}

			temp_df->dnext = df;	 
		} // 2 if ends	

	} // 1 if ends
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  dispatch():                                	     		*/
/* It dispatches the combined data frame to all the destination devices		*/
/* ----------------------------------------------------------------------------	*/

void dispatch(int index) {

	int size,flag = 0;
	sort_data_inside_TSB(index);
	dataframe = NULL;	 
	pthread_mutex_lock(&mutex_Upper_Layer_Details);
	struct Upper_Layer_Details *temp_pdc = ULfirst;

	while(temp_pdc != NULL ) {

		if((temp_pdc->UL_upper_pdc_cfgsent == 1) && (temp_pdc->UL_data_transmission_off == 0)) {			

			if(flag == 0) {

				size = create_dataframe(index);	
				flag = 1;
			}

			if(temp_pdc->config_change == 1) {

				dataframe[14] = 0x04;
				dataframe[15] = 0x00;

			} else {

				dataframe[14] = 0x00;
				dataframe[15] = 0x00;

			}

			if(temp_pdc->port == UDPPORT) {

				if (sendto(temp_pdc->sockfd,dataframe, size, 0,
						(struct sockaddr *)&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr)) == -1)
					perror("sendto");	

			} else if((temp_pdc->port == TCPPORT) && (temp_pdc->tcpup == 1)) {

				if(send(temp_pdc->sockfd,dataframe,size, 0)== -1) {
					perror("send");	
					printf("TCP connection closed\n");	
					temp_pdc->tcpup = 0;
					pthread_cancel(temp_pdc->thread_id);		
				}			
			}		
		}
		temp_pdc = temp_pdc->next;
	}
	pthread_mutex_unlock(&mutex_Upper_Layer_Details);
	if(dataframe != NULL) 	free(dataframe);

	clear_TSB(index);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  sort_data_inside_TSB():                          	     		*/
/* This function sorts the data frames in the TSB[index] in the order of the 	*/
/* Idcodes present in the 'struct pmupdc_id_list list' of the TSB[index]	*/
/* ----------------------------------------------------------------------------	*/

void sort_data_inside_TSB(int index) { 

	struct pmupdc_id_list *temp_list;
	struct data_frame *prev_df,*curr_df,*sorted_df,*r_df,*s_df,*last_df,*p_df;
	int match = 0;
	unsigned int id_check;

	/* Pointer track_df will hold the address of the last sorted data_frame object. 
	Thus we assign to the 'track_df->dnext ' the next sorted data_frame  object and so on */

	temp_list = TSB[index].idlist; /* Starting ID required for sorting */
	last_df = TSB[index].first_data_frame;
	p_df = TSB[index].first_data_frame;

	curr_df = last_df;
	sorted_df = prev_df = NULL;

	while(temp_list != NULL) { // 1 while

		match = 0;
		while(curr_df != NULL) { // 2. Traverse the pmu id in TSB and sort 

			if(!ncmp_cbyc(curr_df->idcode,(unsigned char *)temp_list->idcode,2)){

				match = 1;
				break;

			} else {

				prev_df = curr_df; 
				curr_df = curr_df->dnext;						

			}

		} // 2 while ends	

		if (match == 1) {

			if(prev_df == NULL) {

				r_df = curr_df;
				s_df = curr_df->dnext;
				if(sorted_df == NULL) {

					sorted_df = r_df;
					TSB[index].first_data_frame = sorted_df;
				} else {

					sorted_df->dnext = r_df;
					sorted_df = r_df;
				}
				sorted_df->dnext = s_df ;
				curr_df = last_df = s_df;

			} else {

				if(sorted_df == NULL) {

					r_df = curr_df;
					s_df = r_df->dnext;
					prev_df->dnext = s_df;
					sorted_df = r_df;
					TSB[index].first_data_frame = sorted_df;
					sorted_df->dnext = last_df ;
					curr_df = last_df;
					prev_df = NULL;

				} else {//if(sorted_df != NULL) {

					r_df = curr_df;
					s_df = r_df->dnext;
					prev_df->dnext = s_df;
					sorted_df->dnext = r_df;
					sorted_df = r_df;
					sorted_df->dnext = last_df ;
					curr_df = last_df;
					prev_df = NULL;
				}					
			}                                

		}  else {  // id whose data frame didnot arrive No match

			char *idcode;
			idcode = malloc(3);

			struct data_frame *df = malloc(sizeof(struct data_frame));
			if(!df) {

				printf("Not enough memory data_frame.\n");
			}
			df->dnext = NULL;

			// Allocate memory for df->framesize
			df->framesize = malloc(3);
			if(!df->framesize) {

				printf("Not enough memory df->idcode\n");
				exit(1);
			}

			// Allocate memory for df->idcode
			df->idcode = malloc(3);
			if(!df->idcode) {

				printf("Not enough memory df->idcode\n");
				exit(1);
			}

			// Allocate memory for df->soc
			df->soc = malloc(5);
			if(!df->soc) {

				printf("Not enough memory df->soc\n");
				exit(1);
			}

			// Allocate memory for df->fracsec
			df->fracsec = malloc(5);
			if(!df->fracsec) {

				printf("Not enough memory df->fracsec\n");
				exit(1);
			}

			/* 16 for sync,fsize,idcode,soc,fracsec,checksum */
			unsigned int size = (16 + (temp_list->num_pmu)*2)*sizeof(unsigned char);

			df->num_pmu = temp_list->num_pmu ;

			//Copy FRAMESIZE
			int_to_ascii_convertor(size,df->framesize);
			df->framesize[2] = '\0';

			//Copy IDCODE			
			copy_cbyc (df->idcode,(unsigned char *)temp_list->idcode,2);
			df->idcode[2] = '\0';

			//Copy SOC						
			copy_cbyc (df->soc,(unsigned char *)TSB[index].soc,4);
			df->soc[4] = '\0';

			//Copy FRACSEC						
			copy_cbyc (df->fracsec,(unsigned char *)TSB[index].fracsec,4);
			df->fracsec[4] = '\0';

			df->dpmu = malloc(temp_list->num_pmu * sizeof(struct data_for_each_pmu *));
			if(!df->dpmu) {

				printf("Not enough memory df->dpmu[][]\n");
				exit(1);
			}

			for (i = 0; i < temp_list->num_pmu; i++) {

				df->dpmu[i] = malloc(sizeof(struct data_for_each_pmu));
			}

			int j = 0;		

			// PMU data has not come  
			while(j < temp_list->num_pmu) {

				df->dpmu[j]->stat = malloc(3);
				if(!df->dpmu[j]->stat) {

					printf("Not enough memory for df->dpmu[j]->stat\n");
				}

				df->dpmu[j]->stat[0] = 0x00;
				df->dpmu[j]->stat[1] = 0x0F;
				df->dpmu[j]->stat[2] = '\0';
				j++;
			}

			if(sorted_df == NULL) {

				r_df = df;
				sorted_df = r_df;
				TSB[index].first_data_frame = sorted_df;
				sorted_df->dnext = last_df ;
				curr_df = last_df;
				prev_df = NULL;

			} else {

				r_df = df;
				sorted_df->dnext = r_df;
				sorted_df = r_df;
				sorted_df->dnext = last_df ;
				curr_df = last_df;
				prev_df = NULL;
			}				
		}

		temp_list = temp_list->nextid;  //go for next ID

	} // 1. while ends

	p_df = TSB[index].first_data_frame;
	while(p_df != NULL){

		id_check = to_intconvertor(p_df->idcode);
		p_df = p_df->dnext;
	}
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  clear_TSB():                                	     		*/
/* It clears TSB[index] and frees all data frame objects after the data frames  */
/* in TSB[index] have been dispatched to destination device			*/
/* ----------------------------------------------------------------------------	*/

void clear_TSB(int index) { // 

	unsigned long int tsb_soc,tsb_fracsec;
	tsb_soc = to_long_int_convertor((unsigned char *)TSB[front].soc);
	tsb_fracsec = to_long_int_convertor((unsigned char *)TSB[front].fracsec);

	memset(TSB[index].soc,'\0',5);
	memset(TSB[index].fracsec,'\0',5);

	struct pmupdc_id_list *t_list,*r_list;
	t_list = TSB[index].idlist;

	while(t_list != NULL) {

		r_list = t_list->nextid;
		free(t_list->idcode);
		free(t_list);
		t_list = r_list;			
	}

	struct data_frame *t,*r;
	t = TSB[index].first_data_frame;

	while(t != NULL) {

		r = t->dnext;
		free_dataframe_object(t);
		t = r;			
	}

	TSB[index].first_data_frame = NULL;
	TSB[index].idlist = NULL;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  create_dataframe():                              	     		*/
/* It creates the IEEEC37.118 Standard based combined data frame from the data	*/
/* frames received from all the source devices to be sent to destination 	*/
/* devices									*/
/* ----------------------------------------------------------------------------	*/

int create_dataframe(int index) {

	int total_frame_size = 0;
	unsigned char temp[3];
	struct data_frame *temp_df;
	unsigned int fsize;
	uint16_t chk;

	temp_df = TSB[index].first_data_frame;

	while(temp_df != NULL) {

		fsize = to_intconvertor(temp_df->idcode);
		fsize = to_intconvertor(temp_df->framesize);
		total_frame_size = total_frame_size + fsize;
		total_frame_size -= 16; // skip SYNC + FRAMESIZE + idcode + soc + fracsec + checksum
		temp_df = temp_df->dnext;
	}		

	total_frame_size = total_frame_size + 18/* SYNC + FRAMESIZE + idcode + soc + fracsec + checksum + outer stat */; 

	dataframe = malloc((total_frame_size + 1)*sizeof(char)); // Allocate memory for data frame
	if(!dataframe) {

		printf("No enough memory for dataframe\n");
	}	
	dataframe[total_frame_size] = '\0';	

	// Start the data frame creation 
	int z = 0;
	byte_by_byte_copy(dataframe,DATASYNC,z,2); // SYNC
	z += 2;

	memset(temp,'\0',3);
	int_to_ascii_convertor(total_frame_size,temp);
	byte_by_byte_copy(dataframe,temp,z,2); // FRAME SIZE
	z += 2;

	memset(temp,'\0',3);
	int_to_ascii_convertor(PDC_IDCODE,temp);
	byte_by_byte_copy(dataframe,temp,z,2); // PDC ID
	z += 2;

	byte_by_byte_copy(dataframe,(unsigned char *)TSB[index].soc,z,4); //SOC
	z += 4;
	byte_by_byte_copy(dataframe,(unsigned char *)TSB[index].fracsec,z,4); //FRACSEC
	z += 4;

	unsigned char stat[2]; //Outer Stat
	stat[0] = 0x00;
	stat[1] = 0x00;
	byte_by_byte_copy(dataframe,stat,z,2); //outer stat
	z += 2;

	temp_df = TSB[index].first_data_frame;
	while(temp_df != NULL) { // 1

		int j = 0;
		while(j < temp_df->num_pmu) { // 2

			if(temp_df->dpmu[j]->stat[1] == 0x0f) {

				// Copy STAT
				byte_by_byte_copy(dataframe,temp_df->dpmu[j]->stat,z,2);
				z += 2;
				j++;
				continue;
			}

			//Copy STAT
			byte_by_byte_copy(dataframe,temp_df->dpmu[j]->stat,z,2);

			z += 2;

			int i = 0;

			//Copy Phasors
			if(temp_df->dpmu[j]->phnmr != 0) {

				if(temp_df->dpmu[j]->fmt->phasor == '1') {

					while(i < temp_df->dpmu[j]->phnmr) {

						byte_by_byte_copy(dataframe,temp_df->dpmu[j]->phasors[i],z,8); // Phasors
						z += 8;
						i++;
					}

				} else {

					while(i < temp_df->dpmu[j]->phnmr) {

						byte_by_byte_copy(dataframe,temp_df->dpmu[j]->phasors[i],z,4); // Phasors
						z += 4;
						i++;
					}
				}			
			}

			//Copy FREQ
			if(temp_df->dpmu[j]->fmt->freq == '1') {

				byte_by_byte_copy(dataframe,temp_df->dpmu[j]->freq,z,4); // FREQ
				z += 4;
				byte_by_byte_copy(dataframe,temp_df->dpmu[j]->dfreq,z,4); // FREQ
				z += 4;

			} else {

				byte_by_byte_copy(dataframe,temp_df->dpmu[j]->freq,z,2); // FREQ
				z += 2;
				byte_by_byte_copy(dataframe,temp_df->dpmu[j]->dfreq,z,2); // FREQ
				z += 2;

			}						

			// Copy Analogs
			if(temp_df->dpmu[j]->annmr != 0) {

				if(temp_df->dpmu[j]->fmt->analog == '1') {

					for(i = 0; i<temp_df->dpmu[j]->annmr; i++){					

						byte_by_byte_copy(dataframe,temp_df->dpmu[j]->analog[i],z,4); // ANALOGS
						z += 4;
					}

				} else {

					for(i = 0; i<temp_df->dpmu[j]->annmr; i++){					

						byte_by_byte_copy(dataframe,temp_df->dpmu[j]->analog[i],z,2); // ANALOGS
						z += 2;
					}
				}						
			}

			i = 0;

			//Copy DIGITAL
			if(temp_df->dpmu[j]->dgnmr != 0) {

				while(i < temp_df->dpmu[j]->dgnmr) {

					byte_by_byte_copy(dataframe,temp_df->dpmu[j]->digital[i],z,2); // DIGITAL
					z += 2;
					i++;
				}			
			}	
			j++;			
		} // 2 while

		temp_df = temp_df->dnext;	
	} // 1 while

	// Attach a checksum
	chk = compute_CRC(dataframe,z);
	dataframe[z++] = (chk >> 8) & ~(~0<<8);  /* CHKSUM high byte; */
	dataframe[z++] = (chk ) & ~(~0<<8);   	/* CHKSUM low byte;  */

	return z;	
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  create_cfgframe():                                	     		*/
/* It creates the IEEEC37.118 Standard based combined configuration frame from  */
/* the configuration frames received from all the source devices to be sent to  */
/* destination devices								*/
/* ----------------------------------------------------------------------------	*/

int create_cfgframe() { 

	struct cfg_frame *temp_cfg;
	int total_frame_size = 0,count = 0;
	unsigned char datarate[2],soc[4],fracsec[4]; // hard coded
	int total_num_pmu = 0;
	unsigned char time_base[4];
	unsigned int fsize,num_pmu,phnmr,dgnmr,annmr;
	unsigned int data_rate,temp_data_rate;
	unsigned long int sec,frac = 0,temp_tb,tb;
	uint16_t chk;

	sec = (long int)time (NULL);
	long_int_to_ascii_convertor(sec,soc);
	long_int_to_ascii_convertor(frac,fracsec);

	temp_cfg = cfgfirst;

	while(temp_cfg != NULL) {

		if(count == 0) { // Copy the soc,fracsec,timebase from the first CFG to the combined CFG

			//SEPARATE TIMBASE	
			tb = to_long_int_convertor(temp_cfg->time_base);
			copy_cbyc (time_base,temp_cfg->time_base,4);

			data_rate = to_intconvertor(temp_cfg->data_rate);
			copy_cbyc (datarate,temp_cfg->data_rate,2);

			fsize = to_intconvertor(temp_cfg->framesize);
			total_frame_size += fsize;
			count++;	// count used to count num of cfg
			
			num_pmu = to_intconvertor(temp_cfg->num_pmu);	
			total_num_pmu += num_pmu;	
			temp_cfg = temp_cfg->cfgnext;

		} else {

			fsize = to_intconvertor(temp_cfg->framesize);
			total_frame_size += fsize;
			total_frame_size -= 24;

			// take the Lowest Timebase
			temp_tb = to_long_int_convertor(temp_cfg->time_base);
			if(temp_tb < tb) {

				copy_cbyc (time_base,temp_cfg->time_base,4);
				tb = temp_tb;

			}

			// take the highest data rate
			temp_data_rate = to_intconvertor(temp_cfg->data_rate);
			if(temp_data_rate > data_rate) {

				copy_cbyc (datarate,temp_cfg->data_rate,2);
				data_rate = temp_data_rate;				
			}	
			count++;	// count used to count num of cfg
			
			num_pmu = to_intconvertor(temp_cfg->num_pmu);	
			total_num_pmu += num_pmu;	
			temp_cfg = temp_cfg->cfgnext;
		}

	} // While ENDS	

	cfgframe = malloc((total_frame_size + 1)*sizeof(unsigned char)); // Allocate memory for data frame
	cfgframe[total_frame_size] = '\0';

	// Start the Combined CFG frame creation 
	int z = 0;
	byte_by_byte_copy(cfgframe,CFGSYNC,z,2); // SYNC
	z += 2;

	unsigned char temp[3];
	memset(temp,'\0',3);
	int_to_ascii_convertor(total_frame_size,temp);
	byte_by_byte_copy(cfgframe,temp,z,2); // FRAME SIZE
	z += 2;

	unsigned char tmp[2];
	tmp[0]= cfgframe[2];
	tmp[1]= cfgframe[3];
	int newl;
	newl = to_intconvertor(tmp);
	printf("CFG Frame Len %d.\n",newl);

	memset(temp,'\0',3);
	int_to_ascii_convertor(PDC_IDCODE,temp);
	byte_by_byte_copy(cfgframe,temp,z,2); // PDC ID
	z += 2;

	byte_by_byte_copy(cfgframe,soc,z,4); //SOC
	z += 4;
	byte_by_byte_copy(cfgframe,fracsec,z,4); //FRACSEC
	z += 4;
	byte_by_byte_copy(cfgframe,time_base,z,4); //TIMEBASE
	z += 4;

	memset(temp,'\0',3);
	int_to_ascii_convertor(total_num_pmu,temp);
	byte_by_byte_copy(cfgframe,temp,z,2); // No of PMU
	z += 2;

	int i,j; 
	temp_cfg = cfgfirst;

	while(temp_cfg != NULL) { // 1

		num_pmu = to_intconvertor(temp_cfg->num_pmu);	
		j = 0;

		while (j < num_pmu) { //2

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->stn,z,16); // STN
			z += 16;

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->idcode,z,2); // IDCODE
			z += 2;

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->data_format,z,2); // FORMAT
			z += 2;

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->phnmr,z,2); // PHNMR
			z += 2;

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->annmr,z,2); // ANNMR
			z += 2;

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->dgnmr,z,2); // DGNMR
			z += 2;	

			phnmr = to_intconvertor(temp_cfg->pmu[j]->phnmr);
			annmr = to_intconvertor(temp_cfg->pmu[j]->annmr);
			dgnmr = to_intconvertor(temp_cfg->pmu[j]->dgnmr);

			// Copy Phasor Names
			if(phnmr != 0){

				for(i = 0; i<phnmr;i++) {

					byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->cnext->phnames[i],z,16); // Phasor Names
					z += 16;						
				}
			}

			// Copy Analog Names
			if(annmr != 0){

				for(i = 0; i<annmr;i++) {

					byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->cnext->angnames[i],z,16); // Analog Names
					z += 16;											
				}
			}

			// Copy Digital Names
			if(dgnmr != 0) {

				struct dgnames *temp_dgname = temp_cfg->pmu[j]->cnext->first;
				while (temp_dgname != NULL) {

					for(i = 0;i<16;i++) {

						byte_by_byte_copy(cfgframe,temp_dgname->dgn[i],z,16); // Digital Names
						z += 16; 				

					} // Copy 16 channel names of digital word 	

					temp_dgname = temp_dgname->dg_next;
				} // Go to next Digital word

			}

			// PHUNIT
			if(phnmr != 0){

				for (i = 0; i<phnmr;i++) {

					byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->phunit[i],z,4); // PHUNIT
					z += 4; 									
				}
			}	

			// ANUNIT
			if(annmr != 0){

				for (i = 0; i<annmr;i++) {

					byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->anunit[i],z,4); // ANUNIT
					z += 4; 									

				}
			}	

			// DGUNIT
			if(dgnmr != 0){

				for (i = 0; i<dgnmr;i++) {

					byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->dgunit[i],z,4); // DGUNIT
					z += 4; 									
				}	

			}		

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->fnom,z,2); // FNOM
			z += 2; 									

			byte_by_byte_copy(cfgframe,temp_cfg->pmu[j]->cfg_cnt,z,2); // CFGCNT
			z += 2; 									

			j++; // index for pmu_num

		} // while 2

		temp_cfg = temp_cfg->cfgnext;	// Take next CFG	

	} // while 1


	byte_by_byte_copy(cfgframe,datarate,z,2); // DATA RATE
	z += 2; 									

	chk = compute_CRC(cfgframe,z);
	cfgframe[z++] = (chk >> 8) & ~(~0<<8);  /* CHKSUM high byte; */
	cfgframe[z++] = (chk ) & ~(~0<<8);   	/* CHKSUM low byte;  */
	return z;	
} 

/**************************************** End of File *******************************************************/
