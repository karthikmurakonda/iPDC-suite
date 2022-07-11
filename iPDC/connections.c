/* ----------------------------------------------------------------------------- 
 * connections.c
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
#include  <gtk/gtk.h>
#include  "ipdcGui.h"
#include  "connections.h"
#include  "parser.h"
#include  "global.h"
#include  "new_pmu_or_pdc.h"
#include  "align_sort.h"


/* ---------------------------------------------------------------------*/
/*                  Functions defined in connections.c          	*/
/* ---------------------------------------------------------------------*/

/*                 1.  void  setup()           	    	      		*/
/*                 2.  void* UL_udp()          	    	      		*/
/*                 3.  void* UL_tcp()            	    		*/
/*                 4.  void* UL_tcp_connection()	 		*/
/*                 5.  void  PMU_process_UDP()		               	*/
/*                 6.  void  PMU_process_TCP()                     	*/
/*                 7.  void  sigchld_handler()       	              	*/

/* -------------------------------------------------------------------- */


/* ---------------------------------------------------------------- */
/*                         global variables                         */
/* ---------------------------------------------------------------- */

int yes = 1; 	/* argument to setsockopt */
char display_buf[200];


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  setup():                                	     			*/
/* It creates two threads by calling tcp() and udp() in each thread. 		*/
/* ----------------------------------------------------------------------------	*/


