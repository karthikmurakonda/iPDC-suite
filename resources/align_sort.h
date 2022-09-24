/* ----------------------------------------------------------------------------- 
 * align_sort.h
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


#define MAXTSB 5
#define OLDFRAMECOUNT 5

/* ---------------------------------------------------------------- */
/*                         global variable                          */
/* ---------------------------------------------------------------- */

int front = -1;
int rear=-1;
int Tsb_Count = 0;

/* ---------------------------------------------------------------- */
/*                         global Data Structure                    */
/* ---------------------------------------------------------------- */

/* Data Structure for Time Stamp Buffer */
struct TimeStampBuffer {

	char *soc;
	char *fracsec;
	int count; // Current index
	int num; // Total Number of PMUs
	int counter; // Timer that will be initially set to 0
	int ready_to_send;
	struct pmupdc_id_list *idlist;
	struct data_frame *first_data_frame;

}TSB[MAXTSB];

struct pmupdc_id_list {

	char *idcode;
	int num_pmu;
	struct pmupdc_id_list *nextid;
};

// Kedar K on 8/6/2013
struct  oldDataFramesTimeStampList
{
	unsigned int soc;
	unsigned int fracsec;

}ODFT[OLDFRAMECOUNT];

int old_df_front = -1;
int old_df_rear = -1;

/* ---------------------------------------------------------------- */
/*                 	Function prototypes   		            */
/* ---------------------------------------------------------------- */

void time_align(struct data_frame *df);

void assign_df_to_TSB(struct data_frame *df,int index);

void dispatch(int index);

void clear_TSB(int index);

int create_dataframe(int index);

void checkForMissingFrames(ENTRY*);

int create_cfgframe();

void sort_data_inside_TSB(int index);

struct data_frame* generate_dummy_dataframe(unsigned char *);

void adjustOldFrameCount(int index);

void intermediate_dispatch(int xx);

int matchDataFrameTimeToTSBTime(unsigned int df_soc,unsigned int df_fracsec);

void attacheEachDataFrame(struct data_frame *temp_df,int z);
/**************************************** End of File *******************************************************/
