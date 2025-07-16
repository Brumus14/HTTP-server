#include "server.h"

#include <asm-generic/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdint.h>
#include "helper.h"
#include "request.h"
#include "response.h"

bool server_init(int *server) {
    // Create the TCP network socket
    *server = socket(PF_INET, SOCK_STREAM, 0);

    if (*server == -1) {
        perror("server_init: Failed to create socket");
        return false;
    }

    // Allow the socket to reuse the address if old socket has timeout
    int reuse_address = 1;
    int reuse_address_result =
        setsockopt(*server, SOL_SOCKET, SO_REUSEADDR, &reuse_address,
                   sizeof(reuse_address));

    if (reuse_address_result == -1) {
        perror("server_init: Failed to set reuse address");
        return false;
    }

    return true;
}

bool server_bind(int server) {
    // Form the address for the socket to connect to
    uint8_t ip[4] = {0, 0, 0, 0};
    uint16_t port = 8000;
    struct sockaddr_in address = helper_generate_address(ip, port);

    // Try to bind the socket to the address generated
    int bind_result =
        bind(server, (struct sockaddr *)&address, sizeof(address));

    if (bind_result == -1) {
        fprintf(stderr,
                "server_bind: Failed to bind to %d.%d.%d.%d on port %d: ",
                ip[0], ip[1], ip[2], ip[3], port);
        perror("");
        return false;
    }

    printf("Server is bound to %d.%d.%d.%d on port %d\n", ip[0], ip[1], ip[2],
           ip[3], port);

    return true;
}

void server_handle_client(int client) {
    char *request_content = NULL;
    char request_buffer[1024];

    int request_size = 0;

    // TODO: Handle two requests that were both received at same time

    // Receive a request from the client
    do {
        int received_size =
            recv(client, request_buffer, sizeof(request_buffer), 0);

        request_content =
            realloc(request_content, request_size + received_size + 1);

        memcpy(request_content + request_size, request_buffer, received_size);
        request_size += received_size;
        request_content[request_size] = '\0';
    } while (strstr(request_content, "\r\n\r\n") == NULL);

    http_request request;
    http_request_init(&request);
    http_request_parse(&request, request_content);

    int response_size;
    http_response response = http_response_generate(request);
    // send(client, response, response_size, 0);

    http_request_destroy(&request);
    free(request_content);
}

// Currently multiple processes should switch to event-driven
bool server_listen(int server) {
    // Allow connections to the socket with backlog of 8
    int listen_result = listen(server, 8);
    if (listen_result == -1) {
        perror("server_listen: Failed to set server to listen");
    }

    struct sockaddr_in client_address;
    socklen_t client_length = sizeof(client_address);

    printf("Server is listening for connections\n");

    while (true) {
        // Wait for a connection and accept it
        int client =
            accept(server, (struct sockaddr *)&client_address, &client_length);

        if (client == -1) {
            perror("server_listen: Failed to accept connection");
        }

        // Split the combined ip number into a byte array
        uint8_t client_ip[4] = {
            (ntohl(client_address.sin_addr.s_addr) & 0xff000000) >> 24,
            (ntohl(client_address.sin_addr.s_addr) & 0x00ff0000) >> 16,
            (ntohl(client_address.sin_addr.s_addr) & 0x0000ff00) >> 8,
            ntohl(client_address.sin_addr.s_addr) & 0x000000ff,
        };

        uint16_t client_port = ntohs(client_address.sin_port);

        printf("Connected to client %d.%d.%d.%d on port %d\n", client_ip[0],
               client_ip[1], client_ip[2], client_ip[3], client_port);

        // Fork to another process to handle the client on
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            // The server socket isn't needed for the child process
            int close_result = close(server);
            if (close_result == -1) {
                perror("server_listen: Failed to close server on child");
            }

            server_handle_client(client);

            close_result = close(client);
            if (close_result == -1) {
                perror("server_listen: Failed to close client on child");
            }

            printf("Disconnected from client %d.%d.%d.%d on port %d\n",
                   client_ip[0], client_ip[1], client_ip[2], client_ip[3],
                   client_port);

            // Kill the child process
            exit(0);
        } else if (pid > 0) {
            // Parent process
            // The client socket isn't needed for the parent process
            int close_result = close(client);
            if (close_result == -1) {
                perror("server_listen: Failed to close client on parent");
            }
        } else if (pid == -1) {
            perror("server_listen: Failed to fork");
            return false;
        }
    }

    return true;
}

bool server_close(int server) {
    int close_result = close(server);
    if (close_result == -1) {
        perror("server_close: Failed to close");
        return false;
    }

    return true;
}
