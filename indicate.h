#ifndef INDICATE__H
#define INDICATE__H

void init_indicator(int, char *[], char *, struct program_options *);
void *run_indicator (void *);
void stop_indicator (void);
int add_menu_items (char **);

#endif

