/* ----------------------------------------------------------------------------- 
 * connections.h
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


#include <netinet/in.h>
#include <signal.h>


/* ---------------------------------------------------------------- */
/*                         global variable                          */
/* ---------------------------------------------------------------- */

#define DBPORT 9000     /* The port users will be connecting to */
#define BACKLOG 10      /* How many pending connections queue will hold */
#define MAXBUFLEN 2000

int UL_UDP_addr_len,UL_TCP_sin_size, numbytes;
unsigned char UL_udp_command[19];
unsigned char UL_tcp_command[19];


/* ---------------------------------------------------------------- */
/*                         global Data Structure                    */
/* ---------------------------------------------------------------- */

struct sigaction sa;

struct Lower_Layer_Details {

	unsigned int pmuid;
	char ip[16];
	int port;
	char protocol[4];
	int sockfd;
	int up; 			/* used only in tcp */
	struct sockaddr_in llpmu_addr;
	pthread_t thread_id;
	int data_transmission_off;
	int pmu_remove;
	int request_cfg_frame;
	float latitude;
	float longitude;
	struct Lower_Layer_Details *next;
	struct Lower_Layer_Details *prev;

}*LLfirst,*LLlast;


struct Upper_Layer_Details {

	char ip[16];
	int port;
	char protocol[4];
	int sockfd;
	int tcpup;
	pthread_t thread_id;
	struct sockaddr_in pdc_addr;
	int config_change;
	int UL_upper_pdc_cfgsent;
	int UL_data_transmission_off;
	int address_set;
	struct Upper_Layer_Details *next;
	struct Upper_Layer_Details *prev;

}*ULfirst,*ULlast;


/* ---------------------------------------------------------------- */
/*                 	Function prototypes   		            */
/* ---------------------------------------------------------------- */

void setup();

void* UL_tcp();

void* UL_tcp_connection(void * newfd); 

void* UL_udp();

void PMU_process_UDP(unsigned char *,struct sockaddr_in,int sockfd);

void PMU_process_TCP(unsigned char tcp_buffer[],int sockfd);

void sigchld_handler(int s);	/* TCP signal handler */

/**************************************** End of File *******************************************************/
