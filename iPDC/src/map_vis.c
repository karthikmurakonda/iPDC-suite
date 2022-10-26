#include <stdlib.h>
#include <pthread.h>
#include "global.h"
#include "align_sort.h"
#include "map_vis.h"
#include "parser.h"
#include "osm-gps-map.h"
#include "Attack_detect.h"
#include "connections.h"
#include "livechart.h"
#include "utility_tools.h"


//  debug
int loops = 0;

gboolean update_images(gpointer* pars){
    int match=0,id;
    myParameters* parameters = (myParameters*) pars;

    pthread_mutex_lock(&mutex_on_TSB);
    struct data_frame *df = TSB[0].first_data_frame;
    struct Lower_Layer_Details *LLptr;

    if (df == NULL){
        return TRUE;
    }
    if (curr_measurement==0)
    {
        int i = 0, k = 0;
        float freq;
        while (df!=NULL){
            freq = to_intconvertor(df->dpmu[i]->freq)*0.001+50;
            live_chart_serie_add(serie, freq*1e-1);
            loops++;
            printf("loops: %d\n", loops);
            
            id = to_intconvertor(df->idcode);
            printf("id = %d\n",id);

            LLptr = LLfirst;
            match = 0;
            while(LLptr != NULL){
                printf("pmuid = %d\n",LLptr->pmuid);
                if(LLptr->pmuid == id){
                    match = 1;
                    break;
                }
                LLptr = LLptr->next;
            }

            if(match == 1){
                float lat = 79.347312;
                float lon = -69.439209;
                float freq = to_intconvertor(df->dpmu[i]->freq)*0.001+50;
                printf("lat = %f, lon = %f, freq = %f\n",lat,lon,freq);
                gboolean green =attack_detect(df,&START,&COUNT,&SUM_OF_FREQUENCY);
                // if(parameters->g_last_image != 0){
                //     // osm_gps_map_image_remove(parameters->util_map, parameters->g_last_image);
                // }
                if (freq > 50.300){
                    parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                }else{
                    parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                }
            //    if(parameters->g_last_image != 0){
            //         // osm_gps_map_image_remove(parameters->util_map, parameters->g_last_image);
            //     }
            //     if (green){
            //         parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_green_image);
            //     }else{
            //         parameters->g_last_image = osm_gps_map_image_add(parameters->util_map,15.518597, 74.925584, parameters->g_red_image);
            //     }
            }
            df = df->dnext;
            // i++;
            k++;
        }
    }
    pthread_mutex_unlock(&mutex_on_TSB);

    gtk_widget_queue_draw(GTK_WIDGET(parameters->util_map));
    return TRUE;
}
 
