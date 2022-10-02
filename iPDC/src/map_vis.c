#include <stdlib.h>
#include "align_sort.h"
#include "map_vis.h"
#include "parser.h"
#include "osm-gps-map.h"
#include "Attack_detect.h"


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