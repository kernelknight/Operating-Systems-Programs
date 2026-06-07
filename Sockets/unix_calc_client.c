#include "common.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <socket-path> <expression>\n", argv[0]);
        return 2;
    }

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        die_errno("socket");
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", argv[1]);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        die_errno("connect");
    }
    write_all(fd, argv[2], strlen(argv[2]));
    shutdown(fd, SHUT_WR);

    char response[256];
    ssize_t n = read(fd, response, sizeof(response) - 1);
    if (n == -1) {
        die_errno("read");
    }
    response[n] = '\0';
    printf("%s", response);
    xclose(fd);
    return 0;
}
