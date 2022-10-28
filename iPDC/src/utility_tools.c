#include <gtk/gtk.h>
#include "align_sort.h"
#include "parser.h"
#include "osm-gps-map.h"
#include "map_vis.h"
#include "utility_tools.h"
#include "livechart.h"

#define UI_fILE "./assets/utility_tools.ui"
#define RED_IMAGE "./assets/red.png"
#define GREEN_IMAGE "./assets/green.png"


// void change_image(OsmGpsMap *map, float lat, float lon, OsmGpsMapImage *image)
// {
//     if (g_last_image)
//         osm_gps_map_image_remove(map, g_last_image);
//     osm_gps_map_image_add(map, lat, lon, image);
// }

// on closing the window kill the g_timeout_add
void on_window_destroy(GtkWidget *widget, gpointer data)
{
    g_source_remove(GPOINTER_TO_UINT(data));
    gtk_main_quit();
}

// on clicking the button frequency
void on_frequency_clicked(GtkButton *but, gpointer udata)
{
    curr_measurement = 1;
    gtk_widget_hide(utdata->algorithm);
    gtk_widget_hide(utdata->dimmension);
    gtk_widget_set_sensitive(utdata->voltage, TRUE);
    gtk_widget_set_sensitive(utdata->frequency, FALSE);
    gtk_widget_set_sensitive(utdata->attack_detection, TRUE);

    printf("Frequency\n");
}

// on clicking the button attack_detection
void on_attack_detection_clicked(GtkButton *but, gpointer udata)
{
    curr_measurement = 2;
    gtk_widget_set_sensitive(utdata->voltage, TRUE);
    gtk_widget_set_sensitive(utdata->frequency, TRUE);
    gtk_widget_set_sensitive(utdata->attack_detection, FALSE);

    gtk_widget_show(utdata->algorithm);
    gtk_widget_show(utdata->dimmension);
    printf("Attack Detection\n");
}

// on clicking the button voltage
void on_voltage_clicked(GtkButton *but, gpointer udata)
{
    curr_measurement = 0;
    gtk_widget_hide(utdata->algorithm);
    gtk_widget_hide(utdata->dimmension);
    gtk_widget_set_sensitive(utdata->voltage, FALSE);
    gtk_widget_set_sensitive(utdata->frequency, TRUE);
    gtk_widget_set_sensitive(utdata->attack_detection, TRUE);
    printf("Voltage\n");
}

void utility_tools(GtkButton *but, gpointer udata)
{
    GdkPixbuf *g_red_image;
    GdkPixbuf *g_green_image;
    OsmGpsMapImage *g_last_image;
    // -------------------
    GtkBuilder *builder;
    GError *error = NULL;
    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, UI_fILE, &error))
    {
        g_warning("%s", error->message);
        g_free(error);
    }

    utdata = g_slice_new(UtData);
    utdata->util_window = GTK_WIDGET(gtk_builder_get_object(builder, "util_window"));
    utdata->voltage = GTK_WIDGET(gtk_builder_get_object(builder, "voltage"));
    utdata->frequency = GTK_WIDGET(gtk_builder_get_object(builder, "freq"));
    utdata->attack_detection = GTK_WIDGET(gtk_builder_get_object(builder, "attack_detection"));
    utdata->algorithm = GTK_WIDGET(gtk_builder_get_object(builder, "algorithm"));
    utdata->dimmension = GTK_WIDGET(gtk_builder_get_object(builder, "dimmension"));
    // utdata->util_map = GTK_WIDGET(gtk_builder_get_object(builder, "util_map"));
    utdata->map_layout = GTK_CONTAINER(gtk_builder_get_object(builder, "map_layout"));
    utdata->graph_layout = GTK_CONTAINER(gtk_builder_get_object(builder, "graph_layout"));

    gtk_widget_set_sensitive(utdata->voltage, FALSE);
    gtk_widget_set_visible(utdata->algorithm, FALSE);
    gtk_widget_set_visible(utdata->dimmension, FALSE);

    g_signal_connect(utdata->attack_detection, "clicked", G_CALLBACK(on_attack_detection_clicked), NULL);
    g_signal_connect(utdata->frequency, "clicked", G_CALLBACK(on_frequency_clicked), NULL);
    g_signal_connect(utdata->voltage, "clicked", G_CALLBACK(on_voltage_clicked), NULL);

    g_red_image = gdk_pixbuf_new_from_file_at_size(RED_IMAGE, 24, 24, NULL);
    g_green_image = gdk_pixbuf_new_from_file_at_size(GREEN_IMAGE, 24, 24, NULL);

    utdata->util_map = g_object_new(OSM_TYPE_GPS_MAP,
                                    "map-source", OSM_GPS_MAP_SOURCE_OSMC_TRAILS,
                                    "tile-cache", "/tmp/",
                                    NULL);

    // TODO: centering the map
    osm_gps_map_set_center_and_zoom(utdata->util_map, 15.4589, 75.0078, 10);

    // TODO: add the points to window.
    g_last_image = osm_gps_map_image_add(utdata->util_map, 15.4589, 75.0078, g_red_image);
    g_last_image = osm_gps_map_image_add(utdata->util_map, 15.518597, 74.925584, g_green_image);

    
    curr_measurement = 0;
    algorithm = 0;
    dimmension = 0;
    myParameters parameters = {utdata->util_map, g_red_image, g_green_image, g_last_image};
    gpointer data = (gpointer)&parameters;
    guint pid = g_timeout_add(20, (GSourceFunc)update_images, data);

    gtk_widget_set_size_request(GTK_WIDGET(utdata->util_map), 600, 500);

    gtk_container_add(utdata->map_layout, GTK_WIDGET(utdata->util_map));

    // add live chart
    serie = live_chart_serie_new("Hello", (LiveChartSerieRenderer*)live_chart_line_new (live_chart_values_new(10000)));
    // live_chart set color to the serie
    GdkRGBA color;
    color.red = 1.0;
    color.green = 0.0;
    color.blue = 1.0;
    color.alpha = 1.0;

    live_chart_path_set_color(live_chart_serie_get_line(serie), &color);
    LiveChartConfig *config = live_chart_config_new();
    live_chart_yaxis_set_unit(config->y_axis, "mHz");
    live_chart_xaxis_set_tick_interval(config->x_axis, 20);
    live_chart_xaxis_set_tick_length(config->x_axis, 100);
    live_chart_yaxis_update_bounds(config->y_axis, 1);
    live_chart_path_set_visible(config->x_axis->lines, FALSE);

    LiveChartChart *chart = live_chart_chart_new(config);
    live_chart_chart_add_serie(chart, serie);

    // gtk_widget_set_hexpand(GTK_WIDGET(chart), TRUE);
    // gtk_widget_set_vexpand(GTK_WIDGET(chart), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(chart), 600, 150);

    gtk_container_add(utdata->graph_layout, GTK_WIDGET(chart));


    

    g_signal_connect(utdata->util_window, "destroy", G_CALLBACK(on_window_destroy), GUINT_TO_POINTER(pid));

    // gtk_widget_set_vexpand(GTK_WIDGET(utdata->util_map), TRUE);
    // gtk_widget_set_hexpand(GTK_WIDGET(utdata->util_map), TRUE);
    gtk_widget_show_all(utdata->util_window);

    gtk_widget_hide(utdata->algorithm);
    gtk_widget_hide(utdata->dimmension);

    gtk_main();
}