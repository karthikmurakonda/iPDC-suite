/* ----------------------------------------------------------------------------- 
 * CfgGuiFunctions.h
 * 
 * PMU Simulator - Phasor Measurement Unit Simulator
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


#include <gtk/gtk.h>


/* ---------------------------------------------------------------- */
/*                         global variables                         */
/* ---------------------------------------------------------------- */

GtkWidget *cfg_setup_window;
GtkWidget *cfg_name;
GtkWidget *setup_close_button;
GtkWidget *p_stn, *p_id, *p_ff;
GtkWidget *p_af, *p_pf, *p_pn;
GtkWidget *p_phNumber;
GtkWidget *p_anNumber;
GtkWidget *p_dgNumber;
GtkWidget *p_drate;
GtkWidget *num_text[50], *num_text1[50];
GtkWidget *num_combo[50];
GtkWidget *PhasorChWin;
GtkWidget *AnalogChWin;
GtkWidget *CCWin;
GtkWidget *CCOptionWin;
GtkWidget *phtext, *angtext, *HdrFrm;
GtkWidget *p_frequency;
GtkWidget *hdr_frame_window;
GtkWidget *remove_cfg_window;
GtkWidget *chkBtn1, *chkBtn2;

/* ------------------------------------------------------------------ */
/*                       Function prototypes                          */
/* ------------------------------------------------------------------ */

void cfg_create_function (GtkWidget *widget, gpointer udata);
//void freq_val();
int  validation_cfg_create ();

void channel_names_for_phasor ();
int  validation_phasor_names(GtkWidget *widget, gpointer udata);

void channel_names_for_analog ();
int  validation_analog_names(GtkWidget *widget, gpointer udata);

void channel_names_for_digital ();
int  validation_digital_names(GtkWidget *widget, gpointer udata);

void final_cfg_create ();
void cfg_create_call();
void checkbox_function(GtkWidget *widget, gpointer data);
int  validation_setup_file_name (GtkWidget *widget, gpointer udata);

void cfg_STAT_change (GtkWidget *widget, gpointer udata);
void stat_chng_options(GtkWidget *widget, gpointer udata);

void Change_pmu_configuration(GtkWidget *widget, gpointer udata);
void new_phasor_num(GtkWidget *widget, gpointer udata);
int  validation_ph_an_num (GtkButton *but, gpointer udata);

void new_channel_names_for_phasor ();
int  validation_new_phasor_names();
int  validate_func(GtkWidget *widget, gpointer udata);

void new_channel_names_for_analog ();
int  validation_new_analog_names(GtkWidget *widget, gpointer udata);

void remove_phasor_num (GtkWidget *widget, gpointer udata);
int  validation_remove_ph_an_num (GtkButton *but, gpointer udata);

void change_data_rate_option();
void enter_new_data_rate(GtkWidget *widget, gpointer udata);
int  validation_new_data_rate (GtkButton *but, gpointer udata);
int  final_cfg_update_call (GtkButton *but, gpointer udata);

void hdr_create_function (GtkWidget *widget, gpointer udata);
void validation_hdr_frm (GtkWidget *widget, gpointer udata);

/**************************************** End of File *******************************************************/
