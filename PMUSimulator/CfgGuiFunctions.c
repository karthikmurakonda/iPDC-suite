/* ----------------------------------------------------------------------------- 
 * CfgGuiFunctions.c
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <gtk/gtk.h>
#include "PmuGui.h"
#include "CfgFunction.h"
#include "CfgGuiFunctions.h"
#include "ShearedMemoryStructure.h"

#define PHUNIT_VOLTAGE  (float)1000*100000/32768 
#define PHUNIT_CURRENT  (float)100000/32768 	 
#define ANUNIT_EACH     (float)100000/32768	 

/* -------------------------------------------------------------------------------------- */
/*                 		 Functions in CfgGuiFunctions.c                                  */
/* -------------------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------    */
/*                                                                                 */
/*   	1. int  validation_cfg_create ();                                          */
/*   	2. int  validation_phasor_names(GtkWidget *widget, gpointer udata);        */
/*   	3. int  validation_analog_names(GtkWidget *widget, gpointer udata);        */
/*   	4. int  validation_digital_names(GtkWidget *widget, gpointer udata);       */
/*	5. int  validation_ph_an_num (GtkButton *but, gpointer udata);		   */
/*	6. int  validation_new_phasor_names();					   */
/*	7. int  validation_new_analog_names(GtkWidget *widget, gpointer udata);	   */
/*	8. int  validation_new_data_rate (GtkButton *but, gpointer udata);	   */
/*	9. int  validation_remove_ph_an_num (GtkButton *but, gpointer udata);	   */
/*	10. void validation_hdr_frm (GtkWidget *widget, gpointer udata);	   */
/*	11. int validation_setup_file_name (GtkWidget *widget, gpointer udata);	   */

/*   	12. void cfg_create_function (GtkWidget *widget, gpointer udata);          */
/*   	13. void channel_names_for_phasor ();                                      */
/*   	14. void channel_names_for_analog ();                                      */
/*   	15. void channel_names_for_digital ();                                     */
/*   	16. void final_cfg_create ();                                              */
/*	17. void cfg_create_call();						   */
/*	18. void checkbox_function(GtkWidget *widget, gpointer data);		   */

/*   	19. void stat_chng_options(GtkWidget *widget, gpointer udata);             */
/*   	20. void cfg_STAT_change (GtkWidget *widget, gpointer udata);              */
/*	21. void Change_pmu_configuration(GtkWidget *widget, gpointer udata);      */
/*   	22. void new_phasor_num(GtkWidget *widget, gpointer udata);                */
/*	23. void new_channel_names_for_phasor ();                                  */
/*	24. void new_channel_names_for_analog ();                                  */
/*	25. void change_data_rate_option();                                        */
/*	26. void enter_new_data_rate(GtkWidget *widget, gpointer udata);           */
/*	27. void remove_phasor_num (GtkWidget *widget, gpointer udata);            */
/*	28. int  final_cfg_update_call (GtkButton *but, gpointer udata);           */
/*	29. void hdr_create_function (GtkWidget *widget, gpointer udata);          */
/*                                                                                 */
/* ----------------------------------------------------------------------------    */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_cfg_create ()                                           */
/* It Validates the user enterd details for PMU Simulator Configuration. It     */
/* gives the successful message and go ahead with entered data if all entries   */
/* were right, else it shows the validation error message to user.		*/
/* ----------------------------------------------------------------------------	*/

