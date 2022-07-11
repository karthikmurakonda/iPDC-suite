/* ----------------------------------------------------------------------------- 
 * PmuGui.h
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


#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include<gtk/gtk.h>

/* Convenience macros for obtaining objects from UI file */
#define CH_GET_OBJECT(builder, name, type, data) \
		data->name = type(gtk_builder_get_object (builder, #name) )
#define CH_GET_WIDGET(builder, name, data) \
		CH_GET_OBJECT(builder, name, GTK_WIDGET, data)


/* ---------------------------------------------------------------- */
/*                         global Data Structure                    */
/* ---------------------------------------------------------------- */

/* PMU's main data structure definition */
typedef struct _pmuStruct pmuStruct;

struct _pmuStruct
{
     /* Main application window */
	GtkWidget *Pmu_Simulator;  

     /* Main window button widgets */
	GtkWidget *create_cfg_button;
	GtkWidget *header_frm_button;
	GtkWidget *pmu_details_button;
	GtkWidget *stat_modification_button;
	GtkWidget *cfg_modification_button;
	GtkWidget *manage_data_source;

     /* Manubar button widgets */
	GtkWidget *pmu_menubar;
	GtkWidget *start_server;
	GtkWidget *open_cfg;
	GtkWidget *pmu_properties;
	GtkWidget *exit_menuitem;
	GtkWidget *about_menuitem;

	GtkWidget * menuitem2;
	GtkWidget * menu_setup_cfg;
	GtkWidget * menu_data_source;
	GtkWidget * menu_cfg_modify;
	GtkWidget * menu_stat_modify;
	GtkWidget * menu_header_frm;

     /* Main window extra widgets */
	GtkWidget *rights_lable;
	GtkWidget *admin_label;
	GtkWidget *E_button;
	GtkWidget *text_view;
	GtkWidget *text_view1;
	GtkWidget *time_lable;
};


/* ---------------------------------------------------------------- */
/*                         global variables                         */
/* ---------------------------------------------------------------- */

int srvr_entry;
long int ps_thrd_id;
char *markup;

/* iPDC Setup File path globaly */
char pmuFolderPath[200];
char pmuFilePath[200];
char fptr[200];

pid_t  pidLocal;
pmuStruct *pmu_data;
GtkWidget *pmu_server_window;
GtkWidget *p_id, *p_udp, *p_tcp, *p_ip;
GtkWidget *udp_port, *tcp_port;
GtkWidget *CCWin, *setup_display_window;	
GtkWidget *data_managing_window;
GtkWidget *new_window;
GtkWidget *about_dialog;
GtkWidget *help_dialog;
GtkWidget *pmu_setup_window;


/* ------------------------------------------------------------------ */
/*                       Function prototypes                          */
/* ------------------------------------------------------------------ */

int  checkip(char ip[]);

int  isNumber(char *s);

void destroy (GtkWidget *widget, gpointer udata);

GdkPixbuf *create_pixbuf(const gchar * filename);

void about_pmu (GtkButton *widget, gpointer udata);

void Pmu_Help (GtkButton *but, gpointer udata);

void validation_result (char *msg);

void pmu_colors();

void pmu_server ();

int  validation_pmu_server (GtkButton *button, gpointer udata);

void manage_data_sources();

void Set_data_source(GtkRadioButton *radiobutton, gpointer udata);

void file_ok_sel(GtkWidget *w, GtkFileSelection *fs);

void pmu_setup_file_selection(GtkWidget *widget, gpointer udata);

void view_setup_file (GtkWidget *w, GtkFileSelection *fs);

void apply_pmu_setup (GtkWidget *w, gpointer udata);

void* display_time();

#endif 

/**************************************** End of File *******************************************************/
