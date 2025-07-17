#ifndef TARGET_H
#define TARGET_H

#include <stdbool.h>

bool target_get_content(const char *target, char **content,
                        unsigned int *content_size);
const char *target_get_type(const char *target);

#endif
