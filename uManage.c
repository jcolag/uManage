#ifdef GUI             /* GTK+ runs its own message loop */
#include <pthread.h>
#endif
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
#include "indicate.h"

struct window_state     current;
struct program_options  opts;
FILE                   *report;
int                     poll_continue = 1;

int main (int argc, char *argv[]) {
#ifdef GUI
    void               *status;
    pthread_t           thr_menu;
#endif

    /*
     * Program setup
     *     Handle user interrupt and alarm
     *     Get program options
     *     Open report file for logging if needed
     *     Initialize UI
     */
    signal(SIGINT, handle_break);
    signal(SIGALRM, handle_alarm);
    alarm(1);

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
#ifdef GUI
    init_indicator(argc, argv, &opts);
    pthread_create(&thr_menu, NULL, run_indicator, &current.force);
#endif

    while(poll_continue || current.force) {
        sleep(1);                       /* Just wait for program close */
    }
    /*
     * Clean up
     */
    alarm(1);                           /* Last hurrah */
    sleep(1);
#ifdef GUI
    pthread_join(thr_menu, &status);
#endif
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
#ifdef GUI
    stop_indicator();
#else
    current.force = 1;
#endif
}

void handle_alarm (int sig) {
    /*
     *     Check for new idling
     *     Accumulate total idle time per window use
     *     Check for changed window
     *     Log status if interesting
     *     Set for next iteration
     */
    unsigned long       idle;               /* Idle time in ms */
    time_t              idle_dur;

    if (sig != SIGALRM) {
        return;
    }
    signal(SIGALRM, SIG_IGN);
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
        return;
    }
    signal(SIGALRM, handle_alarm);
    alarm(opts.poll_period);
}

void window_state_init (struct window_state *state) {
    state->force = 0;
    state->last_idle = (unsigned long)(-1);
    state->idle_start = 0;
    state->idle_accumulated = 0;
    time(&state->window_start);
    strcpy((char *)state->window_title, "");
}

