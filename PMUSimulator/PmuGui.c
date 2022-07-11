/* ----------------------------------------------------------------------------- 
 * PmuGui.c
 * 
 * PMU Simulator - Phasor Measurement Unit Simulator
 *
 * Copyright (C) 2011-2012 Nitesh Pandit
 * Copyright (C) 2011-2012 Kedar V. Khandeparkar
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version." Not a valid port number! Try again ";
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "PmuGui.h"
#include "ServerFunction.h"
#include "CfgFunction.h"
#include "CfgGuiFunctions.h"
#include "ShearedMemoryStructure.h"
#include "function.h"

#define MAX_STRING_SIZE 5000

/* -------------------------------------------------------------------------------------- */
/*                                 Functions in PmuGui.c                                  */
/* -------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/*                                                                              */
/*	1. int  isNumber(char *s);              						     */
/*	2. int  checkip(char ip[]);				               		     */
/*	3. void destroy (GtkWidget *widget, gpointer udata);			          */
/*	4. GdkPixbuf *create_pixbuf(const gchar * filename);			*/
/*	5. void about_pmu (GtkButton *widget, gpointer udata);			          */
/*	6. void Pmu_Help (GtkButton *but, gpointer udata);			          */
/*	7. void validation_result (char *msg);					               */
/*	8. void pmu_colors();							                    */
/*	9. int  validation_pmu_server (GtkButton *button, gpointer udata);	     */
/*	10. void pmu_server ();							                    */
/*	11. void manage_data_sources();						               */
/*	12. void Set_data_source(GtkWidget *widget, gpointer udata);		     */
/*	13. void file_ok_sel(GtkWidget *w, GtkFileSelection *fs);		          */
/*   	14. void pmu_setup_file_selection(GtkWidget *widget, gpointer udata);      */
/*   	15. void view_setup_file (GtkWidget *w, GtkFileSelection *fs);             */
/*   	16. void apply_pmu_setup (GtkWidget *w, gpointer udata);                   */
/*   17. void* display_time();                                                       */
/*                                                                              */
/* ---------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  isNumber(char *s):                                                 */
/* It checks the passing character is a number or not, return 0 if its a 	     */
/* otherwise returns 1.								                    */
/* ----------------------------------------------------------------------------	*/

int isNumber(char *s)
{
	int dot=0, num=0, end=0;

	while(isspace(*s))s++;
	if(! *s)return 0;
	while(*s)
		if(isspace(*s))
			end=1, s++;
		else if(end)
			return 0;
		else if(isdigit(*s))
			num=1, s++;
		else if(*s=='.')
			if(dot)
				return 0;  
			else
				dot=1,s++;        
		else
			return 0;
	return num;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  checkip(char ip[])		               					*/			
/* It checks the IP address of passing argument with IP standards.    		*/
/* Returns 0 in case of correct otherwise 1.					               */
/* ----------------------------------------------------------------------------	*/

int checkip(char ip[])
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip, &(sa.sin_addr));

	return result;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  destroy (GtkWidget *widget, gpointer udata)         			*/
/* It Destroy(& close) the PMU Simulator & his both processes.             	*/	
/* ----------------------------------------------------------------------------	*/

