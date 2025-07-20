#include "http.h"

#include <string.h>

const http_method_map_pair method_map[] = {
    {"GET", HTTP_METHOD_GET},         {"HEAD", HTTP_METHOD_HEAD},
    {"POST", HTTP_METHOD_POST},       {"PUT", HTTP_METHOD_PUT},
    {"DELETE", HTTP_METHOD_DELETE},   {"CONNECT", HTTP_METHOD_CONNECT},
    {"OPTIONS", HTTP_METHOD_OPTIONS}, {"TRACE", HTTP_METHOD_TRACE},
};

const char *http_method_to_string(http_method method) {
    return method_map[method].string;
}

bool http_method_from_string(const char *string, http_method *method) {
    static const int method_map_size =
        sizeof(method_map) / sizeof(method_map[0]);

    for (int i = 0; i < method_map_size; i++) {
        if (strcmp(string, method_map[i].string) == 0) {
            *method = method_map[i].method;
            return true;
        }
    }

    return false;
}
