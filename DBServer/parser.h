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


#define MAXBUFLEN 2000


/* ---------------------------------------------------------------- */
/*                 	Data Structure prototypes	            */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/*             	     Configuartion Frame Data Structure	            */
/* ---------------------------------------------------------------- */

struct  cfg_frame {

	unsigned int framesize;
	unsigned int idcode;
	unsigned long int soc;
	unsigned long int fracsec;
	unsigned long int time_base;
	unsigned int num_pmu;
	struct for_each_pmu **pmu;
	unsigned int data_rate;
	struct  cfg_frame *cfgnext;

}*cfgfirst;


struct for_each_pmu{

	unsigned char stn[17];
	unsigned int idcode;
	char data_format[3];
	struct format *fmt;
	unsigned int phnmr;
	unsigned int annmr;
	unsigned int dgnmr;
	struct channel_names *cnext;
	float **phunit;
	float **anunit;
	unsigned char **dgunit;
	unsigned int fnom;
	unsigned int cfg_cnt;
};

struct channel_names {

	unsigned char **phnames;
	unsigned char **angnames;
    	unsigned char **phtypes;
    	unsigned char **antypes;
	struct dgnames *first;
};

struct dgnames {

	unsigned char **dgn; // Stores 16 digital names for each word
	struct dgnames *dg_next;
};

// This struct is added to help find out whether the measurements are floating/fixed, polar/rectangular .
struct format{

	unsigned int freq;
	unsigned int analog;
	unsigned char phasor;
	unsigned char polar;
};

struct DataBuff{

        unsigned char *data;
        unsigned int type;
};


/* ---------------------------------------------------------------- */
/*                 	Function prototypes   		            */
/* ---------------------------------------------------------------- */


void cfgparser(unsigned char []); 

void cfginsert(struct cfg_frame *); 

int dataparser(unsigned char data[]);

char* hexTobin(char s); 

int check_statword(unsigned char stat[]);  

void remove_old_cfg(int idcode,unsigned char frame[]);                    				   

unsigned int to_intconvertor(unsigned char array[]);

unsigned int to_intconvertor1(unsigned char array[]);  //Added by gopal  -- to_intconvertor1

unsigned long int to_long_int_convertor(unsigned char * array);

unsigned long int to_long_int_convertor1(unsigned char * array); //Added by gopal  -- to_long_int_convertor1

float decode_ieee_single(const void *v);

void copy_cbyc(unsigned char dst[],unsigned char *s,int size);

int ncmp_cbyc(unsigned char dst[],unsigned char src[],int size);

void* dataWriteInFile(void * temp_buff);

/**************************************** End of File *******************************************************/
