#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include "indicate.h"
#include "umenu_glade.h"

int *force;

void activate_action (GtkMenuItem *, gpointer);

void activate_action (GtkMenuItem *menu, gpointer data) {
    *force = 1;
    gtk_main_quit();
}

void stop_indicator (void) {
    activate_action(NULL, NULL);
}

void init_indicator(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    load_umenu_from_file(0);
}

void *run_indicator(void *arg) {
    AppIndicator *indicator;

    force = (int *)arg;

    indicator = app_indicator_new("uManage-client",
                                  "indicator-messages",
                                  APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon(indicator, "indicator-messages-new");

    app_indicator_set_menu(indicator, GTK_MENU(umenu_Indicator_Menu));
    gtk_widget_show_all((GtkWidget*)umenu_Indicator_Menu);
    g_signal_connect(umenu_Menu_Quit, "activate", G_CALLBACK(activate_action), NULL);

    gtk_main ();
    return arg;
}