int validation_cfg_create ()
{
	/* local variables */
	int i, j;
	const char *text, *text1, *text2, *text3, *text4;
	const char *text5, *text6, *text7, *text8, *text9, *text10;
	char *display_msg, stn[16], c = ' ';

	/* Get the text entries  and combo box values filled by user */
	text  = gtk_entry_get_text(GTK_ENTRY(p_id));
	text1 = gtk_entry_get_text(GTK_ENTRY(p_stn));
	text2 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_ff));
	text3 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_af));
	text4 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_pf));
	text5 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_pn));
	text6 = gtk_entry_get_text(GTK_ENTRY(p_phNumber));
	text7 = gtk_entry_get_text(GTK_ENTRY(p_anNumber));
	text8 = gtk_entry_get_text(GTK_ENTRY(p_dgNumber));
	text9 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_drate));
	text10 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_frequency));

	if(!isNumber((char *)text) || !atoi(text) > 0)	/* Check text_box entry for valid integer or not?  */
	{
		display_msg = " Not a valid PMU ID number! Try again ";
		validation_result (display_msg);	/* Show the unsuccess message to user */
		return 0;
	}
	else if(strlen(text1) == 0) 	/* Check text_box entry should not be NULL */		
	{ 
		display_msg = " Please enter a PMU Station name! ";
		validation_result (display_msg);	
		return 0;
	}
	else if(!isNumber((char *)text6) || !atoi(text6) > 0)	/* Check text_box entry for valid integer or not? and greater than 1 */
	{ 
		display_msg = " Not a valid Phasor number! Try again ";
		validation_result (display_msg);	
		return 0;
	}
	else if(!isNumber((char *)text7))		/* Check text_box entry for valid integer or not? */
	{ 
		display_msg = " Not a valid Analog number! Try again ";
		validation_result (display_msg);	
		return 0;
	}
	else if(!isNumber((char *)text8))		/* Check text_box entry for valid integer or not? */
	{ 
		display_msg = " Not a valid Digital number! Try again ";
		validation_result (display_msg);	
		return 0;
	}
	else		/* Only if all the text_box entries were right */
	{
		/* Allocate the memory for the ConfigurationFrame object and assigned couple of fields */
		cfg_info = malloc(sizeof(struct ConfigurationFrame));
		cfg_info->cfg_STNname = malloc(16);
		memset(cfg_info->cfg_STNname,'\0',16);

		memset(stn, '\0', 16);
		strcpy(stn, text1);
		j = strlen(stn);
		for(i = j; i < 16 ; i++)
		{
			stn[i] =  c;
		}

		cfg_info->cfg_pmuID = atoi(text);
		strcpy(cfg_info->cfg_STNname, stn);
		cfg_info->cfg_phnmr_val = atoi(text6);
		cfg_info->cfg_annmr_val = atoi(text7);
		cfg_info->cfg_dgnmr_val = atoi(text8);
		cfg_info->cfg_dataRate = atoi(text9);

		if(!strcmp(text2, "Fix Point"))
			cfg_info->cfg_fdf = 0;
		else
			cfg_info->cfg_fdf = 1;

		if(!strcmp(text3, "Fix Point"))
			cfg_info->cfg_af = 0;
		else
			cfg_info->cfg_af = 1;

		if(!strcmp(text4, "Fix Point"))
			cfg_info->cfg_pf = 0;
		else
			cfg_info->cfg_pf = 1;

		if(!strcmp(text5, "Rectangular"))
			cfg_info->cfg_pn = 0;
		else
			cfg_info->cfg_pn = 1;

		if(atoi(text10) == 50)
			cfg_info->cfg_fnom = 1;
		else
			cfg_info->cfg_fnom = 0;

		/* Close/destroy the cfg_setup_window */
		gtk_widget_destroy(cfg_setup_window);

		cfg_setup_window = NULL;

		/* Next call for Enter Phasor channels */
		channel_names_for_phasor ();

		return 1;
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_phasor_names(GtkWidget *widget, gpointer udata)         */
/* It Validates the user enterd Phasor channel names. It gives the		     */
/* successful message and go ahead with entered data if all entries		     */
/* were right, else it shows the validation error message to user.		     */
/* ----------------------------------------------------------------------------	*/

int validation_phasor_names(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	int ia, i, j, ia1 = 0;
	int k, vtr;
	long int li;
	char stn[16], c = ' ';
	char *display_msg;
	const char *aa[20];
	const char *text1;

	/* First get how many Phasor channel we have? */
	vtr = cfg_info->cfg_phnmr_val+1;
	memset(stn, '\0', 16);	

	/* Allocate the memory for Phasor channel names */
	cfg_info->cfg_phasor_channels = (char *)malloc((vtr*16) * sizeof(char));
	memset(cfg_info->cfg_phasor_channels, '\0', sizeof(cfg_info->cfg_phasor_channels));	

	for (ia=1; ia<vtr; ia++)
	{	
		/* Get the text entry value filled by user */
		aa[ia] = gtk_entry_get_text(GTK_ENTRY(num_text[ia]));

		if(strlen(aa[ia]) == 0)
		{
			free (cfg_info->cfg_phasor_channels);
			display_msg = " Please enter Phasor channel names! ";
			validation_result (display_msg);		
			return 1;
		}			
		else
		{
			memset(stn, '\0', 16);
			strcpy(stn, aa[ia]);
			j = strlen(stn);
			if(j < 17)
			{
				for(i = j; i < 16 ; i++)
				{
					stn[i] =  c;
				}
			}
		}

		for (i=0; i< 16 && i< vtr*16; i++, ia1++) 
		{
			/* Copy all phasor name in an array */
			cfg_info->cfg_phasor_channels[ia1] = stn[i]; 
		}		
	}

	/* Get the Pahsor maximum values at text entry filled by user */
	for(i=1, k=0; i<vtr; i++)
	{	
		if(cfg_info->cfg_pf == 1)
		{
			text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

			if(!strcmp(text1, "Voltage"))
			{
			     /* Fill the phasor factor values for voltage */
				phunit_final_val[k++] = 0;    // 0- Indicating Voltage
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 1;
			}
			else
			{
			     /* Fill the phasor factor values for current */
				phunit_final_val[k++] = 1;    // 1- Indicating Current
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 1;
			}
		}
		else
		{
			text1  = gtk_entry_get_text(GTK_ENTRY(num_text1[i]));

			if(!isNumber((char *)text1))	/* Check text_box entry for valid integer or not?  */
			{
				display_msg = " Not a valid Phasor maximum value! Try again ";
				validation_result (display_msg);	
				return 0;
			}
			else
			{
				li = atol(text1);	

				text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

				if(!strcmp(text1, "Voltage"))
				{
					li = li*PHUNIT_VOLTAGE;

	     		     /* Fill the phasor factor values for voltage */
					phunit_final_val[k++] = 0;
					phunit_final_val[k++] = li>>16;
					phunit_final_val[k++] = li>>8;
					phunit_final_val[k++] = li;
				}
				else
				{
					li = li*PHUNIT_CURRENT;

     			     /* Fill the phasor factor values for current */
					phunit_final_val[k++] = 1;
					phunit_final_val[k++] = li>>16;
					phunit_final_val[k++] = li>>8;
					phunit_final_val[k++] = li;
				}
			}
		}		
	}

	/* Close/destroy the PhasorChWin */
	gtk_widget_destroy(PhasorChWin);			

	/* Next call for Enter Analog channels */
	channel_names_for_analog ();

	return 0;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_analog_names(GtkWidget *widget, gpointer udata)		*/
/* It Validates the user enterd Analog channel names. It gives the    		*/
/* successful message and go ahead with entered data if all entries   		*/
/* were right, else it shows the validation error message to user.	     	*/
/* ----------------------------------------------------------------------------	*/

int validation_analog_names(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	int ia, i, j, ia1 = 0, vtr, k;
	long int li;
	char stn[16], c = ' ';
	char *display_msg;
	const char *aa[20], *text1;

	/* First get how many Analog channel we have? */
	vtr = cfg_info->cfg_annmr_val+1;
	memset(stn, '\0', 16);	

	/* Allocate the memory for Analog channel names */
	cfg_info->cfg_analog_channels = (char *)malloc((vtr*16) * sizeof(char));
	memset(cfg_info->cfg_analog_channels, '\0', sizeof(cfg_info->cfg_analog_channels));	

	for (ia=1; ia<vtr; ia++)
	{	
		/* Get the text entry value filled by user */
		aa[ia] = gtk_entry_get_text(GTK_ENTRY(num_text[ia]));

		if(strlen(aa[ia]) == 0)
		{
			free (cfg_info->cfg_analog_channels);
			display_msg = " Please enter Analog channel names! ";
			validation_result (display_msg);	
			return 1;
		}			
		else
		{
			memset(stn, '\0', 16);
			strcpy(stn, aa[ia]);
			j = strlen(stn);
			if(j < 17)
			{
				for(i = j; i < 16 ; i++)
				{
					stn[i] =  c;
				}
			}
		}

		for (i=0; i< 16 && i< vtr*16; i++, ia1++) 
		{
			/* Copy all phasor name in an array */
			cfg_info->cfg_analog_channels[ia1] = stn[i]; 
		}		
	}

	/* Get the Pahsor maximum values at text entry filled by user */
	for(i=1, k=0; i<vtr; i++)
	{	
		if(cfg_info->cfg_af == 1)
		{
			text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

			if(!strcmp(text1, "PMO"))
			{
			     /* Fill the analog factor values for PMO */
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 1;
			}
			else if(!strcmp(text1, "RMS"))
			{
			     /* Fill the analog factor values for RMS */
				anunit_final_val[k++] = 1;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 1;
			}
			else
			{
			     /* Fill the analog factor values for PEAK */
				anunit_final_val[k++] = 2;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 1;
			}
		}
		else
		{
			text1  = gtk_entry_get_text(GTK_ENTRY(num_text1[i]));

			if(!isNumber((char *)text1))	/* Check text_box entry for valid integer or not?  */
			{
				display_msg = " Not a valid analog maximum value! Try again ";
				validation_result (display_msg);	
				return 0;
			}
			else
			{
				li = atol(text1);	

				text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

				if(!strcmp(text1, "POW"))
				{
					li = li*ANUNIT_EACH;

     			     /* Fill the analog factor values for PMO */
					anunit_final_val[k++] = 0;
					anunit_final_val[k++] = li>>16;
					anunit_final_val[k++] = li>>8;
					anunit_final_val[k++] = li;
				}
				else if(!strcmp(text1, "RMS"))
				{
					li = li*ANUNIT_EACH;

     			     /* Fill the analog factor values for RMS */
					anunit_final_val[k++] = 1;
					anunit_final_val[k++] = li>>16;
					anunit_final_val[k++] = li>>8;
					anunit_final_val[k++] = li;
				}
				else
				{
					li = li*ANUNIT_EACH;

     			     /* Fill the analog factor values for PEAK */
					anunit_final_val[k++] = 2;
					anunit_final_val[k++] = li>>16;
					anunit_final_val[k++] = li>>8;
					anunit_final_val[k++] = li;
				}
			}
		}		
	}

	/* Close/destroy the AnalogChWin */
	gtk_widget_destroy(AnalogChWin);			

	/* Next call for Enter Digital channels */
	channel_names_for_digital ();

	return 0;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_digital_names(GtkWidget *widget, gpointer udata)	     */
/* It Validates the user enterd Digital channel names. It gives the		     */
/* successful message and go ahead with entered data if all entries		     */
/* were right, else it shows the validation error message to user.		     */
/* ---------------------------------------------------------------------------- */

int validation_digital_names(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	int ia, i, j, ia1 = 0, vtr;
	char stn[16], c = ' ';
	char *display_msg;
	const char *aa[20];

	/* First get how many Digital channel we have? */
	vtr = cfg_info->cfg_dgnmr_val*16+1;
	memset(stn, '\0', 16);	

	/* Allocate the memory for Digital channel names */
	cfg_info->cfg_digital_channels = (char *)malloc((vtr*16) * sizeof(char));
	memset(cfg_info->cfg_digital_channels, '\0', sizeof(cfg_info->cfg_digital_channels));	

	for (ia=1; ia<vtr; ia++)
	{	
		/* Get the text entry value filled by user */
		aa[ia] = gtk_entry_get_text(GTK_ENTRY(num_text[ia]));

		if(strlen(aa[ia]) == 0)
		{
			free (cfg_info->cfg_digital_channels);
			display_msg = " Please enter Digital channel names! ";
			validation_result (display_msg);
			return 1;
		}			
		else
		{
			memset(stn, '\0', 16);
			strcpy(stn, aa[ia]);
			j = strlen(stn);
			if(j < 17)
			{
				for(i = j; i < 16 ; i++)
				{
					stn[i] =  c;
				}
			}
		}

		for (i=0; i< 16 && i< vtr*16; i++, ia1++) 
		{
			/* Copy all digital name in an array */
			cfg_info->cfg_digital_channels[ia1] = stn[i]; 
		}		
	}

	/* Close/destroy the AnalogChWin */
	gtk_widget_destroy(AnalogChWin);			

	/* Next call for finaly create Configuration Frame */
	final_cfg_create ();

	return 0;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_ph_an_num (GtkButton *but, gpointer udata)		     */
/* It Validates the user enterd number of phasors (PHNMR) and number of	     */
/* analogs (ANNMR) for new CFG. It will go ahead with entered data if all       */ 
/* entries were right, else it shows the validation error message to user.      */
/* ----------------------------------------------------------------------------	*/

int validation_ph_an_num (GtkButton *but, gpointer udata)
{
	/* local variables */
	const char *text, *text1;

	/* Get the text entry value filled by user */
	text   = gtk_entry_get_text(GTK_ENTRY(phtext));
	text1  = gtk_entry_get_text(GTK_ENTRY(angtext));

	if(!isNumber((char *)text) || !(atoi(text) >= 0))
     	{
		validation_result (" Not a valid Phasor number! Try again ");
	     	return 1;
	}
	if(!isNumber((char *)text1) || !(atoi(text1) >= 0))
     	{
		validation_result (" Not a valid Analog number! Try again ");
	     	return 1;
	}
	else
	{
		/* Next call for Enter New Analog channels */
          	new_cfg_info->new_cfg_annmr_val = atoi(text1); 

	     	/* Next call for Enter New Phasor channels */
	     	new_cfg_info->new_cfg_phnmr_val = atoi(text); 
	     	new_channel_names_for_phasor ();
	     	return 0;
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_new_phasor_names(GtkWidget *widget, gpointer udata)	*/
/* It Validates the New Phasor channel names. It willgo ahead with              */
/* entered data if all entries were right, else it shows the validation         */
/* error message to user.                                                       */
/* ----------------------------------------------------------------------------	*/

int validation_new_phasor_names()
{
	/* local variables */
	int ia, i, j, ia1 = 0, vtr;
	int old_ph_count, k;
	long int li;
	char stn[16], c = ' ';
	char *display_msg;
	const char *aa[20], *text1;

	/* First get how many New Phasor channel he wants to add? */
	vtr = new_cfg_info->new_cfg_phnmr_val+1;
	memset(stn, '\0', 16);	

	/* Allocate the memory for New Phasor channel names */
	new_cfg_info->new_cfg_phasor_channels = (char *)malloc((vtr*16) * sizeof(char));
	memset(new_cfg_info->new_cfg_phasor_channels, '\0', sizeof(new_cfg_info->new_cfg_phasor_channels));	

	for (ia=1; ia<vtr; ia++)
	{	
		/* Get the text entry value filled by user */
		aa[ia] = gtk_entry_get_text(GTK_ENTRY(num_text[ia]));

		if(strlen(aa[ia]) == 0)
		{
			free (new_cfg_info->new_cfg_phasor_channels);
			display_msg = " Please enter new Phasor channel names! ";
			validation_result (display_msg);		
			return 1;
		}			
		else
		{
			memset(stn, '\0', 16);
			strcpy(stn, aa[ia]);
			j = strlen(stn);
			if(j < 17)
			{
				for(i = j; i < 16 ; i++)
				{
					stn[i] =  c;
				}
			}
		}

		for (i=0; ((i< 16) && (i< vtr*16)); i++, ia1++) 
		{
			/* Copy all phasor name in an array */
			new_cfg_info->new_cfg_phasor_channels[ia1] = stn[i]; 
		}		
	}

	/* Get the Pahsor maximum values at text entry filled by user */
	old_ph_count = cfg_info->cfg_phnmr_val;

	for(i=1, k=old_ph_count*4; i<vtr; i++, old_ph_count++)
	{	
		if(cfg_info->cfg_pf == 1)
		{
			text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

			if(!strcmp(text1, "Voltage"))
			{
                    		/* Fill the phasor factor values for voltage */
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 1;
			}
			else
			{
                    		/* Fill the phasor factor values for current */
				phunit_final_val[k++] = 1;
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 0;
				phunit_final_val[k++] = 1;
			}
		}
		else
		{
			text1  = gtk_entry_get_text(GTK_ENTRY(num_text1[i]));

			if(!isNumber((char *)text1))	/* Check text_box entry for valid integer or not?  */
			{
				display_msg = " Not a valid Phasor maximum value! Try again ";
				validation_result (display_msg);	
				return 1;
			}
			else
			{
				li = atol(text1);	

				text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

				if(!strcmp(text1, "Voltage"))
				{
					li = li*PHUNIT_VOLTAGE;

     			     /* Fill the phasor factor values for voltage */
					phunit_final_val[k++] = 0;
					phunit_final_val[k++] = li>>16;
					phunit_final_val[k++] = li>>8;
					phunit_final_val[k++] = li;
				}
				else
				{
					li = li*PHUNIT_CURRENT;

     			     /* Fill the phasor factor values for current */
					phunit_final_val[k++] = 1;
					phunit_final_val[k++] = li>>16;
					phunit_final_val[k++] = li>>8;
					phunit_final_val[k++] = li;
				}
			}
		}		
	}

	return 0;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validate_func(GtkWidget *widget, gpointer udata)			*/
/* It will validate the new phasor and analog names. 				*/
/* ----------------------------------------------------------------------------	*/

int validate_func(GtkWidget *widget, gpointer udata)
{
	if(!validation_new_phasor_names())
	{
		/* Close/destroy the PhasorChWin */
		gtk_widget_destroy(PhasorChWin);

		/* Next call for Enter New Phasor channels */
		new_channel_names_for_analog ();
		return 0;
	}
	return 1;
}

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_new_analog_names(GtkWidget *widget, gpointer udata)	*/
/* It Validates the New Analog channel names. It gives the  successful message	*/
/* and go ahead with entered data if all entries were right, 			*/
/* else it shows the validation error message to user.				*/
/* ----------------------------------------------------------------------------	*/

int validation_new_analog_names(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	int ia, i, j, ia1 = 0;
	int k, vtr, old_an_count;
	long int li;
	char stn[16], c = ' ';
	char *display_msg;
	const char *aa[20], *text1;

	/* First get how many New Analog channel he wants to add? */
	vtr = new_cfg_info->new_cfg_annmr_val+1;
	memset(stn, '\0', 16);	

	/* Allocate the memory for New Analog channel names */
	new_cfg_info->new_cfg_analog_channels = (char *)malloc((vtr*16) * sizeof(char));
	memset(new_cfg_info->new_cfg_analog_channels, '\0', sizeof(new_cfg_info->new_cfg_analog_channels));	

	for (ia=1; ia<vtr; ia++)
	{	
		/* Get the text entry value filled by user */
		aa[ia] = gtk_entry_get_text(GTK_ENTRY(num_text[ia]));

		if(strlen(aa[ia]) == 0)
		{
			free (new_cfg_info->new_cfg_analog_channels);
			display_msg = " Please enter new Analog channel names! ";
			validation_result (display_msg);	
			return 1;
		}			
		else
		{
			memset(stn, '\0', 16);
			strcpy(stn, aa[ia]);
			j = strlen(stn);
			if(j < 17)
			{
				for(i = j; i < 16 ; i++)
				{
					stn[i] =  c;
				}
			}
		}

		for (i=0; i< 16 && i< vtr*16; i++, ia1++) 
		{
			/* Copy all Analog name in an array */
			new_cfg_info->new_cfg_analog_channels[ia1] = stn[i]; 
		}		
	}

	/* Get the Analog maximum values at text entry filled by user */
	old_an_count = cfg_info->cfg_annmr_val;

	for(i=1, k=old_an_count*4; i<vtr; i++, old_an_count++)
	{	
		if(cfg_info->cfg_af == 1)
		{
			text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

			if(!strcmp(text1, "POW"))
			{
                    		/* Fill the analog factor values for POW */
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 1;
			}
			else if(!strcmp(text1, "RMS"))
			{
                   	 	/* Fill the analog factor values for RMS */
				anunit_final_val[k++] = 1;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 1;
			}
			else
			{
                    		/* Fill the analog factor values for PEAK */
				anunit_final_val[k++] = 2;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 0;
				anunit_final_val[k++] = 1;
			}
		}
		else
		{
			text1  = gtk_entry_get_text(GTK_ENTRY(num_text1[i]));

			if(!isNumber((char *)text1))	/* Check text_box entry for valid integer or not?  */
			{
				display_msg = " Not a valid Analog maximum value! Try again ";
				validation_result (display_msg);	
				return 0;
			}
			else
			{
				li = atol(text1);	

				text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(num_combo[i]));

				if(!strcmp(text1, "POW"))
				{
					li = li*ANUNIT_EACH;

                         /* Fill the analog factor values for POW */
					anunit_final_val[k++] = 0;
					anunit_final_val[k++] = li>>16;
					anunit_final_val[k++] = li>>8;
					anunit_final_val[k++] = li;
				}
				if(!strcmp(text1, "RMS"))
				{
					li = li*ANUNIT_EACH;

                         /* Fill the analog factor values for RMS */
					anunit_final_val[k++] = 1;
					anunit_final_val[k++] = li>>16;
					anunit_final_val[k++] = li>>8;
					anunit_final_val[k++] = li;
				}
				else
				{
					li = li*ANUNIT_EACH;

                         /* Fill the analog factor values for PEAK */
					anunit_final_val[k++] = 2;
					anunit_final_val[k++] = li>>16;
					anunit_final_val[k++] = li>>8;
					anunit_final_val[k++] = li;
				}
			}
		}		
	}

	/* Close/destroy the PhasorChWin */
	gtk_widget_destroy(AnalogChWin); 				

	/* Next call for Data Rate change option? */
	change_data_rate_option();

	return 0;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_new_data_rate (GtkButton *but, gpointer udata)		*/
/* It checks whether user have changed the old data rate or continue with it.   */
/* ----------------------------------------------------------------------------	*/

int validation_new_data_rate (GtkButton *but, gpointer udata)
{
	/* Get the combo box value selected by user */
	const char *text  = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_drate));

	new_cfg_info->data_rate_choice = 1; 
	new_cfg_info->new_data_rate = atoi(text); 

	/* Close/destroy the CCWin */
	gtk_widget_destroy(CCWin); 				

	/* Final call to recreate configuration frame */
	reconfig_cfg_CC();

	return 0;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_remove_ph_an_num (GtkButton *but, gpointer udata)	     */
/* It Validates the number of phasor and analog channels for removal in new	*/
/* CFG. It gives the successful message and go ahead with entered data if all	*/
/* entries were right, else it shows the validation error message to user.      */
/* ----------------------------------------------------------------------------	*/

int validation_remove_ph_an_num (GtkButton *but, gpointer udata)
{
	/* local variables */
	const char *text, *text1;

	/* Get the text entry value filled by user */
	text  = gtk_entry_get_text(GTK_ENTRY(phtext));
	text1  = gtk_entry_get_text(GTK_ENTRY(angtext));

     /* Atleast one phasor should be present there in CFG Frame */
	if(!isNumber((char *)text) || !(atoi(text) < cfg_info->cfg_phnmr_val))
     	{
		validation_result (" Not a valid Phasor number within range! Try again ");
	     	return 1;
	}
	if(!isNumber((char *)text1) || !(atoi(text1) <= cfg_info->cfg_annmr_val))
     	{
		validation_result (" Not a valid Analog within range! Try again ");
	     	return 1;
	}
	else
	{
		/* If Phasor & Analog removal? */
		new_cfg_info->new_cfg_phnmr_val = atoi(text); 
		new_cfg_info->new_cfg_annmr_val = atoi(text1); 

		/* Close/destroy the CCWin */
		gtk_widget_destroy(CCWin);

		change_data_rate_option();

		return 0;
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_hdr_frm (GtkWidget *widget, gpointer udata)		     */
/* It Validates the Header frame entered by user. It gives the successful	     */ 
/* message and go ahead with entered data if all entries were right, 		     */
/* else it shows the validation error message to user.				          */
/* ----------------------------------------------------------------------------	*/

void validation_hdr_frm (GtkWidget *widget, gpointer udata)
{
	/* local variables */
	int i, j;
	const char *text;
     GtkTextBuffer *buffer;
     GtkTextIter start;
     GtkTextIter end;
     GtkTextIter iter;

     /* Obtain iters for the start and end of points of the buffer */
     buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (HdrFrm));
     gtk_text_buffer_get_start_iter (buffer, &start);
     gtk_text_buffer_get_end_iter (buffer, &end);

	/* Get the text entry value filled by user */
     text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
	//text = gtk_entry_get_text(GTK_ENTRY(HdrFrm));
	i = strlen(text);

	if(i > 0)
	{
		hdr_frame = (char *)malloc(i * sizeof(char));
		memset(hdr_frame, '\0', sizeof(hdr_frame));	

		for (j=0; j<i ; j++) 
		{
			hdr_frame[j] = text[j]; 
		}

		/* Close/destroy the hdr_frame_window */
		gtk_widget_destroy(hdr_frame_window); 				

		/* Finally call below function to generate header frame */
		header_frm_gen(i);	

		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->header_frm_button), FALSE);
	}
	else
	{
		validation_result (" Pleas enter proper Header Frame! ");	
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  validation_setup_file_name (GtkWidget *widget, gpointer udata)     */
/* Validates the setup file name based on check boxes.                          */
/* ----------------------------------------------------------------------------	*/

int validation_setup_file_name (GtkWidget *widget, gpointer udata)
{
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chkBtn1)) == TRUE)
	{
		gtk_widget_destroy(CCWin);

          	memset(pmuFilePath,'\0', 200);
		cfg_create_call();

		return 1;
	}
	else
	{
		const char *text = gtk_entry_get_text(GTK_ENTRY(p_id));
	
		if(!isNumber((char *)text) || !(atoi(text) > 0) || (cfg_info->cfg_pmuID == atoi(text)))	
		{
			validation_result (" Not a valid PMU ID number! Try again ");
			return 0;
		}
		
		cfg_info->cfg_pmuID = atoi(text);
		gtk_widget_destroy(CCWin);
		final_cfg_create ();		
		return 1;
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  cfg_create_function (GtkWidget *widget, gpointer udata)		     */
/* It shows the configuration window to user with diffrent attributes like, 	*/
/* PMU ID Code, data rate, Station name, PHNMR, FREQ, etc.			          */
/* ----------------------------------------------------------------------------	*/

void cfg_create_function (GtkWidget *widget, gpointer udata)
{
	/* local variables */
	GtkWidget *table, *label, *cancel_button;
	GtkWidget *valdbutton, *help_button;

	/* Create a new dialog window for the PMU Configuration Setup window */
	cfg_setup_window = gtk_dialog_new ();
	g_signal_connect (cfg_setup_window, "destroy", G_CALLBACK (gtk_widget_destroy), cfg_setup_window);
	gtk_window_set_title (GTK_WINDOW (cfg_setup_window), "PMU Configuration Setup");
	gtk_container_set_border_width (GTK_CONTAINER (cfg_setup_window), 10);
	//gtk_widget_set_size_request (cfg_setup_window, 350, 600);

	/* Create a table of 11 by 2 squares. */
	table = gtk_table_new (12, 2, FALSE);

	/* Set the spacing to 10 on x and 25 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 10);
	gtk_table_set_col_spacings (GTK_TABLE (table), 25);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(cfg_setup_window)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	/* Add few buttons to the bottom of the dialog */
	valdbutton = gtk_button_new_with_label ("Next");
	help_button = gtk_button_new_with_label ("Help");
	cancel_button = gtk_button_new_with_label ("Cancel");

	/* This simply creates a grid of toggle buttons on the table */
	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='12'><b>Setup PMU Configuration</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);
	g_free (markup);

	label = gtk_label_new ("PMU ID");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 1, 2);
	gtk_widget_show (label);

	label = gtk_label_new ("Station Name");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 2, 3);
	gtk_widget_show (label);

	label = gtk_label_new ("Frequency Format");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 3, 4);
	gtk_widget_show (label);

	label = gtk_label_new ("Analog Format");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 4, 5);
	gtk_widget_show (label);

	label = gtk_label_new ("Phasor Format");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 5, 6);
	gtk_widget_show (label);

	label = gtk_label_new ("Phasor Notation");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 6, 7);
	gtk_widget_show (label);

	label = gtk_label_new ("Number of Phasors");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 7, 8);
	gtk_widget_show (label);

	label = gtk_label_new ("Number of Analog");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 8, 9);
	gtk_widget_show (label);

	label = gtk_label_new ("Digital Status Word");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 9, 10);
	gtk_widget_show (label);

	label = gtk_label_new ("Frequency");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 10, 11);
	gtk_widget_show (label);

	label = gtk_label_new ("Data Rate");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 11, 12);
	gtk_widget_show (label);

	/* Create text boxes for user to enter appropriate values */
	p_id = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_id, 5);
	gtk_table_attach_defaults (GTK_TABLE (table), p_id, 1, 2, 1, 2);
	gtk_widget_show (p_id);

	p_stn = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_stn, 16);
	gtk_table_attach_defaults (GTK_TABLE (table), p_stn, 1, 2, 2, 3);
	gtk_widget_show (p_stn);

	p_ff = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_ff), "Fix Point");
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_ff), "Floating Point");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_ff), 0);
	gtk_table_attach_defaults (GTK_TABLE (table), p_ff, 1, 2, 3, 4);
	gtk_widget_show (p_ff);

	p_af = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_af), "Fix Point");
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_af), "Floating Point");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_af), 0);
	gtk_table_attach_defaults (GTK_TABLE (table), p_af, 1, 2, 4, 5);
	gtk_widget_show (p_af);

	p_pf = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_pf), "Fix Point");
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_pf), "Floating Point");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_pf), 0);
	gtk_table_attach_defaults (GTK_TABLE (table), p_pf, 1, 2, 5, 6);
	gtk_widget_show (p_pf);

	p_pn = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_pn), "Rectangular");
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_pn), "Polar");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_pn), 0);
	gtk_table_attach_defaults (GTK_TABLE (table), p_pn, 1, 2, 6, 7);
	gtk_widget_show (p_pn);

	p_phNumber = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_phNumber, 2);
	gtk_table_attach_defaults (GTK_TABLE (table), p_phNumber, 1, 2, 7, 8);
	gtk_widget_show (p_phNumber);

	p_anNumber = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_anNumber, 2);
	gtk_table_attach_defaults (GTK_TABLE (table), p_anNumber, 1, 2, 8, 9);
	gtk_widget_show (p_anNumber);

	p_dgNumber = gtk_entry_new();
	gtk_entry_set_max_length ((GtkEntry *)p_dgNumber, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), p_dgNumber, 1, 2, 9, 10);
	gtk_widget_show (p_dgNumber);

	p_frequency = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_frequency), "50");
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_frequency), "60");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_frequency), 0);
	gtk_table_attach_defaults (GTK_TABLE (table), p_frequency, 1, 2, 10, 11);
	gtk_widget_show (p_frequency);

	p_drate = gtk_combo_box_new_text();
    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "1");
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "25");
    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "30");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "50");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "60");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "100");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "120");
	gtk_combo_box_set_active(GTK_COMBO_BOX(p_drate), 1);
	gtk_table_attach_defaults (GTK_TABLE (table), p_drate, 1, 2, 11, 12);
	gtk_widget_show (p_drate);

	/* Signal handling for buttons on CFG Setuo Window */
     	//g_signal_connect (p_frequency, "changed", G_CALLBACK (freq_val), NULL);
	g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (Pmu_Help), NULL);
	g_signal_connect_swapped (valdbutton, "clicked", G_CALLBACK (validation_cfg_create), valdbutton);
	g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), cfg_setup_window);
	g_signal_connect_swapped (cfg_setup_window, "destroy", G_CALLBACK (gtk_widget_destroy), cfg_setup_window);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (valdbutton, TRUE);
	gtk_widget_set_can_default (help_button, TRUE);
	gtk_widget_set_can_default (cancel_button, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (cfg_setup_window)->action_area), valdbutton, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (cfg_setup_window)->action_area), help_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (cfg_setup_window)->action_area), cancel_button, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (valdbutton);
	gtk_widget_show (valdbutton);
	gtk_widget_grab_default (help_button);
	gtk_widget_show (help_button);
	gtk_widget_grab_default (cancel_button);
	gtk_widget_show (cancel_button);

	/* Finally show the cfg_setup_window */
	gtk_widget_show (cfg_setup_window);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  freq_val()								*/
