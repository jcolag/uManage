#include <string.h>
#include <gtk/gtk.h>
#include "config.h"
#include "uoptions.h"
#include "uoptions_glade.h"

int     poll_state = 0,
        idle_state = 0,
        file_state = 0,
        tfmt_state = 0;

struct program_options  newest,
                       *oldest;

void open_uoptions(struct program_options *orig) {
    load_uoptions_from_file(1);
    if (orig != NULL) {
        oldest = orig;
        if (oldest->poll_period > 0) {
            gtk_switch_set_active(uoptions_Poll_Switch, 1);
            gtk_spin_button_set_value(uoptions_Poll_Input, oldest->poll_period);
        }
        if (oldest->idle_threshold > 0) {
            gtk_switch_set_active(uoptions_Idle_Switch, 1);
            gtk_spin_button_set_value(uoptions_Idle_Input, oldest->idle_threshold);
        }
        if (strcmp(oldest->filename, "")) {
            gtk_switch_set_active(uoptions_File_Switch, 1);
            gtk_entry_set_text(uoptions_File_Input, oldest->filename);
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

void on_Poll_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    newest.poll_period = gtk_spin_button_get_value_as_int(spinbutton);
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

void on_TFmt_Input_changed (GtkEntry *entry, gpointer user_data) {
    if (user_data == NULL) {
        /* Bogus condition to use parameters */
        ;
    }
    strcpy(newest.time_format, gtk_entry_get_text(entry));
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
    if (idle_state) {
        oldest->idle_threshold = newest.idle_threshold;
    }
    if (file_state) {
        strcpy(oldest->filename, newest.filename);
    }
    if (tfmt_state) {
        strcpy(oldest->time_format, newest.time_format);
    }
    save_configuration(oldest);
}