void destroy (GtkWidget *widget, gpointer udata)
{
     kill(pidLocal, SIGKILL);

	gtk_main_quit();

	g_print ("PMU Simulator Closing Now.\n");
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  GdkPixbuf *create_pixbuf(const gchar * filename)                   */
/* function creates a new pixbuf by loading an image from a file.               */
/* The file format is detected automatically. If NULL, error will be set.       */
/* ----------------------------------------------------------------------------	*/

GdkPixbuf *create_pixbuf(const gchar * filename)
{
     GdkPixbuf *pixbuf;
     GError *error = NULL;

     pixbuf = gdk_pixbuf_new_from_file(filename, &error);

     if(!pixbuf) 
     {
          fprintf(stderr, "%s\n", error->message);
          g_error_free(error);
     }

   return pixbuf;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  about_pmu (GtkButton *widget, gpointer udata)       			*/
/* It display the information about PMU Simulator Software like authors,   	*/
/* licensing, features, usage, etc.                                             */
/* ----------------------------------------------------------------------------	*/

void about_pmu (GtkButton *widget, gpointer udata)
{
	/* Create a about dialog for displaying about the iPDC. */
	about_dialog = gtk_about_dialog_new ();

	const gchar *auth[] = {"\nDr. A.M. Kulkarni <http://www.ee.iitb.ac.in/wiki/faculty/anil>", "\nNitesh Pandit <panditnitesh@gmail.com>", "\nKedar V. Khandeparkar <kedar.khandeparkar@gmail.com>", "\nGopal Gajjar <gopal@ee.iitb.ac.in>", NULL};

	/* Define the diffrent properties of about_dialog. */
	gtk_about_dialog_set_program_name ((GtkAboutDialog *)about_dialog, " ");
	gtk_about_dialog_set_comments ((GtkAboutDialog *)about_dialog, "Authors :\nNitesh Pandit\nKedar Khandeparkar");//\n\nWide Aera Measurement System\nDepartment of Electrical Engineering\nIndian Institute of Technology, Bombay\nMumbai, INDIA");

	gtk_about_dialog_set_license ((GtkAboutDialog *)about_dialog, "iPDC - A Phasor Data Concentrator & PMU Simulator\n\nGeneral Public License (GPL)\n\nAuthors:\n\t\tNitesh Pandit <panditnitesh@gmail.com>\n\t\tKedar V. Khandeparkar <kedar.khandeparkar@gmail.com>\n\n\nThis program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n");

     gtk_about_dialog_set_logo ((GtkAboutDialog *)about_dialog, create_pixbuf("/usr/local/share/PMU/PmuLogo1.png"));
	gtk_about_dialog_set_website ((GtkAboutDialog *)about_dialog, "\nhttp://ipdc.codeplex.com/\n");
	gtk_about_dialog_set_copyright ((GtkAboutDialog *)about_dialog, "Copyright (C) 2011-2012 GPL3");
	gtk_about_dialog_set_authors ((GtkAboutDialog *)about_dialog, auth);

	/* Ensure that the dialog box is destroyed when the user responds */
	g_signal_connect_swapped (about_dialog, "response", G_CALLBACK (gtk_widget_destroy), about_dialog);

	/* Finally show the about_dialog. */
	gtk_widget_show_all (about_dialog);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  Pmu_Help (GtkButton *but, gpointer udata)				          */
/* It shows the general guidelines to user to fill the entries for connecting,	*/
/* or disconnecting devices in PMU Simulator Software.				          */
/* ----------------------------------------------------------------------------	*/

void Pmu_Help (GtkButton *but, gpointer udata)
{
	/* Create the widgets */
	help_dialog = gtk_about_dialog_new ();

	gtk_about_dialog_set_program_name ((GtkAboutDialog *)help_dialog, "PMU HELP\n");
	gtk_about_dialog_set_comments ((GtkAboutDialog *)help_dialog, "1) ID Code should be integer i.e. 50, 60, etc.\t\t\t\n\n2) Port should be greater than 1500 and less than 65535.\t\n\n3) Channel Name could be upto 16 characters.\t\t\t\n\n4) Enter the integer value for phasors, analogs and digitals.\n\n5) Fixed/Floating point and Rectangular/Polar are the\t\trepresentation of measurements.");

	/* Ensure that the dialog box is destroyed when the user responds */
	g_signal_connect_swapped (help_dialog, "response", G_CALLBACK (gtk_widget_destroy), help_dialog);
	gtk_widget_show_all (help_dialog);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_result (char *msg)					               */
/* It displays the status message of performed operation.			          */
/* ----------------------------------------------------------------------------	*/

void validation_result (char *msg)
{
	GtkWidget *val_dialog;

	/* Create the widgets a new dialog box */
	val_dialog = gtk_about_dialog_new ();

	/* Define the diffrent properties of val_dialog. */
	gtk_about_dialog_set_program_name ((GtkAboutDialog *)val_dialog, " ");
	gtk_about_dialog_set_comments ((GtkAboutDialog *)val_dialog, msg);

	/* Ensure that the dialog box is destroyed when the user responds */
	g_signal_connect_swapped (val_dialog, "response", G_CALLBACK (gtk_widget_destroy), val_dialog);

	/* Finally show the val_dialog. */
	gtk_widget_show_all (val_dialog);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  pmu_colors()							                    */
/* It fills the colors in text on lables and set the images on given buttons. 	*/
/* It called in main only one time to initialized all the parameters.		     */
/* ----------------------------------------------------------------------------	*/

void pmu_colors()
{
  	GtkTextBuffer *buffer, *buffer1;
  	GtkTextMark *cursor, *cursor1;
  	GtkTextIter iter, iter1;

	/* Set the font and color for the heading on 'welcome_pmu' lable, and others */
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='10'><b>Admin Operations</b></span>");
	gtk_label_set_markup (GTK_LABEL (pmu_data->admin_label), markup);
	g_free (markup);

	markup = g_markup_printf_escaped ("<span foreground=\"#330000\" font='8'>@All Rights Reserved</span>");
	gtk_label_set_markup (GTK_LABEL (pmu_data->rights_lable), markup);
	g_free (markup);

	/* Set the general image on a textview */
  	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (pmu_data->text_view));
  	cursor = gtk_text_buffer_get_insert (buffer);
  	gtk_text_buffer_get_iter_at_mark (buffer, &iter, cursor);
  	gtk_text_buffer_insert_pixbuf (buffer, &iter, create_pixbuf("/usr/local/share/PMU/PSLab.PNG"));
     	gtk_widget_show (pmu_data->text_view);

	/* Set the image (PMU-logo) on a specific textview1 */
  	buffer1 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (pmu_data->text_view1));
  	cursor1 = gtk_text_buffer_get_insert (buffer1);
  	gtk_text_buffer_get_iter_at_mark (buffer1, &iter1, cursor1);
  	gtk_text_buffer_insert_pixbuf (buffer1, &iter1, create_pixbuf("/usr/local/share/PMU/PmuLogo.png"));
     	gtk_widget_show (pmu_data->text_view1);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_pmu_server (GtkButton *button, gpointer udata)		*/
/* It Validates the user enterd details for PMU Simulator. It gives the		*/
/* successful message and go ahead with entered data if all entries		     */
/* were right, else it shows the validation error message to user.		     */
/* ----------------------------------------------------------------------------	*/

int validation_pmu_server (GtkButton *button, gpointer udata)
{
	/* local variables */
	int tmp_p;
	char *msg;
	const char *text1, *text2;

	/* Get the text entries filled by user */
	text1 = gtk_entry_get_text(GTK_ENTRY(udp_port));
	text2 = gtk_entry_get_text(GTK_ENTRY(tcp_port));

	if(!isNumber((char *)text1) || !(atoi(text1) > 1500 && atoi(text1) < 65535) || (atoi(text2) == 9000)) 	
	{ 
		msg = " Not a valid UDP port number, must not be 9000! Try again ";
		validation_result (msg);		/* Show the failure message to user */
		return 0;
	}
	if(!isNumber((char *)text2) || !(atoi(text2) > 1500 && atoi(text2) < 65535) || (atoi(text2) == 9000)) 
	{ 
		msg = " Not a valid TCP port number, must not be 9000! Try again ";
		validation_result (msg);		/* Show the failure message to user */
		return 0;
	}
	if(atoi(text1) == atoi(text2)) 	/* Check for valid port number */		
	{ 
		msg = " UDP and TCP ports cannot be identical! Try again ";
		validation_result (msg);		/* Show the failure message to user */
		return 0;
	}
	else
	{
		PMU_uport = atoi(text1);
		PMU_tport = atoi(text2);

		p1 = *ShmPTR;
		p1.cfg_bit_change_info = 0;
		ShmPTR->cfg_bit_change_info = 0;
		ShmPTR->UdpPort = atoi(text1);
		ShmPTR->TcpPort = atoi(text2);

		p1.pid = ShmPTR->pid;
		kill(p1.pid, SIGUSR2);
		memset(pmuFilePath, '\0', 200);
		printf("UDP and TCP ports value has been sent to PMU Server through signal.\n");

		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->create_cfg_button), TRUE);					
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menuitem2), TRUE);					
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_setup_cfg), TRUE);					

		gtk_widget_set_visible(GTK_WIDGET(pmu_data->start_server), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->open_cfg), FALSE);

		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_data_source), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_header_frm), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_cfg_modify), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_stat_modify), FALSE);

		/* Close/destroy the pmu_server_window */
		gtk_widget_destroy(pmu_server_window);

		cfg_create_function (NULL, "PMU Configuration Setup");
		return 1;
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  pmu_server ()							                    */
/* It displays the windows for entring PMU Server details to user.		     */
/* ----------------------------------------------------------------------------	*/

