#ifndef TARGET_H
#define TARGET_H

#include <stdbool.h>

bool target_get_content(const char *target, char **content, int *content_size);
bool target_extension_to_type(const char *extension, const char **type);

#endif
