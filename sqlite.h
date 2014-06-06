#ifndef SQLITE__H
#define SQLITE__H

int open_database(char *);
int write_to_database(char *, int);
void close_database(void);

#endif
