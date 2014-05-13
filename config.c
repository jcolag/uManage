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
    int t_int;
    char *t_str;

    /* Defaults */
    strcpy(opts->filename, "");
    opts->poll_period = 1;
    opts->idle_threshold = 180;

    /* Create a new GKeyFile object and a bitwise list of flags. */
    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    /* Load the GKeyFile from keyfile.conf or return. */
    sprintf(keyfilename, "%s/.uManage", homedir);
    if (!g_key_file_load_from_file (keyfile, keyfilename, flags, &error)) {
        return 0;
    }

    /* Fill in any valid settings */
    t_int = g_key_file_get_integer(keyfile, "Timing", "poll", NULL);
    if (t_int != 0) {
        opts->poll_period = t_int;
    }
    t_int = g_key_file_get_integer(keyfile, "Timing", "idle", NULL);
    if (t_int != 0) {
        opts->idle_threshold = t_int;
    }
    t_str = g_key_file_get_string(keyfile, "File", "log", NULL);
    if (t_str != NULL) {
        strcpy(opts->filename, t_str);
    }
    return 1;
}

int parse_options (int argc, char **argv, struct program_options *opts) {
    int chOpt;              /* For getopt() */

    opterr = 0;
    while((chOpt = getopt(argc, argv, "d:f:i:")) != -1) {
        switch(chOpt) {
            case 'f':
                strncpy(opts->filename, optarg, sizeof(opts->filename));
                break;
            case 'd':
                opts->poll_period = atoi(optarg);
                break;
            case 'i':
                opts->idle_threshold = atoi(optarg);
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
                return 2;
        }
    }
    return 0;
}

