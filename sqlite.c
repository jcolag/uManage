#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "sqlite.h"

char        database_name[256];
sqlite3    *sql = NULL;

int open_database(char *name) {
    char *create = "CREATE TABLE IF NOT EXISTS activity (start TEXT, window TEXT, title TEXT, used INTEGER, idle INTEGER);",
         *error;
    int status = 0;

    /* Stash the database name in case we need to re-open */
    strcpy(database_name, name);

    /* Open the file and make sure the table exists */
    status = sqlite3_open(database_name, &sql);
    if (status) {
        return status;
    }
    status = sqlite3_exec(sql, create, NULL, NULL, &error);
    return status;
}

int write_to_database(char *insert, int cycle) {
    char   *error,
            query[512];
    int     status = 0;

    /*
     * Insert a database row.
     * If cycle is non-zero, open (if needed) the database
     * and close when done.
     */
    if (cycle && sql == NULL) {
        open_database(database_name);
    }
    if (sql == NULL) {
        return -1;
    }
    sprintf(query, "INSERT INTO activity VALUES (%s);", insert);
    status = sqlite3_exec(sql, query, NULL, NULL, &error);
    if (status) {
        return -1;
    }
    if (cycle) {
        close_database();
    }
    return status;
}

void close_database(void) {
    if (sql == NULL) {
        return;
    }
    sqlite3_close(sql);
    sql = NULL;
}
