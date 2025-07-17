#include "response.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"
#include "http.h"
#include "target.h"

void http_response_init(http_response *response) {
    response->field_count = 0;
    response->fields = NULL;
}

void http_response_destroy(http_response *response) {
    free(response->fields);
}

void http_response_add_field(http_response *response, http_field field) {
    response->field_count++;
    response->fields =
        realloc(response->fields, sizeof(http_field) * response->field_count);
    response->fields[response->field_count - 1] = field;
}

// TODO: implement status codes properly
http_response http_response_generate(const http_request *request) {
    http_response response;
    http_response_init(&response);

    response.version = (http_version){1, 1};
    response.status_code = 200;

    const char *target_type = target_get_type(request->target);
    http_response_add_field(&response,
                            (http_field){"Content-Type", target_type});

    // TODO: set the content to 404 default if target not found
    bool target_get_content_result = target_get_content(
        request->target, &response.content, &response.content_size);
    if (!target_get_content_result) {
        response.status_code = 404;
        fprintf(stderr, "http_response_generate: Failed to resolve target %s\n",
                request->target);
    }

    // TODO: check for memory leaks that definitely exist
    // TODO: check when strings are being freed

    char *content_size =
        malloc(sizeof(char) * (helper_digit_count(response.content_size) + 1));
    sprintf(content_size, "%u", response.content_size);

    http_response_add_field(&response,
                            (http_field){"Content-Length", content_size});

    return response;
}

char *http_response_to_string(const http_response *response,
                              unsigned int *response_size) {
    *response_size = 0;
    unsigned int response_position = 0;
    char *response_string = NULL;

    // The status line
    *response_size += 14;
    response_string = realloc(response_string, *response_size);
    sprintf(response_string, "HTTP/%u.%u %03u\r\n", response->version.minor,
            response->version.major, response->status_code);

    for (int i = 0; i < response->field_count; i++) {
        response_position = *response_size;
        *response_size += strlen(response->fields[i].name) + 2 +
                          strlen(response->fields[i].value) + 2;
        response_string = realloc(response_string, *response_size);
        sprintf(response_string + response_position, "%s: %s\r\n",
                response->fields[i].name, response->fields[i].value);
    }

    response_position = *response_size;
    *response_size += 2 + response->content_size;
    response_string = realloc(response_string, *response_size);
    sprintf(response_string + response_position, "\r\n%.*s",
            response->content_size, response->content);

    return response_string;
}