/* Call at nominal frequency changes, from 50 to 60 or vice versa		*/
/* ----------------------------------------------------------------------------	*/
/*
void freq_val()
{
     int i;
	const char *tx = gtk_combo_box_get_active_text(GTK_COMBO_BOX(p_frequency));

	if(!strcmp(tx, "50"))
     {
         	for (i=0; i<10; i++)
         	{
              	gtk_combo_box_remove_text(GTK_COMBO_BOX(p_drate), i);
         	}

     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "1");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "10");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "25");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "50");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "100");
    		gtk_combo_box_set_active(GTK_COMBO_BOX(p_drate), 2);
    	}
	else
    	{
         	for (i=0; i<10; i++)
         	{
              	gtk_combo_box_remove_text(GTK_COMBO_BOX(p_drate), i);
         	}

     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "1");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "10");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "12");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "15");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "30");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "60");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "120");
    		gtk_combo_box_set_active(GTK_COMBO_BOX(p_drate), 4);
    	}
};
*/

/* ----------------------------------------------------------------------------	*/
/* FUNCTION  channel_names_for_phasor ()					               */
/* It shows the window for phasor channel names as per PHNMR entred by user.	*/
/* ----------------------------------------------------------------------------	*/

void channel_names_for_phasor ()
{
	/* local variables */
	int i, j, jj, temp_local, table_rows = 10;
	char line[20];
	GtkWidget *table, *label, *next_button;
	GtkWidget *scrolled_window, *cancel_button;

	/* Create a new dialog window for the scrolled window to be packed into */
	PhasorChWin = gtk_dialog_new ();
	g_signal_connect (PhasorChWin, "destroy", G_CALLBACK (gtk_widget_destroy), PhasorChWin);
	gtk_window_set_title (GTK_WINDOW (PhasorChWin), "Phasor Channels");
	gtk_window_set_resizable (GTK_WINDOW (PhasorChWin), FALSE);
     	gtk_container_set_border_width (GTK_CONTAINER (PhasorChWin), 10);
	
	/* Create a new scrolled window */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
	gtk_widget_set_size_request (scrolled_window, -1, 400);

	/* The policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS. */
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	/* The dialog window is created with a vbox packed into it */								
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(PhasorChWin)->vbox), scrolled_window, TRUE, FALSE, 0);
	gtk_widget_show (scrolled_window);

	/* Create a table of ? by 2 squares. */
	temp_local = cfg_info->cfg_phnmr_val;
     	if (temp_local >=  table_rows)
     	{
          	table_rows = temp_local;
     	}
	if(cfg_info->cfg_pf == 0)
		table = gtk_table_new (table_rows+1, 6, FALSE);
	else
		table = gtk_table_new (table_rows+1, 3, FALSE);

	/* Set the spacing to 15 on x and 25 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	/* Pack the table into the window */
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), table);
	gtk_widget_show (table);

	/* Add a "Next" button to the bottom of the dialog */
	next_button = gtk_button_new_with_label ("Next");
	cancel_button = gtk_button_new_with_label ("Cancel");

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='10'><b>Enter Phasor Channel Name</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 6, 0, 1);
	gtk_widget_show (label);
	g_free (markup);

	/* This simply creates a grid of Lables with text on the table to demonstrate the scrolled window. */
	for(i=1, j=1; i<=temp_local; j++)
	{
		memset(line,'\0',sizeof(line));
		sprintf(line, "Phasor Ch %d : ", j);
		label = gtk_label_new (line);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, i, i+1);
		gtk_widget_show (label);

		num_text[j] = gtk_entry_new ();
		gtk_entry_set_max_length ((GtkEntry *)num_text[j], 16);
		gtk_table_attach_defaults (GTK_TABLE (table), num_text[j], 1, 2, i, i+1);
		gtk_widget_show (num_text[j]);

		num_combo[j] = gtk_combo_box_new_text();
		gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "Voltage");
		gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "Current");
		gtk_combo_box_set_active(GTK_COMBO_BOX(num_combo[j]), 0);
		gtk_table_attach_defaults (GTK_TABLE (table), num_combo[j], 2, 3, i, i+1);
		gtk_widget_show (num_combo[j]);

		if(cfg_info->cfg_pf == 0)
		{
			memset(line,'\0',sizeof(line));
			sprintf(line, "Max Limit %d : ", j);
			label = gtk_label_new (line);
			gtk_table_attach_defaults (GTK_TABLE (table), label, 3, 4, i, i+1);
			gtk_widget_show (label);

			num_text1[j] = gtk_entry_new ();
			gtk_entry_set_max_length ((GtkEntry *)num_text1[j], 16);
			gtk_table_attach_defaults (GTK_TABLE (table), num_text1[j], 4, 5, i, i+1);
			gtk_widget_show (num_text1[j]);
			if(cfg_info->cfg_pf == 1) gtk_widget_set_sensitive(GTK_WIDGET(num_text1[j]), FALSE);

			memset(line,'\0',sizeof(line));
			sprintf(line, "(kV or Amp)");
			label = gtk_label_new (line);
			gtk_table_attach_defaults (GTK_TABLE (table), label, 5, 6, i, i+1);
			gtk_widget_show (label);
		}
		i = i+1;
	}
    
     	if (temp_local < table_rows)
     	{
          	for (jj = i; jj < table_rows; jj++)
          	{
		     	label = gtk_label_new ("-- ");
		     	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 6, jj, jj+1);
		     	gtk_widget_show (label);
          	}
     	}

	/* Signal handling for buttons on CFG Setuo Window */
	g_signal_connect_swapped (next_button, "clicked", G_CALLBACK (validation_phasor_names), NULL);
	g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), PhasorChWin);
	g_signal_connect_swapped (PhasorChWin, "destroy", G_CALLBACK (gtk_widget_destroy), PhasorChWin);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (next_button, TRUE);
	gtk_widget_set_can_default (cancel_button, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (PhasorChWin)->action_area), next_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (PhasorChWin)->action_area), cancel_button, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (next_button);
	gtk_widget_show (next_button);
	gtk_widget_grab_default (cancel_button);
	gtk_widget_show (cancel_button);

	/* Finally show the PhasorChWin */
	gtk_widget_show (PhasorChWin);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  channel_names_for_analog ()                                        */
