#include "response.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"
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

void http_response_add_content(http_response *response, char *target) {
    bool target_get_content_result =
        target_get_content(target, &response->content, &response->content_size);
    if (!target_get_content_result) {
        fprintf(stderr,
                "http_response_add_content: Failed to resolve target %s\n",
                target);
    }
}

http_response http_response_generate(http_request request) {
    unsigned int status_code = 200;

    bool has_extension = false;
    char *target_extension;

    for (int i = strlen(request.target) - 2; i > 0; i--) {
        if (request.target[i] == '.') {
            has_extension = true;
            target_extension = &request.target[i + 1];
            break;
        }
    }

    // TODO: what should the default type be
    // TODO: set correct status codes
    const char *target_type = "application/octet-stream";

    if (has_extension) {
        bool extension_to_type_result =
            target_extension_to_type(target_extension, &target_type);

        if (extension_to_type_result) {
            printf("Target content type is: %s\n", target_type);
        } else {
            printf("Couldn't resolve target type using default\n");
        }
    }

    char content_type[14 + strlen(target_type) + 1];
    sprintf(content_type, "Content-Type: %s", target_type);

    char *target_content;
    int target_size;

    char content_length[16 + helper_digit_count(target_size) + 1];
    sprintf(content_length, "Content-Length: %d", target_size);

    char status_line[12 + 1];
    sprintf(status_line, "HTTP/1.1 %03d", status_code);

    printf("%s\n%s\n%s\n%s\n", status_line, content_type, content_length,
           target_content);

    char *response = malloc(sizeof(char) * 1);

    // *size = 13;
    // char *response = malloc(sizeof(char) * *size);
    // memcpy(response,
    //        "HTTP/1.1 200\r\nContent-Type: text/plain\r\nContent-Length: "
    //        "13\r\n\r\nHello, world!",
    //        *size);
    //
    return (http_response){};
}
