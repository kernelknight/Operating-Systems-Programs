#include "common.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/ipc_epoll_echo.sock"

int main(void)
{
    int server = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server == -1) {
        die_errno("socket");
    }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);
    unlink(SOCKET_PATH);
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1 ||
        listen(server, 8) == -1) {
        die_errno("bind/listen");
    }

    for (int i = 0; i < 2; i++) {
        pid_t child = fork();
        if (child == 0) {
            int fd = socket(AF_UNIX, SOCK_STREAM, 0);
            connect(fd, (struct sockaddr *)&addr, sizeof(addr));
            char msg[32];
            int len = snprintf(msg, sizeof(msg), "client-%d\n", i + 1);
            write_all(fd, msg, (size_t)len);
            _exit(0);
        }
    }

    int ep = epoll_create1(0);
    struct epoll_event ev = {.events = EPOLLIN, .data.fd = server};
    epoll_ctl(ep, EPOLL_CTL_ADD, server, &ev);

    int replies = 0;
    while (replies < 2) {
        struct epoll_event events[4];
        int ready = epoll_wait(ep, events, 4, 2000);
        if (ready <= 0) {
            die("epoll_wait timeout");
        }
        for (int i = 0; i < ready; i++) {
            if (events[i].data.fd == server) {
                int client = accept4(server, NULL, NULL, SOCK_NONBLOCK);
                ev.events = EPOLLIN;
                ev.data.fd = client;
                epoll_ctl(ep, EPOLL_CTL_ADD, client, &ev);
            } else {
                char buf[64];
                ssize_t n = read(events[i].data.fd, buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    printf("epoll echo saw: %s", buf);
                    replies++;
                }
                xclose(events[i].data.fd);
            }
        }
    }

    reap_all_children();
    unlink(SOCKET_PATH);
    return 0;
}
