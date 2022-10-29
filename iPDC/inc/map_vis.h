#include<gtk/gtk.h>
#include<osm-gps-map.h>
#include "livechart.h"

typedef struct{
    OsmGpsMap *util_map;
    GdkPixbuf *g_red_image;
    GdkPixbuf *g_green_image;
    OsmGpsMapImage *g_last_image;
    int current_selection;
} myParameters; 

struct vis_data{
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

gboolean update_images(gpointer* pars);