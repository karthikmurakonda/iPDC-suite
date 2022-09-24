#include<gtk/gtk.h>
#include "osm-gps-map.h"

/* Convenience macros for obtaining objects from UI file */
#define CH_GET_OBJECT(builder, name, type, data) \
		data->name = type(gtk_builder_get_object (builder, #name) )
#define CH_GET_WIDGET(builder, name, data) \
		CH_GET_OBJECT(builder, name, GTK_WIDGET, data)

OsmGpsMap *util_map;
GtkWidget *window;
static GdkPixbuf *g_red_image = NULL;
static GdkPixbuf *g_green_image = NULL;
static OsmGpsMapImage *g_last_image = NULL;

void utility_tools(GtkButton *but, gpointer udata);