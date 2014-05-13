struct program_options {
    int             poll_period,
                    idle_threshold;     /* Treat time as idle (s) */
    char            filename[256];
};

int get_configuration (struct program_options *);
int parse_options (int, char **, struct program_options *);

