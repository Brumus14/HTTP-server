#include "response.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "helper.h"
#include "http.h"
#include "request.h"
#include "target.h"

#define STATUS_LINE_LENGTH 14
#define DATE_STRING_LENGTH 29
#define BODY_404 "404 Not Found"

void http_response_init(http_response *response) {
    response->header_count = 0;
    response->headers = NULL;
    response->body_size = 0;
    response->body = NULL;
}

void http_response_destroy(http_response *response) {
    // Free header strings
    for (int i = 0; i < response->header_count; i++) {
        free((char *)response->headers[i].name);
        free((char *)response->headers[i].value);
    }

    free(response->headers);
    free(response->body);
}

void http_response_add_header(http_response *response, http_field header) {
    response->header_count++;
    response->headers =
        realloc(response->headers, sizeof(http_field) * response->header_count);
    response->headers[response->header_count - 1] =
        (http_field){strdup(header.name), strdup(header.value)};
}

void handle_get_request(const http_request *request, http_response *response) {
    // Add the targets content to the response body
    bool exists = target_get_content(request->target, &response->body,
                                     &response->body_size);

    // Target doesn't exist so return 404 response
    // TODO: Abstract this and handle all error responses
    if (!exists) {
        response->status_code = 404;

        response->body = malloc(strlen(BODY_404) + 1);
        strcpy(response->body, BODY_404);
        response->body_size = strlen(BODY_404);

        http_response_add_header(response,
                                 (http_field){"Content-Type", "text/plain"});

        char *content_size = malloc(
            (helper_digit_count(response->body_size) + 1) * sizeof(char));
        sprintf(content_size, "%u", response->body_size);
        http_response_add_header(response,
                                 (http_field){"Content-Length", content_size});

        return;
    }

    // Get the content size as a string
    char *content_size =
        malloc(sizeof(char) * (helper_digit_count(response->body_size) + 1));
    sprintf(content_size, "%u", response->body_size);

    http_response_add_header(response,
                             (http_field){"Content-Length", content_size});
    free(content_size);

    const char *target_type = target_get_type(request->target);
    http_response_add_header(response,
                             (http_field){"Content-Type", target_type});

    char date_string[DATE_STRING_LENGTH + 1];
    helper_get_gmt_time(date_string, sizeof(date_string));
    http_response_add_header(response, (http_field){"Date", date_string});
}

void handle_head_request(const http_request *request, http_response *response) {
    handle_get_request(request, response);
    response->body_size = 0;
}

// TODO: implement status codes properly
http_response http_response_generate(const http_request *request,
                                     bool good_request, http_client client) {
    http_response response;
    http_response_init(&response);

    response.version = (http_version){1, 1};
    response.status_code = 200;

    if (!good_request) {
        response.status_code = 400;
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
        // Not implemented
        response.status_code = 501;
        return response;
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

    for (int i = 0; i < response->header_count; i++) {
        response_position = *response_size;
        *response_size += strlen(response->headers[i].name) + 2 +
                          strlen(response->headers[i].value) + 2;
        response_string = realloc(response_string, *response_size);

        char header_line[*response_size - response_position + 1];
        sprintf(header_line, "%s: %s\r\n", response->headers[i].name,
                response->headers[i].value);
        memcpy(response_string + response_position, header_line,
               *response_size - response_position);
    }

    response_position = *response_size;
    *response_size += 2;
    response_string = realloc(response_string, *response_size);
    memcpy(response_string + response_position, "\r\n", sizeof(char) * 2);

    response_position = *response_size;
    *response_size += response->body_size;
    response_string = realloc(response_string, *response_size);
    memcpy(response_string + response_position, response->body,
           response->body_size);

    return response_string;
}
