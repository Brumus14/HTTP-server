#ifndef REQUEST_H
#define REQUEST_H

#include <stdbool.h>
#include <stdint.h>
#include "http.h"

typedef struct {
    http_version version;
    char *target;
    http_method method;
    unsigned int header_count;
    http_field *headers;
    unsigned int body_size;
    char *body;
} http_request;

void http_request_init(http_request *request);
void http_request_destroy(http_request *request);
void http_request_add_header(http_request *request, http_field header);
bool http_request_parse(http_request *request, char *string,
                        unsigned int string_length, http_client client);

#endif
