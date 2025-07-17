#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdbool.h>
#include "http.h"
#include "request.h"

typedef struct {
    http_version version;
    unsigned int status_code;
    unsigned int field_count;
    http_field *fields;
    unsigned int content_size;
    char *content;
} http_response;

void http_response_init(http_response *response);
void http_response_destroy(http_response *response);
void http_response_add_field(http_response *response, http_field field);
http_response http_response_generate(const http_request *request);
char *http_response_to_string(const http_response *response,
                              unsigned int *response_size);

#endif
