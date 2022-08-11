#include<gtk/gtk.h>
#include<stdio.h>

void data_vis_window(GtkButton *widget,gpointer udata)
{
    GtkWidget *new_window;
    new_window = gtk_dialog_new();

    //gtk_window_fullscreen(GTK_WINDOW(new_window));
    gtk_window_set_default_size(GTK_WINDOW(new_window), 800, 600);
     gtk_window_set_resizable(GTK_WINDOW(new_window), FALSE);
     gtk_window_set_position(GTK_WINDOW(new_window), GTK_WIN_POS_CENTER);
     //gtk_window_set_icon(GTK_WINDOW(new_window), create_pixbuf("/usr/local/share/iPDC/logo.png"));

     /* Set the Title of Main Window */
    gtk_window_set_title (GTK_WINDOW (new_window), "iPDC");
    g_signal_connect (new_window, "destroy", G_CALLBACK(gtk_widget_destroy), new_window);
    gtk_widget_show(new_window);
}