/* It shows the window for analog channel names as per ANNMR entred by user.	*/
/* ----------------------------------------------------------------------------	*/

void channel_names_for_analog ()
{
	/* Analog is not Mandatory component in CFG, it could be zero */
	if (cfg_info->cfg_annmr_val == 0)
	{
		/* Next call to digital channel names */
		channel_names_for_digital ();
	}
	else	/* if ANNMR > 0 */
	{
		/* local variables */
		int i,temp_local,j,jj,table_rows = 9;
		char line[30];
		GtkWidget *table, *scrolled_window;
		GtkWidget *label, *next_button, *cancel_button;

		/* Create a new dialog window for the scrolled window to be packed into */  				
		AnalogChWin = gtk_dialog_new ();
		g_signal_connect (AnalogChWin, "destroy", G_CALLBACK (gtk_widget_destroy), AnalogChWin);
		gtk_window_set_title (GTK_WINDOW (AnalogChWin), "Analog Channels");
		gtk_container_set_border_width (GTK_CONTAINER (AnalogChWin), 10);
          	gtk_window_set_resizable (GTK_WINDOW (AnalogChWin), FALSE);

		/* Create a new scrolled window */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
          	gtk_widget_set_size_request (scrolled_window, -1, 400);

		/* The policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS. */
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

		/* The dialog window is created with a vbox packed into it */
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG(AnalogChWin)->vbox), scrolled_window, TRUE, TRUE, 0);
		gtk_widget_show (scrolled_window);

		/* Create a table as per phasor numbers and phasor type */
          	temp_local = cfg_info->cfg_annmr_val;
          	if (temp_local >=  table_rows)
          	{
               		table_rows = temp_local;
          	} 

	        if(cfg_info->cfg_af == 0)
	          	table = gtk_table_new (table_rows+1, 5, FALSE);
		else
	          	table = gtk_table_new (table_rows+1, 3, FALSE);

		next_button = gtk_button_new_with_label ("Next");
		cancel_button = gtk_button_new_with_label ("Cancel");

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='10'><b>Enter Analog Channel Name</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 5, 0, 1);
		gtk_widget_show (label);
		g_free (markup);

		/* Set the spacing to 15 on x and 25 on y */
		gtk_table_set_row_spacings (GTK_TABLE (table), 8);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		/* Pack the table into the window */
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), table);
		gtk_widget_show (table);

		/* Add a "Next" button to the bottom of the dialog */
		next_button = gtk_button_new_with_label ("Next");

		/* This simply creates a grid of Lables with text on the table to demonstrate the scrolled window. */
		for(i=1, j=1; i<=temp_local; j++)
		{
			memset(line,'\0',sizeof(line));
			sprintf(line, "Analog Ch %d : ", j);
			label = gtk_label_new (line);
			gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, i, i+1);
			gtk_widget_show (label);

			num_text[j] = gtk_entry_new ();
			gtk_entry_set_max_length ((GtkEntry *)num_text[j], 16);
			gtk_table_attach_defaults (GTK_TABLE (table), num_text[j], 1, 2, i, i+1);
			gtk_widget_show (num_text[j]);

			num_combo[j] = gtk_combo_box_new_text();
			gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "RMS");
			gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "POW");
			gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "Peak");
			gtk_combo_box_set_active(GTK_COMBO_BOX(num_combo[j]), 0);
			gtk_table_attach_defaults (GTK_TABLE (table), num_combo[j], 2, 3, i, i+1);
			gtk_widget_show (num_combo[j]);

			/* Add only in case of phasors are in fix point format */
		        if(cfg_info->cfg_af == 0)
			{
				memset(line,'\0',sizeof(line));
				sprintf(line, "Max Value %d : ", j);
				label = gtk_label_new (line);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 3, 4, i  , i+1);
				gtk_widget_show (label);

				num_text1[j] = gtk_entry_new ();
				gtk_entry_set_max_length ((GtkEntry *)num_text1[j], 16);
				gtk_table_attach_defaults (GTK_TABLE (table), num_text1[j], 4, 5, i  , i+1);
				gtk_widget_show (num_text1[j]);
				if(cfg_info->cfg_af == 1) gtk_widget_set_sensitive(GTK_WIDGET(num_text1[j]), FALSE);
			}
			i = i+1;
		}
        
          	if (temp_local < table_rows)
          	{
               		for (jj = i; jj < table_rows; jj++)
               		{
                    		label = gtk_label_new ("-- ");
                    		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 5, jj, jj+1);
                    		gtk_widget_show (label);
               		}
          	}

		/* Signal handling for Next button on Window */
		g_signal_connect_swapped (next_button, "clicked", G_CALLBACK (validation_analog_names), NULL);
		g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), AnalogChWin);
		g_signal_connect_swapped (AnalogChWin, "destroy", G_CALLBACK (gtk_widget_destroy), AnalogChWin);

		/* This makes it so the button is the default. */
		gtk_widget_set_can_default (next_button, TRUE);
		gtk_widget_set_can_default (cancel_button, TRUE);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (AnalogChWin)->action_area), next_button, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (AnalogChWin)->action_area), cancel_button, TRUE, TRUE, 0);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (next_button);
		gtk_widget_show (next_button);
		gtk_widget_grab_default (cancel_button);
		gtk_widget_show (cancel_button);

		/* Finally show the AnalogChWin */
		gtk_widget_show (AnalogChWin);
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  channel_names_for_digital ()                                       */
/* It shows the window for digital channel names as per DGNMR entred by user.	*/
/* ----------------------------------------------------------------------------	*/

