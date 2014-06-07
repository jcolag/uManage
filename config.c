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
    strcpy(opts->time_format, "%Y,%m,%d,%H,%M,%S");
    opts->poll_period = 1;
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
        strcpy(opts->time_format, "%c");
        strcpy(opts->dbname, t_str);
    }
    t_int = g_key_file_get_integer(keyfile, "File", "notext", NULL);
    if (t_int != 0) {
        opts->text_out = !t_int;
    }
    return 1;
}

int save_configuration (struct program_options *opts) {
    struct passwd *passwd = getpwuid(getuid());
    const char *homedir = passwd->pw_dir;
    char keyfilename[256];
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
    if (!g_key_file_save_to_file(keyfile, keyfilename, NULL)) {
        fprintf(stderr, "Cannot save configuration to %s:  %s\n",
                keyfilename, error->message);
        return 0;
    }
    return 1;
}

int parse_options (int argc, char **argv, struct program_options *opts) {
    int chOpt;              /* For getopt() */

    opterr = 0;
    while((chOpt = getopt(argc, argv, "b:d:f:i:nst:")) != -1) {
        switch(chOpt) {
            case 'b':
                opts->use_database = 1;
                strncpy(opts->dbname, optarg, sizeof(opts->dbname));
                strncpy(opts->time_format, "%c", sizeof(opts->time_format));
                break;
            case 'f':
                strncpy(opts->filename, optarg, sizeof(opts->filename));
                break;
            case 'd':
                opts->poll_period = atoi(optarg);
                break;
            case 'i':
                opts->idle_threshold = atoi(optarg);
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