void pmu_server ()
{
	/* local variables */
	GtkWidget *table, *label, *cancel_button;
	GtkWidget *valdbutton, *help_button;

	/* Create a new dialog window for PMU Server Setup */
	pmu_server_window = gtk_dialog_new ();
	g_signal_connect (pmu_server_window, "destroy", G_CALLBACK (gtk_widget_destroy), pmu_server_window);
	gtk_window_set_title (GTK_WINDOW (pmu_server_window), "PMU Server Setup");
	gtk_container_set_border_width (GTK_CONTAINER (pmu_server_window), 10);

	/* Create a table of 4 by 2 squares. */
	table = gtk_table_new (6, 2, FALSE);

	/* Set the spacing to 50 on x and 5 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(pmu_server_window)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	/* Add few buttons to the bottom of the dialog */
	valdbutton = gtk_button_new_with_label ("Run");
	help_button = gtk_button_new_with_label ("Help");
	cancel_button = gtk_button_new_with_label ("Cancel");

	/* This simply creates a grid of toggle buttons on the table */
	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='12'><b>Enter PMU Server Details</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);
	g_free (markup);

	label = gtk_label_new ("UDP Port");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 2, 3);
	gtk_widget_show (label);

	label = gtk_label_new ("TCP Port");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 3, 4);
	gtk_widget_show (label);

	/* Create text boxes for user to enter appropriate values */
	udp_port = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)udp_port, 5);
	gtk_table_attach_defaults (GTK_TABLE (table), udp_port, 1, 2, 2, 3);
	gtk_widget_show (udp_port);

	/* Create combo boxe for user with some fixed values */
	tcp_port = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)tcp_port, 5);
	gtk_table_attach_defaults (GTK_TABLE (table), tcp_port, 1, 2, 3, 4);
	gtk_widget_show (tcp_port);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Use the unreserved ports for PMU Server.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 5, 6);
	gtk_widget_show (label);
	g_free (markup);

	/* Signal handling for buttons on PMU Server Setup Window */
	g_signal_connect_swapped (valdbutton, "clicked", G_CALLBACK (validation_pmu_server), valdbutton);
	g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (Pmu_Help), NULL);
	g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), pmu_server_window);
	g_signal_connect_swapped (pmu_server_window, "response", G_CALLBACK (gtk_widget_destroy), pmu_server_window);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (valdbutton, TRUE);
	gtk_widget_set_can_default (help_button, TRUE);
	gtk_widget_set_can_default (cancel_button, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (pmu_server_window)->action_area), valdbutton, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (pmu_server_window)->action_area), help_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (pmu_server_window)->action_area), cancel_button, TRUE, TRUE, 0);	    

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (valdbutton);
	gtk_widget_show (valdbutton);
	gtk_widget_grab_default (help_button);
	gtk_widget_show (help_button);
	gtk_widget_grab_default (cancel_button);
	gtk_widget_show (cancel_button);

	/* Finally show the pmu_server_window */
	gtk_widget_show (pmu_server_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  manage_data_sources()						                    */
/* It displays the option window to user for selection of data/measurement      */
/* source. Either auto generated or from measurement file.                      */
/* ----------------------------------------------------------------------------	*/

void manage_data_sources()
{
	/* local variables */
	GtkWidget *table;
	GtkWidget *close_butn, *label;
	GtkWidget *Addbutton, *Removebutton;

	/* Create a new dialog window for the Option Window */ 
	data_managing_window = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (data_managing_window), "Select Data Source");
	gtk_container_set_border_width (GTK_CONTAINER (data_managing_window), 10);

	/* Create a table of 4 by 2 squares */
	table = gtk_table_new (6, 2, FALSE);

	/* Set the spacing to 15 on x and 25 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(data_managing_window)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='12'><b>Select Measurement Source</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);
	g_free (markup);

	/* Add few radio buttons on the dialog window */
	Addbutton = gtk_radio_button_new_with_label (NULL, "Auto Genetared");
	gtk_table_attach_defaults (GTK_TABLE (table), Addbutton, 0, 1, 2, 3);
     if (ShmPTR->dataFileVar == 0 )
     {
          gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (Addbutton), TRUE);
     }
     gtk_widget_show (Addbutton);

	Removebutton = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (Addbutton), "Measurement File");
	gtk_table_attach_defaults (GTK_TABLE (table), Removebutton, 0, 1, 3, 4);
     if (ShmPTR->dataFileVar == 1 )
     {
          gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (Removebutton), TRUE);
     }
	gtk_widget_show (Removebutton);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Auto Genetared Data may be raw measurements.\nMay use CSV file of Phasor measurements.\nFor CSV file format see our Sample measurements files.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 5, 6);
	gtk_widget_show (label);
	g_free (markup);

	/* Add 'Close' buttons on the dialog window */
	close_butn = gtk_button_new_with_label ("Close");

	/* Signal handling for buttons on Option Window */
	g_signal_connect (Addbutton, "toggled", G_CALLBACK (Set_data_source), (gpointer) "1");
	g_signal_connect (Removebutton, "toggled", G_CALLBACK (Set_data_source), (gpointer) "2");
	g_signal_connect_swapped (close_butn, "clicked", G_CALLBACK (gtk_widget_destroy), data_managing_window);
	g_signal_connect_swapped (data_managing_window, "response", G_CALLBACK (gtk_widget_destroy), data_managing_window);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (close_butn, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (data_managing_window)->action_area), close_butn, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (close_butn);
	gtk_widget_show (close_butn);

	/* Finally show the data_managing_window */
	gtk_widget_show (data_managing_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  Set_data_source(GtkWidget *widget, gpointer udata)			     */
/* It will show user the dialog box that have browser of his system to		*/
/* choose the file for measurements reading, otherwise just raw data.		     */
/* ----------------------------------------------------------------------------	*/

void Set_data_source(GtkRadioButton *radiobutton, gpointer udata)
{
    char *file_path;
    file_path = malloc(200*sizeof(char));
    strcpy(file_path, getenv("HOME"));
    strcat(file_path,"/iPDC/DataDir/");

    if(strcmp("Auto Genetared", gtk_button_get_label(GTK_BUTTON(radiobutton))))
    {
        strcpy(ShmPTR->filePath, "nil");
        ShmPTR->dataFileVar = 0;
        p1.pid = ShmPTR->pid;

        kill(p1.pid, SIGUSR1);
        printf("Raw Data selection is new choice of user, sent to the PMU Server by signal.\n");

        gtk_widget_destroy(data_managing_window);
    }   
    else
    {
        /* Create a new file selection widget */
        gtk_widget_destroy(data_managing_window);      

        data_managing_window = gtk_file_selection_new ("PMU Simulator measurement file selection");

        /* Connect the ok_button to file_ok_sel function */
        gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (data_managing_window)->ok_button), "clicked", (GtkSignalFunc) file_ok_sel, data_managing_window );

        /* Connect the cancel_button to destroy the widget */
        gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (data_managing_window)->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT (data_managing_window));

        /* Lets set the filename, as if this were a save dialog, and we are giving a default filename */
        gtk_file_selection_set_filename (GTK_FILE_SELECTION(data_managing_window), file_path);

        gtk_widget_show(data_managing_window);
    }
}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  file_ok_sel(GtkWidget *w, GtkFileSelection *fs)			     */
/* This function gets the user file and check whether it has proper number      */
/* of columns or not, if yes then sent signal to server.	                    */
/* ----------------------------------------------------------------------------	*/

