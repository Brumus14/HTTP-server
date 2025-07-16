#include "request.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    const char *string;
    http_method method;
} method_map_pair;

static const method_map_pair method_map[] = {
    {"GET", HTTP_METHOD_GET},         {"HEAD", HTTP_METHOD_HEAD},
    {"POST", HTTP_METHOD_POST},       {"PUT", HTTP_METHOD_PUT},
    {"DELETE", HTTP_METHOD_DELETE},   {"CONNECT", HTTP_METHOD_CONNECT},
    {"OPTIONS", HTTP_METHOD_OPTIONS}, {"TRACE", HTTP_METHOD_TRACE},
};

void http_request_init(http_request *request) {
    request->field_count = 0;
    request->fields = NULL;
}

void http_request_destroy(http_request *request) {
    free(request->fields);
}

// TODO: Maybe add merging fields if the request has multiple fields with same
// name
void http_request_add_field(http_request *request, http_field field) {
    request->field_count++;
    request->fields =
        realloc(request->fields, sizeof(http_field) * request->field_count);
    request->fields[request->field_count - 1] = field;
}

bool parse_method(const char *string, http_method *method) {
    static const int method_map_size =
        sizeof(method_map) / sizeof(method_map[0]);

    for (int i = 0; i < method_map_size; i++) {
        if (strcmp(string, method_map[i].string) == 0) {
            *method = method_map[i].method;
            return true;
        }
    }

    return false;
}

void parse_request_line(http_request *request, char *line) {
    // Split up the request line into its segments
    char *method = strtok(line, " ");
    char *target = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    bool parse_method_result = parse_method(method, &request->method);
    if (!parse_method_result) {
        fprintf(stderr, "parse_request_line: Failed to parse method\n");
    }

    // TODO: Maybe copy the string
    request->target = target;
    // Skip past the HTTP/ part of the version
    char *version_number = version + 5;
    request->version.minor = atoi(version_number);
    request->version.major = atoi(version_number + 2);

    printf("%s request for %s\n", method, request->target);
}

void parse_field_line(http_request *request, char *line) {
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

    http_request_add_field(request, (http_field){name, value});
}

void http_request_parse(http_request *request, char *string) {
    // Parse the request line by line
    char *line = string;
    char *end;

    while ((end = strstr(line, "\r\n")) != NULL) {
        *end = '\0';

        if (line == string) {
            parse_request_line(request, line);
        } else {
            parse_field_line(request, line);
        }

        // Skip past \r\n to reach next line
        line = end + 2;

        if (*line == '\r') {
            break;
        }
    }
}
