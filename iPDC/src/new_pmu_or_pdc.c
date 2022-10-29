/* ----------------------------------------------------------------------------- 
 * new_pmu_or_pdc.c
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


/* ---------------------------------------------------------------------------------------------*/
/*                          Functions defined in new_pmu_or_pdc.c     		  		*/
/* ---------------------------------------------------------------------------------------------*/

/*	1.  int   add_PMU(char pmuid[], char ip[], char port[], char protocol[])		*/
/*	2.  void* connect_pmu_tcp(void *temp)       	    	          		*/
/*	3.  void* connect_pmu_udp(void *temp)		     				*/
/*	4.  void  add_PMU_Node(struct Lower_Layer_Details *temp_pmu)		        */
/*	5.  int   remove_Lower_Node(char pmuid[], char protocol[])                     	*/
/*	6.  void* remove_llnode(void*)                     				*/
/*	7.  int   put_data_transmission_off(char pmuid[], char protocol[])             	*/
/*	8.  void* data_off_llnode(void* temp)                				*/
/*	9.  int   put_data_transmission_on(char pmuid[], char protocol[])		*/
/*	10. void* data_on_llnode(void* temp)               				*/
/*	11. int   configuration_request(char pmuid[], char protocol[])     	     	*/
/*	12. void* config_request(void* temp)	          				*/
/*	13. int   add_PDC(char ip[], char protocol[])					*/
/*	14. int   remove_PDC(char ip[], char port_num[], char protocol[])		*/
/*	15. void  display_CT()	          						*/
/*	16. void  create_command_frame(int type,int pmuid,char *)   			*/
/*	17. int   checkip(char ip[])	          					*/

/* ---------------------------------------------------------------------------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <errno.h>
#include  <string.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <sys/wait.h>
#include  <signal.h>
#include  <pthread.h>
#include  <errno.h>
#include  <time.h>
#include  <ctype.h>
#include  <gtk/gtk.h>
#include  "ipdcGui.h"
#include  "connections.h"
#include  "parser.h"
#include  "global.h"
#include  "new_pmu_or_pdc.h"
#include  "dallocate.h"


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  add_PMU():                            	     			*/
/* It Makes an entry in iPDC Setup File for each new Lower layer PMU/PDC.  	*/
/* A node is created of the type Lower_Layer_Details. A separate thread is	*/
/* created for each added lower layer PMU/PDC accoring to the protcol (TCP/UDP).*/
/* ----------------------------------------------------------------------------	*/

int add_PMU(char pmuid[], char ip[], char port[], char protocol[], char latitude[], char longitude[]) {

	int err;
	int flag = 0;

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

	if(LLfirst != NULL) 
	{
		struct Lower_Layer_Details *temp_ptr;

		temp_ptr = malloc(sizeof(struct Lower_Layer_Details));
		if(!temp_ptr) {

			printf("Not enough memory temp_pmu\n");
			exit(1);
		}

		temp_ptr = LLfirst;

		while (temp_ptr != NULL)
		{
			if(temp_ptr->pmuid == atoi(pmuid)) {

				flag = 1;
				break;

			} else {

				temp_ptr = temp_ptr->next;
				continue;
			}
		}
	}

	if(flag)  /* 2 if there is a match */		
	{
		printf("%s %s is already in the LowerDevices list Enter another PMU\n",pmuid,protocol);
		return 1;

	}
	else if(!flag)
	{
		/* Make a node that contains PMU IP, Port and  Protocol details */
		struct Lower_Layer_Details *temp_pmu;

		temp_pmu = malloc(sizeof(struct Lower_Layer_Details));
		if(!temp_pmu) {

			printf("Not enough memory temp_pmu\n");
			exit(1);
		}

		temp_pmu->pmuid = atoi(pmuid); // PMUID
		strcpy(temp_pmu->ip,ip); // ip
		temp_pmu->port = atoi(port); // port
		strcpy(temp_pmu->protocol,protocol); // protocol
		temp_pmu->protocol[3] = '\0';
		temp_pmu->up = 1;
		temp_pmu->data_transmission_off = 0;
		temp_pmu->pmu_remove = 0;
		temp_pmu->request_cfg_frame = 0;
		temp_pmu->latitude = atof(latitude);
		temp_pmu->longitude = atof(longitude);
		printf(" lat = %f long = %f\n",temp_pmu->latitude,temp_pmu->longitude);
		temp_pmu->next = NULL;
		temp_pmu->prev = NULL;

		pthread_t t;

		if(!strncasecmp(protocol,"UDP",3)) {

			if((err = pthread_create(&t,&attr,connect_pmu_udp,(void *)temp_pmu))) {

				perror(strerror(err));
				exit(1);
			}

		} else if(!strncasecmp(protocol,"TCP",3)) {

			if((err = pthread_create(&t,&attr,connect_pmu_tcp,(void *)temp_pmu))) {

				perror(strerror(err));
				exit(1);
			}
		}

		struct Upper_Layer_Details *temp_pdc = ULfirst;

		pthread_mutex_lock(&mutex_Upper_Layer_Details);

		while(temp_pdc != NULL ) {

			temp_pdc->config_change = 1;
			temp_pdc = temp_pdc->next;
		}

		pthread_mutex_unlock(&mutex_Upper_Layer_Details);
		return 0;
	} // If no match

	return 1;
}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  connect_pmu_tcp():                        	     			*/
/* It Makes a new tcp connections with each added Lower Layer PMU/PDC.		*/
/* ----------------------------------------------------------------------------	*/

