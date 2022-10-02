/* ----------------------------------------------------------------------------- 
 * new_pmu_or_pdc.h
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


/* ------------------------------------------------------------------ */
/*                       Function prototypes                          */
/* ------------------------------------------------------------------ */

int add_PMU(char pmuid[], char ip[], char port[], char protocol[]);

void add_PMU_Node(struct Lower_Layer_Details *temp_pmu);

void* connect_pmu_tcp(void *);

void* connect_pmu_udp(void *);

int remove_Lower_Node(char pmuid[], char protocol[]);

void* remove_llnode(void*);

int put_data_transmission_off(char pmuid[], char protocol[]);

void* data_off_llnode(void* temp);

int put_data_transmission_on(char pmuid[], char protocol[]);

void* data_on_llnode(void* temp);

int configuration_request(char pmuid[], char protocol[]);

void* config_request(void* temp);

int add_PDC(char ip[], char protocol[]);

int remove_PDC(char ip[], char port_num[], char protocol[]);

void display_CT();

void create_command_frame(int type,int pmuid,char *);

int checkip(char ip[]);

/**************************************** End of File *******************************************************/
