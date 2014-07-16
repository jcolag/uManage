void open_uoptions(char *, struct program_options *);
void on_Poll_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_Jiggle_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_Distance_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_Idle_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_File_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_Db_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_TFmt_Switch_state_flags_changed (GtkSwitch *button, gpointer user_data);
void on_Poll_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
void on_Jiggle_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
void on_Distance_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
void on_Idle_Input_value_changed (GtkSpinButton *spinbutton, gpointer user_data);
void on_File_Input_changed (GtkEntry *entry, gpointer user_data);
void on_Db_Input_changed (GtkEntry *entry, gpointer user_data);
void on_TFmt_Input_changed (GtkEntry *entry, gpointer user_data);
void on_Save_Button_clicked (GtkButton *button, gpointer user_data);