void file_ok_sel(GtkWidget *w, GtkFileSelection *fs)
{
     /* Local Variables */
	int total_column, count;
	char *l1, *d3, *d4;

	size_t l2 = 0;
	FILE *temp_fp;
	ssize_t result;

	char *s = (char *)gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs));

     /* 3 for UNIX time stamp, FREQ & DFREQ */
	total_column = cfg_info->cfg_phnmr_val*2 + cfg_info->cfg_annmr_val + 3; 

     /* Open the user given measurements in read mode */
	temp_fp = fopen(s, "r");

	if (temp_fp == NULL)
	{
		validation_result(" Measurement File doesn't exist! ");
	}
	else 
	{
		if ((result = getdelim(&l1, &l2, ('\n'), temp_fp)) >0)
		{
			d3 = strtok (l1, ","); 

			for(count=1; ; count++) //count<=total_column
			{
				if((d4 = strtok (NULL,",")) == NULL)
					break;
			}

			if(count == total_column)
			{
				strcpy(ShmPTR->filePath, s);
				ShmPTR->dataFileVar = 1;
				p1.pid = ShmPTR->pid;

				kill(p1.pid, SIGUSR1);

				fclose(temp_fp);
				gtk_widget_destroy(data_managing_window);

				printf("\nInform PMU Server that Data Source has been changed to measurement file by signal.\n");
				validation_result(" Now measurements are taken from measurement file. ");
			}
			else
			{
				fclose(temp_fp);
				validation_result("File data does not match with configuration!\nPlease Select a proper file as per your configuration.");
			}
		}
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  pmu_setup_file_selection(GtkWidget *widget, gpointer udata)	*/
/* It will display a file selector window to the user.	User have the          	*/
/* responsibility to choose the correct previously saved file for PMU Setup.	*/
/* ----------------------------------------------------------------------------	*/

void pmu_setup_file_selection(GtkWidget *widget, gpointer udata)
{
    	char  *file_path;
    	file_path = malloc( 200*sizeof(char));
    	memset(file_path,'\0',200);
    	strcpy(file_path,getenv("HOME"));
    	strcat(file_path,"/iPDC/PMU/");

	/* Create a new file selection widget */
	pmu_setup_window = gtk_file_selection_new ("PMU Setup File Selector");

	/* Connect the ok_button to view_setup_file function */
	gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (pmu_setup_window)->ok_button), "clicked", (GtkSignalFunc) view_setup_file, pmu_setup_window);

	/* Connect the cancel_button to destroy the widget */
	gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (pmu_setup_window)->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy,
			GTK_OBJECT (pmu_setup_window));

	/* Lets set the filename, as if this were a save dialog, and we are giving a default filename */
	gtk_file_selection_set_filename (GTK_FILE_SELECTION(pmu_setup_window), file_path);

	gtk_widget_show(pmu_setup_window);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  view_setup_file(GtkWidget *w, GtkFileSelection *fs)                */
