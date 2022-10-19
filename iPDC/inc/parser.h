/* ----------------------------------------------------------------------------- 
 * parser.h
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


#include  <stdint.h>
#define MAXBUFLEN 2000


/* ---------------------------------------------------------------- */
/*                 	Data Structure prototypes	            */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/*             	     Configuartion Frame Data Structure	            */
/* ---------------------------------------------------------------- */

struct  cfg_frame {

	unsigned char *framesize;
	unsigned char *idcode;
	unsigned char *soc;
	unsigned char *fracsec;
	unsigned char *time_base;
	unsigned char *num_pmu;
	struct for_each_pmu **pmu;
	unsigned char *data_rate;
	struct  cfg_frame *cfgnext;

}*cfgfirst;


struct for_each_pmu{

	unsigned char *stn;
	unsigned char *idcode;
	unsigned char *data_format;
	struct format *fmt;
	unsigned char *phnmr;
	unsigned char *annmr;
	unsigned char *dgnmr;
	struct channel_names *cnext;
	unsigned char **phunit;
	unsigned char **anunit;
	unsigned char **dgunit;
	unsigned char *fnom;
	unsigned char *cfg_cnt;
};

struct channel_names {

	unsigned char **phnames;
	unsigned char **angnames;
	struct dgnames *first;
};

struct dgnames {

	unsigned char **dgn; // Stores 16 digital names for each word
	struct dgnames *dg_next;
};

struct format{

	unsigned char freq;
	unsigned char analog;
	unsigned char phasor;
	unsigned char polar;
};


/* ---------------------------------------------------------------- */
/*             	  	Data Frame Data Structure	            */
/* ---------------------------------------------------------------- */

struct data_frame {

	unsigned char *framesize;
	unsigned char *idcode;
	unsigned char *soc;
	unsigned char *fracsec;
	int num_pmu;
	struct data_for_each_pmu **dpmu;
	struct data_frame *dnext;
};

struct data_for_each_pmu {

	unsigned char *stat;
	int phnmr;
	int annmr;
	int dgnmr;
	struct format *fmt;
	unsigned char **phasors;
	unsigned char **analog;	
	unsigned char *freq;
	unsigned char *dfreq;
	unsigned char **digital;
};

/* ---------------------------------------------------------------- */
/*             	  	Status change Data Structure	            */
/* ---------------------------------------------------------------- */

struct status_change_pmupdcid {

	unsigned char idcode[3];
	struct status_change_pmupdcid *pmuid_next;

}*root_pmuid;


/* ---------------------------------------------------------------- */
/*                 	Function prototypes   		            */
/* ---------------------------------------------------------------- */

void cfgparser(unsigned char []); 

void write_cfg_to_file();

int dataparser(unsigned char data[]);

int check_statword(unsigned char stat[]);  

void add_id_to_status_change_list(unsigned char idcode[]);

void remove_id_from_status_change_list(unsigned char idcode[]); 

unsigned int to_intconvertor(unsigned char []);

void long_int_to_ascii_convertor(unsigned long int n,unsigned char hex[]);

void int_to_ascii_convertor(unsigned int n,unsigned char hex[]);

void copy_cbyc(unsigned char dst[],unsigned char *s,int size);

int ncmp_cbyc(unsigned char dst[],unsigned char src[],int size);

void byte_by_byte_copy(unsigned char dst[],unsigned char src[],int index,int n);

unsigned long int to_long_int_convertor(unsigned char array[]);

uint16_t compute_CRC(unsigned char *message,int length);

float decode_ieee_single(const void *v);

/**************************************** End of File *******************************************************/
