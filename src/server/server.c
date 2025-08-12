#include "server.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include "helper.h"
#include "http.h"
#include "request.h"
#include "response.h"
#include "target.h"

bool server_init(int *server, char *directory) {
    target_set_directory(directory);

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

    printf("Server socket created\n");

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
                "server_bind: Failed to bind to %u.%u.%u.%u on port %u: ",
                ip[0], ip[1], ip[2], ip[3], port);
        perror(NULL);
        return false;
    }

    printf("Server socket bound to %u.%u.%u.%u on port %u\n", ip[0], ip[1],
           ip[2], ip[3], port);

    return true;
}

// TODO: use more pointers to structs
bool handle_request(http_client client, char *request_string,
                    unsigned int request_string_length) {
    bool close_connection = false;

    http_request request;
    http_request_init(&request);
    bool request_parse_result = http_request_parse(
        &request, request_string, request_string_length, client);

    for (int i = 0; i < request.header_count; i++) {
        if (strcmp(request.headers[i].name, "Connection") == 0 &&
            strcmp(request.headers[i].value, "close") == 0) {
            close_connection = true;
        }
    }

    http_response response =
        http_response_generate(&request, request_parse_result, client);

    unsigned int response_size;
    char *response_string = http_response_to_string(&response, &response_size);

    // Send the whole response to the client
    unsigned int bytes_sent = 0;

    do {
        bytes_sent += send(client.descriptor, response_string + bytes_sent,
                           response_size - bytes_sent, 0);
    } while (bytes_sent < response_size);

    if (request_parse_result) {
        printf("Responded to %s request for %s from %u.%u.%u.%u port %u\n",
               http_method_to_string(request.method), request.target,
               client.ip[0], client.ip[1], client.ip[2], client.ip[3],
               client.port);
    } else {
        printf("Responded to %u.%u.%u.%u port %u\n", client.ip[0], client.ip[1],
               client.ip[2], client.ip[3], client.port);
    }

    free(response_string);
    http_response_destroy(&response);
    http_request_destroy(&request);

    return close_connection;
}

void handle_client(http_client client) {
    char *received_data = NULL;
    char received_data_buffer[1024];

    unsigned int total_recieved_size = 0;

    bool overflow_data = false;

    // Receive a request from the client
    while (true) {
        if (!overflow_data) {
            int received_size = recv(client.descriptor, received_data_buffer,
                                     sizeof(received_data_buffer), 0);

            // Client closed connection
            if (received_size == 0) {
                break;
            }

            // Append the received data to the total data buffer
            received_data =
                realloc(received_data, total_recieved_size + received_size + 1);

            memcpy(received_data + total_recieved_size, received_data_buffer,
                   received_size);
            total_recieved_size += received_size;
            received_data[total_recieved_size] = '\0';

            overflow_data = false;
        }

        char *request_end = strstr(received_data, "\r\n\r\n");

        // The headers have been fully received
        if (request_end != NULL) {
            unsigned int request_size = (request_end - received_data + 4);
            bool close_connection =
                handle_request(client, received_data, request_size);

            if (close_connection) {
                return;
            }

            // If also received part of another request
            if (request_size < total_recieved_size) {
                // Move the partial request to the front of the buffer
                memmove(received_data, received_data + request_size,
                        request_size);
                overflow_data = true;
            }

            // Resize the data buffer
            received_data =
                realloc(received_data, total_recieved_size - request_size);
            total_recieved_size -= request_size;
        }
    }

    free(received_data);
}

// TODO: Currently multiple processes should switch to event-driven
bool server_listen(int server) {
    // Allow connections to the socket with backlog of 8
    int listen_result = listen(server, 8);
    if (listen_result == -1) {
        perror("server_listen: Failed to set server to listen");
        return false;
    }

    struct sockaddr_in client_address;
    socklen_t client_length = sizeof(client_address);

    printf("Server is listening for connections\n");

    // Accept connections
    while (true) {
        http_client client;

        // Wait for a connection and accept it
        client.descriptor =
            accept(server, (struct sockaddr *)&client_address, &client_length);

        if (client.descriptor == -1) {
            perror("server_listen: Failed to accept connection");
            continue;
        }

        // Split the combined ip number into a byte array
        memcpy(client.ip,
               (uint8_t[4]){
                   (ntohl(client_address.sin_addr.s_addr) & 0xff000000) >> 24,
                   (ntohl(client_address.sin_addr.s_addr) & 0x00ff0000) >> 16,
                   (ntohl(client_address.sin_addr.s_addr) & 0x0000ff00) >> 8,
                   ntohl(client_address.sin_addr.s_addr) & 0x000000ff,
               },
               sizeof(uint8_t) * 4);

        client.port = ntohs(client_address.sin_port);
        client.connected = true;

        printf("Connected to client %u.%u.%u.%u on port %u\n", client.ip[0],
               client.ip[1], client.ip[2], client.ip[3], client.port);

        // Fork another process to handle the client on
        pid_t pid = fork();

        if (pid == 0) {
            // Child process for client
            // The server socket isn't needed for the child process
            int close_result = close(server);
            if (close_result == -1) {
                perror("server_listen: Failed to close server on child");
            }

            handle_client(client);

            close_result = close(client.descriptor);
            if (close_result == -1) {
                perror("server_listen: Failed to close client on child");
            }

            printf("Closed connection to client at %u.%u.%u.%u on port %u\n",
                   client.ip[0], client.ip[1], client.ip[2], client.ip[3],
                   client.port);

            // Kill the child process
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            // Parent process for server
            // The client socket isn't needed for the parent process
            int close_result = close(client.descriptor);
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
