#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

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
    int client = socket(AF_INET, SOCK_STREAM, 0);

    if (client == -1) {
        printf("Failed to create socket");
        return 1;
    }

    // Form the address for the socket to connect to
    uint8_t ip[4] = {127, 0, 0, 1};
    uint16_t port = 8000;
    struct sockaddr_in address = generate_address(ip, port);

    // Attempt to connect the socket to the address
    int connect_result =
        connect(client, (struct sockaddr *)&address, sizeof(address));

    if (connect_result == -1) {
        // Generate an error message with the IP and port
        char message[42 + 1];
        sprintf(message, "Failed to connect to %u.%u.%u.%u on port %u", ip[0],
                ip[1], ip[2], ip[3], port);

        // Output the error from the connect attempt with the generated message
        perror(message);

        return 1;
    } else {
        printf("Connected\n");
    }

    char request[] = "GET /index.html HTTP/1.1\r\nExample-Field: Foo, "
                     "Bar\r\nField-Two: Baz\r\nContent-Type:      "
                     "text/html\r\nFoo:    Bar\r\n\r\n";

    send(client, request, sizeof(request), 0);

    char response[1024];
    response[recv(client, response, sizeof(response), 0)] = '\0';

    printf("%s\n", response);

    close(client);

    return 0;
}
