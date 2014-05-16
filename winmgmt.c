#include <stdio.h>
#include <string.h>
#include <time.h>
#include <xdo.h>
#include "uManage.h"
#include "winmgmt.h"

xdo_t  *xdo;

void init_winmgmt(void) {
    xdo = xdo_new(NULL);
}

int is_window_updated (struct window_state *state, int *poll_continue) {
    /*
     *  Grab the focused window's ID and title
     *  If it's a new title, better to keep track of file saves than not,
     *          likewise with browser tabs, which is why we don't test
     *          the window ID
     *      Generate the CSV
     *      Reset the window state
     *  Notify upstream whether anything useful is going on
     *
     *  Note that the XDO documentation appears to be out of date.  It
     *  doesn't list xdo_get_focused_window_sane(), but does have a
     *  xdo_window_get_focus(), which doesn't actually exist.
     */
    Window          win;
    unsigned char  *name;
    int             n_len,
                    n_type;
    time_t          now;

    xdo_get_focused_window_sane(xdo, &win);
    xdo_get_window_name(xdo, win, &name, &n_len, &n_type);
    if(strcmp((char *)name, (char *)state->window_title) || state->force) {
        now = window_state_report(state);
        state->window_start = now;
        state->window_id = win;
        strcpy((char *)state->window_title, (char *)name);
        state->idle_accumulated = 0;
        state->idle_start = 0;
        state->last_idle = (unsigned long)(-1);
        if (state->force) {
            *poll_continue = 0;
        }
        return 1;
    }
    return 0;
}

time_t window_state_report (struct window_state *state) {
    /*
     *  Get the duration the most recent window has been used and
     *  retrieve the relevant CSV.
     */
    time_t  window_end,
            window_dur;

    time(&window_end);
    window_dur = window_end - state->window_start;

    window_state_format(state, NULL, &window_dur);
    return window_end;
}

char * window_state_format (struct window_state *state, time_t *instead, time_t *duration) {
    /*
     *  Create the CSV line inside the window state, ready for printing.
     *
     *  If the caller set the instead parameter, use it instead of the
     *  state's duration.
     */
    time_t     *time = instead;
    struct tm  *start = NULL;

    if (time == NULL) {
        time = &state->window_start;
    }

    start = localtime(time);
    /* YYYY,MM,DD,HH,MM,SS,Window ID,Window Title,Time Used,Time Idle */
    sprintf(state->csv, "%04d,%02d,%02d,%02d,%02d,%02d,%08X,%s,%u,%u",
            start->tm_year + 1900, start->tm_mon + 1, start->tm_mday,
            start->tm_hour, start->tm_min, start->tm_sec,
            (unsigned)state->window_id, state->window_title,
            (unsigned)(*duration - state->idle_accumulated),
            (unsigned)state->idle_accumulated);
    return state->csv;
}

