#include "response.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"
#include "http.h"
#include "request.h"
#include "target.h"

#define STATUS_LINE_LENGTH 14

void http_response_init(http_response *response) {
    response->field_count = 0;
    response->fields = NULL;
    response->content_size = 0;
    response->content = NULL;
}

void http_response_destroy(http_response *response) {
    for (int i = 0; i < response->field_count; i++) {
        free((char *)response->fields[i].name);
        free((char *)response->fields[i].value);
    }

    free(response->fields);
    free(response->content);
}

void http_response_add_field(http_response *response, http_field field) {
    response->field_count++;
    response->fields =
        realloc(response->fields, sizeof(http_field) * response->field_count);
    response->fields[response->field_count - 1] =
        (http_field){strdup(field.name), strdup(field.value)};
}

void handle_get_request(const http_request *request, http_response *response) {
    target_get_content(request->target, &response->content,
                       &response->content_size);

    char *content_size =
        malloc(sizeof(char) * (helper_digit_count(response->content_size) + 1));
    sprintf(content_size, "%u", response->content_size);

    http_response_add_field(response,
                            (http_field){"Content-Length", content_size});
    free(content_size);

    const char *target_type = target_get_type(request->target);
    http_response_add_field(response,
                            (http_field){"Content-Type", target_type});
}

void handle_head_request(const http_request *request, http_response *response) {
    unsigned int target_size = target_get_size(request->target);

    char *content_size =
        malloc(sizeof(char) * (helper_digit_count(target_size) + 1));
    sprintf(content_size, "%u", target_size);

    http_response_add_field(response,
                            (http_field){"Content-Length", content_size});
    free(content_size);

    const char *target_type = target_get_type(request->target);
    http_response_add_field(response,
                            (http_field){"Content-Type", target_type});
}

// TODO: implement status codes properly
http_response http_response_generate(const http_request *request) {
    http_response response;
    http_response_init(&response);

    response.version = (http_version){1, 1};
    response.status_code = 200;

    if (target_exists(request->target)) {
        response.status_code = 404;
        fprintf(stderr, "http_response_generate: Failed to resolve target %s\n",
                request->target);
        return response;
    }

    // TODO: check when strings are being freed

    switch (request->method) {
    case HTTP_METHOD_GET:
        handle_get_request(request, &response);
        break;
    case HTTP_METHOD_HEAD:
        handle_head_request(request, &response);
        break;
    default:
        break;
    }

    return response;
}

// TODO: review memcpy's and sizeof's and types here
// TODO: handle realloc fails
char *http_response_to_string(const http_response *response,
                              unsigned int *response_size) {
    *response_size = 0;
    unsigned int response_position = 0;
    char *response_string = NULL;

    // The status line
    *response_size += STATUS_LINE_LENGTH;
    response_string = realloc(response_string, *response_size);

    char status_line[STATUS_LINE_LENGTH + 1];
    sprintf(status_line, "HTTP/%u.%u %03u\r\n", response->version.minor,
            response->version.major, response->status_code);
    memcpy(response_string, status_line, sizeof(char) * STATUS_LINE_LENGTH);

    for (int i = 0; i < response->field_count; i++) {
        response_position = *response_size;
        *response_size += strlen(response->fields[i].name) + 2 +
                          strlen(response->fields[i].value) + 2;
        response_string = realloc(response_string, *response_size);

        char field_line[*response_size - response_position + 1];
        sprintf(field_line, "%s: %s\r\n", response->fields[i].name,
                response->fields[i].value);
        memcpy(response_string + response_position, field_line,
               *response_size - response_position);
    }

    response_position = *response_size;
    *response_size += 2;
    response_string = realloc(response_string, *response_size);
    memcpy(response_string + response_position, "\r\n", sizeof(char) * 2);

    response_position = *response_size;
    *response_size += response->content_size;
    response_string = realloc(response_string, *response_size);
    memcpy(response_string + response_position, response->content,
           response->content_size);

    return response_string;
}
