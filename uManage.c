#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <xdo.h>
#include <xcb/screensaver.h>
#include "uManage.h"

int                 poll_continue = 1;
FILE               *report;
xdo_t              *xdo_instance;
struct window_state current;

int main (int argc, char *argv[]) {
    int                 chOpt,              /* For getopt() */
                        poll_period = 1,
                        idle_threshold = 180;/* Treat time as idle at (s) */
    char               *filename = "";      /* Target file for *report */
    unsigned long       idle;               /* Idle time in ms */
    time_t              idle_dur;

    xcb_connection_t   *connection = xcb_connect(NULL, NULL);
    xcb_screensaver_query_info_cookie_t cookie;
    xcb_screen_t       *screen;

    /*
     * Program setup
     *     Gracefully handle user interrupt
     *     Get command-line options
     *     Open report file for logging
     *     Initialize variables
     */

    signal(SIGINT, handle_break);

    opterr = 0;
    while((chOpt = getopt(argc, argv, "d:f:i:")) != -1) {
        switch(chOpt) {
            case 'f':
                filename = optarg;
                break;
            case 'd':
                poll_period = atoi(optarg);
                break;
            case 'i':
                idle_threshold = atoi(optarg);
                break;
            case '?':
                if(optopt == 'f' || optopt == 'd' || optopt == 'i')
                    fprintf(stderr, "Option -%c requires an argument.\n",
                            optopt);
                else if(isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                abort();
        }
    }

    if(filename != NULL) {
        report = fopen(filename, "a");
    }
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
            if(idle > idle_threshold * (unsigned long)1000) {
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

        if(poll_period < 1) {
            /* I might want to use this for one-off scripts */
            break;
        }

        sleep(poll_period);
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

void window_state_init (struct window_state *ws) {
    ws->force = 0;
    ws->last_idle = (unsigned long)(-1);
    ws->idle_start = 0;
    ws->idle_accumulated = 0;
    time(&ws->window_start);
    strcpy((char *)ws->window_title, "");
}

int is_window_updated (xdo_t *xdo, struct window_state *ws) {
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
    if(strcmp((char *)name, (char *)ws->window_title) || ws->force) {
        now = window_state_report(ws);
        ws->window_start = now;
        ws->window_id = win;
        strcpy((char *)ws->window_title, (char *)name);
        ws->idle_accumulated = 0;
        ws->idle_start = 0;
        ws->last_idle = (unsigned long)(-1);
        if (ws->force) {
            poll_continue = 0;
        }
        return 1;
    }
    return 0;
}

time_t window_state_report (struct window_state *ws) {
    /*
     *  Get the duration the most recent window has been used and
     *  retrieve the relevant CSV.
     */
    time_t  window_end,
            window_dur;

    time(&window_end);
    window_dur = window_end - ws->window_start;

    window_state_format(ws, NULL, &window_dur);
    return window_end;
}

char * window_state_format (struct window_state *ws, time_t *instead, time_t *duration) {
    /*
     *  Create the CSV line inside the window state, ready for printing.
     *
     *  If the caller set the instead parameter, use it instead of the
     *  state's duration.
     */
    time_t     *time = instead;
    struct tm  *t = NULL;

    if (time == NULL) {
        time = &ws->window_start;
    }

    t = localtime(time);
    /* YYYY,MM,DD,HH,MM,SS,Window ID,Window Title,Time Used,Time Idle */
    sprintf(ws->csv, "%04d,%02d,%02d,%02d,%02d,%02d,%08X,%s,%u,%u",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            (unsigned)ws->window_id, ws->window_title,
            (unsigned)(*duration - ws->idle_accumulated),
            (unsigned)ws->idle_accumulated);
    return ws->csv;
}

unsigned long idle_time(xcb_connection_t *conn, xcb_screensaver_query_info_cookie_t c) {
    /*
     *  Grab the current idle time, expressed in milliseconds.
     */
    xcb_screensaver_query_info_reply_t *info =
            xcb_screensaver_query_info_reply(conn, c, NULL);
    uint32_t idle = info->ms_since_user_input;
    free(info);
    return idle;
}

