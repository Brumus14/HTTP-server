#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>

struct sockaddr_in helper_generate_address(uint8_t ip[4], uint16_t port);
unsigned int helper_digit_count(unsigned int n);

#endif
