#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <xdo.h>
#include "uManage.h"
#include "winmgmt.h"
#include "config.h"
#include "idle.h"

struct window_state current;

int main (int argc, char *argv[]) {
    unsigned long       idle;               /* Idle time in ms */
    time_t              idle_dur;
    int                 poll_continue = 1;
    FILE               *report;

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

    init_idle();
    init_winmgmt();
    window_state_init(&current);

    /*
     * Main loop
     *     Check for new idling
     *     Accumulate total idle time per window use
     *     Check for changed window
     *     Log status if interesting
     *     Set for next iteration
     */
    while(poll_continue || current.force) {
        idle = idle_time();

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

        if(is_window_updated(&current, &poll_continue)) {
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
    if (report != stdout) {
        fclose(report);
    }
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

