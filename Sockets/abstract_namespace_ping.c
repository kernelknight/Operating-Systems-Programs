#include "common.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static socklen_t abstract_addr(struct sockaddr_un *addr, const char *name)
{
    memset(addr, 0, sizeof(*addr));
    addr->sun_family = AF_UNIX;
    addr->sun_path[0] = '\0';
    snprintf(addr->sun_path + 1, sizeof(addr->sun_path) - 1, "%s", name);
    return (socklen_t)(offsetof(struct sockaddr_un, sun_path) + 1 + strlen(name));
}

int main(void)
{
    int server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server == -1) {
        die_errno("socket");
    }
    struct sockaddr_un addr;
    socklen_t len = abstract_addr(&addr, "ipc.abstract.ping");
    if (bind(server, (struct sockaddr *)&addr, len) == -1 || listen(server, 4) == -1) {
        die_errno("bind/listen");
    }

    pid_t child = fork();
    if (child == -1) {
        die_errno("fork");
    }
    if (child == 0) {
        int client = socket(AF_UNIX, SOCK_STREAM, 0);
        if (connect(client, (struct sockaddr *)&addr, len) == -1) {
            die_errno("connect");
        }
        write_all(client, "ping abstract namespace\n", 24);
        _exit(0);
    }

    int accepted = accept(server, NULL, NULL);
    char buffer[64];
    ssize_t n = read(accepted, buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    printf("server received: %s", buffer);
    wait_for_pid(child);
    return 0;
}
