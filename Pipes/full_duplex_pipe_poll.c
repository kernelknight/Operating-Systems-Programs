#include "common.h"

#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void child_loop(int rx, int tx)
{
    char buffer[128];
    for (;;) {
        ssize_t n = read(rx, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            break;
        }
        buffer[n] = '\0';
        char reply[160];
        int len = snprintf(reply, sizeof(reply), "child pid=%ld processed: %s",
                           (long)getpid(), buffer);
        write_all(tx, reply, (size_t)len);
    }
}

int main(void)
{
    int parent_to_child[2];
    int child_to_parent[2];
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        die_errno("pipe");
    }

    pid_t child = fork();
    if (child == -1) {
        die_errno("fork");
    }
    if (child == 0) {
        xclose(parent_to_child[1]);
        xclose(child_to_parent[0]);
        child_loop(parent_to_child[0], child_to_parent[1]);
        _exit(0);
    }

    xclose(parent_to_child[0]);
    xclose(child_to_parent[1]);

    const char *requests[] = {"read inode\n", "flush page cache\n", "sync journal\n"};
    char buffer[160];
    for (size_t i = 0; i < sizeof(requests) / sizeof(requests[0]); i++) {
        write_all(parent_to_child[1], requests[i], strlen(requests[i]));
        struct pollfd pfd = {.fd = child_to_parent[0], .events = POLLIN};
        if (poll(&pfd, 1, 1000) <= 0) {
            die("poll timed out waiting for child");
        }
        ssize_t n = read(child_to_parent[0], buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            die_errno("read reply");
        }
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    xclose(parent_to_child[1]);
    xclose(child_to_parent[0]);
    wait_for_pid(child);
    return 0;
}
