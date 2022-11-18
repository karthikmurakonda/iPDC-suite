/* -----------------------------------------------------------------------------
 * data_vis.c
 *
 * iPDC - Phasor Data Concentrator
 *
 * Copyright (C) 2022-2023 Nitesh Pandit
 * Copyright (C) 2022-2023 Kedar V. Khandeparkar
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Authors:
 *		M V Karthik <mvkarthik@gmail.com>
 *		Pavan Kumar V Patil <pavanvpatil01@gmail.com>
 *
 * ----------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include "global.h"
#include "align_sort.h"
#include "data_vis.h"
#include "parser.h"
#include "osm-gps-map.h"
#include "Attack_detect.h"
#include "Kmeans.h"
#include "Dynamic_time_warping.h"
#include "connections.h"
#include "livechart.h"
#include "utility_tools.h"
#include "Kmeans2.h"

int loops = 0;

gboolean update_vis(gpointer *pars)
{
    int match = 0, id, cfg_match = 0;
    myParameters *parameters = (myParameters *)pars;
    struct cfg_frame *temp_cfg;

    pthread_mutex_lock(&mutex_on_TSB);
    struct data_frame *df = TSB[MAXTSB - 1].first_data_frame;
    struct Lower_Layer_Details *LLptr;
    struct vis_data *vis_ptr;

    if (df == NULL)
    {
        pthread_mutex_unlock(&mutex_on_TSB);
        return TRUE;
    }

    int i = 0, k = 0;
    float freq, vol_magnitude, angle, dfreq;
    unsigned char freq_fmt, anal_fmt, phas_fmt, polar_fmt;
    while (df != NULL)
    {
        float lat;
        float lon;
        loops++;
        id = to_intconvertor(df->idcode);
        pthread_mutex_lock(&mutex_cfg);
        temp_cfg = cfgfirst;
        while (temp_cfg != NULL)
        {
            if (id == to_intconvertor(temp_cfg->idcode))
            {
                cfg_match = 1;
                freq_fmt = temp_cfg->pmu[0]->fmt->freq;
                anal_fmt = temp_cfg->pmu[0]->fmt->analog;
                phas_fmt = temp_cfg->pmu[0]->fmt->phasor;
                polar_fmt = temp_cfg->pmu[0]->fmt->polar;
                break;
            }
            else
            {
                temp_cfg = temp_cfg->cfgnext;
            }
        }
        pthread_mutex_unlock(&mutex_cfg);

        // get data from df.
        if (freq_fmt == '1')
        {
            freq = decode_ieee_single(df->dpmu[i]->freq);
            printf("freq = %f\n", freq);
        }
        else
        {
            freq = to_intconvertor(df->dpmu[i]->freq) * 1e-3 + 50;
            printf("freq = %f\n", freq);
        }

        unsigned char first2bytes[2];
        strncpy(first2bytes, df->dpmu[i]->phasors[0], 2);
        unsigned char last2bytes[2];
        strncpy(last2bytes, df->dpmu[i]->phasors[0] + 2, 2);
        vol_magnitude = to_intconvertor(first2bytes);
        float imaginary = to_intconvertor(last2bytes);

        vis_ptr = vis_data_head;
        match = 0;
        while (vis_ptr != NULL)
        {
            if (vis_ptr->id == id)
            {
                match = 1;
                break;
            }
            vis_ptr = vis_ptr->next;
        }

        lat = vis_ptr->lat;
        lon = vis_ptr->lon;

        live_chart_serie_add(vis_ptr->serie_freq, freq);
        live_chart_serie_add(vis_ptr->serie_vol, vol_magnitude);
        live_chart_serie_add(vis_ptr->serie_dfreq, dfreq);

        if (match == 1 && cfg_match == 1)
        {
            if (vis_ptr->last_image != 0)
            {
                osm_gps_map_image_remove(parameters->util_map, vis_ptr->last_image);
            }

            if (curr_measurement == 0)
            {
                if (vol_magnitude > 65600 || vol_magnitude < 65300)
                {
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                }
                else
                {
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                }
            }
            else if (curr_measurement == 1)
            {
                if (freq > 50.3)
                {
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                }
                else
                {
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                }
            }
            else if (curr_measurement == 2)
            {
                if (dfreq < 0.5)
                {
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                }
                else
                {
                    vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                }
            }
            else if (curr_measurement == 3)
            {
                if (algorithm == 0 && dimmension == 0)
                {
                    if (!attack_detect_freq(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
                else if (algorithm == 0 && dimmension == 1)
                {
                    if (!attack_detect_vol(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
                else if (algorithm == 0 && dimmension == 2)
                {
                    if (!attack_detect_freq_vol(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
                else if (algorithm == 1 && dimmension == 0)
                {
                    if (!kmeans(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
                else if (algorithm == 1 && dimmension == 1)
                {
                }
                else if (algorithm == 1 && dimmension == 2)
                {
                    if (!Kmeans2(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
                else if (algorithm == 2 && dimmension == 0)
                {
                    if (!DTWfreqDistance(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
                else if (algorithm == 2 && dimmension == 1)
                {
                    if (!DTWvolDistance(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
                else if (algorithm == 2 && dimmension == 2)
                {
                    if (!DTWfreqvolDistance(df))
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_red_image);
                    }
                    else
                    {
                        vis_ptr->last_image = osm_gps_map_image_add(parameters->util_map, lat, lon, parameters->g_green_image);
                    }
                }
            }
        }
        df = df->dnext;
        k++;
    }
    pthread_mutex_unlock(&mutex_on_TSB);

    gtk_widget_queue_draw(GTK_WIDGET(parameters->util_map));
    return TRUE;
}
