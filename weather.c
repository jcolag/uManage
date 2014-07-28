#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>

/*
 * Internal-only Functions
 */
int openSocket(char *, char *, char *);
int retrievePage(int, int, int, int, char *);
char *parseLine(char *);
char *matchNext(char *, char *);
char *matchNth(char *, char *, int);

/*
 * Global state, including a full buffer and indicators inside
 */
char    outcsv[16384],
       *current;
int     linesRemaining = 0;

/*
 * getWxMonth()
 * Parameters are year and month.
 *
 * Connect to the Weather Underground website to download
 * the previous month's weather.  Data is parsed and stored
 * in the outcsv string for later retrieval.
 *
 * Returns -1 on error, the number of data lines (days) found
 * for the month.
 */
int getWxMonth(int year, int month) {
    int     sock, status, count = -1;
    char    inmesg[16384], *pin, *eol = "<br />\n";

    current = outcsv;

    /*
     * Pull the weather data.
     */
    sock = openSocket("www.wunderground.com", "http", "tcp");
    if (sock < 3) {
        return -1;
    }
    status = retrievePage(sock, year, month, 31, inmesg);
    if (status < 0) {
        return -1;
    }

    /*
     * Find the end of the table header, to skip it and the
     * HTTP response header, since we don't need either.
     */
    pin = strstr(inmesg, eol);
    if (pin == NULL) {
        return 0;
    }

    /*
     * Count up and reformat each day of the month.
     */
    while (pin != NULL) {
        pin = parseLine(pin);
        ++count;
    }
    linesRemaining = count;
    return count;
}

/*
 * nextWxLine()
 *
 * Pull the next day's data from the buffer, copying it to
 * the provided buffer.
 *
 * Returns the length of the data.
 */
int nextWxLine(char *line) {
    int     len;
    char   *pin = strstr(current, "\n");

    /*
     * No line feed, so no data available...
     */
    if (pin == NULL) {
        if (linesRemaining > 0) {
            --linesRemaining;
        }
        line[0] = '\000';
        return 0;
    }

    /*
     * Otherwise, divide the strings and copy the first.
     */
    pin[0] = '\000';
    strcpy(line, current);
    len = strlen(line);
    current = pin + strlen("\n");
    --linesRemaining;

    return len;
}

/*
 * wxLinesRemaining()
 *
 * Returns the number of days' data still available.
 */
int wxLinesRemaining(void) {
    return linesRemaining;
}

/*
 * openSocket() - Internal use only
 * Parameters are the server, web service (HTTP), and protocol (TCP)
 *
 * Allocate a socket and connect it to the remote web server.
 *
 * Return -1 on any error (pick up errno later)
 * Otherwise, return the socket.
 */
int openSocket(char *hostname, char *service, char *protocol) {
    int                 sock, status;
    struct sockaddr_in  sockdata;
    struct hostent     *host;
    struct protoent    *proto;
    struct servent     *serv;

    host = gethostbyname(hostname);
    proto = getprotobyname(protocol);
    serv = getservbyname(service, protocol);

    /*
     * Initialize connection information
     */
    bzero((char *)&sockdata, sizeof (sockdata));
    sockdata.sin_family = AF_INET;
    sockdata.sin_addr.s_addr = INADDR_ANY;
    sockdata.sin_port = serv->s_port;
    bcopy(host->h_addr_list[0], (char*)&sockdata.sin_addr, host->h_length);

    /*
     * Open the socket and try to connect
     */
    sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
    if (sock < 3) {
        return -1;
    }
    status = connect(sock, (struct sockaddr *)&sockdata, sizeof (sockdata));
    if (status < 0) {
        return -1;
    }
    return sock;
}

/*
 * retrievePage()
 * Parameters are the socket, date (year, month, day), and input buffer
 *
 * Return -1 on any error (pick up errno later)
 * Otherwise, the length of the returned page.
 */
int retrievePage(int sock, int year, int month, int day, char *input){
    int     offset = 0, status;
    char    request[128];

    /*
     * Send a simple HTTP request for the date.
     */
    sprintf (request, "GET /history/airport/KFRG/%04d/%02d/%02d/MonthlyHistory.html?format=1 HTTP/1.0\n\n", year, month, day);
    status = send(sock, request, strlen(request), 0);
    if (status < 0) {
        return -1;
    }
    status = 1;

    /*
     * Keep reading until we have the entire file.
     */
    while (status > 0) {
        status = recv(sock, input + offset, sizeof(input), 0);
        offset += status;
    }
    return offset;
}

/*
 * parseLine()
 * Parameter is the starting point in the page to start parsing.
 *
 * Assumes data is from the Weather Underground.  Obviously, do
 * not use this to parse some sort of generic data.  That would
 * be a bad thing.
 *
 * Notes key fields as strings rather than numerical values.
 * Adds the resulting string to the buffer.
 *
 * Returns the point past the parsed string, updating the
 * start parameter.
 */
char *parseLine(char *start) {
static  int     icsv = 0;
        int     comma = 20;
        char    message[128], *pin = start, *shadow, *eol = "<br />\n";

    /*
     * No input?  No problem!
     */
    if (pin == NULL) {
        return NULL;
    }

    /*
     * Skip the end of the previous CSV line
     * Find the end of the first field
     * Quote that field (a date)
     */
    pin += strlen(eol);
    shadow = pin;
    if ((pin = matchNext(pin, ",")) == NULL) {
        return NULL;
    }
    sprintf(message, "\"%s\",", shadow);
    shadow = pin;

    /*
     * Find the description field and quote it
     * Don't forget to copy the fields in between
     */
    pin = matchNth(pin, ",", comma);
    strcat(message, shadow);
    strcat(message, ",\"");
    shadow = pin = pin + 1;
    if ((pin = matchNext(pin, ",")) == NULL) {
        return NULL;
    }
    strcat(message, shadow);
    strcat(message, "\",");
    shadow = pin;

    /*
     * Get to the end of the line
     * Copy the remainder of the line along
     */
    if ((pin = matchNext(pin, eol)) == NULL) {
        return NULL;
    }
    strcat(message, shadow);
    strcat(message, "\n");
    sprintf(outcsv + icsv, "%s", message);

    /*
     * Set the pointers
     */
    icsv += strlen(message);
    return pin;
}

/*
 * matchNext()
 * Parameters are the starting point to search and the string to match
 *
 * Returns the location of the target string, using it as a
 * separation point between the two strings.
 */
char *matchNext(char *message, char *target) {
    char   *start = message;

    start = strstr(start, target);
    if (start == NULL) {
        return start;
    }
    start[0] = '\000';
    return start + strlen(target);
}

/*
 * matchNth()
 * Parameters are the starting point to search, the string to match,
 * and which instance of the target is important
 *
 * Returns the location of the nth instance of target string, using it
 * as a separation point between the two strings.
 */
char *matchNth(char *message, char *target, int which) {
    int     idx;
    char   *start = message;

    for (idx = 0; idx < which; idx++) {
        start = strstr(start, target) + 1;
        if (start == NULL) {
            return NULL;
        }
    }
    --start;
    start[0] = '\000';
    return start;
}

