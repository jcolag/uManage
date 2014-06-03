#ifndef U_MANAGE__H
#define U_MANAGE__H

struct window_state {
    int             force;              /* Exiting, so report anyway */
    unsigned long   last_idle;          /* Milliseconds of idle time */
    Window          window_id;
    unsigned char   window_title[512];
    time_t          window_start,       /* Use of window started at (s) */
                    idle_start,         /* Idle started at (s) */
                    idle_accumulated;   /* Idle with window focus (s) */
    char            csv[1024];          /* For reporting - output only */
};

void   handle_break (int);
void   handle_alarm (int);
void   window_state_init (struct window_state *);
void   get_executable_path (char *, size_t);
#endif

