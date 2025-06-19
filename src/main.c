#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

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
    int descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (descriptor == -1) {
        printf("Failed to create socket");
    }

    // Form the address for the socket to connect to
    unsigned char ip[4] = {127, 0, 0, 1};
    unsigned short port = 8000;
    struct sockaddr_in address = generate_address(ip, port);

    // Attempt to connect the socket to the address
    int connect_result =
        connect(descriptor, (struct sockaddr *)&address, sizeof(address));

    if (connect_result == -1) {
        // Generate an error message with the IP and port
        char message[42 + 1];
        sprintf(message, "Failed to connect to %d.%d.%d.%d:%d", ip[0], ip[1],
                ip[2], ip[3], port);

        // Output the error from the connect attempt with the generated message
        perror(message);
    }

    char request[] = "GET / HTTP/1.1\r\nHost: 127.0.0.1:8000\r\n\r\n";
    send(descriptor, request, sizeof(request), 0);

    char response[128];
    recv(descriptor, response, sizeof(response), 0);

    printf("%s", response);

    return 0;
}
