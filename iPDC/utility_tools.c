#include<gtk/gtk.h>
#include<stdio.h>

void utility_tools(GtkButton *but, gpointer udata)
{
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Utility Tools");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 600, 400);
    gtk_window_set_position(GTK_WINDOW(new_window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(new_window), 10);
    g_signal_connect (new_window, "destroy", G_CALLBACK(gtk_widget_destroy), new_window);

    gtk_widget_show_all(new_window);
}