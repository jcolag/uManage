#ifndef WINMGMT__H
#define WINMGMT__H

void init_winmgmt(void);
time_t window_state_report (struct window_state *);
char * window_state_format (struct window_state *, time_t *, time_t *);
int    is_window_updated (struct window_state *, int *);

#endif

