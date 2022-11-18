/* -----------------------------------------------------------------------------
 * data_vis.h
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
 *		M V Karthik <karthik.murakonda14@gmail.com>
 *		Pavan Kumar V Patil <pavanvpatil01@gmail.com>
 *
 * ----------------------------------------------------------------------------- */

#include <gtk/gtk.h>
#include <osm-gps-map.h>
#include "livechart.h"

typedef struct
{
    OsmGpsMap *util_map;
    GdkPixbuf *g_red_image;
    GdkPixbuf *g_green_image;
    OsmGpsMapImage *g_last_image;
    int current_selection;
} myParameters;

struct vis_data
{
    int id;
    float lat;
    float lon;
    GdkColor color;
    OsmGpsMapImage *last_image; // TODO:
    GTimeVal last_update_time;
    LiveChartSerie *serie_vol;
    LiveChartSerie *serie_freq;
    LiveChartSerie *serie_dfreq;
    struct vis_data *next;
};

struct vis_data *vis_data_head;

gboolean update_vis(gpointer *pars);