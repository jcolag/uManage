#ifndef WINMGMT__H
#define WINMGMT__H

extern struct program_options *xyz;     /* Prevent spurious warnings */

void init_winmgmt(struct program_options *);
time_t window_state_report (struct window_state *);
char * window_state_format (struct window_state *, time_t *, time_t *);
int    is_window_updated (struct window_state *, int *);

#endif

