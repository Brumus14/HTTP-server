#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdbool.h>
#include "http.h"
#include "request.h"

typedef struct {
    http_version version;
    unsigned int status_code;
    unsigned int header_count;
    http_field *headers;
    unsigned int body_size;
    char *body;
} http_response;

void http_response_init(http_response *response);
void http_response_destroy(http_response *response);
void http_response_add_header(http_response *response, http_field header);
http_response http_response_generate(const http_request *request,
                                     bool good_request, http_client client);
char *http_response_to_string(const http_response *response,
                              unsigned int *response_size);

#endif
