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
#include "sqlite.h"
#include "mouse.h"

struct window_state     current;
struct program_options  opts;
FILE                   *report;
int                     poll_continue = 1;

int main (int argc, char *argv[]) {
#ifdef GUI
    void               *status;
    char                path[256];
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

    if (opts.text_out) {
        report = fopen(opts.filename, "a");
        if(report == NULL) {
            report = stdout;
        }
    }

    init_idle();
    init_winmgmt(&opts);
    window_state_init(&current);
#ifdef GUI
    get_executable_path(path, sizeof(path));
    init_indicator(argc, argv, path, &opts);
    if (opts.menu_items != NULL) {
        add_menu_items(opts.menu_items);
    }
    fflush(NULL);
    pthread_create(&thr_menu, NULL, run_indicator, &current.force);
#endif
    if (opts.use_database) {
        open_database(opts.dbname);
    }

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
    if (report != stdout && opts.text_out) {
        fclose(report);
    }
    if (opts.use_database) {
        close_database();
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
    opts.pause = 0;
    opts.jiggle = 0;
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
#ifdef GUI
    int                 idx;
#endif

    if (sig != SIGALRM) {
        return;
    }
    signal(SIGALRM, SIG_IGN);

    if (opts.pause) {
        if (current.pause_since == 0) {
            time(&current.pause_since);
        }
        signal(SIGALRM, handle_alarm);
        alarm(opts.poll_period);
        return;
    }

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

#ifdef GUI
    for(idx = 0; idx < opts.menu_len; idx++) {
        if (opts.userdef[idx] == 1) {
            /* Newly active */
            time((time_t *)&opts.userdef[idx]);
        } else if (opts.userdef[idx] < 0) {
            /* Deactivated Option */
            time(&idle_dur);
            opts.userdef[idx] = -opts.userdef[idx];
            report_duration(current.csv, opts.time_format, (time_t *)&opts.userdef[idx], &idle_dur);
            if (opts.use_database) {
                write_duration_to_database(current.csv, opts.menu_items[idx]);
            }
            opts.userdef[idx] = 0;
        }
    }

    if(opts.jiggle != 0) {
        time(&idle_dur);
        /* We need to worry about jiggling the mouse */
        if(current.last_jiggle == 0) {
            time(&current.last_jiggle);
        } else if ((idle_dur - current.last_jiggle) / opts.mouse_period > 0) {
            move_mouse(opts.mouse_dist);
            current.last_jiggle = 0;
        }
    }

    if (current.jiggle_since == 0 && opts.jiggle != 0) {
        /* Feature turned on, needs to be tracked */
        time(&current.jiggle_since);
    } else if (current.jiggle_since != 0 && opts.jiggle == 0) {
        /* Feature turned off, emit duration */
        time(&idle_dur);
        report_duration(current.csv, opts.time_format, &current.jiggle_since, &idle_dur);
        current.jiggle_since = 0;
        if (opts.use_database) {
            write_keepalive_to_database(current.csv);
        }
    }

    if (current.pause_since != 0 && opts.pause == 0) {
        /* Feature turned off, emit duration */
        time(&idle_dur);
        report_duration(current.csv, opts.time_format, &current.pause_since, &idle_dur);
        current.pause_since = 0;
        if (opts.use_database) {
            write_duration_to_database(current.csv, "Pause");
        }
    }
#endif

    if(is_window_updated(&current, &poll_continue, opts.use_database)) {
        if (opts.use_database) {
            write_activity_to_database(current.csv, 0);
        }
        if (opts.text_out) {
            /* Flush in case someone monitors the output file */
            fprintf(report, "%s\n", current.csv);
            fflush(report);
        }
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
    state->last_jiggle = 0;
    state->jiggle_since = 0;
    state->pause_since = 0;
    state->last_idle = (unsigned long)(-1);
    state->idle_start = 0;
    state->idle_accumulated = 0;
    time(&state->window_start);
    strcpy((char *)state->window_title, "");
}

void get_executable_path (char *path, size_t length) {
    /*
     * Please note that this routine is not entirely portable.
     *  /proc/self/exe is specific to Linux
     *  For FreeBSD, change to "/proc/curproc/file"
     *  For Solaris, change to "/proc/self/path/a.out"
     *  Others will be added as discovered, and argv[0] may be
     *  manipulated, in some cases.
     */
    unsigned int index;

    for(index=0;index<sizeof(path);index++) {
        path[index] = '\000';
    }
    readlink("/proc/self/exe", path, length);
    for(index = length - 1; index > 0 && path[index] != '/'; index--) {
        path[index] = '\000';
    }
}

