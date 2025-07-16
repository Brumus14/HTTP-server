#include "target.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Review type sizes here
bool target_get_content(const char *target, char **content, int *content_size) {
    FILE *file = fopen(target + 1, "rb");

    // Target file doesn't exist
    if (file == NULL) {
        return false;
    }

    fseek(file, 0, SEEK_END);
    *content_size = ftell(file);
    rewind(file);

    *content = malloc(sizeof(char) * *content_size);
    fread(*content, 1, *content_size, file);

    return true;
}

typedef struct {
    const char *extension;
    const char *type;
} type_map_pair;

static const type_map_pair type_map[] = {
    {"bmp", "image/bmp"},
    {"css", "text/css"},
    {"csv", "text/csv"},
    {"gif", "image/gif"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"ico", "image/vnd.microsoft.icon"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"js", "text/javascript"},
    {"json", "application/json"},
    {"md", "text/markdown"},
    {"mp3", "audio/mpeg"},
    {"mp4", "video/mp4"},
    {"otf", "font/otf"},
    {"png", "image/png"},
    {"pdf", "application/pdf"},
    {"svg", "image/svg+xml"},
    {"tif", "image/tiff"},
    {"tiff", "image/tiff"},
    {"ttf", "font/ttf"},
    {"txt", "text/plain"},
    {"wav", "audio/wav"},
    {"webp", "image/webp"},
    {"woff", "font/woff"},
    {"woff2", "font/woff2"},
    {"xml", "application/xml"},
};

bool target_extension_to_type(const char *extension, const char **type) {
    static const int type_map_size = sizeof(type_map) / sizeof(type_map[0]);

    for (int i = 0; i < type_map_size; i++) {
        if (strcmp(extension, type_map[i].extension) == 0) {
            *type = type_map[i].type;
            return true;
        }
    }

    return false;
}
