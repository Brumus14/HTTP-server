#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

void server_handle_client(int client);
bool server_init(int *server);
bool server_bind(int server);
bool server_listen(int server);
bool server_close(int server);

#endif
