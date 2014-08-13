#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include "config.h"
#include "indicate.h"
#include "umenu_glade.h"
#include "uabout.h"
#include "uoptions.h"

struct program_options *progopts = NULL;
char pathname[256];
int *force;
static const int user_loc = 6;
static GtkWidget **user_menu_items;

void activate_about (GtkMenuItem *, gpointer);
void activate_options (GtkMenuItem *, gpointer);
void activate_refresh (GtkMenuItem *, gpointer);
void activate_pause (GtkCheckMenuItem *, gpointer);
void activate_jiggle (GtkCheckMenuItem *, gpointer);
void activate_quit (GtkMenuItem *, gpointer);
void activate_user (GtkCheckMenuItem *, gpointer);

void activate_about (GtkMenuItem *menu, gpointer data) {
    if (menu == NULL && data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    open_uabout(pathname);
}

void activate_options (GtkMenuItem *menu, gpointer data) {
    if (menu == NULL && data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    open_uoptions(pathname, progopts);
}

void activate_refresh (GtkMenuItem *menu, gpointer data) {
    struct program_options  options;

    if (menu == NULL && data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
/*    remove_menu_items();*/
    get_configuration(&options);
    if (options.menu_items != NULL) {
        add_menu_items(options.menu_items);
    }
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
    int idx;

    if (menu == NULL && data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    *force = 1;
    progopts->pause = 0;
    progopts->jiggle = 0;
    for (idx = 0; idx < progopts->menu_len; idx++) {
        if (progopts->userdef[idx] > 1) {
            progopts->userdef[idx] = -progopts->userdef[idx];
        }
    }
    gtk_main_quit();
}

void activate_user (GtkCheckMenuItem *menu, gpointer data) {
    const char *label;
    gboolean active;
    int idx = 0;

    if (data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    label = gtk_menu_item_get_label((GtkMenuItem *)menu);
    active = gtk_check_menu_item_get_active(menu);
    for (idx = 0; idx < progopts->menu_len; idx++) {
        if (!strcmp(progopts->menu_items[idx], label)) {
            progopts->userdef[idx] = active ? 1 : -progopts->userdef[idx];
            break;
        }
    }
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
    g_signal_connect(umenu_Menu_Refresh, "activate", G_CALLBACK(activate_refresh), NULL);
    g_signal_connect(umenu_Menu_About, "activate", G_CALLBACK(activate_about), NULL);
    g_signal_connect(umenu_Menu_Pause, "toggled", G_CALLBACK(activate_pause), NULL);
    g_signal_connect(umenu_Menu_Jiggle, "toggled", G_CALLBACK(activate_jiggle), NULL);

    gtk_main ();
    return arg;
}

int add_menu_items(char **items) {
    int         idx,
                count = 0,
                loc = 7;
    GtkWidget  *item;

    for (idx = 0; items[idx] != NULL; idx++) {
        ++count;
    }
    user_menu_items = malloc(sizeof(GtkWidget*) * (count + 2));
    user_menu_items[count + 1] = NULL;
    for (idx = 0; idx < count; idx++) {
        item = gtk_check_menu_item_new_with_label(items[idx]);
        user_menu_items[idx] = item;
        g_signal_connect(item, "activate", G_CALLBACK(activate_user), NULL);
        gtk_menu_shell_insert((GtkMenuShell *)umenu_Indicator_Menu, item, loc);
        ++loc;
    }
    if (idx > 0) {
        item = gtk_separator_menu_item_new();
        g_object_ref((GObject*)item);
        gtk_menu_shell_insert((GtkMenuShell *)umenu_Indicator_Menu, item, loc);
        user_menu_items[count] = item;
        g_object_unref((GObject*)item);
    }
    return idx;
}

int remove_menu_items(void) {
    int         idx;
    GtkWidget  *item;

    if (user_menu_items == NULL) {
        return 0;
    }
    for (idx = 0; user_menu_items[idx] != NULL; idx++) {
        item = user_menu_items[idx];
        g_object_ref((GObject*)item);
        gtk_container_remove((GtkContainer*)umenu_Indicator_Menu, item);
/*        gtk_widget_destroy((GtkWidget*)item);*/
        g_object_unref((GObject*)item);
        user_menu_items[idx] = NULL;
    }
    free(user_menu_items);
    user_menu_items = NULL;
    return idx;
}

