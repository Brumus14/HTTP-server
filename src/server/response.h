#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdbool.h>
#include "http.h"
#include "request.h"

typedef struct {
    http_version version;
    int status_code;
    int field_count;
    http_field *fields;
    int content_size;
    char *content;
} http_response;

void http_response_init(http_response *response);
void http_response_destroy(http_response *response);
void http_response_add_field(http_response *response, http_field field);
void http_response_add_content(http_response *response, char *target);

http_response http_response_generate(http_request request);

#endif
