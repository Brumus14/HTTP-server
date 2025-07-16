#ifndef REQUEST_H
#define REQUEST_H

#include <stdbool.h>
#include "http.h"

typedef struct {
    http_version version;
    char *target;
    http_method method;
    int field_count;
    http_field *fields;
} http_request;

void http_request_init(http_request *request);
void http_request_destroy(http_request *request);
void http_request_add_field(http_request *request, http_field field);
void http_request_parse(http_request *request, char *string);

#endif
