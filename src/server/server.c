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
#include <stdbool.h>
#include <stdint.h>
#include "helper.h"
#include "request.h"

int server_init(int *server) {
    // Create the TCP network socket
    *server = socket(PF_INET, SOCK_STREAM, 0);

    if (*server == -1) {
        printf("Failed to create socket");
        return 1;
    }

    int reuse_address = 1;
    setsockopt(*server, SOL_SOCKET, SO_REUSEADDR, &reuse_address,
               sizeof(reuse_address));

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

void parse_request_line(http_request *request, char *line) {
    char *method = strtok(line, " ");
    char *target = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    request->method = string_to_method(method);
    request->target = target;
    request->version = atof(version + 5);

    printf("%s - %f\n", version, request->version);
}

void parse_field_line(http_request *request, char *line) {
    int line_end = strlen(line) - 1;

    while (line[line_end] == ' ' || line[line_end] == '\t') {
        line_end--;
    }

    line[line_end + 1] = '\0';

    char *name = line;
    *strchr(line, ':') = '\0';

    char *value = name + strlen(name) + 1;
    value += strspn(value, " \t");

    http_request_add_field(request, (http_field){name, value});
}

void handle_client(int client) {
    char *request_content = NULL;
    char request_buffer[1024];

    int request_size = 0;

    // TODO: Handle two requests that were both recieved at same time
    do {
        int recieved_size =
            recv(client, request_buffer, sizeof(request_buffer), 0);
        request_content =
            realloc(request_content, request_size + recieved_size + 1);
        memcpy(request_content + request_size, request_buffer, recieved_size);
        request_size += recieved_size;
        request_content[request_size] = '\0';
    } while (strstr(request_content, "\r\n\r\n") == NULL);

    http_request request;
    http_request_init(&request);

    printf("%s\n", request_content);

    char *line = request_content;
    char *end;

    while ((end = strstr(line, "\r\n")) != NULL) {
        *end = '\0';

        if (line == request_content) {
            parse_request_line(&request, line);
        } else {
            parse_field_line(&request, line);
        }

        line = end + 2;

        if (*line == '\r') {
            break;
        }
    }

    http_request_destroy(&request);
    free(request_content);
}