void* connect_pmu_tcp(void *temp) {

	int tcp_sockfd,port_num,yes = 1;
	struct sockaddr_in PMU_addr;
	struct Lower_Layer_Details *temp_pmu = (struct Lower_Layer_Details *) temp;
	unsigned char *tcp_BUF,*ptr,length[2];
	unsigned int flen;	
	uint16_t cal_crc,frame_crc;

	port_num = temp_pmu->port;

	if ((tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(tcp_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	bzero(&PMU_addr,sizeof(PMU_addr));
	PMU_addr.sin_family = AF_INET;
	PMU_addr.sin_addr.s_addr =  inet_addr(temp_pmu->ip);  
	PMU_addr.sin_port = htons(port_num);
	memset(&(PMU_addr.sin_zero), '\0', 8);   // zero the rest of the struct

	/* Copy the information of Lower Layer PMU/PDC to the node */
	temp_pmu->thread_id = pthread_self();
	bzero(&temp_pmu->llpmu_addr,sizeof(PMU_addr));
	temp_pmu->llpmu_addr.sin_family = AF_INET;
	temp_pmu->llpmu_addr.sin_addr.s_addr =  inet_addr(temp_pmu->ip); 
	temp_pmu->llpmu_addr.sin_port = htons(port_num);
	memset(&(temp_pmu->llpmu_addr.sin_zero), '\0', 8);   // zero the rest of the struct
	temp_pmu->sockfd = tcp_sockfd;
	temp_pmu->up = 1;

	if (connect(tcp_sockfd, (struct sockaddr *)&PMU_addr,
			sizeof(PMU_addr)) == -1) { // Main if

		perror("connect");
		temp_pmu->up = 0;
		add_PMU_Node(temp_pmu);	
		pthread_exit(NULL);

	} else {
		/* Add PMU*/
		add_PMU_Node(temp_pmu);	

		tcp_BUF = malloc(MAXBUFLEN* sizeof(unsigned char));

		/* Sending Command for obtaining CFG */
		int n,bytes_read;
		char *cmdframe = malloc(19);
		cmdframe[18] = '\0';
		create_command_frame(1,temp_pmu->pmuid,cmdframe);
		if ((n = send (tcp_sockfd,cmdframe,18,0) == -1)) { 

			perror("send");

		} else {	

			free(cmdframe);
			while(1) {

				memset(tcp_BUF, '\0', MAXBUFLEN * sizeof(unsigned char));						
				bytes_read = recv (tcp_sockfd, tcp_BUF,MAXBUFLEN-1,0);

				if(bytes_read == 0) {  /* When  TCP Peer Terminates */

					printf("No data received Closing tcp socket %d\n",tcp_sockfd);
					temp_pmu->up = 0;

					struct Upper_Layer_Details *temp_pdc = ULfirst;
					pthread_mutex_lock(&mutex_Upper_Layer_Details);							
					while(temp_pdc != NULL ) {

						temp_pdc->config_change = 1;
						temp_pdc = temp_pdc->next;
					}
					pthread_mutex_unlock(&mutex_Upper_Layer_Details);

					pthread_exit(NULL);

				} else if(bytes_read == -1) {/* When  TCP Peer Terminates */

					perror("recv");
					temp_pmu->up = 0;

					struct Upper_Layer_Details *temp_pdc = ULfirst;
					pthread_mutex_lock(&mutex_Upper_Layer_Details);							
					while(temp_pdc != NULL ) {

						temp_pdc->config_change = 1;
						temp_pdc = temp_pdc->next;
					}
					pthread_mutex_unlock(&mutex_Upper_Layer_Details);

				} else {

					ptr = tcp_BUF;
					ptr += 2;
					copy_cbyc(length,ptr,2);
					flen = to_intconvertor(length);
					cal_crc = compute_CRC(tcp_BUF,flen-2); 
					ptr += flen -4;									
					frame_crc = *ptr;
					frame_crc <<= 8;
					frame_crc |= *(ptr + 1);

					if(frame_crc != cal_crc) {

						continue;		
					}
					// if (sendto(DB_sockfd,tcp_BUF, MAXBUFLEN-1, 0,
					// 		(struct sockaddr *)&DB_Server_addr,sizeof(DB_Server_addr)) == -1) {
					// 	perror("sendto");
					// }

					tcp_BUF[bytes_read] = '\0';
					PMU_process_TCP(tcp_BUF,tcp_sockfd);
				}  

			} // while ends
		} 
	}// Main if

	close(tcp_sockfd);
	pthread_exit(NULL);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  connect_pmu_udp():                        	     			*/
/* It Makes a new udp connections with each added Lower Layer PMU/PDC.		*/
/* ----------------------------------------------------------------------------	*/

void* connect_pmu_udp(void *temp) {

	int udp_sockfd,port_num,addr_len,yes = 1;
	unsigned char *udp_BUF,*ptr,length[2];
	unsigned int flen;	
	uint16_t cal_crc,frame_crc;

	struct sockaddr_in PMU_addr,their_addr;
	struct Lower_Layer_Details *temp_pmu = (struct Lower_Layer_Details *) temp;
	struct Lower_Layer_Details *t ;

	port_num = temp_pmu->port;

	if ((udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(udp_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	bzero(&PMU_addr,sizeof(PMU_addr));
	PMU_addr.sin_family = AF_INET;
	PMU_addr.sin_addr.s_addr =  inet_addr(temp_pmu->ip); 
	PMU_addr.sin_port = htons(port_num);
	memset(&(PMU_addr.sin_zero), '\0', 8);   // zero the rest of the struct

	/* Copy the information of Lower Layer PMU/PDC to the node */
	temp_pmu->thread_id = pthread_self();
	bzero(&temp_pmu->llpmu_addr,sizeof(PMU_addr));
	temp_pmu->llpmu_addr.sin_family = AF_INET;
	temp_pmu->llpmu_addr.sin_addr.s_addr =  inet_addr(temp_pmu->ip);  
	temp_pmu->llpmu_addr.sin_port = htons(port_num);
	memset(&(temp_pmu->llpmu_addr.sin_zero), '\0', 8);   // zero the rest of the struct
	temp_pmu->sockfd = udp_sockfd;
	temp_pmu->up = 1;

	/* Add PMU*/
	add_PMU_Node(temp_pmu);	

	udp_BUF = malloc(MAXBUFLEN* sizeof(unsigned char));

	/* Sending Command fro obtaining CFG */
	addr_len = sizeof(struct sockaddr);	
	int n,bytes_read;
	unsigned char *cmdframe = malloc(19);
	cmdframe[18] = '\0';
	create_command_frame(1,temp_pmu->pmuid,(char *)cmdframe);

	if ((n = sendto(udp_sockfd,cmdframe, 18, 0, (struct sockaddr *)&PMU_addr,sizeof(PMU_addr)) == -1)) {

		perror("sendto"); 

	} else {

		free(cmdframe);

		/* UDP data Received */
		while(1) {

			memset(udp_BUF,'\0',MAXBUFLEN * sizeof(unsigned char));
			bytes_read = recvfrom (udp_sockfd, udp_BUF,MAXBUFLEN-1,0,(struct sockaddr *)&their_addr,(socklen_t *)&addr_len); 				
			if(bytes_read == -1) {

				perror("recvfrom");
				exit(1);

			} else { // New Datagram received

				pthread_mutex_lock(&mutex_Lower_Layer_Details);
				int flag = 0;
				if(LLfirst == NULL) {

					flag = 0;

				} else {

					t = LLfirst;
					while(t != NULL) {

						if((!strcmp(t->ip,inet_ntoa(their_addr.sin_addr))) 
								&& (!strncasecmp(t->protocol,"UDP",3))) {

							flag = 1;
							break;			

						} else {

							t = t->next;
						}
					}
				}
				pthread_mutex_unlock(&mutex_Lower_Layer_Details);		

				if(flag) {

					ptr = udp_BUF;
					ptr += 2;
					copy_cbyc(length,ptr,2);
					flen = to_intconvertor(length);
					cal_crc = compute_CRC(udp_BUF,flen-2); 
					ptr += flen -4;									
					frame_crc = *ptr;
					frame_crc <<= 8;
					frame_crc |= *(ptr + 1);

					if(frame_crc != cal_crc) {

						continue;		
					}

					//process the frame
					int n;

					if ((n = sendto(DB_sockfd,udp_BUF, MAXBUFLEN-1, 0,
							(struct sockaddr *)&DB_Server_addr,sizeof(DB_Server_addr)) == -1)) {
						perror("sendto");
					} 

					udp_BUF[bytes_read] = '\0';

					//Call the udphandler							 			
					PMU_process_UDP(udp_BUF,PMU_addr,udp_sockfd);

				} else {

					printf("Datagram PMU not authentic. We donot pass the buffer for further processing %s\n",inet_ntoa(their_addr.sin_addr));
				} 					
			} // Main if ends

		} // while ends
	}

	close(udp_sockfd);
	pthread_exit(NULL);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  add_PMU_Node():                        	     			*/
/* It creates a node of the type Lower_Layer_Details for each newly added  	*/
/* Lower Layer PMU/PDC.						  		*/
/* ----------------------------------------------------------------------------	*/

void add_PMU_Node(struct Lower_Layer_Details *temp_pmu) {

	pthread_mutex_lock(&mutex_Lower_Layer_Details);

	if(LLfirst == NULL) {

		temp_pmu->prev = NULL;	
		LLfirst = temp_pmu;

	} else {

		LLlast->next = temp_pmu;
		temp_pmu->prev = LLlast;
	}

	temp_pmu->next = NULL;

	LLlast = temp_pmu;

	write_cfg_to_file();

	pthread_mutex_unlock(&mutex_Lower_Layer_Details);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_Lower_Node():                        	     		*/
/* It sets the remove flag for a Lower Layer PMU/PDC do that it can be removed	*/
/* later.									*/
/* ----------------------------------------------------------------------------	*/

int remove_Lower_Node(char pmuid[], char protocol[]) {

	int flag = 0, err;

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

	if(LLfirst == NULL) {

		printf("No PMU present?\n");
		return 1;	

	} else {

		flag = 1;
	}

	if(flag) {

		int match = 0;
		struct Lower_Layer_Details *temp_pmu = LLfirst;

		while(temp_pmu != NULL) {

			if(set_all == 1) {

				match = 1;	
				break;			

			} else {

			 	if((temp_pmu->pmuid == atoi(pmuid)) && (!strncasecmp(temp_pmu->protocol,protocol,3))) {	

					match = 1;	
					break;			

				} else {

					temp_pmu = temp_pmu->next; 
				}
			}
		}

		if(match) {

			if(set_all == 1) {

				temp_pmu = LLfirst;

				while(temp_pmu != NULL) {

					pthread_t t;
					if((err = pthread_create(&t,&attr,remove_llnode,(void *)temp_pmu))) {

						perror(strerror(err));		     
						exit(1);
					}
					temp_pmu = temp_pmu->next; 
				}

			} else {

				pthread_t t;
				if((err = pthread_create(&t,&attr,remove_llnode,(void *)temp_pmu))) {

					perror(strerror(err));		     
					exit(1);
				}							
			}

			struct Upper_Layer_Details *temp_pdc = ULfirst;

			pthread_mutex_lock(&mutex_Upper_Layer_Details);							

			while(temp_pdc != NULL ) {

				temp_pdc->config_change = 1;
				temp_pdc = temp_pdc->next;
			}

			pthread_mutex_unlock(&mutex_Upper_Layer_Details);	

			return 0;	

		} else {

			printf("No match for entered PMU\n");
			return 1;	
		} 
	} 

	return 1;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_llnode():                        	     			*/
/* ----------------------------------------------------------------------------	*/

void* remove_llnode(void* temp) {

	int flag=0;

	struct Lower_Layer_Details *temp_pmu = (struct Lower_Layer_Details *) temp;
	struct Lower_Layer_Details *temp_ptr;
	pthread_t tid = temp_pmu->thread_id;

	/* Remove the object from structure 'Lower Layer Details'*/
	pthread_mutex_lock(&mutex_Lower_Layer_Details);

	/* remove the entry from CFG linked list and remove that CFG objects */
	/* remove the entry from iPDC Setup File */
	if(LLfirst != NULL) 
	{
		temp_ptr = malloc(sizeof(struct Lower_Layer_Details));
		if(!temp_ptr) 
		{
			printf("Not enough memory temp_pmu\n");
			exit(1);
		}
		temp_ptr = LLfirst;

		while (temp_ptr != NULL)
		{
			if(((temp_ptr->pmuid == temp_pmu->pmuid)) && (!strcmp(temp_ptr->protocol,temp_pmu->protocol))) 
			{
				if((temp_ptr->prev != NULL) && (temp_ptr->next != NULL))         //deletion of inbetween node
				{
					temp_ptr->prev->next = temp_ptr->next;
					temp_ptr->next->prev = temp_ptr->prev;
					temp_ptr = temp_ptr->next;
				}
				else if((temp_ptr->prev == NULL) && (temp_ptr->next != NULL))    //deletion of first node
				{
					temp_ptr = temp_ptr->next;
					temp_ptr->prev = NULL;
					LLfirst = temp_ptr;
				}
				else if((temp_ptr->prev != NULL) && (temp_ptr->next == NULL))    //deletion of last node
				{
					temp_ptr = temp_ptr->prev;
					temp_ptr->next = NULL;
					LLlast = temp_ptr;
				}
				else
				{
					LLfirst = NULL;
					LLlast  = NULL;
				}
				flag = 1;
				break;
			} 
			else
			{
				temp_ptr = temp_ptr->next;
				continue;
			}
		}
	}

	pthread_mutex_unlock(&mutex_Lower_Layer_Details);

	if(flag == 1)
	{
	     /* remove the cfg object from memory */
	     int ind = 0,match = 0;
	     struct cfg_frame *temp_cfg = cfgfirst,*tprev_cfg;
	     unsigned char id_CODE[2];
	     tprev_cfg = temp_cfg;

	     id_CODE[0] = temp_pmu->pmuid >> 8;
	     id_CODE[1] = temp_pmu->pmuid ;

	     while(temp_cfg != NULL){
		     if(!ncmp_cbyc(id_CODE,temp_cfg->idcode,2)) {

			     match = 1;
			     break;	

		     } else {

			     ind++;
			     tprev_cfg = temp_cfg;
			     temp_cfg = temp_cfg->cfgnext;
		     }
	     }// While ends

	     if(match) {

		     pthread_mutex_lock(&mutex_cfg);			

		     if(!ind) {

			     // Replace the cfgfirst
			     cfgfirst = cfgfirst->cfgnext;
			     free_cfgframe_object(temp_cfg);				

		     } else {

			     // Replace in between cfg
			     tprev_cfg->cfgnext = temp_cfg->cfgnext;
			     free_cfgframe_object(temp_cfg);
		     }

		     pthread_mutex_unlock(&mutex_cfg);
	     }
     }

     pthread_cancel(tid);

     /* Close the socket connection */
     close(temp_pmu->sockfd);

     free(temp_pmu);

     write_cfg_to_file();

     pthread_exit(NULL);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  put_data_transmission_off():                        	     	*/
/* ----------------------------------------------------------------------------	*/

int put_data_transmission_off(char pmuid[], char protocol[]) {

	int flag = 0,err;

	// A new thread is created for each TCP connection in 'detached' mode. Thus allowing any number of threads to be created. 
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/* In  the detached state, the thread resources are immediately freed when it terminates, but
	   pthread_join(3) cannot be used to synchronize on the thread termination. */
	if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) {

		perror(strerror(err));
		exit(1);
	}								

	if((err = pthread_attr_setschedpolicy(&attr,SCHED_FIFO))) {

		perror(strerror(err));		     
		exit(1);
	}  

	pthread_mutex_lock(&mutex_Lower_Layer_Details);

	if(LLfirst == NULL) {

		printf("No PMU present?\n");
		return 1;	

	} else {

		flag = 1;
	}

	pthread_mutex_unlock(&mutex_Lower_Layer_Details);

	if(flag) {

		int match = 0;
		struct Lower_Layer_Details *temp_pmu = LLfirst;

		while(temp_pmu != NULL) {

			if(set_all == 1) {

				match = 1;	
				break;			

			} else {

				if((temp_pmu->pmuid == atoi(pmuid)) && (!strncasecmp(temp_pmu->protocol,protocol,3))) {	

					match = 1;	
					break;			

				} else {

					temp_pmu = temp_pmu->next; 
				}
			}	
		}

		if(match) {

			if(set_all == 1) {

				temp_pmu = LLfirst;

				while(temp_pmu != NULL) {

					pthread_t t;
					temp_pmu->data_transmission_off = 1;

					if((err = pthread_create(&t,&attr,data_off_llnode,(void *)temp_pmu))) {

						perror(strerror(err));		     
						exit(1);
					}
					temp_pmu = temp_pmu->next; 
				}

			} else {

				pthread_t t;
				temp_pmu->data_transmission_off = 1;

				if((err = pthread_create(&t,&attr,data_off_llnode,(void *)temp_pmu))) {

					perror(strerror(err));		     
					exit(1);
				}
			}

			return 0;			

		} else {

			printf("No match for entered PMU\n");
			return 1;			
		} 
	} 

	return 1;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  data_off_llnode():		                        	     	*/
/* ----------------------------------------------------------------------------	*/

void* data_off_llnode(void* temp) {

	char *cmdframe = malloc(19);
	struct Lower_Layer_Details *temp_pmu = (struct Lower_Layer_Details *) temp;

	create_command_frame(3,temp_pmu->pmuid,cmdframe);	
	cmdframe[18] = '\0';

	if(!strncasecmp(temp_pmu->protocol,"UDP",3)) {

		int n;

		if ((n = sendto(temp_pmu->sockfd,cmdframe, 18, 0,(struct sockaddr *)&temp_pmu->llpmu_addr,sizeof(temp_pmu->llpmu_addr)) == -1)) {

			perror("sendto"); 

		} else {

			printf("Sent CMD to put data transmission OFF\n");
		}

	} else if(!strncasecmp(temp_pmu->protocol,"TCP",3)){

		int n;

		if(temp_pmu->up == 1) {
			if ((n = send(temp_pmu->sockfd,cmdframe, 18,0) == -1)) {

				perror("send"); 

			} else {

				printf("Sent CmD to put data transmission OFF\n");
			}
		}
	}

	free(cmdframe);
	pthread_exit(NULL);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  put_data_transmission_on():		              	     	*/
/* ----------------------------------------------------------------------------	*/

int put_data_transmission_on(char pmuid[], char protocol[]) {

	int flag = 0,err;

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

	pthread_mutex_lock(&mutex_Lower_Layer_Details);

	if(LLfirst == NULL) {

		printf("No PMU Present?\n");
		return 1;

	} else {

		flag = 1;
	}

	pthread_mutex_unlock(&mutex_Lower_Layer_Details);

	if(flag) {

		int match = 0;
		struct Lower_Layer_Details *temp_pmu = LLfirst;

		while(temp_pmu != NULL) {

			if(set_all == 1) {

				match = 1;	
				break;			

			} else {

				if((temp_pmu->pmuid == atoi(pmuid)) && (!strncasecmp(temp_pmu->protocol,protocol,3))) {	

					match = 1;	
					break;			

				} else {

					temp_pmu = temp_pmu->next; 
				}	
			}
		}

		if(match) {

			if(set_all == 1) {

				temp_pmu = LLfirst;

				while(temp_pmu != NULL) {

					pthread_t t;
					temp_pmu->data_transmission_off = 0;

					if((err = pthread_create(&t,&attr,data_on_llnode,(void *)temp_pmu))) {

						perror(strerror(err));		     
						exit(1);
					}
					temp_pmu = temp_pmu->next; 
				}

			} else {

				pthread_t t;
				temp_pmu->data_transmission_off = 0;

				if((err = pthread_create(&t,&attr,data_on_llnode,(void *)temp_pmu))) {

					perror(strerror(err));		     
					exit(1);
				}
			}

			return 0;

		} else {

			printf("No match for entered PMU\n");
			return 1;
		} 
	}

	return 1;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  data_on_llnode():		              	     			*/
/* ----------------------------------------------------------------------------	*/

void* data_on_llnode(void* temp) {

	char *cmdframe = malloc(19);
	struct Lower_Layer_Details *temp_pmu = (struct Lower_Layer_Details *) temp;

	create_command_frame(2,temp_pmu->pmuid,cmdframe);
	cmdframe[18] = '\0';

	if(!strncasecmp(temp_pmu->protocol,"UDP",3)) {

		int n;

		if ((n = sendto(temp_pmu->sockfd,cmdframe, 18, 0,(struct sockaddr *)&temp_pmu->llpmu_addr,sizeof(temp_pmu->llpmu_addr)) == -1)) {

			perror("sendto"); 

		} else {

			printf("Sent CMD to put data transmission ON.\n");
		}

	} else if(!strncasecmp(temp_pmu->protocol,"TCP",3)){

		int n;

		if(temp_pmu->up == 1) {

			if ((n = send(temp_pmu->sockfd,cmdframe, 18,0) == -1)) {

				perror("send"); 

			} else {

				printf("Sent CmD to put data transmission ON.\n");
			}
		}
	}

	free(cmdframe);			
	pthread_exit(NULL);
}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  configuration_request():		              	     		*/
/* ----------------------------------------------------------------------------	*/

int configuration_request(char pmuid[], char protocol[]) {

	int flag = 0,err;

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

	pthread_mutex_lock(&mutex_Lower_Layer_Details);

	if(LLfirst == NULL) {

		printf("No PMU Present?\n");
		return 1;

	} else {

		flag = 1;
	}

	pthread_mutex_unlock(&mutex_Lower_Layer_Details);

	if(flag) {

		int match = 0;
		struct Lower_Layer_Details *temp_pmu = LLfirst;

		while(temp_pmu != NULL) {

			if(set_all == 1) {

				match = 1;	
				break;			

			} else {

				if((temp_pmu->pmuid == atoi(pmuid)) && (!strncasecmp(temp_pmu->protocol,protocol,3))) {	

					match = 1;	
					break;			

				} else {
					temp_pmu = temp_pmu->next; 		
				}				
			}
		}

		if(match) {

			if(set_all == 1) {

				temp_pmu = LLfirst;

				while(temp_pmu != NULL) {

					pthread_t t;

					if((err = pthread_create(&t,&attr,config_request,(void *)temp_pmu))) {

						perror(strerror(err));		     
						exit(1);
					}
					temp_pmu = temp_pmu->next; 
				}

			} else {

				pthread_t t;

				if((err = pthread_create(&t,&attr,config_request,(void *)temp_pmu))) {

					perror(strerror(err));		     
					exit(1);
				}							
			}

			return 0;

		} else {

			printf("No match for entered PMU\n");
			return 1;
		} 
	}

	return 1;
}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  config_request():		            	  	     		*/
/* ----------------------------------------------------------------------------	*/

void* config_request(void* temp) {

	int err;
	char *cmdframe = malloc(19);
	struct Lower_Layer_Details *temp_pmu = (struct Lower_Layer_Details *) temp;

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

	if(!strncasecmp(temp_pmu->protocol,"UDP",3)) { /* If Peer is UDP */

		int n;
		cmdframe[18] = '\0';

		create_command_frame(1,temp_pmu->pmuid,cmdframe);

		if ((n = sendto(temp_pmu->sockfd,cmdframe, 18, 0,(struct sockaddr *)&temp_pmu->llpmu_addr,sizeof(temp_pmu->llpmu_addr)) == -1)) {

			perror("sendto"); 

		} else {

			free(cmdframe);	
			temp_pmu->data_transmission_off = 0;	
		}

	} else if(!strncasecmp(temp_pmu->protocol,"TCP",3)){ /* If Peer is TCP */

		int n;

		if(temp_pmu->up == 0) { /* If TCP Peer is DOWN */

			pthread_t t;

			if((err = pthread_create(&t,&attr,connect_pmu_tcp,(void *)temp_pmu))) {

				perror(strerror(err));		     
				exit(1);
			}

			struct Upper_Layer_Details *temp_pdc = ULfirst;

			pthread_mutex_lock(&mutex_Upper_Layer_Details);							

			while(temp_pdc != NULL ) {

				temp_pdc->config_change = 1;
				temp_pdc = temp_pdc->next;
			}

			pthread_mutex_unlock(&mutex_Upper_Layer_Details);							

		} else { /* If TCP Peer is UP */

			cmdframe[18] = '\0';

			create_command_frame(1,temp_pmu->pmuid,cmdframe);

			if ((n = send(temp_pmu->sockfd,cmdframe, 18,0) == -1)) {

				printf("temp_pmu->sockfd %d \n",temp_pmu->sockfd);
				perror("send"); 

			} else {

				printf("CMD to send CFG \n");
				free(cmdframe);		
				temp_pmu->data_transmission_off = 0;			
			}			
		}
	}

	pthread_exit(NULL);
}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  add_PDC((char ip[], char protocol[]):                              */
/* It Makes an entry in iPDC Setup File for the upper PDC. The pre-existing 	*/
/* entry will be removed from the file and also the list Upper_Layer_Details 	*/
/* ----------------------------------------------------------------------------	*/

int add_PDC(char ip[], char protocol[]) {

	int flag = 0;
	struct Upper_Layer_Details *temp_ptr;

	pthread_mutex_lock(&mutex_Upper_Layer_Details);

	if(ULfirst != NULL) 
	{
		temp_ptr = malloc(sizeof(struct Upper_Layer_Details));

		if(!temp_ptr) {

			printf("Not enough memory temp_pmu\n");
			exit(1);
		}
		temp_ptr = ULfirst;

		while (temp_ptr != NULL)
		{
			if((!strcmp(temp_ptr->ip, ip)) && (!strncasecmp(temp_ptr->protocol, protocol, 3))) {

				flag = 1;
				break;

			} else {

				temp_ptr = temp_ptr->next;
				continue;
			}
		}
	}

	if(flag)  /* 2 if there is a match */		
	{
		printf("%s %s is already in the UpperDevices list Enter another PDC.\n",ip,protocol);
		return 1;

	}
	else if(!flag)
	{
		/* Make a node that contains PMU IP, Port and  Protocol details */
		struct Upper_Layer_Details *temp_pdc;
		temp_pdc = malloc(sizeof(struct Upper_Layer_Details));

		if(!temp_pdc) {

			printf("Not enough memory temp_pdc\n");
			exit(1);
		}

		strcpy(temp_pdc->ip,ip);     // ip

		if(!strncasecmp(protocol,"UDP",3)) {

			temp_pdc->port = UDPPORT;   // port

		} else {

			temp_pdc->port = TCPPORT;   // port
		}

		strncpy(temp_pdc->protocol,protocol,3); // protocol
		temp_pdc->protocol[3] = '\0';

		bzero(&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr));
		temp_pdc->pdc_addr.sin_family = AF_INET;
		temp_pdc->pdc_addr.sin_addr.s_addr =  inet_addr(temp_pdc->ip);
		temp_pdc->pdc_addr.sin_port = htons(temp_pdc->port);
		memset(&(temp_pdc->pdc_addr.sin_zero), '\0', 8);   // zero the rest of the struct
		temp_pdc->config_change = 0;
		temp_pdc->tcpup = 1;
		temp_pdc->UL_upper_pdc_cfgsent = 0;
		temp_pdc->UL_data_transmission_off = 1;
		temp_pdc->address_set = 0;

		if(ULfirst == NULL) {

			ULfirst = temp_pdc;
			temp_pdc->prev = NULL;

		} else {

			ULlast->next = temp_pdc;
			temp_pdc->prev = ULlast;
		}

		ULlast = temp_pdc;
		temp_pdc->next = NULL;

		write_cfg_to_file();
	}

	pthread_mutex_unlock(&mutex_Upper_Layer_Details);
	return 0;			
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_PDC((char ip[], char port_num[], char protocol[]):          */
/* ----------------------------------------------------------------------------	*/

int remove_PDC(char ip[], char port_num[], char protocol[]) {

	int flag = 0;
	struct Upper_Layer_Details *temp_pdc;

	if(ULfirst == NULL) {

		printf("No PDC Present?\n");
		return 1;

	} else {

		temp_pdc = malloc(sizeof(struct Upper_Layer_Details));
		temp_pdc = ULfirst;

		while(temp_pdc != NULL) {

			if(!strcmp(temp_pdc->ip,ip)) { 

				if(((!strncasecmp(temp_pdc->protocol,"UDP",3)) && (temp_pdc->port == UDPPORT)) || ((!strncasecmp(temp_pdc->protocol,"TCP",3)) && (temp_pdc->port == TCPPORT))) {

					flag = 1;
					break;
				}
			}
			temp_pdc = temp_pdc->next;
		}			

		if(flag == 1) {

			pthread_mutex_lock(&mutex_Upper_Layer_Details);							

			if(temp_pdc->prev == NULL)  {

				ULfirst = temp_pdc->next;
				if(ULfirst != NULL) ULfirst->prev = NULL;		

			} else {

				temp_pdc->prev->next = temp_pdc->next;
			}

			if(temp_pdc->next == NULL) {

				ULlast = temp_pdc->prev;

			} else {
				if(temp_pdc->prev != NULL)
					temp_pdc->prev->next = temp_pdc->next;
			}

			write_cfg_to_file();
			pthread_mutex_unlock(&mutex_Upper_Layer_Details);

     		return 0;
		}
		return 1;
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  display_CT():		            	  	     		*/
/* ----------------------------------------------------------------------------	*/

void display_CT() {

	printf("####         CONNECTION TABLE OF SOURCE DEVICES        ####\n");
	printf("--------------------------------------------------------------------------------\n");
	printf("|   PMU ID	|  	IP 	|	Port	|     Protocol  |	Up	|\n");
	printf("--------------------------------------------------------------------------------\n");

	pthread_mutex_lock(&mutex_Lower_Layer_Details);

	if(LLfirst == NULL) {

		printf("No PMU Present?\n");

	} else {

		struct Lower_Layer_Details *t = LLfirst;

		while(t != NULL) {

			printf("|\t%d\t|%s\t|\t%d\t|\t%s\t|\t%d\t|\n",t->pmuid,t->ip,t->port,t->protocol,t->up);			
			t = t->next;

		}			
		printf("--------------------------------------------------------------------------------\n");
	}

	pthread_mutex_unlock(&mutex_Lower_Layer_Details);

	printf("####  CONNECTION TABLE OF DESTINATION DEVICES  ####\n");
	printf("-------------------------------------------------\n");
	printf("|      IP 	|	Port	|     Protocol  |\n");
	printf("-------------------------------------------------\n");

	if(ULfirst == NULL) {

		printf("No PDC Present?\n");


	} else {

		struct Upper_Layer_Details *t = ULfirst;

		while(t != NULL) {

			printf("|%s\t|\t%d\t|\t%s\t|\n",t->ip,t->port,t->protocol);			
			t = t->next;
		}			
		printf("-------------------------------------------------\n");
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  create_command_frame():		            	  	     	*/
/* ----------------------------------------------------------------------------	*/

void create_command_frame(int type,int pmu_id,char cmdframe[]) {

	int f = 18;
	long int sec,frac = 0;
	unsigned char fsize[2],pmuid[2],soc[4],fracsec[4];
	uint16_t chk;

	memset(cmdframe,'\0',19);
	memset(fsize,'\0',2);

	int_to_ascii_convertor(f,fsize);
	int_to_ascii_convertor(pmu_id,pmuid);

	sec = (long int)time (NULL);
	long_int_to_ascii_convertor(sec,soc);
	long_int_to_ascii_convertor(frac,fracsec);

	int index = 0;

	switch(type) {

	case 1 : byte_by_byte_copy((unsigned char *)cmdframe,CMDSYNC,index,2); // SEND CFG
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,fsize,index,2);
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,pmuid,index,2);
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,soc,index,4);
	index += 4;
	byte_by_byte_copy((unsigned char *)cmdframe,fracsec,index,4);
	index += 4;
	byte_by_byte_copy((unsigned char *)cmdframe,CMDCFGSEND,index,2);
	index += 2;
	chk = compute_CRC((unsigned char *)cmdframe,index);
	cmdframe[index++] = (chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte; */
	cmdframe[index] = (chk ) & ~(~0<<8);     	/* CHKSUM low byte;  */
	break;

	case 2 : byte_by_byte_copy((unsigned char *)cmdframe,CMDSYNC,index,2);  // SEND DATA ON
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,fsize,index,2);
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,pmuid,index,2);
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,soc,index,4);
	index += 4;
	byte_by_byte_copy((unsigned char *)cmdframe,fracsec,index,4);
	index += 4;
	byte_by_byte_copy((unsigned char *)cmdframe,CMDDATASEND,index,2);
	index += 2;
	chk = compute_CRC((unsigned char *)cmdframe,index);
	cmdframe[index++] = (chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte; */
	cmdframe[index] = (chk ) & ~(~0<<8);     	/* CHKSUM low byte;  */
	break;

	case 3 : byte_by_byte_copy((unsigned char *)cmdframe,CMDSYNC,index,2);  // PUT OFF DATA TRANSMISSION
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,fsize,index,2);
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,pmuid,index,2);
	index += 2;
	byte_by_byte_copy((unsigned char *)cmdframe,soc,index,4);
	index += 4;
	byte_by_byte_copy((unsigned char *)cmdframe,fracsec,index,4);
	index += 4;
	byte_by_byte_copy((unsigned char *)cmdframe,CMDDATAOFF,index,2);
	index += 2;
	chk = compute_CRC((unsigned char *)cmdframe,index);
	cmdframe[index++] = (chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte; */
	cmdframe[index] = (chk ) & ~(~0<<8);     	/* CHKSUM low byte;  */
	break;

	default: printf("Please enter a valid request?\n");
	break;
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  checkip():		            	  	     		*/
/* ----------------------------------------------------------------------------	*/

int checkip(char ip[]) {

	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
	return result;
}	

/**************************************** End of File *******************************************************/