void channel_names_for_digital ()
{
	/* Analog is not Mandatory component in CFG, it could be zero */
	if (cfg_info->cfg_dgnmr_val == 0)
	{
		/* Next call to 'final_cfg_create ()' */
		final_cfg_create ();
	}
	else	/* if DGNMR > 0 */
	{
		/* local variables */
		int i;
		char line[30];
		GtkWidget *table, *scrolled_window;
		GtkWidget *label, *next_button, *cancel_button;

		/* Create a new dialog window for the scrolled window to be packed into */  				
		AnalogChWin = gtk_dialog_new ();
		g_signal_connect (AnalogChWin, "destroy", G_CALLBACK (gtk_widget_destroy), AnalogChWin);
		gtk_window_set_title (GTK_WINDOW (AnalogChWin), "Digital Channels");
		gtk_container_set_border_width (GTK_CONTAINER (AnalogChWin), 10);
        	gtk_window_set_resizable (GTK_WINDOW (AnalogChWin), FALSE);

		/* Create a new scrolled window */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
		gtk_widget_set_size_request (scrolled_window, -1, 550);

		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

		/* The dialog window is created with a vbox packed into it */
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG(AnalogChWin)->vbox), scrolled_window, TRUE, TRUE, 0);
		gtk_widget_show (scrolled_window);

		/* Create a table of ? by 2 squares. */
		table = gtk_table_new (cfg_info->cfg_dgnmr_val*16+1, 2, FALSE);

		/* Set the spacing to 15 on x and 25 on y */
		gtk_table_set_row_spacings (GTK_TABLE (table), 8);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		/* Pack the table into the window */
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), table);
		gtk_widget_show (table);

		/* Add a "Next" button to the bottom of the dialog */
		next_button = gtk_button_new_with_label ("Next");
		cancel_button = gtk_button_new_with_label ("Cancel");

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='10'><b>Enter Digital Channels Name</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
		gtk_widget_show (label);
		g_free (markup);

		/* This simply creates a grid of Lables with text on the table to demonstrate the scrolled window. */
		for(i=1; i<cfg_info->cfg_dgnmr_val*16+1; i++)
		{
			memset(line,'\0',sizeof(line));
			sprintf(line, "Digital Ch %d : ", i);
			label = gtk_label_new (line);
			gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, i, i+1);
			gtk_widget_show (label);
		}

		for(i=1; i<cfg_info->cfg_dgnmr_val*16+1; i++)
		{
			num_text[i] = gtk_entry_new ();
			//gtk_entry_set_max_length (GtkEntry *entry, gint max);
			gtk_entry_set_max_length ((GtkEntry *)num_text[i], 16);
			gtk_table_attach_defaults (GTK_TABLE (table), num_text[i], 1, 2, i, i+1);
			gtk_widget_show (num_text[i]);
		}

		/* Signal handling for Next button on Window */
		g_signal_connect_swapped (next_button, "clicked", G_CALLBACK (validation_digital_names), NULL);
		g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), AnalogChWin);
		g_signal_connect_swapped (AnalogChWin, "response", G_CALLBACK (gtk_widget_destroy), AnalogChWin);

		/* This makes it so the button is the default. */
		gtk_widget_set_can_default (next_button, TRUE);
		gtk_widget_set_can_default (cancel_button, TRUE);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (AnalogChWin)->action_area), next_button, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (AnalogChWin)->action_area), cancel_button, TRUE, TRUE, 0);	    

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (next_button);
		gtk_widget_show (next_button);
		gtk_widget_grab_default (cancel_button);
		gtk_widget_show (cancel_button);

		/* Finally show the AnalogChWin */
		gtk_widget_show (AnalogChWin);
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  final_cfg_create ()						                    */
/* It call the final function for generate configuration frame.			     */
/* ----------------------------------------------------------------------------	*/

void final_cfg_create ()
{
	/* local variables */
	char buff[50], tbuff[200];
	FILE *fp1;

	strcpy(pmuFilePath, pmuFolderPath);
	strcat(pmuFilePath, "/");
	strcat(pmuFilePath, "pmu");
	sprintf(buff, "%d", cfg_info->cfg_pmuID);
	strcat(pmuFilePath, buff);
	strcat(pmuFilePath, ".bin");
	pmuFilePath[strlen(pmuFilePath)] = '\0';

	/* Check whether any PMU Setup File already exist with the same name? */
	fp1 = fopen (pmuFilePath,"rb");

	if (fp1 != NULL)
	{
		/* Close the open PMU Setup File */
		fclose(fp1);		

		/* local variables */
		GtkWidget *table, *label, *ok;

		/* Create a new dialog window for PMU Server Setup */
		CCWin = gtk_dialog_new ();
		gtk_window_set_title (GTK_WINDOW (CCWin), "Setup Warning!");
		gtk_container_set_border_width (GTK_CONTAINER (CCWin), 10);

		/* Create a table of 4 by 2 squares. */
		table = gtk_table_new (9, 2, FALSE);

		/* Set the spacing to 50 on x and 5 on y */
		gtk_table_set_row_spacings (GTK_TABLE (table), 8);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		/* Pack the table into the window */
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG(CCWin)->vbox), table, TRUE, TRUE, 0);
		gtk_widget_show (table);

		/* Add few buttons to the bottom of the dialog */
		ok = gtk_button_new_with_label ("OK");

		/* This simply creates a grid of toggle buttons on the table */
		memset(tbuff, '\0', 200);
		strcat(tbuff, "Setup File with PMU ID - ");
		sprintf(buff, "%d", cfg_info->cfg_pmuID);
		strcat(tbuff, buff);
		strcat(tbuff, " is already present in system!");
		label = gtk_label_new (tbuff);
	     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
		gtk_widget_show (label);

	        label = gtk_hseparator_new();
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 1, 2);
		gtk_widget_show (label);

	  	chkBtn1 = gtk_check_button_new_with_label("Overwrite with new PMU setup");
		gtk_table_attach_defaults (GTK_TABLE (table), chkBtn1, 0, 2, 3, 4);
          	gtk_widget_show (chkBtn1);

	  	chkBtn2 = gtk_check_button_new_with_label("Change the PMU ID Code");
		gtk_table_attach_defaults (GTK_TABLE (table), chkBtn2, 0, 2, 4, 5);
          	gtk_widget_show (chkBtn2);

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#113312\" font='10'><b>New PMU ID</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 5, 6);
		gtk_widget_show (label);
		g_free (markup);

		/* Create text boxes for user to enter appropriate values */
		p_id = gtk_entry_new();
		gtk_entry_set_max_length ((GtkEntry *)p_id, 5);
		gtk_table_attach_defaults (GTK_TABLE (table), p_id, 1, 2, 5, 6);
		gtk_widget_show (p_id);

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Please give a new PMU ID to save the old PMU Configuration.</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
	     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 7, 8);
		gtk_widget_show (label);
		g_free (markup);

	        label = gtk_hseparator_new();
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 8, 9);
		gtk_widget_show (label);

		/* Signal handling for buttons on Setup Warning Window */
		g_signal_connect_swapped (ok, "clicked", G_CALLBACK (validation_setup_file_name), NULL);
    		g_signal_connect (chkBtn1, "toggled", G_CALLBACK (checkbox_function), "1");
    		g_signal_connect (chkBtn2, "toggled", G_CALLBACK (checkbox_function), "2");
		g_signal_connect (CCWin, "destroy", G_CALLBACK (gtk_widget_destroy), CCWin);
		g_signal_connect_swapped (CCWin, "response", G_CALLBACK (gtk_widget_destroy), CCWin);

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (chkBtn2),TRUE);

		/* This makes it so the button is the default. */
		gtk_widget_set_can_default (ok, TRUE);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), ok, TRUE, TRUE, 0);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (ok);
		gtk_widget_show (ok);

		/* Finally show the CCWin */
		gtk_widget_show (CCWin);
	}
	else
	{
		/* If not matched, call final call to create a new CFG */
		cfg_create_call();
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  checkbox_function(GtkWidget *widget, gpointer data)		*/
/* This is called when checkboxes are selected, and do the appropriate		*/
/* changes in the other checkbox or widget?					*/
/* ----------------------------------------------------------------------------	*/

void checkbox_function(GtkWidget *widget, gpointer data)
{
	char t[2];
	strcpy(t, (char *) data);

	if(!strcmp(t, "1"))
	{
		if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chkBtn1)) == TRUE)
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (chkBtn2),FALSE);
			gtk_widget_set_sensitive(GTK_WIDGET(p_id), FALSE);
		}
		else
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (chkBtn2),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(p_id), TRUE);
			gtk_entry_set_text(GTK_ENTRY(p_id), " ");
		}
	}
	else
	{
		if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chkBtn2)) == TRUE)
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (chkBtn1),FALSE);
			gtk_widget_set_sensitive(GTK_WIDGET(p_id), TRUE);
			gtk_entry_set_text(GTK_ENTRY(p_id), " ");
		}
		else
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (chkBtn1),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(p_id), FALSE);
		}
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  cfg_create_call()							*/
/* This is final call for configuration creation. It will call the create_cfg()	*/
/* function in CfgFunction.c file and make the setup file also.			*/
/* ----------------------------------------------------------------------------	*/

