#include<gtk/gtk.h>
#include "align_sort.h"
#include "parser.h"
#include "osm-gps-map.h"
#include "map_vis.h"

#define UI_fILE "./assets/utility_tools.ui"
#define RED_IMAGE "./assets/red.png"
#define GREEN_IMAGE "./assets/green.png"



// void change_image(OsmGpsMap *map, float lat, float lon, OsmGpsMapImage *image)
// {
//     if (g_last_image)
//         osm_gps_map_image_remove(map, g_last_image);
//     osm_gps_map_image_add(map, lat, lon, image);
// }

// on closing the window kill the g_timeout_add
void on_window_destroy(GtkWidget *widget, gpointer data)
{
    g_source_remove(GPOINTER_TO_UINT(data));
    gtk_main_quit();
}

void utility_tools(GtkButton *but, gpointer udata)
{
    OsmGpsMap *util_map;
    GtkWidget *window;
    GdkPixbuf *g_red_image;
    GdkPixbuf *g_green_image;
    OsmGpsMapImage *g_last_image;
    // -------------------
    GtkBuilder *builder;
    GError     *error = NULL;
    builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(builder, UI_fILE, &error))
	{
		g_warning("%s", error->message);
		g_free(error);
	}
    g_red_image = gdk_pixbuf_new_from_file_at_size (RED_IMAGE, 24,24,NULL);
    g_green_image = gdk_pixbuf_new_from_file_at_size (GREEN_IMAGE, 24,24,NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "util_window"));
    GtkContainer *map_container = GTK_CONTAINER(gtk_builder_get_object(builder, "map_layout"));

    

    util_map = g_object_new (OSM_TYPE_GPS_MAP,
                                  "map-source", OSM_GPS_MAP_SOURCE_OPENSTREETMAP,
                                  "tile-cache", "/tmp/",
                                  NULL);
    osm_gps_map_set_center_and_zoom (util_map, 15.4589, 75.0078, 10);

    //g_last_image =  osm_gps_map_image_add(util_map,15.4589, 75.0078, g_red_image);
    g_last_image =  osm_gps_map_image_add(util_map,15.518597, 74.925584, g_green_image);
    myParameters parameters = {util_map, g_red_image, g_green_image, g_last_image};
    gpointer data = (gpointer) &parameters;
    guint pid = g_timeout_add(20, (GSourceFunc) update_images, data);

    gtk_widget_set_size_request(GTK_WIDGET(util_map), 600, 500);

    gtk_container_add(map_container, GTK_WIDGET(util_map));
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), GUINT_TO_POINTER(pid));

    gtk_widget_show_all(window);
    gtk_main();
}