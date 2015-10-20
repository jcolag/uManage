#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include "sqlite.h"

int write_to_database(char *table, char *, int);

char        database_name[256];
sqlite3    *sql = NULL;

int open_database(char *name) {
    char *activity = "CREATE TABLE IF NOT EXISTS activity (start TEXT, window TEXT, title TEXT, used INTEGER, idle INTEGER);",
         *pauses = "CREATE TABLE IF NOT EXISTS pauses (start TEXT, end TEXT, reason TEXT);",
         *keepalives = "CREATE TABLE IF NOT EXISTS keepalives (start TEXT, end TEXT);",
         *weather = "CREATE TABLE IF NOT EXISTS weather (date TEXT, maxTemp INTEGER, meanTemp INTEGER, minTemp INTEGER, maxDew INTEGER, meanDew INTEGER, minDew INTEGER, maxHumid INTEGER, meanHumid INTEGER, minHumid INTEGER, maxPressure REAL, meanPressure REAL, minPressure REAL, maxVisibility INTEGER, meanVisibility INTEGER, minVisibility INTEGER, maxWind INTEGER, meanWind INTEGER, maxGust INTEGER, precipitation REAL, clouds INTEGER, events TEXT, windDirection INTEGER);",
         *error;
    int status = 0,
        sqlflags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX;

    /* Stash the database name in case we need to re-open */
    strcpy(database_name, name);

    /* Open the file and make sure the table exists */
    status = sqlite3_open_v2(database_name, &sql, sqlflags, NULL);
    if (status) {
        return status;
    }
    status = sqlite3_exec(sql, activity, NULL, NULL, &error);
    if (status) {
        return status;
    }
    status = sqlite3_exec(sql, weather, NULL, NULL, &error);
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
        sleep(1);
    }
    if (sql == NULL) {
        return -1;
    }
    status = sqlite3_exec(sql, "BEGIN IMMEDIATE;", NULL, NULL, &error);
    sprintf(query, "INSERT INTO %s VALUES (%s);", table, insert);
    status = sqlite3_exec(sql, query, NULL, NULL, &error);
    status = sqlite3_exec(sql, "COMMIT;", NULL, NULL, &error);
    if (status) {
        close_database();
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

int write_keepalive_to_database(char *insert, int cycle) {
    return write_to_database("keepalives", insert, cycle);
}

int write_duration_to_database(char *insert, char *reason, int cycle) {
    char csv[512];

    sprintf(csv, "%s,'%s'", insert, reason);
    return write_to_database("pauses", csv, cycle);
}

int write_weather_to_database(char *insert, int cycle) {
    return write_to_database("weather", insert, cycle);
}


int queryRowsForMonth(char *table, int year, int month) {
    int status, rows;
    char *fmt = "SELECT COUNT(DATE(date)) FROM %s WHERE date > DATE('%04d-%02d-01') AND date < DATE('%04d-%02d-31')", *remain, query[256];
    sqlite3_stmt *stmt;

    sprintf(query, fmt, table, year, month, year, month);
    status = sqlite3_prepare_v2(sql, query, strlen(query), &stmt, (const char**)&remain);
    if (status != SQLITE_OK) {
        return -status;
    }

    status = sqlite3_step(stmt);
    if (status != SQLITE_ROW) {
        return -1;
    }

    rows = sqlite3_column_int(stmt, 0);
    return rows;
}

void close_database(void) {
    int status = -1;

    if (sql == NULL) {
        return;
    }
    while (status != 0) {
        status = sqlite3_close_v2(sql);
        sleep(1);
    }
    sql = NULL;
}
