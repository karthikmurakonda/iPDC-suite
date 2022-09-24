#include<gtk/gtk.h>
#include "utility_tools.h"

#define UI_fILE "./utility_tools.ui"



// void change_image(OsmGpsMap *map, float lat, float lon, OsmGpsMapImage *image)
// {
//     if (g_last_image)
//         osm_gps_map_image_remove(map, g_last_image);
//     osm_gps_map_image_add(map, lat, lon, image);
// }


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
    window = GTK_WIDGET(gtk_builder_get_object(builder, "util_window"));
    GtkContainer *map_container = GTK_CONTAINER(gtk_builder_get_object(builder, "map_layout"));

    

    util_map = g_object_new (OSM_TYPE_GPS_MAP,
                                  "map-source", OSM_GPS_MAP_SOURCE_GOOGLE_HYBRID,
                                  "tile-cache", "/tmp/",
                                  NULL);
    osm_gps_map_set_center_and_zoom (util_map, 15.4589, 75.0078, 10);
    g_red_image = gdk_pixbuf_new_from_file_at_size ("red.png", 24,24,NULL);
    g_green_image = gdk_pixbuf_new_from_file_at_size ("green.png", 24,24,NULL);

    // g_last_image =  osm_gps_map_image_add(util_map,15.4589, 75.0078, g_red_image);
    // g_last_image =  osm_gps_map_image_add(util_map,15.518597, 74.925584, g_green_image);
    gtk_widget_set_size_request(GTK_WIDGET(util_map), 600, 500);

    gtk_container_add(map_container, GTK_WIDGET(util_map));

    gtk_widget_show_all(window);
    gtk_main();
}