#ifndef SQLITE__H
#define SQLITE__H

int open_database(char *);
int write_activity_to_database(char *, int);
int write_keepalive_to_database(char *, int);
int write_duration_to_database(char *, char *, int);
int write_weather_to_database(char *, int);
int queryRowsForMonth(char *, int, int);
void close_database(void);

#endif
