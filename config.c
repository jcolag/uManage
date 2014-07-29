#include <ctype.h>
#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xdo.h>
#include <sys/types.h>
#include <glib.h>
#include "config.h"

int get_configuration (struct program_options *opts) {
    struct passwd *passwd = getpwuid(getuid());
    const char *homedir = passwd->pw_dir;
    char keyfilename[256];
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    int t_int, idx;
    char *t_str;

    /* Defaults */
    strcpy(opts->filename, "");
    strcpy(opts->time_format, "%Y,%m,%d,%H,%M,%S");
    opts->poll_period = 1;
    opts->mouse_period = 60;
    opts->mouse_dist = 25;
    opts->idle_threshold = 180;
    opts->save_options = 0;
    opts->text_out = 1;

    /* Preserve non-settings if we mess with anything */
    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    /* Leave if the file isn't available */
    sprintf(keyfilename, "%s/.uManage", homedir);
    if (!g_key_file_load_from_file (keyfile, keyfilename, flags, &error)) {
        return 0;
    }

    /* Fill in any valid settings */
    t_int = g_key_file_get_integer(keyfile, "Timing", "poll", NULL);
    if (t_int != 0) {
        opts->poll_period = t_int;
    }
    t_int = g_key_file_get_integer(keyfile, "Timing", "jiggle", NULL);
    if (t_int != 0) {
        opts->mouse_period = t_int;
    }
    t_int = g_key_file_get_integer(keyfile, "Timing", "idle", NULL);
    if (t_int != 0) {
        opts->idle_threshold = t_int;
    }
    t_str = g_key_file_get_string(keyfile, "Timing", "format", NULL);
    if (t_str != NULL) {
        strcpy(opts->time_format, t_str);
    }
    t_str = g_key_file_get_string(keyfile, "File", "log", NULL);
    if (t_str != NULL) {
        strcpy(opts->filename, t_str);
    }
    t_str = g_key_file_get_string(keyfile, "File", "database", NULL);
    if (t_str != NULL) {
        opts->use_database = 1;
        strcpy(opts->time_format, "%Y-%m-%dT%T");
        strcpy(opts->dbname, t_str);
    }
    t_str = g_key_file_get_string(keyfile, "User", "airport", NULL);
    if (t_str != NULL) {
        strcpy(opts->airport, t_str);
    }
    t_int = g_key_file_get_integer(keyfile, "File", "notext", NULL);
    if (t_int != 0) {
        opts->text_out = !t_int;
    }
    opts->menu_items = g_key_file_get_keys(keyfile, "Menu", (gsize*)&t_int, NULL);
    opts->menu_len = t_int;
    if (t_int != 0) {
        opts->userdef = malloc(t_int * sizeof(int));
        for (idx = 0; idx < t_int; idx++) {
            opts->userdef[idx] = 0;
        }
    }
    return 1;
}

int save_configuration (struct program_options *opts) {
    struct passwd *passwd = getpwuid(getuid());
    const char *homedir = passwd->pw_dir;
    char keyfilename[256];
    int  menu;
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;

    /* Preserve as much of the file later as possible */
    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    /* Try to load the configuration file */
    sprintf(keyfilename, "%s/.uManage", homedir);
    if (!g_key_file_load_from_file (keyfile, keyfilename, flags, &error)) {
        ;
    }

    /* Save any valid settings */
    if (opts->poll_period != 0) {
        g_key_file_set_integer(keyfile, "Timing", "poll", opts->poll_period);
    }
    if (opts->poll_period != 0) {
        g_key_file_set_integer(keyfile, "Timing", "jiggle", opts->mouse_period);
    }
    if (opts->idle_threshold != 0) {
        g_key_file_set_integer(keyfile, "Timing", "idle", opts->idle_threshold);
    }
    if (strcmp(opts->time_format, "")) {
        g_key_file_set_string(keyfile, "Timing", "format", opts->time_format);
    }
    if (strcmp(opts->filename, "")) {
        g_key_file_set_string(keyfile, "File", "log", opts->filename);
    }
    if (opts->use_database != 0) {
        g_key_file_set_string(keyfile, "File", "database", opts->dbname);
    }
    if (opts->text_out != 0) {
        g_key_file_set_integer(keyfile, "File", "notext", !opts->text_out);
    }
    if (opts->airport != 0) {
        g_key_file_set_string(keyfile, "User", "airport", opts->airport);
    }
    for (menu = 0; menu < opts->menu_len; menu++) {
        if (opts->airport != 0) {
            g_key_file_set_string(keyfile, "Menu", opts->menu_items[menu], "");
        }
    }
    if (!g_key_file_save_to_file(keyfile, keyfilename, NULL)) {
        fprintf(stderr, "Cannot save configuration to %s:  %s\n",
                keyfilename, error->message);
        return 0;
    }
    return 1;
}

int parse_options (int argc, char **argv, struct program_options *opts) {
    int chOpt,              /* For getopt() */
        optIndex = 0;
    static struct option long_options[] = {
        { "airport",         required_argument, NULL, 'a' },
        { "database",        required_argument, NULL, 'b' },
        { "delay",           required_argument, NULL, 'd' },
        { "filename",        required_argument, NULL, 'f' },
        { "idle",            required_argument, NULL, 'i' },
        { "jiggle",          required_argument, NULL, 'j' },
        { "no-output",       no_argument,       NULL, 'n' },
        { "save",            no_argument,       NULL, 's' },
        { "time-format",     required_argument, NULL, 't' },
        { "weather",         required_argument, NULL, 'w' },
        { "jiggle-distance", required_argument, NULL, 'x' },
        { NULL,              0,                 NULL,  0  }
    };

    opterr = 0;
    while((chOpt = getopt_long(argc, argv, "a:b:d:f:i:j:nst:w:x:",
            long_options, &optIndex)) != -1) {
        switch(chOpt) {
            case 'a':
                strncpy(opts->airport, optarg, sizeof(opts->airport));
                break;
            case 'b':
                opts->use_database = 1;
                strncpy(opts->dbname, optarg, sizeof(opts->dbname));
                strncpy(opts->time_format, "%Y-%m-%dT%T", sizeof(opts->time_format));
                break;
            case 'd':
                opts->poll_period = atoi(optarg);
                break;
            case 'f':
                strncpy(opts->filename, optarg, sizeof(opts->filename));
                break;
            case 'i':
                opts->idle_threshold = atoi(optarg);
                break;
            case 'j':
                opts->mouse_period = atoi(optarg);
                break;
            case 'n':
                opts->text_out = 0;
                break;
            case 's':
                opts->save_options = 1;
                break;
            case 't':
                if (!opts->use_database) {
                    strncpy(opts->time_format, optarg, sizeof(opts->time_format));
                }
                break;
            case 'w':
                opts->weather = strcmp(optarg, "off");
                break;
            case 'x':
                opts->mouse_dist = atoi(optarg);
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
                fprintf(stderr, "%s ", argv[0]);
                fprintf(stderr, "[-a airport] ");
                fprintf(stderr, "[-b database] ");
                fprintf(stderr, "[-d period] ");
                fprintf(stderr, "[-f file.csv] ");
                fprintf(stderr, "[-i idle] ");
                fprintf(stderr, "[-j period] ");
                fprintf(stderr, "[-n] ");
                fprintf(stderr, "[-s] ");
                fprintf(stderr, "[-t format] ");
                fprintf(stderr, "[-w weather] ");
                fprintf(stderr, "[-x distance]\n\n");
                return 1;
            default:
                return 2;
        }
    }
    return 0;
}

