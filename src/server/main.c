#include "server.h"

int main() {
    int server;

    int init_result = server_init(&server);
    if (init_result != 0) {
        return init_result;
    }

    int bind_result = server_bind(server);
    if (bind_result != 0) {
        return bind_result;
    }

    int listen_result = server_listen(server);
    if (listen_result != 0) {
        return listen_result;
    }

    server_close(server);

    return 0;
}
