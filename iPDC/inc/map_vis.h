#include<gtk/gtk.h>
#include<osm-gps-map.h>
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
    GdkPixbuf *last_image;
    GTimeVal last_update_time;
    struct vis_data *next;
};

struct vis_data *head;

gboolean update_images(gpointer* pars);