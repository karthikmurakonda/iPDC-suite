/* ----------------------------------------------------------------------------- 
 * dallocate.c
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


#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h> 
#include  "parser.h"
#include  "dallocate.h" 
#include  "global.h"

/* ------------------------------------------------------------------------------------ */
/*                           Functions defined in dallocate.c           		*/
/* ------------------------------------------------------------------------------------ */

/*       1. void free_cfgframe_object(struct cfg_frame *cfg)         			*/
/*       2. void free_2darray(char** array, int x)	           	    		*/

/* ------------------------------------------------------------------------------------ */


/* -------------------------------------------------------------*/
/* FUNCTION  free_cfgframe_object():                  	     	*/
/* It frees memory allocated to cfg objects. 			*/
/* ------------------------------------------------------------ */

void free_cfgframe_object(struct cfg_frame *cfg) { 

	int j = 0;
	struct dgnames *t_dgnames,*r_dgnames; 
	printf("Inside free_cfgframe_object()\n");

	while(j<cfg->num_pmu) {		  					

		if(cfg->pmu[j]->phnmr != 0)
			free_2darray(cfg->pmu[j]->cnext->phnames,cfg->pmu[j]->phnmr);
		if(cfg->pmu[j]->annmr != 0)
			free_2darray(cfg->pmu[j]->cnext->angnames,cfg->pmu[j]->annmr);

		if(cfg->pmu[j]->dgnmr != 0) {

			t_dgnames = cfg->pmu[j]->cnext->first;
			while(t_dgnames != NULL) {

				r_dgnames = t_dgnames->dg_next;
				free_2darray(t_dgnames->dgn,16);
				t_dgnames = r_dgnames;

			}
		}

		if(cfg->pmu[j]->dgnmr != 0) 
			free_2darray(cfg->pmu[j]->dgunit,cfg->pmu[j]->dgnmr);

		j++;
	} // End of While 

	free(cfg);			
}

/* -------------------------------------------------------------*/
/* FUNCTION  free_2darray:  					*/
/* It frees memory allocated to 2D Arrays. 			*/
/* -------------------------------------------------------------*/

void free_2darray_l(long int **array, int x){ 

	int i;
	for(i=0; i<x; i++)
		free(array[i]);
	free(array);
} 

/* -------------------------------------------------------------*/
/* FUNCTION  free_2darray:  					*/
/* It frees memory allocated to 2D Arrays. 			*/
/* -------------------------------------------------------------*/

void free_2darray(unsigned char **array, int x){ 

	int i;
	for(i=0; i<x; i++)
		free(array[i]);
	free(array);
} 

/**************************************** End of File *******************************************************/
