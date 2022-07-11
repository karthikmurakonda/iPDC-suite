/* ----------------------------------------------------------------------------- 
 * global.h
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


#include <mysql.h>
#include <pthread.h>


/* ---------------------------------------------------------------- */
/*                 	Database variables						   */
/* ---------------------------------------------------------------- */

char *server;
char *user;
char *password ;
char *database ;
int BUFF_LEN;

MYSQL *conn_data; // Connection to database for data insertion
MYSQL *conn_cfg;  // Connection to database for cfg insertion

pthread_mutex_t mutex_cfg;  // To lock cfg objects
pthread_mutex_t mutex_file;  // To lock cfg.txt
pthread_mutex_t mutex_MYSQL_CONN_ON_DATA;  // To lock 'conn_data' connection for DATA insertion
pthread_mutex_t mutex_MYSQL_CONN_ON_CFG;   // To lock 'conn_cfg' connection for CFG insertion

pthread_mutex_t mutex_phasor_buffer;
char *phasorBuff;
char *frequencyBuff;
char *digitalBuff;
char *analogBuff;
char *delayBuff;

/**************************************** End of File *******************************************************/