/* This function displays the different attributes of selected PMU Setup File.  */
/* It will show the PMU Server & CFG details before apply.                      */
/* ----------------------------------------------------------------------------	*/

void view_setup_file (GtkWidget *w, GtkFileSelection *fs)
{
	/* local variables */
	GtkWidget *table, *label;
	GtkWidget *apply_button, *cancel_button;

	int tempi, i;
	char stn[17], *s,*rline = NULL;
	char *d1, buff[15], *markup;
	unsigned int framesize;
	unsigned char *line, tempC[2];
	size_t len = 0;
	ssize_t read;
	FILE *fp1;

	s = (char *)gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs));
	strcpy(fptr, s);

	gtk_widget_destroy(pmu_setup_window);

	fp1 = fopen(s, "rb");

	if (fp1 == NULL)
	{
		validation_result(" PMU Setup file doesn't exist! Try again ");
	}
	else 
	{
		/* If its available, then create a new dialog window for displaying the iPDC details. */
		setup_display_window = gtk_dialog_new ();
		gtk_window_set_title (GTK_WINDOW (setup_display_window), "PMU Simulator Setup");
		gtk_container_set_border_width (GTK_CONTAINER (setup_display_window), 10);

		/* Create a table of ? by 2 squares */
		table = gtk_table_new (17, 2, FALSE);

		/* set the spacing to 10 on x and 10 on y */
		gtk_table_set_row_spacings (GTK_TABLE (table), 8);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		gtk_box_pack_start (GTK_BOX (GTK_DIALOG(setup_display_window)->vbox), table, TRUE, TRUE, 0);
		gtk_widget_show (table);

		/* Add a "close" button to the bottom of the dialog */
		apply_button = gtk_button_new_with_label ("Apply");
		cancel_button = gtk_button_new_with_label ("cancel");

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#7D2252\" font='12'><b>PMU Setup File Details</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
		gtk_widget_show (label);
		g_free (markup);

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#0000FF\" font='12'>Server Info</span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 2, 3);
		gtk_widget_show (label);
		g_free (markup);

		label = gtk_label_new ("UDP Port");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 3, 4);
		gtk_widget_show (label);

		label = gtk_label_new ("TCP Port");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 4, 5);
		gtk_widget_show (label);

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#0000FF\" font='12'>Configuration Info</span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 6, 7);
		gtk_widget_show (label);
		g_free (markup);

		label = gtk_label_new ("PMU ID");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 7, 8);
		gtk_widget_show (label);

		label = gtk_label_new ("Station Name");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 8, 9);
		gtk_widget_show (label);

		label = gtk_label_new ("Pasor Number");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 9, 10);
		gtk_widget_show (label);

		label = gtk_label_new ("Analog Number");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 10, 11);
		gtk_widget_show (label);

		label = gtk_label_new ("Digital Number");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 11, 12);
		gtk_widget_show (label);

		label = gtk_label_new ("Format Word");
	     gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 12, 13);
		gtk_widget_show (label);

		label = gtk_label_new ("CFG Count");
	     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 13, 14);
		gtk_widget_show (label);

		label = gtk_label_new ("Data Rate ");
	     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 14, 15);
		gtk_widget_show (label);

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Configuration information of selected PMU Setup file.</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
	     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 16, 17);
		gtk_widget_show (label);
		g_free (markup);

		tempi = 1;
		while(tempi < 4)
		{
			read = getline(&rline, &len, fp1);

			if(read == 0)
				break;

			if(tempi == 1)
			{
				label = gtk_label_new (rline);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 3, 4);
				gtk_widget_show (label);
			}
			else if(tempi == 2)
			{
				label = gtk_label_new (rline);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 4, 5);
				gtk_widget_show (label);
			}
			tempi++;
		}

		if(read > 0)
		{
			d1 = strtok (rline," ");
			d1 = strtok (NULL," ");
			tempi = atoi(d1);

			if (tempi == 1)
			{
				d1 = strtok (NULL,"\n");
				framesize = atoi(d1);

				line = malloc(framesize*sizeof(unsigned char));
				fread(line, sizeof(unsigned char), framesize, fp1);

				tempC[0] = line[4];
				tempC[1] = line[5];

	               tempi = tempC[0];
	              	tempi<<=8;
	              	tempi |=tempC[1];

				sprintf(buff, "%d", tempi);
				label = gtk_label_new (buff);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 7, 8);
				gtk_widget_show (label);

				for(i=0; i<16; i++)
				{
					stn[i] = line[20+i];
				}
				stn[16] = '\0';

				label = gtk_label_new (stn);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 8, 9);
				gtk_widget_show (label);

				tempC[0] = line[38];
				tempC[1] = line[39];
				tempi = c2i(tempC);

				memset(buff, '\0', 15);
				sprintf(buff, "%d", tempi);
				label = gtk_label_new (buff);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 12, 13);
				gtk_widget_show (label);

				tempC[0] = line[40];
				tempC[1] = line[41];
				tempi = c2i(tempC);

				memset(buff, '\0', 15);
				sprintf(buff, "%d", tempi);
				label = gtk_label_new (buff);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 9, 10);
				gtk_widget_show (label);

				tempC[0] = line[42];
				tempC[1] = line[43];
				tempi = c2i(tempC);

				memset(buff, '\0', 15);
				sprintf(buff, "%d", tempi);
				label = gtk_label_new (buff);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 10, 11);
				gtk_widget_show (label);

				tempC[0] = line[44];
				tempC[1] = line[45];
				tempi = c2i(tempC);

				memset(buff, '\0', 15);
				sprintf(buff, "%d", tempi);
				label = gtk_label_new (buff);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 11, 12);
				gtk_widget_show (label);

				tempC[0] = line[framesize-6];
				tempC[1] = line[framesize-5];
				tempi = c2i(tempC);

				memset(buff, '\0', 15);
				sprintf(buff, "%d", tempi);
				label = gtk_label_new (buff);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 13, 14);
				gtk_widget_show (label);

				tempC[0] = line[framesize-4];
				tempC[1] = line[framesize-3];
				tempi = c2i(tempC);

				memset(buff, '\0', 15);
				sprintf(buff, "%d", tempi);
				label = gtk_label_new (buff);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 14, 15);
				gtk_widget_show (label);
			}
		}
		fclose(fp1);

		/* Define signal on ok_button when it "clicked". */
		g_signal_connect_swapped (apply_button, "clicked", G_CALLBACK (apply_pmu_setup), setup_display_window);
		g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), setup_display_window);
		g_signal_connect (setup_display_window, "destroy", G_CALLBACK (gtk_widget_destroy), setup_display_window);

		/* This makes it so the ok_button is the default. */
		gtk_widget_set_can_default (cancel_button, TRUE);
		gtk_widget_set_can_default (apply_button, TRUE);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (setup_display_window)->action_area), apply_button, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (setup_display_window)->action_area), cancel_button, TRUE, TRUE, 0);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (cancel_button);
		gtk_widget_show (cancel_button);
		gtk_widget_grab_default (apply_button);
		gtk_widget_show (apply_button);

		/* Finally show the setup_display_window. */
		gtk_widget_show (setup_display_window);
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  apply_pmu_setup (GtkWidget *w, gpointer udata)			*/
/* This function apply the user selected PMU Setup File and go ahead with the 	*/
/* Setup file's information.                                                    */
/* ----------------------------------------------------------------------------	*/

