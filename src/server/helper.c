#include "helper.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

struct sockaddr_in helper_generate_address(uint8_t ip[4], uint16_t port) {
    return (struct sockaddr_in){
        AF_INET,
        // Convert hardware bytes to network (little endian to big)
        htons(port),
        // Merge the IP bytes to form a single integer
        {htonl(((ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3]))},
    };
}

unsigned int helper_digit_count(unsigned int n) {
    unsigned int digit_count = 0;

    do {
        digit_count++;
        n /= 10;
    } while (n > 0);

    return digit_count;
}

void helper_get_gmt_time(char *date_string, unsigned int date_string_length) {
    static time_t t;
    static struct tm *tm;

    time_t now = time(NULL);

    if (t != now) {
        t = now;
        tm = gmtime(&t);
    }

    strftime(date_string, date_string_length, "%a, %d %b %Y %H:%M:%S GMT", tm);
}
