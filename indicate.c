#include <string.h>
#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include "config.h"
#include "indicate.h"
#include "umenu_glade.h"
#include "uoptions.h"

struct program_options *progopts = NULL;
char pathname[256];
int *force;

void activate_options (GtkMenuItem *, gpointer);
void activate_pause (GtkCheckMenuItem *, gpointer);
void activate_jiggle (GtkCheckMenuItem *, gpointer);
void activate_quit (GtkMenuItem *, gpointer);

void activate_options (GtkMenuItem *menu, gpointer data) {
    if (menu == NULL && data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    open_uoptions(pathname, progopts);
}

void activate_pause (GtkCheckMenuItem *menu, gpointer data) {
    gboolean pause;

    if (data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    pause = gtk_check_menu_item_get_active(menu);
    progopts->pause = pause ? 1 : 0;
}

void activate_jiggle (GtkCheckMenuItem *menu, gpointer data) {
    gboolean jiggle;

    if (data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    jiggle = gtk_check_menu_item_get_active(menu);
    progopts->jiggle = jiggle ? 1 : 0;
}

void activate_quit (GtkMenuItem *menu, gpointer data) {
    if (menu == NULL && data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    *force = 1;
    gtk_main_quit();
}

void stop_indicator (void) {
    activate_quit(NULL, NULL);
}

void init_indicator(int argc, char *argv[], char *path, struct program_options *options) {
    strcpy(pathname, path);
    progopts = options;
    gtk_init(&argc, &argv);
    load_umenu_from_file(pathname, 0);
}

void *run_indicator(void *arg) {
    AppIndicator *indicator;
    char          icon[256];

    force = (int *)arg;

    strcpy(icon, pathname);
    strcat(icon, "/clock_ind.png");
    indicator = app_indicator_new("uManage-client", icon,
                                  APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon(indicator, "indicator-messages-new");

    app_indicator_set_menu(indicator, GTK_MENU(umenu_Indicator_Menu));
    gtk_widget_show_all((GtkWidget*)umenu_Indicator_Menu);
    g_signal_connect(umenu_Menu_Quit, "activate", G_CALLBACK(activate_quit), NULL);
    g_signal_connect(umenu_Menu_Opts, "activate", G_CALLBACK(activate_options), NULL);
    g_signal_connect(umenu_Menu_Pause, "toggled", G_CALLBACK(activate_pause), NULL);
    g_signal_connect(umenu_Menu_Jiggle, "toggled", G_CALLBACK(activate_jiggle), NULL);

    gtk_main ();
    return arg;
}

