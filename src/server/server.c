#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include "helper.h"

int server_init(int *server) {
    // Create the TCP network socket
    *server = socket(PF_INET, SOCK_STREAM, 0);

    if (*server == -1) {
        printf("Failed to create socket");
        return 1;
    }

    return 0;
}

int server_bind(int server) {
    // Form the address for the socket to connect to
    uint8_t ip[4] = {0, 0, 0, 0};
    uint16_t port = 8000;
    struct sockaddr_in address = generate_address(ip, port);

    // Try to bind the socket to the address generated
    int bind_result =
        bind(server, (struct sockaddr *)&address, sizeof(address));

    // Error message if binding failed
    if (bind_result == -1) {
        fprintf(stderr, "Error binding to %d.%d.%d.%d on port %d: ", ip[0],
                ip[1], ip[2], ip[3], port);
        perror("");

        return 2;
    }

    printf("Socket is bound to %d.%d.%d.%d on port %d\n", ip[0], ip[1], ip[2],
           ip[3], port);

    return 0;
}

// Currently multiple processes should switch to event-driven
int server_listen(int server) {
    listen(server, 4);

    struct sockaddr_in client_address;
    socklen_t client_length = sizeof(client_address);

    printf("Listening for client connections\n");

    while (true) {
        int client =
            accept(server, (struct sockaddr *)&client_address, &client_length);

        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            // The server socket isn't needed for the child process
            close(server);

            uint8_t client_ip[4] = {
                (ntohl(client_address.sin_addr.s_addr) & 0xff000000) >> 24,
                (ntohl(client_address.sin_addr.s_addr) & 0x00ff0000) >> 16,
                (ntohl(client_address.sin_addr.s_addr) & 0x0000ff00) >> 8,
                ntohl(client_address.sin_addr.s_addr) & 0x000000ff,
            };

            printf("Connected to client %d.%d.%d.%d on port %d\n", client_ip[0],
                   client_ip[1], client_ip[2], client_ip[3],
                   ntohs(client_address.sin_port));

            handle_client(client);

            // Disconnect the client socket
            close(client);
            printf("Disconnected from client\n");

            // Kill the child process
            exit(0);
        } else if (pid > 0) {
            // Parent process
            // The client socket isn't needed for the parent process
            close(client);
        } else {
            perror("fork");
            return 3;
        }
    }
}

void server_close(int server) {
    close(server);
}

void handle_client(int client) {
    char *message = "Hello, world!";
    send(client, message, 13, 0);
}
