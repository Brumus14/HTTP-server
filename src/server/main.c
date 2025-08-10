#include "server.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int server;

    bool init_result = server_init(&server, (argc > 1) ? argv[1] : ".");
    if (!init_result) {
        fprintf(stderr, "main: Failed to initialise server\n");
        return EXIT_FAILURE;
    }

    bool bind_result = server_bind(server);
    if (!bind_result) {
        fprintf(stderr, "main: Failed to bind server\n");
        return EXIT_FAILURE;
    }

    bool listen_result = server_listen(server);
    if (!listen_result) {
        fprintf(stderr, "main: Failed to listen on the server\n");
        return EXIT_FAILURE;
    }

    bool close_result = server_close(server);
    if (!close_result) {
        fprintf(stderr, "main: Failed to close server\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
