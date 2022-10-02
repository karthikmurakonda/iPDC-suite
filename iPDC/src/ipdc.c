/* ----------------------------------------------------------------------------- 
 * ipdc.c
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


#include <stdio.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h> 
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include  <errno.h>
#include <gtk/gtk.h>
#include "connections.h"
#include "parser.h"
#include "global.h"
#include "ipdcGui.h"
#include "attack_detection.h"
#include "data_vis.h"
#include "utility_tools.h"

/* Common fixed path for storage of few common files */
#define UI_fILE "../assets/iPDC.ui"
//#define UI_fILE "/usr/local/share/iPDC/iPDC.xml"


/* ---------------------------------------------------------------- */
/*                   main program starts here                       */
/* ---------------------------------------------------------------- */

int main(int argc, char **argv)
{
	/* local variables */
	int err;
	char *ptr1;
	struct stat st;

	memset(tname,'\0',20);
	strcpy(tname,"/dev/pts/");

	GtkBuilder *builder;
	GError     *error = NULL;

	PDC_IDCODE = 0;
	system("clear");

	/* Init GTK+ */
	gtk_init(&argc, &argv);

	/* Create new GtkBuilder object */
	builder = gtk_builder_new();

	if(!gtk_builder_add_from_file(builder, UI_fILE, &error))
	{
		g_warning("%s", error->message);
		g_free(error);
		return(1);
	}

	/* Allocate data structure */
	data = g_slice_new(ChData);

	/* Get objects from UI */
	#define GW(name) CH_GET_WIDGET(builder, name, data)
		GW(ipdc);
		GW(add_pmu_button);
		GW(remove_pmu_button);
		GW(cmd_data_off_button);
		GW(cmd_data_on_button);
		GW(cmd_cfg_button);
		GW(add_pdc_button);
		GW(remove_pdc_button);
		GW(display_conn_table_button);
		GW(enter_pdc_detail_menuitem);
		GW(exit_button);
		GW(menubar);
		GW(menuitem2);
		GW(open_ipdc_setup);
		GW(pdc_details_menuitem);
		GW(menu_add_source);
		GW(menu_remove_source);
		GW(menu_data_off);
		GW(menu_data_on);
		GW(menu_request_cfg);
		GW(menu_add_destination);
		GW(menu_remove_destination);
		GW(menu_conn_table);
		GW(about_menuitem);
		GW(exit_menuitem);
		GW(text_view);
		GW(text_view1);
		GW(rights_label);
		GW(admin_label);
		GW(time_lable);
		GW(menuitem3);
		GW(attack_detection);
		GW(data_vis);
		GW(utility_tools);
	#undef GW

	/* Connect signals */
	gtk_builder_connect_signals(builder, data);
	gtk_builder_connect_signals(builder, NULL);

     // Changes how a toplevel window deals with its size request and user resize attempts. 
     gtk_window_set_default_size(GTK_WINDOW(data->ipdc), 800, 600);
	 gtk_window_set_resizable(GTK_WINDOW(data->ipdc), FALSE);
     gtk_window_set_position(GTK_WINDOW(data->ipdc), GTK_WIN_POS_CENTER);
     gtk_window_set_icon(GTK_WINDOW(data->ipdc), create_pixbuf("/usr/local/share/iPDC/logo.png"));

     /* Set the Title of Main Window */
	gtk_window_set_title (GTK_WINDOW (data->ipdc), "iPDC");

	/* Disable all the (mentioned) buttons on main ipdc window */
	gtk_widget_set_sensitive(GTK_WIDGET(data->add_pmu_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->remove_pmu_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_data_off_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_data_on_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->cmd_cfg_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->add_pdc_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->remove_pdc_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(data->display_conn_table_button), FALSE);
	//gtk_widget_set_sensitive(GTK_WIDGET(data->utility_tools), FALSE);

	// gtk_widget_set_visible(GTK_WIDGET(data->menuitem2), TRUE);
	gtk_widget_set_visible(GTK_WIDGET(data->menu_conn_table), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(data->pdc_details_menuitem), FALSE);

	memset(ipdcFilePath, '\0', 200);
     	ptr1 = malloc(200*sizeof(char));
     	memset(ptr1, '\0', 200);

	/* Get the user's name for storing the iPDC Setup File */
	strcpy(ptr1,getenv ("HOME"));

	if (ptr1 == NULL)
	{
		printf("user not found\n");
		exit(1);
	}
	strcat(ptr1, "/iPDC");

	if(stat(ptr1,&st) == 0)	/* If main iPDC folder not present in the system? */
	{
		strcat(ptr1, "/iPDC");

		if(stat(ptr1,&st) == 0)
		{
			strcat(ipdcFolderPath, ptr1);

			FILE * output;
			int check;
			char buff[200], is_empty[100];

			strcpy(buff, "ls ");
			strcat(buff, ipdcFolderPath);
			strcat(buff, " | wc -l");

			output = popen(buff,"r");
			fgets (is_empty, 100, output); //write to the char
			pclose (output);

			check = atoi(is_empty);

			if (check == 0) //No Setup files are present
			{
				gtk_widget_set_visible(GTK_WIDGET(data->open_ipdc_setup), FALSE);

				/* fill the iPDC details */
				fill_pdc_details (NULL);
			}
			else
			{
				char ch[300];

				//ptr1 = getenv ("HOME");
				strcpy(ch, "Fill the iPDC setup details manualy or open the iPDC setup file\nfrom the ");
				//strcat(ch, ptr1);
				//strcat(ch, "/");
				strcat(ch, ipdcFolderPath);

				//validation_result (ch);
			}
		}
		else
		{
			if (mkdir (ptr1, 0700))
			{
				printf ("Cannot create directory `%s': %s\n", ipdcFolderPath, strerror (errno));
				validation_result ("Cannot create iPDC directory on proposed path! ");
			}
			else
			{
				strcat(ipdcFolderPath, ptr1);
				gtk_widget_set_visible(GTK_WIDGET(data->open_ipdc_setup), FALSE);

				/* fill the iPDC details */
				fill_pdc_details (NULL);
			}
		}
	}
	else
	{
		if (mkdir (ptr1, 0700))
		{
			printf ("Cannot create directory `%s': %s", ipdcFolderPath, strerror (errno));
			validation_result ("Cannot create iPDC directory on proposed path! ");
		}
		else
		{
			strcat(ptr1, "/iPDC");

			if (mkdir (ptr1, 0700))
			{
				printf ("Cannot create directory `%s': %s", ipdcFolderPath, strerror (errno));
				validation_result ("Cannot create iPDC sub-directory on proposed path! ");
			}
			else
			{
				strcat(ipdcFolderPath, ptr1);
				gtk_widget_set_visible(GTK_WIDGET(data->open_ipdc_setup), FALSE);

				/* fill the iPDC details */
				fill_pdc_details (NULL);
			}
		}
	}

	/* Decorate the main window of iPDC */
	ipdc_colors();

	/* Signals Definitions*/
	g_signal_connect (data->add_pmu_button, "clicked", G_CALLBACK(add_pmu), NULL);
	g_signal_connect (data->menu_add_source, "activate", G_CALLBACK(add_pmu), NULL);
	g_signal_connect(data->utility_tools, "clicked", G_CALLBACK(utility_tools), NULL);

	g_signal_connect (data->remove_pmu_button, "clicked", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "1");
	g_signal_connect (data->menu_remove_source, "activate", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "1");

	g_signal_connect (data->cmd_data_off_button, "clicked", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "2");
	g_signal_connect (data->menu_data_off, "activate", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "2");

	g_signal_connect (data->cmd_data_on_button, "clicked", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "3");
	g_signal_connect (data->menu_data_on, "activate", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "3");

	g_signal_connect (data->cmd_cfg_button, "clicked", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "4");
	g_signal_connect (data->menu_request_cfg, "activate", G_CALLBACK(cmd_or_remove_pmu), (gpointer) "4");

	g_signal_connect (data->add_pdc_button, "clicked", G_CALLBACK(add_new_pdc), NULL);
	g_signal_connect (data->menu_add_destination, "activate", G_CALLBACK(add_new_pdc), NULL);

	g_signal_connect (data->remove_pdc_button, "clicked", G_CALLBACK(remove_pdc), NULL);
	g_signal_connect (data->menu_remove_destination, "activate", G_CALLBACK(remove_pdc), NULL);

	g_signal_connect (data->display_conn_table_button, "clicked", G_CALLBACK(connection_table), NULL);
	g_signal_connect (data->menu_conn_table, "activate", G_CALLBACK(connection_table), NULL);

	g_signal_connect (data->exit_button, "clicked", G_CALLBACK(destroy), NULL);
	g_signal_connect (data->open_ipdc_setup, "activate", G_CALLBACK(ipdc_setup_fileSelector), NULL);
	g_signal_connect (data->pdc_details_menuitem, "activate", G_CALLBACK(display_pdc_detail), NULL);
	g_signal_connect_swapped (data->enter_pdc_detail_menuitem, "activate", G_CALLBACK(fill_pdc_details), NULL);
	g_signal_connect (data->exit_menuitem, "activate", G_CALLBACK(destroy), NULL);
	g_signal_connect (data->about_menuitem, "activate", G_CALLBACK(about_ipdc), NULL);
	g_signal_connect (data->ipdc, "destroy", G_CALLBACK(destroy), NULL);

    g_signal_connect(data->attack_detection,"activate",G_CALLBACK(attack_detection_window),NULL);
	g_signal_connect(data->data_vis, "activate", G_CALLBACK(data_vis_window), NULL);
	

	/* Destroy builder, since we don't need it anymore */
	g_object_unref(G_OBJECT(builder));

	/* Show window. All other widgets are automatically shown by GtkBuilder */
	gtk_widget_show(data->ipdc);

	pthread_t t;
	if((err = pthread_create(&t,NULL,display_time,NULL))) 
     {
		perror(strerror(err));  
		exit(1);
	}

	/* GTK Main */
	gtk_main();

	pthread_join(UDP_thread, NULL);
	pthread_join(TCP_thread, NULL);
	pthread_join(p_thread, NULL);
//	pthread_join(t, NULL);

	close(UL_UDP_sockfd);
	close(UL_TCP_sockfd);

	/* Free any allocated data */
	g_slice_free(ChData, data);

	return(0);
}

/**************************************** End of File *******************************************************/
