#include "common.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void die(const char *message)
{
    fprintf(stderr, "error: %s\n", message);
    exit(EXIT_FAILURE);
}

void die_errno(const char *message)
{
    fprintf(stderr, "error: %s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void xclose(int fd)
{
    if (fd >= 0 && close(fd) == -1) {
        die_errno("close");
    }
}

void wait_for_pid(pid_t pid)
{
    int status;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            die_errno("waitpid");
        }
    }
}

void reap_all_children(void)
{
    while (wait(NULL) > 0) {
    }
}

ssize_t write_all(int fd, const void *buffer, size_t length)
{
    const char *cursor = buffer;
    size_t remaining = length;
    while (remaining > 0) {
        ssize_t n = write(fd, cursor, remaining);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        cursor += n;
        remaining -= (size_t)n;
    }
    return (ssize_t)length;
}

int set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
