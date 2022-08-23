#include<gtk/gtk.h>
#include "osm-gps-map.h"

#define UI_fILE "./utility_tools.ui"

void utility_tools(GtkButton *but, gpointer udata)
{
    GtkBuilder *builder;
    GError     *error = NULL;
    builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(builder, UI_fILE, &error))
	{
		g_warning("%s", error->message);
		g_free(error);
	}
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "util_window"));
    GtkContainer *map_container = GTK_CONTAINER(gtk_builder_get_object(builder, "map_layout"));

    OsmGpsMap *map = g_object_new (OSM_TYPE_GPS_MAP,
                                  "map-source", OSM_GPS_MAP_SOURCE_OPENSTREETMAP,
                                  "tile-cache", "/tmp/",
                                  NULL);
    gtk_widget_set_size_request(GTK_WIDGET(map), 800, 600);

    gtk_container_add(map_container, GTK_WIDGET(map));

    gtk_widget_show_all(window);
    gtk_main();
}