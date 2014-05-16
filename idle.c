#include <stdlib.h>
#include <xcb/screensaver.h>
#include "idle.h"

xcb_connection_t   *conn;
xcb_screen_t       *screen;

void init_idle(void) {
    conn = xcb_connect(NULL, NULL);
    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
}

unsigned long idle_time(void) {
    /*
     *  Grab the current idle time, expressed in milliseconds.
     */
    xcb_screensaver_query_info_cookie_t cookie;
    xcb_screensaver_query_info_reply_t *info;
    uint32_t idle;

    cookie = xcb_screensaver_query_info(conn, screen->root);
    info = xcb_screensaver_query_info_reply(conn, cookie, NULL);
    idle = info->ms_since_user_input;
    free(info);
    return idle;
}

