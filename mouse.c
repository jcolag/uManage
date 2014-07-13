#include <stdio.h>
#include <time.h>
#include <xdo.h>
#include "mouse.h"

static xdo_t *xdo;
static int mouse_move_dir = -25;

void move_mouse(void) {
    /*
     * Move the mouse, then flip the direction.
     */
    if(xdo == NULL) {
        xdo = xdo_new(NULL);
    }
    xdo_move_mouse_relative(xdo, mouse_move_dir, mouse_move_dir);
    mouse_move_dir = -mouse_move_dir;
}

void report_duration(char *output, char *format, time_t *start, time_t *end) {
    /*
     *  Create the CSV line, ready for printing.
     */
    char        startstr[64],
                endstr[64],
                format_string[64];
    struct tm  *ltime = NULL;

    ltime = localtime(start);
    strftime(startstr, sizeof(startstr), format, ltime);
    ltime = localtime(end);
    strftime(endstr, sizeof(endstr), format, ltime);
    sprintf(output, "'%s','%s'", startstr, endstr);
}

