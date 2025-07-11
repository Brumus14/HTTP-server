#include "server.h"
#include <stdio.h>

int main() {
    int server;

    bool init_result = server_init(&server);
    if (!init_result) {
        fprintf(stderr, "main: Failed to initialise server\n");
    }

    bool bind_result = server_bind(server);
    if (!bind_result) {
        fprintf(stderr, "main: Failed to bind server\n");
    }

    bool listen_result = server_listen(server);
    if (!listen_result) {
        fprintf(stderr, "main: Failed to listen on the server\n");
    }

    bool close_result = server_close(server);
    if (!close_result) {
        fprintf(stderr, "main: Failed to close server\n");
    }

    return 0;
}
