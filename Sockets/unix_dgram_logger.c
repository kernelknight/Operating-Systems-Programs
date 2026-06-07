#include "common.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/ipc_dgram_logger.sock"
#define CLIENT_PATH "/tmp/ipc_dgram_client.sock"

static void bind_unix(int fd, const char *path)
{
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", path);
    unlink(path);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        die_errno("bind");
    }
}

int main(void)
{
    int server = socket(AF_UNIX, SOCK_DGRAM, 0);
    int client = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server == -1 || client == -1) {
        die_errno("socket");
    }
    bind_unix(server, SERVER_PATH);
    bind_unix(client, CLIENT_PATH);

    pid_t child = fork();
    if (child == -1) {
        die_errno("fork");
    }
    if (child == 0) {
        char buffer[128];
        for (int i = 0; i < 3; i++) {
            ssize_t n = recv(server, buffer, sizeof(buffer) - 1, 0);
            buffer[n] = '\0';
            printf("logger received: %s", buffer);
        }
        fflush(stdout);
        _exit(0);
    }

    struct sockaddr_un to;
    memset(&to, 0, sizeof(to));
    to.sun_family = AF_UNIX;
    snprintf(to.sun_path, sizeof(to.sun_path), "%s", SERVER_PATH);
    for (int i = 0; i < 3; i++) {
        char line[64];
        int len = snprintf(line, sizeof(line), "event=%d source=client\n", i + 1);
        sendto(client, line, (size_t)len, 0, (struct sockaddr *)&to, sizeof(to));
    }

    wait_for_pid(child);
    unlink(SERVER_PATH);
    unlink(CLIENT_PATH);
    return 0;
}
