#ifndef SERVER_H
#define SERVER_H

void handle_client(int client);
int server_init(int *server);
int server_bind(int server);
int server_listen(int server);
void server_close(int server);

#endif
