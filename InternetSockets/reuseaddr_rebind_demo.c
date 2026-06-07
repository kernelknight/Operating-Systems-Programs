#include "common.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int bind_once(unsigned short port, unsigned short *actual)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        die_errno("socket");
    }
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        die_errno("bind");
    }
    socklen_t len = sizeof(addr);
    getsockname(fd, (struct sockaddr *)&addr, &len);
    *actual = ntohs(addr.sin_port);
    return fd;
}

int main(void)
{
    unsigned short port;
    int first = bind_once(0, &port);
    xclose(first);
    int second = bind_once(port, &port);
    printf("SO_REUSEADDR allowed rebind on loopback port %u\n", port);
    xclose(second);
    return 0;
}
