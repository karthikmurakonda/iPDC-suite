#include <stdlib.h>
#include "align_sort.h"
#include "map_vis.h"
#include "parser.h"
#include "osm-gps-map.h"
#include "Attack_detect.h"
#include "connections.h"
// #include "utility_tools.h"


gboolean update_images(gpointer* pars){
    int match=0,id;
    myParameters* parameters = (myParameters*) pars;
    struct data_frame *df = TSB[0].first_data_frame;
    struct Lower_Layer_Details *LLptr;

    if (df == NULL){
        return TRUE;
    }
    // if (curr_measurement==1)
    // {
           while (df != NULL){
           id = to_intconvertor(df->idcode);
            printf("id = %d\n",id);

            LLptr = LLfirst;

            while(LLptr != NULL){
                printf("pmuid = %d\n",LLptr->pmuid);
                if(LLptr->pmuid == id){
                    match = 1;
                    break;
                }
                LLptr = LLptr->next;
            }

            if(match == 1){
                float lat = LLptr->latitude;
                float lon = LLptr->longitude;
                float freq = to_intconvertor(df->dpmu[0]->freq)*0.001+50;
                printf("lat = %f, lon = %f, freq = %f\n",lat,lon,freq);
            // gboolean green =attack_detect(df,&START,&COUNT,&SUM_OF_FREQUENCY);

                if(parameters->g_last_image != 0){
                    osm_gps_map_image_remove(parameters->util_map, parameters->g_last_image);
                }
                if (freq > 50.300){
                    parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                }else{
                    parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                }
            //    if(parameters->g_last_image != 0){
            //         osm_gps_map_image_remove(parameters->util_map, parameters->g_last_image);
            //     }
            //     if (green){
            //         parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_green_image);
            //     }else{
            //         parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_red_image);
            //     }
                

            }
        df = df->dnext;
    }
    // }
    
 

 

    gtk_widget_queue_draw(GTK_WIDGET(parameters->util_map));
    return TRUE;
}