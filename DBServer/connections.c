/* ----------------------------------------------------------------------------- 
 * connections.c
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
/*                       Functions defined in connections.c          	    	*/
/* -----------------------------------------------------------------------------*/

/*                 1.  void setup()           	    	      			*/
/*                 2.  void* DB_udp()          	    	      			*/
/*                 3.  void* DB_udphandler(void * udp_BUF)            	    	*/
/*                 4.  void DB_process_UDP(char* udp_BUF)             	      	*/

/* ---------------------------------------------------------------------------- */


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
#include  "connections.h"
#include  "parser.h"
#include  "global.h"

/* ---------------------------------------------------------------- */
/*                         global variables                         */
/* ---------------------------------------------------------------- */

pthread_mutex_t  mutex_on_buffer;

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  setup():                                	     			*/
/* ----------------------------------------------------------------------------	*/

void setup(){

	printf("Entering DBServer setup()\n");

	/* MySQL data base parameters */
	server = "localhost";
	user = "root";
	password = "root";	/* Change this password value, if not the same in your case? */
	database = "iPDC";

	/* MySQL data base connection */
	conn_data = mysql_init(NULL);
	conn_cfg = mysql_init(NULL);

	if(conn_data == NULL) {

		printf("No sufficient memory\n");
		exit(1);
	}

	if(conn_cfg == NULL) {

		printf("No sufficient memory\n");
		exit(1);
	}

	/* Connect to database */
	if (!mysql_real_connect(conn_data, server,
			user, password, database, 0, NULL, 0)) {

		fprintf(stderr, "%s\n", mysql_error(conn_data));
		exit(1);
	}

	if (!mysql_real_connect(conn_cfg, server,
			user, password, database, 0, NULL, 0)) {

		fprintf(stderr, "%s\n", mysql_error(conn_cfg));
		exit(1);
	}

	/* Create UDP socket and bind to port */
	int yes;
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

	DB_server_addr.sin_family = AF_INET;            // host byte order
	DB_server_addr.sin_port = htons(DBPORT);        // short, network byte order
	DB_server_addr.sin_addr.s_addr = INADDR_ANY;    // automatically fill with my IP
	memset(&(DB_server_addr.sin_zero),'\0', 8);     // zero the rest of the struct

	if (bind(DB_sockfd, (struct sockaddr *)&DB_server_addr,
			sizeof(DB_server_addr)) == -1) {
		perror("bind");
		exit(1);

	} else {

		printf("DB Socket Bind :Sucessfull\n");
	} 

	printf("\nDB Server Listening on port %d\n",DBPORT);

	DB_addr_len = sizeof(struct sockaddr);

	DB_udp();
	mysql_close(conn_cfg);
	mysql_close(conn_data);

}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  udp():                                	     			*/
/* It creates a Pthread for each received UDP data.		 		*/
/* ----------------------------------------------------------------------------	*/


void DB_udp(){

	// UDP threads are created for each received UDP message in 'detached' mode. Thus allowing any number of threads to be created. 
	int err;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) { // In  the detached state, the thread resources are
		// immediately freed when it terminates, but 
		perror(strerror(err)); 	                                        // pthread_join(3) cannot be used to synchronize
		exit(1);							//  on the thread termination.	       

	}								

	if((err = pthread_attr_setschedpolicy(&attr,SCHED_FIFO))) { // Shed policy = SCHED_FIFO (realtime, first-in first-out)

		perror(strerror(err));
		exit(1);
	}  


	/* UDP data Received */
	while(1) {

		if ((numbytes = recvfrom(DB_sockfd,buffer, MAXBUFLEN-1, 0,(struct sockaddr *)&their_addr, (socklen_t *)&DB_addr_len)) == -1) { 
			// Main if
			perror("recvfrom");
			exit(1);

		} else { /* New datagram has been received */

			pthread_t t;
			int tt;
			unsigned char *udp_BUF;
			udp_BUF = malloc(MAXBUFLEN*sizeof(unsigned char));

			if(!udp_BUF) {

				printf("No space for udp_BUF\n");
				exit(1);
			}

			printf("UDP Server got packet from %s\n",inet_ntoa(their_addr.sin_addr));
			printf("packet is %d bytes long.\n",numbytes);
			buffer[numbytes] = '\0';

			/* Copy the udp_buf data to a local variable */
			pthread_mutex_lock(&mutex_on_buffer);
			copy_cbyc(udp_BUF,buffer,MAXBUFLEN);
			memset(buffer, '\0', MAXBUFLEN);
			pthread_mutex_unlock(&mutex_on_buffer);

			/* Creates a new thread for each received UDP message */
			if((tt = pthread_create(&t,&attr,DB_udphandler,(void *)udp_BUF))) {

				perror(strerror(tt));	
				exit(1);
			}		

		} // Main if ends

	} // while ends

	pthread_attr_destroy(&attr);		
} 

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  udphandler():                                	     		*/
/* It internally calls process_UDP().				 	*/
/* ----------------------------------------------------------------------------	*/

void* DB_udphandler(void * udp_BUF) {

	DB_process_UDP((unsigned char*)udp_BUF);

	free((unsigned char*)udp_BUF);

	pthread_exit(NULL); /* Exit the thread once the task is done. */

}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  process_UDP():                                	     		*/
/* This function Processes the frames as per their type(command, data, config).	*/
/* ----------------------------------------------------------------------------	*/

void DB_process_UDP(unsigned char* udp_BUF){

	int stat_status,i;
	unsigned char c = udp_BUF[1];
	c <<= 1;
	c >>= 5;
	if(c == 0x00){ 					/* If data frame */

		printf("\nData frame reveived\n");
		stat_status = dataparser(udp_BUF);
		printf("Return from Data Parser\n");

	} else if(c == 0x03) { 			/* If configuration frame */

		printf("\nConfiguration frame reveived\n");
		cfgparser(udp_BUF);
		printf("Return from CFG\n");


	} else {	

		printf("Erroneous frame\n");

	}	
	fflush(stdout);
} 

/**************************************** End of File *******************************************************/
