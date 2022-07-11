/* ----------------------------------------------------------------------------- 
 * ServerFunction.h
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


#include  <stdio.h>
#include  <pthread.h>
#include  <netinet/in.h>

#define BACKLOG 10            /* How many pending connections queue will hold */
#define MAX_STRING_SIZE 5000

/* ---------------------------------------------------------------- */
/*                         global variables                         */
/* ---------------------------------------------------------------- */

int dataFileVar, cfg_crc_error; 
int temp_pahsor_type[50], temp_analog_type[50];
long int temp_PHUNIT_val[50], temp_ANUNIT_val[50];

char pmuFolderPath[200];
char pmuFilePath[200];

unsigned char cline[MAX_STRING_SIZE];
unsigned char hdrline[MAX_STRING_SIZE];
unsigned char data_frm[MAX_STRING_SIZE], df_temp[2], df_temp_1[4];

struct sockaddr_in UDP_my_addr,TCP_my_addr; /* my address information */
struct sockaddr_in UDP_addr,TCP_addr; /* connector’s address information */
struct sigaction sa;

int UDP_sockfd,TCP_sockfd; /* socket descriptors */
int UDP_addr_len,TCP_sin_size, numbytes;

pthread_t UDP_thread,TCP_thread,DATA_thread;;
FILE *fp_DataFile;
uint16_t df_chk;

//pthread_mutex_t mutex_pdc_object;  /* To lock cfg data objects */

/* ------------------------------------------------------------------ */
/*                       Global Datastructure                         */
/* ------------------------------------------------------------------ */

struct PDC_Details {

	int  port;
	char ip[16];
	char protocol[4];
	int  sockfd;
	int  tcpup;
	int  STAT_change; //0-no change, 1-changed
	int  pmu_cfgsent;   //0- not sent, 1- sent
	int  data_transmission;  //0 maens ON, 1 maeans OFF
	int  cmd_received;  ////0 maens Not-received, 1 maeans received
	int  address_set;
	pthread_t thread_id;
	struct sockaddr_in pdc_addr;
	struct PDC_Details *next;
	struct PDC_Details *prev;

}*PDCfirst,*PDClast;

/* ------------------------------------------------------------------ */
/*                       Function prototypes                          */
/* ------------------------------------------------------------------ */

void  frame_size();

void generate_data_frame();

void* SEND_DATA();

void PDC_MATCH(int proto, int newfd);

void* UDP_PMU();

void* TCP_CONNECTIONS(void * temp_pdc);

void remove_tcp_node(void * node);

void* TCP_PMU();

void start_server();

int get_header_frame();

void  SIGUSR1_handler(int);   /* Signal handler */

void  SIGUSR2_handler(int);   /* Signal handler */

/**************************************** End of File *******************************************************/
