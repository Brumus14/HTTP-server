#ifndef REQUEST_H
#define REQUEST_H

#include <stdbool.h>
#include <stdint.h>
#include "http.h"

typedef struct {
    http_version version;
    char *target;
    http_method method;
    int header_count;
    http_field *headers;
} http_request;

void http_request_init(http_request *request);
void http_request_destroy(http_request *request);
void http_request_add_header(http_request *request, http_field header);
void http_request_parse(http_request *request, char *string,
                        unsigned int string_length, uint8_t ip[4],
                        uint16_t port);

#endif
