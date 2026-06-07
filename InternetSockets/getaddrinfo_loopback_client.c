#include "common.h"

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;

    struct addrinfo *list;
    int rc = getaddrinfo("127.0.0.1", "80", &hints, &list);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return 1;
    }

    int count = 0;
    for (struct addrinfo *it = list; it != NULL; it = it->ai_next) {
        count++;
    }
    printf("resolved loopback stream endpoints=%d\n", count);
    freeaddrinfo(list);
    return 0;
}
