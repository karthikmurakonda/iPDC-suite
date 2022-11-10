#include <stdio.h>
#include<gtk/gtk.h>
#include "osm-gps-map.h"
#include "livechart.h"

/* Convenience macros for obtaining objects from UI file */
#define CH_GET_OBJECT(builder, name, type, data) \
		data->name = type(gtk_builder_get_object (builder, #name) )
#define CH_GET_WIDGET(builder, name, data) \
		CH_GET_OBJECT(builder, name, GTK_WIDGET, data)

/* Main data structure definition */
typedef struct _UtData UtData;

struct _UtData
{
	/* Main window */
	GtkWidget *util_window;  

	/* ui buttons */
	GtkWidget *voltage;
	GtkWidget *frequency;
	GtkWidget *dfreq;
	GtkWidget *attack_detection;
	GtkWidget *algorithm;
	GtkWidget *dimmension;
	OsmGpsMap *util_map;
	GtkContainer *map_layout;
	GtkContainer *graph_layoutvol;
	GtkContainer *graph_layoutfreq;
	GtkContainer *graph_layoutdfreq;
	GtkLabel *algo_label;
	GtkLabel *dimm_label;
	GtkBox * ml_vol;
	GtkBox * ml_freq;
	GtkBox * ml_dfreq;
	GtkBox * ml_ad;
	GtkWidget *swvol;
	GtkWidget *swfreq;
	GtkWidget *swdfreq;
	GtkWidget *swad;
	GtkWidget *maplabel;
	GtkWidget *graphlabel;
	GtkWidget *gl1;
	GtkWidget *gl2;
	GtkWidget *gl3;
};

UtData *utdata;

// global variables
int curr_measurement;
int algorithm;
int dimmension;



void utility_tools(GtkButton *but, gpointer udata);