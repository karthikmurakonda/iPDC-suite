/* ----------------------------------------------------------------------------- 
 * CfgFunction.h
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


#include<gtk/gtk.h>

/* ---------------------------------------------------------------- */
/*                         global variables                         */
/* ---------------------------------------------------------------- */

int  PMU_uport;
int  PMU_tport;
char *hdr_frame, *markup1;
unsigned char phunit_final_val[500];
unsigned char anunit_final_val[500];


/* ---------------------------------------------------------------- */
/*                         global Data Structure                    */
/* ---------------------------------------------------------------- */

struct ConfigurationFrame 
{
	int  cfg_pmuID; 
	int  cfg_fdf; 
	int  cfg_af; 
	int  cfg_pf; 
	int  cfg_pn; 
	int  cfg_phnmr_val; 
	int  cfg_annmr_val; 
	int  cfg_dgnmr_val; 
	int  cfg_fnom; 
	int  cfg_dataRate; 
	char *cfg_STNname; 
	char *cfg_phasor_channels; 
	char *cfg_analog_channels; 
	char *cfg_digital_channels; 
};
struct ConfigurationFrame *cfg_info;

struct ConfigurationChange 
{
	int  add_remove_choice; 
	int  new_cfg_phnmr_val; 
	int  new_cfg_annmr_val; 
	int  data_rate_choice; 
	int  new_data_rate; 
	char *new_cfg_phasor_channels; 
	char *new_cfg_analog_channels; 
};
struct ConfigurationChange *new_cfg_info;


/* ------------------------------------------------------------------ */
/*                       Function prototypes                          */
/* ------------------------------------------------------------------ */

void header_frm_gen(int len);

void reconfig_cfg_CC();

void show_pmu_details (GtkWidget *widget, gpointer udata);

int  create_cfg();

/**************************************** End of File *******************************************************/
