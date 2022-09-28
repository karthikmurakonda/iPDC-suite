#include<gtk/gtk.h>
#include "utility_tools.h"
#include "align_sort.h"
#include "parser.h"
#include "Attack_detect.h"

#define UI_fILE "./utility_tools.ui"



// void change_image(OsmGpsMap *map, float lat, float lon, OsmGpsMapImage *image)
// {
//     if (g_last_image)
//         osm_gps_map_image_remove(map, g_last_image);
//     osm_gps_map_image_add(map, lat, lon, image);
// }

typedef struct{
    OsmGpsMap *util_map;
    GdkPixbuf *g_red_image;
    GdkPixbuf *g_green_image;
    OsmGpsMapImage *g_last_image;
} myParameters; 

gboolean update_images(gpointer* pars){
    myParameters* parameters = (myParameters*) pars;
    struct data_frame *df = TSB[0].first_data_frame;
    if(parameters->g_green_image == NULL){
        return FALSE;
    }
    if (parameters->util_map == NULL){
        return FALSE;
    }
    if (df == NULL){
        return FALSE;
    }
    int freq = to_intconvertor(df->dpmu[0]->freq);
	gboolean green =attack_detect(df,&START,&COUNT,&SUM_OF_FREQUENCY);
    if(parameters->util_map != NULL){

        // if(parameters->g_last_image != 0){
        //     osm_gps_map_image_remove(parameters->util_map, parameters->g_last_image);
        // }
        // if (freq > 300){
        //     parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_green_image);
        // }else{
        //     parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_green_image);
        // }
       if(parameters->g_last_image != 0){
            osm_gps_map_image_remove(parameters->util_map, parameters->g_last_image);
        }
        if (green){
            parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_green_image);
        }else{
            parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_red_image);
        }

    }
    gtk_widget_queue_draw(GTK_WIDGET(parameters->util_map));
    return TRUE;
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
    g_red_image = gdk_pixbuf_new_from_file_at_size ("red.png", 24,24,NULL);
    g_green_image = gdk_pixbuf_new_from_file_at_size ("green.png", 24,24,NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "util_window"));
    GtkContainer *map_container = GTK_CONTAINER(gtk_builder_get_object(builder, "map_layout"));

    

    util_map = g_object_new (OSM_TYPE_GPS_MAP,
                                  "map-source", OSM_GPS_MAP_SOURCE_GOOGLE_HYBRID,
                                  "tile-cache", "/tmp/",
                                  NULL);
    osm_gps_map_set_center_and_zoom (util_map, 15.4589, 75.0078, 10);

    g_last_image =  osm_gps_map_image_add(util_map,15.4589, 75.0078, g_red_image);
    g_last_image =  osm_gps_map_image_add(util_map,15.518597, 74.925584, g_green_image);
    myParameters parameters = {util_map, g_red_image, g_green_image, g_last_image};
    gpointer data = (gpointer) &parameters;
    g_timeout_add(20, (GSourceFunc) update_images, data);

    gtk_widget_set_size_request(GTK_WIDGET(util_map), 600, 500);

    gtk_container_add(map_container, GTK_WIDGET(util_map));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}