void cfg_create_call()
{
	/* local variables */
	int ret_val;

	/* To create CFG call 'create_cfg()' */
	ret_val = create_cfg();

	if(ret_val == 0)
	{
		/* Enable rest of the buttons on PMU Simulator Window */
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->manage_data_source), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->header_frm_button), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->stat_modification_button), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->cfg_modification_button), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->pmu_details_button), TRUE);

		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menuitem2), TRUE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->pmu_properties), TRUE);

		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_data_source), TRUE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_header_frm), TRUE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_cfg_modify), TRUE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_stat_modify), TRUE);

		/* Disable create CFG buttons on PMU Simulator Window */
		gtk_widget_set_sensitive(GTK_WIDGET(pmu_data->create_cfg_button), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(pmu_data->menu_setup_cfg), FALSE);
	}
	else
	{
		validation_result (" PMU Simulator Setup has failed! Try again ");			
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  stat_chng_options(GtkWidget *widget, gpointer udata)		          */
/* It provides multiple options to user regarding STAT Word change. User can	*/
/* introduce error bit, Configuration modification, etc.                        */
/* ----------------------------------------------------------------------------	*/

void stat_chng_options(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	GtkWidget *table, *label;
	GtkWidget *ID_butn, *PE_butn, *DS_butn, *chk_butn;
	GtkWidget *PT_butn, *CC_butn, *close_butn;

	/* Create a new dialog window for the Configration Modification */ 
	CCOptionWin = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (CCOptionWin), "STAT Word Modification");
	gtk_container_set_border_width (GTK_CONTAINER (CCOptionWin), 10);

	/* Create a table of 7 by 3 squares */
	table = gtk_table_new (10, 3, FALSE);

	/* Set the spacing to 15 on x and 60 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(CCOptionWin)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='12'><b>Modification of bits in STAT Word</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);
	g_free (markup);

	ID_butn = gtk_button_new_with_label ("Set Invalid Data bit (Bit-15)");
	gtk_table_attach_defaults (GTK_TABLE (table), ID_butn, 1, 2, 2, 3);
	gtk_widget_show (ID_butn);

	PE_butn = gtk_button_new_with_label ("Set PMU Error bit (Bit-14)");
	gtk_table_attach_defaults (GTK_TABLE (table), PE_butn, 1, 2, 3, 4);
	gtk_widget_show (PE_butn);

	DS_butn = gtk_button_new_with_label ("Set Data Sorting bit (Bit-12)");
	gtk_table_attach_defaults (GTK_TABLE (table), DS_butn, 1, 2, 4, 5);
	gtk_widget_show (DS_butn);

	PT_butn = gtk_button_new_with_label ("Set PMU Trigger bit (Bit-11)");
	gtk_table_attach_defaults (GTK_TABLE (table), PT_butn, 1, 2, 5, 6);
	gtk_widget_show (PT_butn);

	chk_butn = gtk_button_new_with_label ("CheckSum Error in Data Frame");
	gtk_table_attach_defaults (GTK_TABLE (table), chk_butn, 1, 2, 6, 7);
	gtk_widget_show (chk_butn);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : The Synchronization Error (Bit-13) handled\ninternaly. CheckSum Error will not change\nany bit in STAT Word.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label,  0, 2, 9, 10);
	gtk_widget_show (label);
	g_free (markup);

	/* Add a "close" button to the bottom of the dialog */
	close_butn = gtk_button_new_with_label ("close");

	/* Signal handling for all buttons on Window */
	g_signal_connect (ID_butn, "clicked", G_CALLBACK (cfg_STAT_change), (gpointer) "a");
	g_signal_connect (PE_butn, "clicked", G_CALLBACK (cfg_STAT_change), (gpointer) "b");
	g_signal_connect (DS_butn, "clicked", G_CALLBACK (cfg_STAT_change), (gpointer) "c");
	g_signal_connect (PT_butn, "clicked", G_CALLBACK (cfg_STAT_change), (gpointer) "d");
	g_signal_connect (chk_butn, "clicked", G_CALLBACK (cfg_STAT_change), (gpointer) "e");
	g_signal_connect_swapped (close_butn, "clicked", G_CALLBACK (gtk_widget_destroy), CCOptionWin);
	g_signal_connect_swapped (CCOptionWin, "response", G_CALLBACK (gtk_widget_destroy), CCOptionWin);

	/* This makes it so the button is the default */
	gtk_widget_set_can_default (close_butn, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCOptionWin)->action_area), close_butn, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (close_butn);
	gtk_widget_show (close_butn);

	/* Finally show the CCOptionWin */
	gtk_widget_show (CCOptionWin);
};	


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  cfg_STAT_change (GtkWidget *widget, gpointer udata)		*/
/* This function change the STAT Word bits according to user's wish. It also 	*/
/* has option to do CheckSum invalid for next data frame, and its not a STAT	*/
/* Word error.									*/
/* ----------------------------------------------------------------------------	*/

void cfg_STAT_change (GtkWidget *widget, gpointer udata)
{
	/* Local variables */
	char find_butn[2];

	/* For identifing singnal came from which button? */
	strcpy(find_butn,(char *) udata);

	if(!strcmp(find_butn, "a"))
	{
		validation_result(" STAT WORD CHANGED\nBit 15 of Stat word has been set to 1 for Invalid Data.");
		ShmPTR->cfg_bit_change_info = 2;
	}
	else if(!strcmp(find_butn, "b"))
	{
		validation_result("STAT WORD CHANGED\nBit 14 of Stat word has been set to 1 for PMU Error.");
		ShmPTR->cfg_bit_change_info = 3;
	}
	else if(!strcmp(find_butn, "c"))
	{
		validation_result("STAT WORD CHANGED\nBit 12 of Stat word has been set to 1 for Data Sorting.");
		ShmPTR->cfg_bit_change_info = 4;
	}
	else if(!strcmp(find_butn, "d"))
	{
		validation_result("STAT WORD CHANGED\nBit 11 of Stat word has been set to 1 for PMU Trigger Detected.");

		ShmPTR->cfg_bit_change_info = 5;
	}
	else if(!strcmp(find_butn, "e"))
	{
		validation_result("CHECKSUM ERROR\nA data frame with invalid CRC would be sent.");

		ShmPTR->cfg_bit_change_info = 6;
	}

	p1.pid = ShmPTR->pid;

	kill(p1.pid, SIGUSR2);
	printf("STAT word bit changed signal sent to PMU Server Process.\n");

	/* Close/destroy the CCOptionWin */
	gtk_widget_destroy(CCOptionWin); 				
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  Change_pmu_configuration(GtkWidget *widget, gpointer udata)	*/
/* By this function user have option window to choose Add New Channels or	*/
/* Remove Channels?.								*/
/* ----------------------------------------------------------------------------	*/

void Change_pmu_configuration(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	GtkWidget *table;
	GtkWidget *close_butn, *label;
	GtkWidget *Addbutton, *Removebutton;
	GSList *group;
     int ch = 0;

	/* Create a new dialog window for the Option Window */ 
	CCWin = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (CCWin), "Operation");
	gtk_container_set_border_width (GTK_CONTAINER (CCWin), 10);

	/* Create a table of 4 by 2 squares */
	table = gtk_table_new (6, 2, FALSE);

	/* Set the spacing to 15 on x and 25 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(CCWin)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='12'><b>Channel Operation</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);
	g_free (markup);

	/* Add few radio buttons on the dialog window */
	Addbutton = gtk_radio_button_new_with_label (NULL, "Add New Channels");
	gtk_table_attach_defaults (GTK_TABLE (table), Addbutton, 0, 1, 2, 3);
     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (Addbutton), TRUE);
     gtk_widget_show (Addbutton);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (Addbutton));
	Removebutton = gtk_radio_button_new_with_label (NULL, "Remove Channels");
	gtk_table_attach_defaults (GTK_TABLE (table), Removebutton, 0, 1, 3, 4);
	gtk_widget_show (Removebutton);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Both Phasor and Analog channels could be add.\nYou will get the change Data Rate option at the end.\nIt will change the bit-10 from 0 to 1 in STAT Word in data frames.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 5, 6);
	gtk_widget_show (label);
	g_free (markup);

	/* Add 'Close' buttons on the dialog window */
	close_butn = gtk_button_new_with_label ("Close");

	/* Signal handling for buttons on Option Window */
	g_signal_connect_swapped (Addbutton, "clicked", G_CALLBACK (new_phasor_num), NULL);
	g_signal_connect_swapped (Removebutton, "clicked", G_CALLBACK (remove_phasor_num), NULL);
	g_signal_connect_swapped (close_butn, "clicked", G_CALLBACK (gtk_widget_destroy), CCWin);
	g_signal_connect_swapped (CCWin, "response", G_CALLBACK (gtk_widget_destroy), CCWin);

	/* This makes it so the button is the default. */
	gtk_widget_set_can_default (close_butn, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), close_butn, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (close_butn);
	gtk_widget_show (close_butn);

	/* Finally show the CCWin */
	gtk_widget_show (CCWin);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  new_phasor_num(GtkWidget *widget, gpointer udata)			     */
/* It will ask user to number of new phasor channels that he wants to add?	     */
/* ----------------------------------------------------------------------------	*/

void new_phasor_num(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	GtkWidget *table, *help_button;
	GtkWidget *next_butn, *label, *cancel_button;

	/* Allocate the memory for the ConfigurationChange object */
	new_cfg_info = malloc(sizeof(struct ConfigurationChange));
	new_cfg_info->add_remove_choice = 1; 

	/* Create a new dialog window for New Phasor Channels */ 
	gtk_widget_destroy(CCWin); 				
	CCWin = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (CCWin), "Add New Phasors");
	gtk_container_set_border_width (GTK_CONTAINER (CCWin), 10);
	//gtk_widget_set_size_request (CCWin, 350, 300);

	/* Create a table of 3 by 3 squares. */
	table = gtk_table_new (3, 3, FALSE);

	/* Set the spacing to 25 on x and 40 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(CCWin)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	/* Add few buttons to the bottom of the dialog */
	next_butn = gtk_button_new_with_label ("Next");
	help_button = gtk_button_new_with_label ("Help");
	cancel_button = gtk_button_new_with_label ("Cancel");

	label = gtk_label_new ("New Phasors");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
	gtk_widget_show (label);

	label = gtk_label_new ("New Analogs");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 1, 2);
	gtk_widget_show (label);

	/* Create text boxes for user to enter appropriate values */
	phtext = gtk_entry_new ();
	gtk_table_attach_defaults (GTK_TABLE (table), phtext, 1, 2, 0, 1);
	gtk_widget_show (phtext);

	angtext = gtk_entry_new ();
	gtk_table_attach_defaults (GTK_TABLE (table), angtext, 1, 2, 1, 2);
	gtk_widget_show (angtext);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : New channels will be added to existing configuration frame.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 2, 3);
	gtk_widget_show (label);
	g_free (markup);

	/* Signal handling for buttons on Window */
	g_signal_connect (next_butn, "clicked", G_CALLBACK (validation_ph_an_num), NULL);
	g_signal_connect_swapped (help_button, "clicked", G_CALLBACK (Pmu_Help), NULL);
	g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), CCWin);
	g_signal_connect_swapped (CCWin, "response", G_CALLBACK (gtk_widget_destroy), CCWin);

	/* This makes it so the button is the default */
	gtk_widget_set_can_default (next_butn, TRUE);
	gtk_widget_set_can_default (help_button, TRUE);
	gtk_widget_set_can_default (cancel_button, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), next_butn, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), help_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), cancel_button, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (next_butn);
	gtk_widget_show (next_butn);
	gtk_widget_grab_default (help_button);
	gtk_widget_show (help_button);
	gtk_widget_grab_default (cancel_button);
	gtk_widget_show (cancel_button);

	/* Finally show the CCWin */
	gtk_widget_show (CCWin);
};	


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  new_channel_names_for_phasor ()					               */
/* It shows the window to enter new phasor channel names.                       */
/* ----------------------------------------------------------------------------	*/

