#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

bool server_init(int *server, char *directory);
bool server_bind(int server);
bool server_listen(int server);
bool server_close(int server);

#endif
