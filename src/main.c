#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int main() {
    // Create the TCP network socket
    int descriptor = socket(AF_INET, SOCK_STREAM, 0);

    // The target port
    short port = 8000;

    unsigned char ip[4] = {127, 0, 0, 1};

    // The target IP address
    struct in_addr ip_address = {
        htonl(((ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3]))};

    // Data for the sockets target address
    struct sockaddr_in address = {AF_INET, htonl(port), ip_address};

    // Make a connection with the socket to the address
    if (connect(descriptor, (struct sockaddr *)&address, sizeof(address)) ==
        -1) {
        // Generate an error message with the ip and port
        char message[41 + 1];
        sprintf(message, "Error connecting to %d.%d.%d.%d:%d", ip[0], ip[1],
                ip[2], ip[3], port);

        // Output the error from the connect attempt with the generated message
        perror(message);
    }

    return 0;
}
