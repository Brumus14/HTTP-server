#include "request.h"
#include "http.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void http_request_init(http_request *request) {
    request->header_count = 0;
    request->headers = NULL;
    request->body_size = 0;
    request->body = NULL;
}

void http_request_destroy(http_request *request) {
    free(request->headers);
}

// TODO: add merging headers if the request has multiple headers with same name
void http_request_add_header(http_request *request, http_field header) {
    request->header_count++;
    request->headers =
        realloc(request->headers, sizeof(http_field) * request->header_count);
    request->headers[request->header_count - 1] = header;
}

bool parse_request_line(http_request *request, char *line, http_client client) {
    // Split up the request line into its segments
    char *method = strtok(line, " ");
    char *target = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    if (method == NULL || target == NULL || version == NULL) {
        return false;
    }

    bool method_from_string_result =
        http_method_from_string(method, &request->method);
    if (!method_from_string_result) {
        fprintf(stderr, "parse_request_line: Failed to parse method\n");
        return false;
    }

    request->target = target;
    // Skip past the HTTP/ part of the version
    char *version_number = version + 5;
    request->version.minor = atoi(version_number);
    request->version.major = atoi(version_number + 2);

    printf("%s request for %s from %u.%u.%u.%u port %u\n", method,
           request->target, client.ip[0], client.ip[1], client.ip[2],
           client.ip[3], client.port);

    return true;
}

bool parse_header_line(http_request *request, char *line) {
    // Split the line into the name and value
    char *name = line;
    char *colon = strchr(line, ':');

    if (colon == NULL) {
        return false;
    }

    *colon = '\0';

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

    return true;
}

bool http_request_parse(http_request *request, char *string,
                        unsigned int string_length, http_client client) {
    // Parse the request line by line
    char *line = string;
    char *end;

    // Iterate over each line in the request
    for (int i = 0; i < string_length - 1; i++) {
        if (string[i] == '\r' && string[i + 1] == '\n') {
            // Replace the \r with \0 to null terminate the line
            end = string + i;
            *end = '\0';

            // The request line is the first line
            if (line == string) {
                bool result = parse_request_line(request, line, client);

                if (!result) {
                    return result;
                }
            } else {
                bool result = parse_header_line(request, line);

                if (!result) {
                    return result;
                }
            }

            // Skip past \r\n to reach next line
            line = end + 2;

            // Don't parse the end of headers marker line
            if (*line == '\r') {
                break;
            }
        }
    }

    return true;
}
