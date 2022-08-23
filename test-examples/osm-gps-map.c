#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gthread.h>
#include <cairo/cairo.h>
#include <glib/gprintf.h>

#include <osm-gps-map.h>

int main (int argc, char **argv)
{
    g_thread_init(NULL);
    gtk_init (&argc, &argv);
    OsmGpsMapSource_t source = OSM_GPS_MAP_SOURCE_OPENSTREETMAP;

    if ( !osm_gps_map_source_is_valid(source) )
        return 1;

    GtkWidget *map = g_object_new (OSM_TYPE_GPS_MAP,
                     "map-source", source,
                     "tile-cache", "/tmp/",
                      NULL);
    GtkWidget *w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_add (GTK_CONTAINER(w), map);
    gtk_widget_show_all (w);

    gtk_main ();
    return 0;
}