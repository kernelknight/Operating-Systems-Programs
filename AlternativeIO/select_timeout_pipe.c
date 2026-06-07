#include "common.h"

#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        die_errno("pipe");
    }
    write_all(pipefd[1], "ready via select\n", 17);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(pipefd[0], &readfds);
    struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};

    int ready = select(pipefd[0] + 1, &readfds, NULL, NULL, &timeout);
    if (ready == -1) {
        die_errno("select");
    }
    if (ready == 0) {
        die("select timed out");
    }

    char buffer[64];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    printf("%s", buffer);
    return 0;
}
