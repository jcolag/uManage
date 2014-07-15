#include <string.h>
#include <gtk/gtk.h>
#include "config.h"
#include "uabout.h"
#include "uabout_glade.h"

void open_uabout(char *path) {
    load_uabout_from_file(path, 1);
    gtk_widget_show_all((GtkWidget*)GTK_WINDOW(uabout_About_Dialog));
}

void on_About_Dialog_response (GtkButton *button, gpointer user_data) {
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }

    gtk_widget_destroy(GTK_WIDGET(uabout_About_Dialog));
}

