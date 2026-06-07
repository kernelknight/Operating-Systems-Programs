#include "common.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int listen_loopback(struct sockaddr_in *addr)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        die_errno("socket");
    }
    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr->sin_port = 0;
    if (bind(fd, (struct sockaddr *)addr, sizeof(*addr)) == -1 ||
        listen(fd, 8) == -1) {
        die_errno("bind/listen");
    }
    socklen_t len = sizeof(*addr);
    getsockname(fd, (struct sockaddr *)addr, &len);
    return fd;
}

static void connect_client(struct sockaddr_in addr, const char *text)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        die_errno("connect");
    }
    write_all(fd, text, strlen(text));
    xclose(fd);
}

int main(void)
{
    struct sockaddr_in addr;
    int server = listen_loopback(&addr);

    for (int i = 0; i < 2; i++) {
        if (fork() == 0) {
            connect_client(addr, i == 0 ? "alpha\n" : "beta\n");
            _exit(0);
        }
    }

    for (int i = 0; i < 2; i++) {
        int client = accept(server, NULL, NULL);
        if (fork() == 0) {
            char buffer[64];
            ssize_t n = read(client, buffer, sizeof(buffer) - 1);
            buffer[n] = '\0';
            printf("tcp worker echo: %s", buffer);
            fflush(stdout);
            _exit(0);
        }
        xclose(client);
    }

    reap_all_children();
    return 0;
}
