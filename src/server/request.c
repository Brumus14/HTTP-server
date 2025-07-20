#include "request.h"
#include "http.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void http_request_init(http_request *request) {
    request->header_count = 0;
    request->headers = NULL;
}

void http_request_destroy(http_request *request) {
    free(request->headers);
}

// TODO: Maybe add merging headers if the request has multiple headers with same
// name
void http_request_add_header(http_request *request, http_field header) {
    request->header_count++;
    request->headers =
        realloc(request->headers, sizeof(http_field) * request->header_count);
    request->headers[request->header_count - 1] = header;
}

void parse_request_line(http_request *request, char *line, http_client client) {
    // Split up the request line into its segments
    char *method = strtok(line, " ");
    char *target = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    bool parse_method_result =
        http_method_from_string(method, &request->method);
    if (!parse_method_result) {
        fprintf(stderr, "parse_request_line: Failed to parse method\n");
    }

    request->target = target;
    // Skip past the HTTP/ part of the version
    char *version_number = version + 5;
    request->version.minor = atoi(version_number);
    request->version.major = atoi(version_number + 2);

    printf("%s request for %s from %u.%u.%u.%u port %u\n", method,
           request->target, client.ip[0], client.ip[1], client.ip[2],
           client.ip[3], client.port);
}

void parse_header_line(http_request *request, char *line) {
    // Split the line into the name and value
    char *name = line;
    *strchr(line, ':') = '\0';

    char *value = name + strlen(name) + 1;
    // Skip past any whitespace between the name and value
    value += strspn(value, " \t");

    // Ignore any whitespace at the end of the line
    int line_end = strlen(line) - 1;

    while (line[line_end] == ' ' || line[line_end] == '\t') {
        line_end--;
    }

    line[line_end + 1] = '\0';

    http_request_add_header(request, (http_field){name, value});
}

void http_request_parse(http_request *request, char *string,
                        unsigned int string_length, http_client client) {
    // Parse the request line by line
    char *line = string;
    char *end;

    for (int i = 0; i < string_length - 1; i++) {
        if (string[i] == '\r' && string[i + 1] == '\n') {
            end = string + i;
            *end = '\0';

            if (line == string) {
                parse_request_line(request, line, client);
            } else {
                parse_header_line(request, line);
            }

            // Skip past \r\n to reach next line
            line = end + 2;

            if (*line == '\r') {
                break;
            }
        }
    }
}
