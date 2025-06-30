#ifndef HELPER_H
#define HELPER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

struct sockaddr_in generate_address(uint8_t ip[4], uint16_t port);

#endif
