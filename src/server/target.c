#include "target.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TARGET_TYPE_DEFAULT "application/octet-stream"

bool target_exists(const char *target) {
    // Access the target relative to current directory
    int exists = access(target + 1, F_OK);

    if (exists == 0) {
        return true;
    }

    return false;
}

unsigned int target_get_size(const char *target) {
    // Open the target relative to current directory
    FILE *file = fopen(target + 1, "rb");

    // Target file doesn't exist
    if (file == NULL) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);

    fclose(file);

    return file_size;
}

// TODO: Review type sizes here
bool target_get_content(const char *target, char **content,
                        unsigned int *content_size) {
    FILE *file = fopen(target + 1, "rb");

    // Target file doesn't exist
    if (file == NULL) {
        *content_size = 0;
        *content = NULL;
        return false;
    }

    fseek(file, 0, SEEK_END);
    *content_size = ftell(file);
    rewind(file);

    *content = malloc(sizeof(char) * *content_size);
    fread(*content, 1, *content_size, file);

    fclose(file);

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

const char *target_get_type(const char *target) {
    static const int type_map_size = sizeof(type_map) / sizeof(type_map[0]);

    bool has_extension = false;
    const char *extension;

    // Get the target extension if possible
    for (int i = strlen(target) - 2; i > 0; i--) {
        if (target[i] == '.') {
            has_extension = true;
            extension = &target[i + 1];
            break;
        }
    }

    // Convert the target extension to its type
    if (has_extension) {
        for (int i = 0; i < type_map_size; i++) {
            if (strcmp(extension, type_map[i].extension) == 0) {
                return type_map[i].type;
            }
        }
    }

    return TARGET_TYPE_DEFAULT;
}
