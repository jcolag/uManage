#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "config.h"
#include "uoptions.h"
#include "uoptions_glade.h"

int     poll_state = 0,
        idle_state = 0,
        jiggle_state = 0,
        dist_state = 0,
        file_state = 0,
        db_state = 0,
        tfmt_state = 0,
        userdef_state = 0;

struct program_options  newest,
                       *oldest;

void open_uoptions(char *path, struct program_options *orig) {
    char    items[2048];
    int     count;

    load_uoptions_from_file(path, 1);
    if (orig != NULL) {
        oldest = orig;
        memcpy(&newest, oldest, sizeof(newest));
        if (oldest->poll_period > 0) {
            gtk_switch_set_active(uoptions_Poll_Switch, 1);
            gtk_spin_button_set_value(uoptions_Poll_Input, oldest->poll_period);
        }
        if (oldest->mouse_period > 0) {
            gtk_switch_set_active(uoptions_Jiggle_Switch, 1);
            gtk_spin_button_set_value(uoptions_Jiggle_Input, oldest->mouse_period);
        }
        if (oldest->mouse_dist > 0) {
            gtk_switch_set_active(uoptions_Distance_Switch, 1);
            gtk_spin_button_set_value(uoptions_Distance_Input, oldest->mouse_dist);
        }
        if (oldest->idle_threshold > 0) {
            gtk_switch_set_active(uoptions_Idle_Switch, 1);
            gtk_spin_button_set_value(uoptions_Idle_Input, oldest->idle_threshold);
        }
        if (strcmp(oldest->time_format, "")) {
            gtk_switch_set_active(uoptions_TFmt_Switch, 1);
            gtk_entry_set_text(uoptions_TFmt_Input, oldest->time_format);
        }
        if (strcmp(oldest->filename, "")) {
            gtk_switch_set_active(uoptions_File_Switch, 1);
            gtk_entry_set_text(uoptions_File_Input, oldest->filename);
        }
        if (oldest->use_database) {
            gtk_switch_set_active(uoptions_Db_Switch, 1);
        }
        if (strcmp(oldest->dbname, "")) {
            gtk_entry_set_text(uoptions_Db_Input, oldest->dbname);
        }
        if (oldest->menu_len > 0) {
            items[0] = '\000';
            for (count = 0; count < oldest->menu_len; count++) {
                strcat(items, oldest->menu_items[count]);
                if (count + 1 < oldest->menu_len) {
                    strcat(items, ",");
                }
            }
            gtk_entry_set_text(uoptions_UserDef_Input, items);
            gtk_switch_set_active(uoptions_UserDef_Switch, 1);
        }
    }
    gtk_widget_show_all((GtkWidget*)GTK_WINDOW(uoptions_uManage_Options));
}

void on_Poll_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_Poll_Input, onSw);
    poll_state = onSw;
}

void on_Jiggle_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_Jiggle_Input, onSw);
    jiggle_state = onSw;
}

void on_Distance_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_Distance_Input, onSw);
    dist_state = onSw;
}

void on_Idle_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_Idle_Input, onSw);
    idle_state = onSw;
}

void on_File_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_File_Input, onSw);
    file_state = onSw;
}

void on_Db_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_Db_Input, onSw);
    db_state = onSw;
    newest.use_database = 1;
}

void on_TFmt_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_TFmt_Input, onSw);
    tfmt_state = onSw;
}

void on_UserDef_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data) {
    gboolean onSw;
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    onSw = gtk_switch_get_active(button);
    gtk_widget_set_sensitive ((GtkWidget *)uoptions_UserDef_Input, onSw);
    userdef_state = onSw;
}

void on_Poll_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    newest.poll_period = gtk_spin_button_get_value_as_int(spinbutton);
}

void on_Jiggle_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    newest.mouse_period = gtk_spin_button_get_value_as_int(spinbutton);
}

void on_Distance_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    newest.mouse_dist = gtk_spin_button_get_value_as_int(spinbutton);
}

void on_Idle_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    newest.idle_threshold = gtk_spin_button_get_value_as_int(spinbutton);
}

void on_File_Input_changed (GtkEntry *entry, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    strcpy(newest.filename, gtk_entry_get_text(entry));
}

void on_Db_Input_changed (GtkEntry *entry, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    strcpy(newest.dbname, gtk_entry_get_text(entry));
}

void on_TFmt_Input_changed (GtkEntry *entry, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    strcpy(newest.time_format, gtk_entry_get_text(entry));
}

void on_UserDef_Input_changed (GtkEntry *entry, gpointer user_data) {
    char   *start,
            copy[2048];
    int     commas = 0;

    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    strcpy(copy, gtk_entry_get_text(entry));
    start = strtok(copy, ",");
    if (newest.menu_items != NULL) {
        newest.menu_items = NULL;
    }
    while (start) {
        ++commas;
        newest.menu_items = realloc(newest.menu_items, sizeof(char*) * commas);
        if (newest.menu_items == NULL) {
            break;
        }
        newest.menu_items[commas - 1] = malloc(sizeof(char) * strlen(start) + 1);
        strcpy(newest.menu_items[commas - 1], start);
        start = strtok(NULL, ",");
    }
    newest.menu_len = commas;
}

void on_Save_Button_clicked (GtkButton *button, gpointer user_data) {
    if (button == NULL && user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    if (oldest == NULL) {
        return;
    }
    if (poll_state) {
        oldest->poll_period = newest.poll_period;
    }
    if (jiggle_state) {
        oldest->mouse_period = newest.mouse_period;
    }
    if (dist_state) {
        oldest->mouse_dist = newest.mouse_dist;
    }
    if (idle_state) {
        oldest->idle_threshold = newest.idle_threshold;
    }
    if (file_state) {
        strcpy(oldest->filename, newest.filename);
    }
    if (tfmt_state) {
        strcpy(oldest->time_format, newest.time_format);
    }
    if (userdef_state) {
        oldest->menu_len = newest.menu_len;
        oldest->menu_items = newest.menu_items;
    }
    save_configuration(oldest);
}

