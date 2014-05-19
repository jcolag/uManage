#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include "config.h"
#include "indicate.h"
#include "umenu_glade.h"
#include "uoptions.h"

struct program_options *opts;
int *force;

void activate_options (GtkMenuItem *, gpointer);
void activate_quit (GtkMenuItem *, gpointer);

void activate_options (GtkMenuItem *menu, gpointer data) {
    open_uoptions(opts);
}

void activate_quit (GtkMenuItem *menu, gpointer data) {
    *force = 1;
    gtk_main_quit();
}

void stop_indicator (void) {
    activate_quit(NULL, NULL);
}

void init_indicator(int argc, char *argv[], struct program_options *options) {
    opts = options;
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
    g_signal_connect(umenu_Menu_Quit, "activate", G_CALLBACK(activate_quit), NULL);
    g_signal_connect(umenu_Menu_Opts, "activate", G_CALLBACK(activate_options), NULL);

    gtk_main ();
    return arg;
}

