#ifndef HTTP_H
#define HTTP_H

typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_TRACE,
} http_method;

typedef struct {
    unsigned int minor;
    unsigned int major;
} http_version;

typedef struct {
    char *name;
    char *value;
} http_field;

#endif
