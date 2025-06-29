#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdbool.h>

void handle_client(int client) {
    char *message = "Hello, world!";
    send(client, message, 13, 0);
}

struct sockaddr_in generate_address(unsigned char ip[4], unsigned short port) {
    return (struct sockaddr_in){
        AF_INET,
        // Convert hardware bytes to network (little endian to big)
        htons(port),
        // Merge the IP bytes to form a single integer
        {htonl(((ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3]))},
    };
}

int main() {
    // Create the TCP network socket
    int server = socket(PF_INET, SOCK_STREAM, 0);

    if (server == -1) {
        printf("Failed to create socket");
        return 1;
    }

    // Form the address for the socket to connect to
    unsigned char ip[4] = {127, 0, 0, 1};
    unsigned short port = 8000;
    struct sockaddr_in address = generate_address(ip, port);

    // Try to bind the socket to the address generated
    int bind_result =
        bind(server, (struct sockaddr *)&address, sizeof(address));

    // Error message if binding failed
    if (bind_result == -1) {
        char error_message[43 + 1];
        sprintf(error_message, "Error binding to %d.%d.%d.%d port %d", ip[0],
                ip[1], ip[2], ip[3], port);

        perror(error_message);
    }

    printf("Socket is bound to %d.%d.%d.%d port %d\n", ip[0], ip[1], ip[2],
           ip[3], port);

    listen(server, 4);

    struct sockaddr client_address;
    socklen_t client_length = sizeof(client_address);

    printf("Listening for client connections\n");

    while (true) {
        int client = accept(server, &client_address, &client_length);

        pid_t pid = fork();

        if (pid == 0) {
            // Child
            printf("Connected to client\n");
            close(server);

            handle_client(client);

            close(client);
            printf("Disconnected from child\n");

            exit(0);
        } else if (pid > 0) {
            // Parent
            close(client);
        }
    }

    // Shutdown and close the socket as not needed anymore
    shutdown(server, SHUT_RDWR);
    close(server);

    return 0;
}
