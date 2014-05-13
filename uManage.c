#include <ctype.h>
#include <getopt.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <xdo.h>
#include <sys/types.h>
#include <xcb/screensaver.h>
#include <glib.h>
#include "uManage.h"
#include "config.h"

int                 poll_continue = 1;
FILE               *report;
xdo_t              *xdo_instance;
struct window_state current;

int main (int argc, char *argv[]) {
    unsigned long       idle;               /* Idle time in ms */
    time_t              idle_dur;

    xcb_connection_t   *connection = xcb_connect(NULL, NULL);
    xcb_screensaver_query_info_cookie_t cookie;
    xcb_screen_t       *screen;

    struct program_options opts;

    /*
     * Program setup
     *     Gracefully handle user interrupt
     *     Get command-line options
     *     Open report file for logging
     *     Initialize variables
     */
    signal(SIGINT, handle_break);

    get_configuration(&opts);
    if (parse_options(argc, argv, &opts)) {
        return 1;
    }
    if (opts.save_options) {
        save_configuration(&opts);
    }

    report = fopen(opts.filename, "a");
    if(report == NULL) {
        report = stdout;
    }

    window_state_init(&current);
    xdo_instance = xdo_new(NULL);
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    /*
     * Main loop
     *     Check for new idling
     *     Accumulate total idle time per window use
     *     Check for changed window
     *     Log status if interesting
     *     Set for next iteration
     */
    while(poll_continue || current.force) {
        cookie = xcb_screensaver_query_info(connection, screen->root);
        idle = idle_time(connection, cookie);

        if(idle < current.last_idle) {
            /* New idle "session" */
            if(idle > opts.idle_threshold * (unsigned long)1000) {
                /* It has been long enough, so reset the idle timer */
                /* ...but round up. */
                time(&current.idle_start);
                current.idle_start -= (idle + 500) / 1000;
            }
            else if(current.idle_start != 0) {
                /* If we're tracking idle time and it grows, add it */
                time(&idle_dur);
                idle_dur -= current.idle_start;
                current.idle_accumulated += idle_dur;
                current.idle_start = 0;
            }
        }

        if(is_window_updated(xdo_instance, &current)) {
            /* Flush in case someone monitors the output file */
            fprintf(report, "%s\n", current.csv);
            fflush(report);
        }
        /* Reset this, so that we don't have problems exiting */
        current.force = 0;

        if(opts.poll_period < 1) {
            /* I might want to use this for one-off scripts */
            break;
        }

        sleep(opts.poll_period);
    }

    /*
     * Clean up
     */
    fclose(report);
    return 0;
}

void handle_break (int signal) {
    /*
     *  On our way out, we need to identify the final, buffered
     *  window, flush any buffers, and close the file.
     */
    if (signal != SIGINT) {
        return;
    }
    current.force = 1;
}

void window_state_init (struct window_state *state) {
    state->force = 0;
    state->last_idle = (unsigned long)(-1);
    state->idle_start = 0;
    state->idle_accumulated = 0;
    time(&state->window_start);
    strcpy((char *)state->window_title, "");
}

int is_window_updated (xdo_t *xdo, struct window_state *state) {
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
            poll_continue = 0;
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

unsigned long idle_time(xcb_connection_t *conn, xcb_screensaver_query_info_cookie_t cookie) {
    /*
     *  Grab the current idle time, expressed in milliseconds.
     */
    xcb_screensaver_query_info_reply_t *info =
            xcb_screensaver_query_info_reply(conn, cookie, NULL);
    uint32_t idle = info->ms_since_user_input;
    free(info);
    return idle;
}

