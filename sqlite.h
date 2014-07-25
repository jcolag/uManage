#ifndef SQLITE__H
#define SQLITE__H

int open_database(char *);
int write_activity_to_database(char *, int);
int write_keepalive_to_database(char *);
int write_pause_to_database(char *, char *);
void close_database(void);

#endif
