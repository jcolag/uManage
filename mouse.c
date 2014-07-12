#include <time.h>
#include <xdo.h>
#include "mouse.h"

static xdo_t *xdo;
static int mouse_move_dir = -25;

void move_mouse(void) {
    if(xdo == NULL) {
        xdo = xdo_new(NULL);
    }
    xdo_move_mouse_relative(xdo, mouse_move_dir, mouse_move_dir);
    mouse_move_dir = -mouse_move_dir;
}
