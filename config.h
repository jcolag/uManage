#ifndef CONFIG__H
#define CONFIG__H

struct program_options {
    int             poll_period,
                    mouse_period,       /* Move mouse every (s) */
                    idle_threshold,     /* Treat time as idle (s) */
                    save_options,
                    use_database,
                    text_out,           /* Print to console or file */
                    pause;              /* Don't record */
    char            filename[256],
                    dbname[256],
                    time_format[64];
};

int get_configuration (struct program_options *);
int save_configuration (struct program_options *);
int parse_options (int, char **, struct program_options *);

#endif