void setup(){

	/* ---------------------------------------------------------------- */
	/*        Initialize Global Mutex Variables from global.h           */
	/* ---------------------------------------------------------------- */

	int err;

	/* Create UDP socket and bind to port */

	if ((UL_UDP_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {

		perror("socket");
		exit(1);

	} else {

		printf("UDP Socket:Sucessfully created\n");

	} 	

	if (setsockopt(UL_UDP_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	UDP_my_addr.sin_family = AF_INET;            // host byte order
	UDP_my_addr.sin_port = htons(UDPPORT);       // short, network byte order
	UDP_my_addr.sin_addr.s_addr = INADDR_ANY;    // automatically fill with my IP
	memset(&(UDP_my_addr.sin_zero),'\0', 8);     // zero the rest of the struct

	if (bind(UL_UDP_sockfd, (struct sockaddr *)&UDP_my_addr,
			sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	} else {

		printf("UDP Socket Bind :Sucessfull\n");
	} 


	/* Created socket and bound to port */
	/* Create TCP socket and bind and listen on port */

	if ((UL_TCP_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	} else {

		printf("TCP Socket:Sucessfully created\n");
	}

	if (setsockopt(UL_TCP_sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	TCP_my_addr.sin_family = AF_INET;          // host byte order
	TCP_my_addr.sin_port = htons(TCPPORT);     // short, network byte order
	TCP_my_addr.sin_addr.s_addr = INADDR_ANY;  // automatically fill with my IP
	memset(&(TCP_my_addr.sin_zero), '\0', 8);  // zero the rest of the struct

	if (bind(UL_TCP_sockfd, (struct sockaddr *)&TCP_my_addr, sizeof(struct sockaddr))
			== -1) {
		perror("bind");
		exit(1);

	} else {

		printf("TCP Socket Bind :Sucessfull\n");
	}

	if (listen(UL_TCP_sockfd, BACKLOG) == -1) {

		perror("listen");
		exit(1);

	} else {

		printf("TCP Listen :Sucessfull\n");
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	/* TCP created socket and is litening for connections */

	printf("\nUDP Listening on port %d for command frames from Upper PDC\n",UDPPORT);
	printf("\nTCP Listening on port %d for command frames from Upper PDC\n",TCPPORT);
	printf("\nPort %d for Sending the data frames for archival from iPDC\n",DBPORT);

	UL_TCP_sin_size = sizeof(struct sockaddr_in);
	UL_UDP_addr_len = sizeof(struct sockaddr);
	DB_addr_len = sizeof(struct sockaddr);

	/* Threads are created for UDP and TCP to listen on port 6001 and 6000 respectively in default attr mode*/
	if((err = pthread_create(&UDP_thread,NULL,UL_udp,NULL))) { 

		perror(strerror(err));
		exit(1);	
	}

	if((err = pthread_create(&TCP_thread,NULL,UL_tcp,NULL))) {

		perror(strerror(err));
		exit(1);	
	}    
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  UL_udp():                                	     			*/
/* Handles upper layer PDC command frames			 		*/
/* ----------------------------------------------------------------------------	*/

void* UL_udp(){

	/* UDP data Received */
	while(1) {

		memset(UL_udp_command,'\0',19);
		memset(display_buf,'\0',200);

		if ((numbytes = recvfrom(UL_UDP_sockfd,UL_udp_command, 18, 0,(struct sockaddr *)&UL_UDP_addr, (socklen_t *)&UL_UDP_addr_len)) == -1) { 
			// Main if
			perror("recvfrom");
			exit(1);

		} else { /* New datagram has been received */

			int pdc_flag = 0;
			pthread_mutex_lock(&mutex_Upper_Layer_Details);
			struct Upper_Layer_Details *temp_pdc = ULfirst;	
			
			if(ULfirst == NULL) {

				pdc_flag = 0;				

			} else  {

				while(temp_pdc != NULL ) {

					if((!strcmp(temp_pdc->ip,inet_ntoa(UL_UDP_addr.sin_addr))) && 
							(!strncasecmp(temp_pdc->protocol,"UDP",3)) && (temp_pdc->port == UDPPORT)) {

						pdc_flag = 1;		
						break;
					} else {

						temp_pdc = temp_pdc->next;
					}									
				}  												
			}

			if(pdc_flag){ 

				unsigned char c = UL_udp_command[1];
				c <<= 1;
				c >>= 5;	
				temp_pdc->sockfd = UL_UDP_sockfd;

				if(c  == 0x04) { /* Check if it is a command frame from Upper PDC */ 

					printf("\nCommand frame Received at iPDC.\n");
					c = UL_udp_command[15];

					if((c & 0x05) == 0x05){ //Send CFg frame to PDC

						printf("\nCommand frame for CFG Received\n");

						while(root_pmuid != NULL); // Wait till all the status change has been cleared

						printf("sockfd = %d,ipaddress = %s\n",temp_pdc->sockfd,inet_ntoa(temp_pdc->pdc_addr.sin_addr));

						if(temp_pdc->address_set == 0) {

							memcpy(&temp_pdc->pdc_addr,&UL_UDP_addr,sizeof(UL_UDP_addr));

						}
						numbytes = create_cfgframe();

						if ((numbytes = sendto (temp_pdc->sockfd,cfgframe, numbytes, 0,
								(struct sockaddr *)&temp_pdc->pdc_addr,sizeof(temp_pdc->pdc_addr)) == -1)) {

							perror("sendto");

						} else {

							printf("Sent iPDC Configuration Frame\n");
						}
						free(cfgframe);

						temp_pdc->UL_upper_pdc_cfgsent = 1;
						temp_pdc->config_change = 0;

					} else if((c & 0x02) == 0x02) { // if data frame 

						if(temp_pdc->UL_upper_pdc_cfgsent == 1) { // Only if cfg is sent send the data	

							temp_pdc->UL_data_transmission_off = 0;

						} else {

							printf("Data cannot be sent as CMD for CFG not received\n");

						}			

					} else if ((c & 0x01) == 0x01){

						temp_pdc->UL_data_transmission_off = 1;

					}				  

				} else { /* If it is a frame other than command frame */

					printf("Not a command frame\n");						
				}


			} else { /* If the command frame is not from authentic PDC*/

				printf("Command frame from un-authentic PDC\n");
			}

		} // Main if ends	
		pthread_mutex_unlock(&mutex_Upper_Layer_Details);
	} // while ends		
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  UL_tcp():                                	     			*/
/* It Handles Upper Layer PDC connections.					*/
/* ----------------------------------------------------------------------------	*/

void* UL_tcp() {

	int err;

	// A new thread is created for each TCP connection in 'detached' mode. Thus allowing any number of threads to be created. 
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) { // In  the detached state, the thread resources are

		// immediately freed when it terminates, but 
		perror(strerror(err));	                                        // pthread_join(3) cannot be used to synchronize
		exit(1);							//  on the thread termination.	       
	}																																		    

	if((err = pthread_attr_setschedpolicy(&attr,SCHED_FIFO))) { // Shed policy = SCHED_FIFO (realtime, first-in first-out)

		perror(strerror(err));		     
		exit(1);
	}  

	int sin_size,new_fd,pdc_flag = 0;

	while (1) {

		sin_size = sizeof(struct sockaddr_in);

		if (((new_fd = accept(UL_TCP_sockfd, (struct sockaddr *)&UL_TCP_addr,
				(socklen_t *)&sin_size)) == -1)) { // main if starts
			perror("accept");

		} else { /* New TCP connection has been received*/ 

			pthread_mutex_lock(&mutex_Upper_Layer_Details);							
			struct Upper_Layer_Details *temp_pdc = ULfirst;
			if(ULfirst == NULL) {

				pdc_flag = 0;				

			} else {

				while(temp_pdc != NULL ) {

					if((!strcmp(temp_pdc->ip,inet_ntoa(UL_TCP_addr.sin_addr))) && 
							(!strncasecmp(temp_pdc->protocol,"TCP",3)) && (temp_pdc->port == TCPPORT)) {

						pdc_flag = 1;		
						break;
					} else {

						temp_pdc = temp_pdc->next;
					}									
				}  												
			}
			if(pdc_flag) {

				temp_pdc->sockfd = new_fd;				
				pthread_t t;

				/* PDC is authentic. Send the command frame for cfg frame */
				printf("server: got connection from %s\n",
						inet_ntoa(temp_pdc->pdc_addr.sin_addr));

				/* Creates a new thread for each TCP connection. */
				if((err = pthread_create(&t,&attr,UL_tcp_connection,(void *)temp_pdc))) {

					perror(strerror(err));		     
					exit(1);
				}				

			} else { /* If PMU ip is not in the ipdcINFO.bin */

				printf("Request from %s TCP which is un-authentic\n",
						inet_ntoa(UL_TCP_addr.sin_addr));			
			}	
			pthread_mutex_unlock(&mutex_Upper_Layer_Details);									

		} // main if ends	

	} // While ends

	pthread_attr_destroy(&attr);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  UL_tcp_connection():                                     		*/
/* It handles command frames from upper layer PDC on TCP	.		*/
/* ----------------------------------------------------------------------------	*/

void* UL_tcp_connection(void * temp_pdc) {

	struct Upper_Layer_Details *udetails = (struct Upper_Layer_Details *) temp_pdc;
	int UL_new_fd = udetails->sockfd;
	udetails->thread_id = pthread_self();

	while(1) {

		memset(UL_tcp_command,19,0);	
		int bytes_read = recv(UL_new_fd,UL_tcp_command,18,0);
		if(bytes_read == -1) {

			perror("recv");
			udetails->tcpup = 0;
			pthread_exit(NULL);

		} else if(bytes_read == 0){

			printf("The Client connection exit.\n");
			udetails->tcpup = 0;
			pthread_exit(NULL);

		} else {

			pthread_mutex_lock(&mutex_Upper_Layer_Details);							
			unsigned char c = UL_tcp_command[1];
			c <<= 1;
			c >>= 5;

			if(c  == 0x04) {	/* Check if it is a command frame from Upper PDC*/			

				printf("Command frame Received\n"); // Need to further check if the command is for cfg or data
				c = UL_tcp_command[15];

				if((c & 0x05) == 0x05){ //Send CFg frame to PDC

					while(root_pmuid != NULL); // Wait till the staus chage list becomes empty
					numbytes = create_cfgframe();
					udetails->tcpup = 1;

					if (send(UL_new_fd,cfgframe,numbytes, 0)== -1)
						perror("send");						
					free(cfgframe);

					udetails->UL_upper_pdc_cfgsent = 1;
					udetails->config_change = 0;

				} else if((c & 0x02) == 0x02) {

					if(udetails->UL_upper_pdc_cfgsent == 1) { // Only if cfg is sent send the data

						udetails->UL_data_transmission_off = 0;

					} else {

						printf("Data cannot be sent as CMD for CFG not received\n");

					}			

				} else if ((c & 0x01) == 0x01){ // Put the data transmission off

					udetails->UL_data_transmission_off = 1;
				}
			} else { /* If it is a frame other than command frame */

				printf("Not a command frame\n");						
			}

			pthread_mutex_unlock(&mutex_Upper_Layer_Details);							
		}        		

	} // while
	close(UL_new_fd);
	pthread_exit(NULL);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  PMU_process_UDP():                                	     		*/
/* This function processes the frames as per their type( data, config).	 	*/
/* The received frames are from Lower Layer PMU/PDC on UDP.		 	*/
/* ----------------------------------------------------------------------------	*/

void PMU_process_UDP(unsigned char *udp_buffer,struct sockaddr_in PMU_addr,int sockfd){

	int stat_status;
	unsigned int id;
	unsigned char id_char[2];

	id_char[0] = udp_buffer[4];
	id_char[1] = udp_buffer[5];
	id = to_intconvertor(id_char);

	unsigned char c = udp_buffer[1];
	c <<= 1;
	c >>= 5;
	if(c == 0x00){ 							/* If data frame */

		stat_status = dataparser(udp_buffer);

		/* Change in cfg frame is handled */
		if((stat_status == 10)||(stat_status == 14)) {

			unsigned char *cmdframe = malloc(19);
			cmdframe[18] = '\0';
			create_command_frame(1,id,(char *)cmdframe);

			if (sendto(sockfd,cmdframe,18, 0,
					(struct sockaddr *)&PMU_addr,sizeof(PMU_addr)) == -1)
				perror("sendto");
			free(cmdframe);						

		} else if (stat_status == 15) { 			/* Data Invalid */

			printf("Data Invalid\n");

		}

	} else if(c == 0x03) { 						/* If configuration frame */

		printf("\nConfiguration frame received.\n");
		cfgparser(udp_buffer);

		unsigned char *cmdframe = malloc(19);
		cmdframe[18] = '\0';
		create_command_frame(2,id,(char *)cmdframe);
		printf("\nReturn from create_command_frame\n");

		/* Command frame sent to send the data frames */
		if (sendto(sockfd,cmdframe, 18, 0,
				(struct sockaddr *)&PMU_addr,sizeof(PMU_addr)) == -1)
			perror("sendto");
		free(cmdframe);

	} else {	

		printf("Erroneous frame\n");

	}	
	fflush(stdout);
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  PMU_process_TCP():                                	     		*/
/* This function processes the frames as per their type( data, config).	 	*/
/* The received frames are from Lower Layer PMU/PDC on TCP.		 	*/
/* ----------------------------------------------------------------------------	*/

void PMU_process_TCP(unsigned char tcp_buffer[],int sockfd) {

	int stat_status;
	unsigned int id;
	unsigned char id_char[2];

	id_char[0] = tcp_buffer[4];
	id_char[1] = tcp_buffer[5];
	id = to_intconvertor(id_char);

	unsigned char c = tcp_buffer[1];
	c <<= 1;
	c >>= 5;

	if(c == 0x00){ 							/* If data frame */

		stat_status = dataparser(tcp_buffer);

		/* Handle the Stat word */
		if((stat_status == 10)||(stat_status == 14)) {

			unsigned char *cmdframe = malloc(19);
			cmdframe[18] = '\0';
			create_command_frame(1,id,(char *)cmdframe);

			if (send(sockfd,cmdframe,18, 0)== -1)
				perror("send");	
			free(cmdframe);					

		} else if (stat_status == 15) { 			/* Data Invalid */

			printf("Data Invalid\n");
		}

	} else if(c == 0x03) { 						/* If configuration frame */

		printf("\nConfiguration frame received.\n");
		cfgparser(tcp_buffer);
		unsigned char *cmdframe = malloc(19);
		cmdframe[18] = '\0';
		create_command_frame(2,id,(char *)cmdframe);
		printf("Return from create_command_frame().\n");

		/* Command frame sent to send the data frames */
		if (send(sockfd,cmdframe,18, 0)== -1)
			perror("send");
		free(cmdframe);

	} else {	

		printf("\nErroneous frame\n");
	}	
	fflush(stdout);
} 


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  sigchld_handler():                                	     		*/
/* ----------------------------------------------------------------------------	*/

void sigchld_handler(int s) {
	while(wait(NULL) > 0);
}

/**************************************** End of File *******************************************************/
