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

#define DBPORT 9000     
#define MAXBUFLEN 2000

int DB_sockfd; //socket descriptor
struct sockaddr_in DB_server_addr,their_addr; 
int DB_addr_len,numbytes;
unsigned char buffer[MAXBUFLEN];

/* ---------------------------------------------------------------- */
/*                 	Function prototypes   		            */
/* ---------------------------------------------------------------- */

void setup();

void DB_udp();

void* DB_udphandler(void * udp_BUF);

void DB_process_UDP(unsigned char* udp_BUF);

/**************************************** End of File *******************************************************/
