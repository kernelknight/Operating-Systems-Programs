#include "common.h"

#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PIPES 3

int main(void)
{
    int pipefd[PIPES][2];
    struct pollfd pfds[PIPES];

    for (int i = 0; i < PIPES; i++) {
        if (pipe(pipefd[i]) == -1) {
            die_errno("pipe");
        }
        char msg[32];
        int len = snprintf(msg, sizeof(msg), "pipe-%d\n", i + 1);
        write_all(pipefd[i][1], msg, (size_t)len);
        pfds[i].fd = pipefd[i][0];
        pfds[i].events = POLLIN;
    }

    if (poll(pfds, PIPES, 1000) <= 0) {
        die("poll did not report readiness");
    }

    for (int i = 0; i < PIPES; i++) {
        if (pfds[i].revents & POLLIN) {
            char buffer[32];
            ssize_t n = read(pfds[i].fd, buffer, sizeof(buffer) - 1);
            buffer[n] = '\0';
            printf("poll saw %s", buffer);
        }
    }
    return 0;
}
