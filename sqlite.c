#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "sqlite.h"

int write_to_database(char *table, char *, int);

char        database_name[256];
sqlite3    *sql = NULL;

int open_database(char *name) {
    char *activity = "CREATE TABLE IF NOT EXISTS activity (start TEXT, window TEXT, title TEXT, used INTEGER, idle INTEGER);",
         *pauses = "CREATE TABLE IF NOT EXISTS pauses (start TEXT, end TEXT);",
         *keepalives = "CREATE TABLE IF NOT EXISTS keepalives (start TEXT, end TEXT);",
         *error;
    int status = 0;

    /* Stash the database name in case we need to re-open */
    strcpy(database_name, name);

    /* Open the file and make sure the table exists */
    status = sqlite3_open(database_name, &sql);
    if (status) {
        return status;
    }
    status = sqlite3_exec(sql, activity, NULL, NULL, &error);
    if (status) {
        return status;
    }
    status = sqlite3_exec(sql, pauses, NULL, NULL, &error);
    if (status) {
        return status;
    }
    status = sqlite3_exec(sql, keepalives, NULL, NULL, &error);
    return status;
}

int write_to_database(char *table, char *insert, int cycle) {
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
    sprintf(query, "INSERT INTO %s VALUES (%s);", table, insert);
    status = sqlite3_exec(sql, query, NULL, NULL, &error);
    if (status) {
        return -1;
    }
    if (cycle) {
        close_database();
    }
    return status;
}

int write_activity_to_database(char *insert, int cycle) {
    return write_to_database("activity", insert, cycle);
}

int write_keepalive_to_database(char *insert) {
    return write_to_database("keepalives", insert, 0);
}

int write_pause_to_database(char *insert) {
    return write_to_database("pauses", insert, 0);
}

void close_database(void) {
    if (sql == NULL) {
        return;
    }
    sqlite3_close(sql);
    sql = NULL;
}
