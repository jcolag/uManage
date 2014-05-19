void open_uoptions(struct program_options *);
void on_Poll_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_Idle_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_File_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_Poll_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
void on_Idle_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
void on_File_Input_changed (GtkEntry *entry, gpointer user_data);
void on_Save_Button_clicked (GtkButton *button, gpointer user_data);

