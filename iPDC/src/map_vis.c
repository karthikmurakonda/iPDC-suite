#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include "global.h"
#include "align_sort.h"
#include "map_vis.h"
#include "parser.h"
#include "osm-gps-map.h"
#include "Attack_detect.h"
#include "Kmeans.h"
#include "Dynamic_time_warping.h"
#include "connections.h"
#include "livechart.h"
#include "utility_tools.h"


//  debug
int loops = 0;

gboolean update_images(gpointer* pars){
    int match=0,id, cfg_match=0;
    myParameters* parameters = (myParameters*) pars;
    struct cfg_frame* temp_cfg;

    pthread_mutex_lock(&mutex_on_TSB);
    struct data_frame *df = TSB[MAXTSB-1].first_data_frame;
    struct Lower_Layer_Details *LLptr;
    struct vis_data *vis_ptr;

    if (df == NULL){
        pthread_mutex_unlock(&mutex_on_TSB);
        return TRUE;
    }
    //int freq = to_intconvertor(df->dpmu[0]->freq);
	//gboolean green =attack_detect(df);
    //printf("map_vis A: %Lf, B: %Lf,C: %Lf\n",A,B,C);
    //gboolean green = kmeans(df);
   
    int i = 0, k = 0;
    float freq,vol_magnitude,angle, dfreq;
    unsigned char freq_fmt, anal_fmt, phas_fmt, polar_fmt;
    while (df!=NULL){
        float lat;
        float lon;
        loops++;
        printf("loops: %d\n", loops);
        
        id = to_intconvertor(df->idcode);
        printf("id = %d\n",id);
        pthread_mutex_lock(&mutex_cfg);
        temp_cfg = cfgfirst;
        // Check for the IDCODE in Configuration Frame
        while(temp_cfg != NULL){
            if(id == to_intconvertor(temp_cfg->idcode)){
                cfg_match = 1;
                printf("Matched - id : %d\n",id);
                freq_fmt = temp_cfg->pmu[0]->fmt->freq;
                anal_fmt = temp_cfg->pmu[0]->fmt->analog;
                phas_fmt = temp_cfg->pmu[0]->fmt->phasor;
                polar_fmt = temp_cfg->pmu[0]->fmt->polar;
                break;	
            } else {
                temp_cfg = temp_cfg->cfgnext;
            }
        }
        pthread_mutex_unlock(&mutex_cfg);

        // get data from df.
        if(freq_fmt == '1'){
            freq = decode_ieee_single(df->dpmu[i]->freq);
            printf("freq = %f\n",freq);
        }else{
            freq = to_intconvertor(df->dpmu[i]->freq)*1e-6+50;
        }
        
        unsigned char first2bytes[2];
        strncpy(first2bytes, df->dpmu[i]->phasors[0], 2);
        unsigned char last2bytes[2];
        strncpy(last2bytes, df->dpmu[i]->phasors[0]+2, 2);
        vol_magnitude = to_intconvertor(first2bytes);
        float imaginary = to_intconvertor(last2bytes);
        printf("vol = %f imag = %f\n",vol_magnitude, imaginary);

        vis_ptr = vis_data_head;
        match = 0;
        while(vis_ptr != NULL){
            printf("vis_ptr->id = %d\n",vis_ptr->id);
            if(vis_ptr->id == id){
                match = 1;
                printf("Matched - id : %d\n",id);
                break;
            }
            vis_ptr = vis_ptr->next;
        }

        lat = vis_ptr->lat;
        lon = vis_ptr->lon;

        live_chart_serie_add(vis_ptr->serie_freq, freq);
        live_chart_serie_add(vis_ptr->serie_vol, vol_magnitude);
        live_chart_serie_add(vis_ptr->serie_dfreq, dfreq);

        if(match == 1 && cfg_match == 1){
            printf("lat = %f, lon = %f, freq = %f\n",lat,lon,freq);
            if(vis_ptr->last_image != 0){
                osm_gps_map_image_remove(parameters->util_map, vis_ptr->last_image);
            }

            if(curr_measurement == 0){
                if(vol_magnitude > 65600 || vol_magnitude < 65300){
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                }else{
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                }
            }else if(curr_measurement == 1){
                if (freq > 300){
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                }else{
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                }
            }else if(curr_measurement == 2){
                if (dfreq < 0.5){
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                }else{
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                }
            }else if(curr_measurement == 3){
                if(algorithm==0 && dimmension == 0){
                    if (!attack_detect_vol(df)){
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                    }else{
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                    }
                }else if (algorithm==0 && dimmension == 1){
                    if (!attack_detect_freq(df)){
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                    }else{
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                    }
                }else if (algorithm==0 && dimmension == 2){
                    if (!attack_detect_freq_vol(df)){
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                    }else{
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                    }
                }else if (algorithm==1 && dimmension == 0){

                }else if (algorithm==1 && dimmension == 1){

                }else if (algorithm==1 && dimmension == 2){

                }else if (algorithm==2 && dimmension == 0){
                    if(!DTWvolDistance(df)){
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                    }else{
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                    }
                }else if (algorithm==2 && dimmension == 1){
                    if(!DTWfreqDistance(df)){
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_red_image);
                    }else{
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map,lat, lon, parameters->g_green_image);
                    }
                }else if (algorithm==2 && dimmension == 2){

                }
            }
        }
        df = df->dnext;
        // i++;
        k++;
    }
    pthread_mutex_unlock(&mutex_on_TSB);

    gtk_widget_queue_draw(GTK_WIDGET(parameters->util_map));
    return TRUE;
}
 