void new_channel_names_for_phasor ()
{
	/* new_phasor number could be zero? */	
	if(new_cfg_info->new_cfg_phnmr_val > 0)
	{
	     /* local variables */
	     int i, j, jj, temp_local, table_rows = 10;
	     char line[20];
	     GtkWidget *table, *scrolled_window, *cancel_button;
	     GtkWidget *label, *next_button;

	     /* Create a new dialog window for the scrolled window to be packed into */
	gtk_widget_destroy(CCWin);
	     PhasorChWin = gtk_dialog_new ();
	     g_signal_connect (PhasorChWin, "destroy", G_CALLBACK (gtk_widget_destroy), PhasorChWin);
	     gtk_window_set_title (GTK_WINDOW (PhasorChWin), "New Phasor Channels");
	     gtk_window_set_resizable (GTK_WINDOW (PhasorChWin), FALSE);
          gtk_container_set_border_width (GTK_CONTAINER (PhasorChWin), 10);
	
	     /* Create a new scrolled window */
	     scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	     gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
	     gtk_widget_set_size_request (scrolled_window, -1, 400);

	     /* The policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS. */
	     gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	     /* The dialog window is created with a vbox packed into it */
	     gtk_box_pack_start (GTK_BOX (GTK_DIALOG(PhasorChWin)->vbox), scrolled_window, TRUE, FALSE, 0);
	     gtk_widget_show (scrolled_window);

	     /* Create a table of ? by 2 squares. Based on phasor numbers and phasor type */
	     temp_local = new_cfg_info->new_cfg_phnmr_val;
             if (temp_local >=  table_rows)
             {
               	table_rows = temp_local;
             } 
	     if(cfg_info->cfg_pf == 0)
		table = gtk_table_new (table_rows+1, 6, FALSE);
	     else
		table = gtk_table_new (table_rows+1, 3, FALSE);

	     /* Set the spacing to 15 on x and 25 on y */
	     gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	     gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	     /* Pack the table into the window */
	     gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), table);
	     gtk_widget_show (table);

	     /* Add a "Next" button to the bottom of the dialog */
	     next_button = gtk_button_new_with_label ("Next");
	     cancel_button = gtk_button_new_with_label ("Cancel");

	     label = gtk_label_new (" ");
	     markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='10'><b>Enter Phasor Channel Name</b></span>");
	     gtk_label_set_markup (GTK_LABEL (label), markup);
	     gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 6, 0, 1);
	     gtk_widget_show (label);
	     g_free (markup);

	     /* This simply creates a grid of Lables with text on the table to demonstrate the scrolled window. */
	     for(i=1, j=1; i<=temp_local; j++)
	     {
		     memset(line,'\0',sizeof(line));
		     sprintf(line, "Phasor Ch %d : ", j);
		     label = gtk_label_new (line);
		     gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, i, i+1);
		     gtk_widget_show (label);

		     num_text[j] = gtk_entry_new ();
		     gtk_entry_set_max_length ((GtkEntry *)num_text[j], 16);
		     gtk_table_attach_defaults (GTK_TABLE (table), num_text[j], 1, 2, i, i+1);
		     gtk_widget_show (num_text[j]);

		     num_combo[j] = gtk_combo_box_new_text();
		     gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "Voltage");
		     gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "Current");
		     gtk_combo_box_set_active(GTK_COMBO_BOX(num_combo[j]), 0);
		     gtk_table_attach_defaults (GTK_TABLE (table), num_combo[j], 2, 3, i, i+1);
		     gtk_widget_show (num_combo[j]);

		     /* Add only in case of phasors are in fix point format */
	     	     if(cfg_info->cfg_pf == 0)
		     {
			     memset(line,'\0',sizeof(line));
			     sprintf(line, "Max Limit %d : ", j);
			     label = gtk_label_new (line);
			     gtk_table_attach_defaults (GTK_TABLE (table), label, 3, 4, i, i+1);
			     gtk_widget_show (label);

			     num_text1[j] = gtk_entry_new ();
			     gtk_entry_set_max_length ((GtkEntry *)num_text1[j], 16);
			     gtk_table_attach_defaults (GTK_TABLE (table), num_text1[j], 4, 5, i, i+1);
			     gtk_widget_show (num_text1[j]);
			     if(cfg_info->cfg_pf == 1) gtk_widget_set_sensitive(GTK_WIDGET(num_text1[j]), FALSE);

			     memset(line,'\0',sizeof(line));
			     sprintf(line, "(kV or Amp)");
			     label = gtk_label_new (line);
			     gtk_table_attach_defaults (GTK_TABLE (table), label, 5, 6, i, i+1);
			     gtk_widget_show (label);
		     }
		     i = i+1;
	     }

          if (temp_local < table_rows)
          {
               for (jj = i; jj < table_rows; jj++)
               {
                    label = gtk_label_new ("-- ");
                    gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 6, jj, jj+1);
                    gtk_widget_show (label);
               }
          }

		/* Signal handling for Next button on Window */
		g_signal_connect_swapped (next_button, "clicked", G_CALLBACK (validate_func), NULL);
		g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), PhasorChWin);
		g_signal_connect_swapped (PhasorChWin, "response", G_CALLBACK (gtk_widget_destroy), PhasorChWin);

		/* This makes it so the button is the default */
		gtk_widget_set_can_default (next_button, TRUE);
		gtk_widget_set_can_default (cancel_button, TRUE);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (PhasorChWin)->action_area), next_button, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (PhasorChWin)->action_area), cancel_button, TRUE, TRUE, 0);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (next_button);
		gtk_widget_show (next_button);
		gtk_widget_grab_default (cancel_button);
		gtk_widget_show (cancel_button);

		/* Finally show the PhasorChWin */
		gtk_widget_show (PhasorChWin);
	}
	else	/* In case of number of new phasors are zero */
	{
		gtk_widget_destroy(CCWin);

		new_channel_names_for_analog ();
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  new_channel_names_for_analog ()					               */
/* It shows the window to enter new analog channel names.				     */
/* ----------------------------------------------------------------------------	*/

void new_channel_names_for_analog ()
{
	/* new_analog number could be zero? */	
	if(new_cfg_info->new_cfg_annmr_val > 0)
	{
		/* local variables */
		int i,temp_local,j,jj,table_rows = 9;
		char line[30];
		GtkWidget *table, *scrolled_window;
		GtkWidget *label, *next_button, *cancel_button;

		/* Create a new dialog window for the scrolled window to be packed into */  				
		AnalogChWin = gtk_dialog_new ();
		g_signal_connect (AnalogChWin, "destroy", G_CALLBACK (gtk_widget_destroy), AnalogChWin);
		gtk_window_set_title (GTK_WINDOW (AnalogChWin), "New Analog Channels");
		gtk_container_set_border_width (GTK_CONTAINER (AnalogChWin), 10);
          	gtk_window_set_resizable (GTK_WINDOW (AnalogChWin), FALSE);

		/* Create a new scrolled window */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);
		gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
          	gtk_widget_set_size_request (scrolled_window, -1, 400);

		/* The policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS.*/
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

		/* The dialog window is created with a vbox packed into it */
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG(AnalogChWin)->vbox), scrolled_window, TRUE, TRUE, 0);
		gtk_widget_show (scrolled_window);

		/* Create a table of ? by 2 squares. Based on analog numbers and analog types */
          	temp_local = new_cfg_info->new_cfg_annmr_val;
          	if (temp_local >=  table_rows)
          	{
               		table_rows = temp_local;
          	} 

	        if(cfg_info->cfg_af == 0)
	          	table = gtk_table_new (table_rows+1, 5, FALSE);
		else
	          	table = gtk_table_new (table_rows+1, 3, FALSE);

		next_button = gtk_button_new_with_label ("Next");
		cancel_button = gtk_button_new_with_label ("Cancel");

		label = gtk_label_new (" ");
		markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='10'><b>Enter Analog Channel Name</b></span>");
		gtk_label_set_markup (GTK_LABEL (label), markup);
		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 5, 0, 1);
		gtk_widget_show (label);
		g_free (markup);

		/* Set the spacing to 15 on x and 25 on y */
		gtk_table_set_row_spacings (GTK_TABLE (table), 8);
		gtk_table_set_col_spacings (GTK_TABLE (table), 2);

		/* Pack the table into the window */
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), table);
		gtk_widget_show (table);

		/* Add a "Next" button to the bottom of the dialog */
		next_button = gtk_button_new_with_label ("Next");

		/* This simply creates a grid of Lables with text on the table to demonstrate the scrolled window. */
		for(i=1, j=1; i<=temp_local; j++)
		{
			memset(line,'\0',sizeof(line));
			sprintf(line, "Analog Ch %d : ", j);
			label = gtk_label_new (line);
			gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, i, i+1);
			gtk_widget_show (label);

			num_text[j] = gtk_entry_new ();
			gtk_entry_set_max_length ((GtkEntry *)num_text[j], 16);
			gtk_table_attach_defaults (GTK_TABLE (table), num_text[j], 1, 2, i, i+1);
			gtk_widget_show (num_text[j]);

			num_combo[j] = gtk_combo_box_new_text();
			gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "RMS");
			gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "POW");
			gtk_combo_box_append_text(GTK_COMBO_BOX(num_combo[j]), "Peak");
			gtk_combo_box_set_active(GTK_COMBO_BOX(num_combo[j]), 0);
			gtk_table_attach_defaults (GTK_TABLE (table), num_combo[j], 2, 3, i, i+1);
			gtk_widget_show (num_combo[j]);

			/* Add only in case of analogs are in fix point format */
		        if(cfg_info->cfg_af == 0)
			{
				memset(line,'\0',sizeof(line));
				sprintf(line, "Max Value %d : ", j);
				label = gtk_label_new (line);
				gtk_table_attach_defaults (GTK_TABLE (table), label, 3, 4, i  , i+1);
				gtk_widget_show (label);

				num_text1[j] = gtk_entry_new ();
				gtk_entry_set_max_length ((GtkEntry *)num_text1[j], 16);
				gtk_table_attach_defaults (GTK_TABLE (table), num_text1[j], 4, 5, i  , i+1);
				gtk_widget_show (num_text1[j]);
				if(cfg_info->cfg_af == 1) gtk_widget_set_sensitive(GTK_WIDGET(num_text1[j]), FALSE);
			}
			i = i+1;
		}
        
          	if (temp_local < table_rows)
          	{
               		for (jj = i; jj < table_rows; jj++)
               		{
                    		label = gtk_label_new ("-- ");
                    		gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 5, jj, jj+1);
                    		gtk_widget_show (label);
               		}
          	}

		/* Signal handling for Next button on Window */
		g_signal_connect_swapped (next_button, "clicked", G_CALLBACK (validation_new_analog_names), NULL);
		g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), AnalogChWin);
		g_signal_connect_swapped (AnalogChWin, "response", G_CALLBACK (gtk_widget_destroy), AnalogChWin);

		/* This makes it so the button is the default */
		gtk_widget_set_can_default (next_button, TRUE);
		gtk_widget_set_can_default (cancel_button, TRUE);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (AnalogChWin)->action_area), next_button, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (GTK_DIALOG (AnalogChWin)->action_area), cancel_button, TRUE, TRUE, 0);

		/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
		gtk_widget_grab_default (next_button);
		gtk_widget_show (next_button);
		gtk_widget_grab_default (cancel_button);
		gtk_widget_show (cancel_button);

		/* Finally show the AnalogChWin */
		gtk_widget_show (AnalogChWin);
	}
	else	/* if new_phasor < 0 */
	{
		/* Next call to data rate change option? */
		change_data_rate_option();
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  change_data_rate_option()						*/
/* It will ask user to choose wether he wants to change data rate or not?	*/
/* ----------------------------------------------------------------------------	*/

void change_data_rate_option()
{
	/* local variables */
	GtkWidget *table;
	GtkWidget *Addbutton, *Removebutton;
	GtkWidget *close_butn, *label;
	GSList *group;

	/* Create a new dialog window for PMU Data Rate */ 
	CCWin = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (CCWin), "PMU Data Rate ");
	gtk_container_set_border_width (GTK_CONTAINER (CCWin), 10);

	/* Create a table of 4 by 2 squares */
	table = gtk_table_new (6, 2, FALSE);

	/* Set the spacing to 15 on x and 25 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 2);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(CCWin)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	label = gtk_label_new ("Do you want to change data rate?");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);

	/* Add few radio buttons on the dialog window */
	Addbutton = gtk_radio_button_new_with_label (NULL, "YES");
	gtk_table_attach_defaults (GTK_TABLE (table), Addbutton, 0, 1, 2, 3);
     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (Addbutton), TRUE);
     gtk_widget_show (Addbutton);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (Addbutton));
	Removebutton = gtk_radio_button_new_with_label (NULL, "NO");
	gtk_table_attach_defaults (GTK_TABLE (table), Removebutton, 0, 1, 3, 4);
	gtk_widget_show (Removebutton);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Changed data rate will be mentioned in the new CFG\nand will be informed to communicating PDCs.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 5, 6);
	gtk_widget_show (label);
	g_free (markup);

	/* Add a "close" button to the bottom of the dialog */
	close_butn = gtk_button_new_with_label ("Close");

	/* Signal handling for buttons on Data rate Window */
	g_signal_connect_swapped (Addbutton, "clicked", G_CALLBACK (enter_new_data_rate), NULL);
	g_signal_connect_swapped (Removebutton, "clicked", G_CALLBACK (final_cfg_update_call), NULL);
	g_signal_connect_swapped (close_butn, "clicked", G_CALLBACK (gtk_widget_destroy), CCWin);
	g_signal_connect_swapped (CCWin, "response", G_CALLBACK (gtk_widget_destroy), CCWin);

	/* This makes it so the button is the default */
	gtk_widget_set_can_default (close_butn, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), close_butn, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (close_butn);
	gtk_widget_show (close_butn);

	/* Finally show the CCWin */
	gtk_widget_show (CCWin);
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  enter_new_data_rate(GtkWidget *widget, gpointer udata)		     */
/* If user wants to change data rate then it will show him a window for		*/
/* entring new data rate.							                    */
/* ----------------------------------------------------------------------------	*/