void apply_pmu_setup (GtkWidget *w, gpointer udata)
{
	/* local variables */
	int  tmp_p, tempi;
	int tmudp_port, tmudp_port1;
	char *d1, *rline=NULL;

	size_t len = 0;
	FILE *fp1;
	ssize_t read;

	gtk_widget_destroy(setup_display_window);

	/* Open the saved PMU Setup File and read the informations */
	fp1 = fopen (fptr,"rb");

	if (fp1 != NULL)			
	{
		tempi = 1;

          /* To get the PmuServer details from the setup file */
		while(tempi < 3)
		{
			read = getline(&rline, &len, fp1);

			if(read == 0)
				break;

			if(tempi == 1)
			{
				PMU_uport = atoi(rline);
				tmudp_port = PMU_uport;
			}
			else
			{
				d1 = strtok (rline,"\n");
				PMU_tport = atoi(rline);
				tmudp_port1 = PMU_tport;
			}
			tempi++;
		}

          /* Make this file as a common file to operate on */
		memset(pmuFilePath,'\0',sizeof(pmuFilePath));
		strcpy(pmuFilePath, fptr);
		fclose(fp1);

		if(read > 0)
		{
               /* Finally for starting the PMU Server */
			p1 = *ShmPTR;
			p1.cfg_bit_change_info = 0;
			ShmPTR->cfg_bit_change_info = 0;
			ShmPTR->UdpPort = tmudp_port;
			ShmPTR->TcpPort = tmudp_port1;

			PMU_uport = tmudp_port;
			PMU_tport = tmudp_port1;

			p1.pid = ShmPTR->pid;

			kill(p1.pid, SIGUSR2);
			printf("UDP Port = %d & TCP Port = %d, info sent to PMU Server by signal.\n", PMU_uport, PMU_tport);

			gtk_widget_set_visible(GTK_WIDGET(pmu_data->menuitem2), TRUE);					
			gtk_widget_set_visible(GTK_WIDGET(pmu_data->pmu_properties), TRUE);					

			gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_setup_cfg), FALSE);
			gtk_widget_set_visible(GTK_WIDGET(pmu_data->start_server), FALSE);
			gtk_widget_set_visible(GTK_WIDGET(pmu_data->open_cfg), FALSE);

               /* Finally creating the CFG objects */
			cfg_create_call();
		}
		else
		{
			validation_result(" Not a PMU Setup File. Try again! ");
		}
	}
	else 
	{
		validation_result(" PMU Setup File doesn't exist! ");
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  display_time()                                                     */
/* Display the time on time lable on main PMU Simulator window.                 */
/* ----------------------------------------------------------------------------	*/

void* display_time()
{
     struct timeval tv;
     struct tm* ptm;
     char time_string[40];

     while(1)
     {
          /* Obtain the time of day, and convert it to a tm struct. */
          gettimeofday (&tv, NULL);
      
          /* Format the date and time, down to a single second. */
          memset(time_string, '\0', 40);
          ptm = localtime (&tv.tv_sec);
          strftime (time_string, sizeof (time_string), "%Y-%m-%d, %H:%M:%S", ptm);

          gtk_label_set_text(GTK_LABEL(pmu_data->time_lable), time_string);
          sleep(1);          
     }

     /* Exit the display time thread at the time of PMU Closing */
     pthread_exit(NULL);
}

/**************************************** End of File *******************************************************/
