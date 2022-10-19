#include<gtk/gtk.h>
#include<osm-gps-map.h>
typedef struct{
    OsmGpsMap *util_map;
    GdkPixbuf *g_red_image;
    GdkPixbuf *g_green_image;
    OsmGpsMapImage *g_last_image;
    int current_selection;
} myParameters; 



gboolean update_images(gpointer* pars);