void enter_new_data_rate(GtkWidget *widget, gpointer udata)
{
	/* local variables */
	GtkWidget *table, *cancel_button;
	GtkWidget *next_butn, *label;

	/* Create a new dialog window for the New Data Rate */ 
	gtk_widget_destroy(CCWin); 				
	CCWin = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (CCWin), "New Frame Rate");
	gtk_container_set_border_width (GTK_CONTAINER (CCWin), 10);

	/* Create a table of 3 by 3 squares */
	table = gtk_table_new (5, 2, FALSE);

	/* Set the spacing to 25 on x and 40 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 10);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(CCWin)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	/* Add a "next" button to the bottom of the dialog */
	next_butn = gtk_button_new_with_label ("Next");
	cancel_button = gtk_button_new_with_label ("Cancel");

	label = gtk_label_new ("Select New Data Rate");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
	gtk_widget_show (label);

	/* Create combo boxe for user with some fixed values */
	p_drate = gtk_combo_box_new_text();
    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "1");
	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "25");
    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "30");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "50");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "60");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "100");
     	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "120");
     	gtk_combo_box_set_active(GTK_COMBO_BOX(p_drate), 3);
	gtk_table_attach_defaults (GTK_TABLE (table), p_drate, 1, 2, 0, 1);
	gtk_widget_show (p_drate);
/*
	if(cfg_info->cfg_fnom == 1)
     {
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "1");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "10");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "25");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "50");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "100");
     	gtk_combo_box_set_active(GTK_COMBO_BOX(p_drate), 2);
     }
	else
     {
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "1");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "10");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "12");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "15");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "30");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "60");
	    	gtk_combo_box_append_text(GTK_COMBO_BOX(p_drate), "120");
     	gtk_combo_box_set_active(GTK_COMBO_BOX(p_drate), 4);
     }
*/

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Frames would be send as per the new data rate.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 4, 5);
	gtk_widget_show (label);
	g_free (markup);

	/* Signal handling for buttons on Data Rate Window */
	g_signal_connect (next_butn, "clicked", G_CALLBACK (validation_new_data_rate), NULL);
	g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), CCWin);
	g_signal_connect_swapped (CCWin, "response", G_CALLBACK (gtk_widget_destroy), CCWin);

	/* This makes it so the button is the default */
	gtk_widget_set_can_default (next_butn, TRUE);
	gtk_widget_set_can_default (cancel_button, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), next_butn, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), cancel_button, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (next_butn);
	gtk_widget_show (next_butn);
	gtk_widget_grab_default (cancel_button);
	gtk_widget_show (cancel_button);

	/* Finally show the CCWin */
	gtk_widget_show (CCWin);
};	


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  remove_phasor_num (GtkWidget *widget, gpointer udata)		     */
/* If wish to remove some of phasor channels from CFG then it provides this	*/
/* option, but it should be lessthan maximum phasor numbers.			     */
/* ----------------------------------------------------------------------------	*/

void remove_phasor_num (GtkWidget *widget, gpointer udata)
{
	/* local variables */
	char line[50], buff[10];
	GtkWidget *table, *label;
	GtkWidget *next_butn, *cancel_button;

	/* Allocate the memory for the ConfigurationChange object */
	new_cfg_info = malloc(sizeof(struct ConfigurationChange));
	new_cfg_info->add_remove_choice = 2; 

	/* Create a new dialog window for the Remove Phasor */ 
	gtk_widget_destroy(CCWin); 				
	CCWin = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (CCWin), "Phasor Removal");
	gtk_container_set_border_width (GTK_CONTAINER (CCWin), 10);

	/* Create a table of 5 by 3 squares */
	table = gtk_table_new (3, 3, FALSE);

	/* Set the spacing to 5 on x and 40 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table),8);
	gtk_table_set_col_spacings (GTK_TABLE (table),2);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(CCWin)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	/* Add a "next" button to the bottom of the dialog */
	next_butn = gtk_button_new_with_label ("Next");
	cancel_button = gtk_button_new_with_label ("Cancel");

	/* Create text boxes for user to enter appropriate values */
	label = gtk_label_new ("Phasors to be remove");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
	gtk_widget_show (label);

	label = gtk_label_new ("Analogs to be remove");
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 1, 2);
	gtk_widget_show (label);

	/* Create text boxes for user to enter appropriate values */
	phtext = gtk_entry_new ();
	gtk_table_attach_defaults (GTK_TABLE (table), phtext, 1, 2, 0, 1);
	gtk_widget_show (phtext);

	angtext = gtk_entry_new ();
	gtk_table_attach_defaults (GTK_TABLE (table), angtext, 1, 2, 1, 2);
	gtk_widget_show (angtext);

	memset(line, '\0', 50);
	memset(buff, '\0', 10);
	strcat(line, "< ");    		
	sprintf(buff,"%d",cfg_info->cfg_phnmr_val);
	strcat(line, buff);    		
	label = gtk_label_new (line);
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 2, 3, 0, 1);
	gtk_widget_show (label);

	memset(line, '\0', 50);
	memset(buff, '\0', 10);
	strcat(line, "<= ");    		
	sprintf(buff,"%d",cfg_info->cfg_annmr_val);
	strcat(line, buff);    		
	label = gtk_label_new (line);
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 2, 3, 1, 2);
	gtk_widget_show (label);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : New channels will be added to existing configuration frame.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
     gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 2, 3);
	gtk_widget_show (label);
	g_free (markup);

	/* Signal handling for buttons on Window */
	g_signal_connect (next_butn, "clicked", G_CALLBACK (validation_remove_ph_an_num), NULL);
	g_signal_connect_swapped (cancel_button, "clicked", G_CALLBACK (gtk_widget_destroy), CCWin);
	g_signal_connect_swapped (CCWin, "response", G_CALLBACK (gtk_widget_destroy), CCWin);

	/* This makes it so the button is the default */
	gtk_widget_set_can_default (next_butn, TRUE);
	gtk_widget_set_can_default (cancel_button, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), next_butn, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (CCWin)->action_area), cancel_button, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (next_butn);
	gtk_widget_show (next_butn);
	gtk_widget_grab_default (cancel_button);
	gtk_widget_show (cancel_button);

	/* Finally show the CCWin */
	gtk_widget_show (CCWin);
};	


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  final_cfg_update_call (GtkButton *but, gpointer udata)		     */
/* It finally call the 'reconfig_cfg_CC()' for recreate configuration frame. 	*/
/* ----------------------------------------------------------------------------	*/

int final_cfg_update_call (GtkButton *but, gpointer udata)
{
	new_cfg_info->data_rate_choice = 0; 
	new_cfg_info->new_data_rate = 0;

	/* Close/destroy the CCWin */
	gtk_widget_destroy(CCWin); 				

	/* Final call to recreate configuration frame */
	reconfig_cfg_CC();

	return 0;
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  hdr_create_function (GtkWidget *widget, gpointer udata)		     */
/* It will take the PMU information and make Header Frame.			          */
/* ----------------------------------------------------------------------------	*/

void hdr_create_function (GtkWidget *widget, gpointer udata)
{
	/* local variables */
	GtkWidget *table, *label;
	GtkWidget *close_butn, *vald_butn;

	/* Create a new dialog window for the Header Frame Window */ 
	hdr_frame_window = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (hdr_frame_window), "Header Frame");
	gtk_container_set_border_width (GTK_CONTAINER (hdr_frame_window), 10);

	/* Create a table of 3 by 2 squares */
	table = gtk_table_new (3, 2, FALSE);

	/* Set the spacing to 30 on x */
	gtk_table_set_row_spacings (GTK_TABLE (table), 10);

	/* Pack the table into the window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(hdr_frame_window)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#990033\" font='12'><b>Enter Header Frame Details</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);
	g_free (markup);

	/* Create text boxes for user to enter appropriate values */
	HdrFrm = gtk_text_view_new ();//gtk_entry_new();
	gtk_table_attach_defaults (GTK_TABLE (table), HdrFrm, 0, 1, 1, 2);
	gtk_widget_show (HdrFrm);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Header frame includes the information about the PMU, the data sources,\nscaling, algorithms, filtering, or other related information.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 2, 3);
	gtk_widget_show (label);
	g_free (markup);

	/* Add few buttons at the bottom of the dialog */
	vald_butn = gtk_button_new_with_label ("Create");
	close_butn = gtk_button_new_with_label ("Close");

	/* Signal handling for buttons on Window */
	g_signal_connect (vald_butn, "clicked", G_CALLBACK (validation_hdr_frm), (gpointer) "2");
	g_signal_connect_swapped (close_butn, "clicked", G_CALLBACK (gtk_widget_destroy), hdr_frame_window);
	g_signal_connect_swapped (hdr_frame_window, "response", G_CALLBACK (gtk_widget_destroy), hdr_frame_window);

	/* This makes it so the button is the default */
	gtk_widget_set_can_default (vald_butn, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (hdr_frame_window)->action_area), vald_butn, TRUE, TRUE, 0);
	gtk_widget_set_can_default (close_butn, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (hdr_frame_window)->action_area), close_butn, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (vald_butn);
	gtk_widget_show (vald_butn);
	gtk_widget_grab_default (close_butn);
	gtk_widget_show (close_butn);

	/* Finally show the hdr_frame_window */
	gtk_widget_show (hdr_frame_window);
};	

/*************************************** End of Program ***********************************************/ 
