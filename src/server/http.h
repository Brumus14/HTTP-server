#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int descriptor;
    bool connected;
    uint8_t ip[4];
    uint16_t port;
} http_client;

typedef enum {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_TRACE,
} http_method;

typedef struct {
    const char *string;
    http_method method;
} http_method_map_pair;

extern const http_method_map_pair method_map[];

typedef struct {
    unsigned int minor;
    unsigned int major;
} http_version;

typedef struct {
    const char *name;
    const char *value;
} http_field;

const char *http_method_to_string(http_method method);
bool http_method_from_string(const char *string, http_method *method);

#endif
