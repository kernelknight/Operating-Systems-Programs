#include "common.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        die_errno("pipe");
    }
    if (set_nonblock(pipefd[0]) == -1) {
        die_errno("set_nonblock");
    }

    int ep = epoll_create1(0);
    if (ep == -1) {
        die_errno("epoll_create1");
    }
    struct epoll_event ev = {.events = EPOLLIN | EPOLLET, .data.fd = pipefd[0]};
    epoll_ctl(ep, EPOLL_CTL_ADD, pipefd[0], &ev);

    write_all(pipefd[1], "edge-triggered epoll\n", 21);

    struct epoll_event out;
    if (epoll_wait(ep, &out, 1, 1000) != 1) {
        die("epoll_wait failed");
    }

    char buffer[64];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    printf("%s", buffer);
    return 0;
}
