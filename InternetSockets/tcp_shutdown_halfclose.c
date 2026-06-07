#include "common.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        die_errno("socket");
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1 ||
        listen(server, 1) == -1) {
        die_errno("bind/listen");
    }
    socklen_t len = sizeof(addr);
    getsockname(server, (struct sockaddr *)&addr, &len);

    pid_t child = fork();
    if (child == 0) {
        int client = socket(AF_INET, SOCK_STREAM, 0);
        connect(client, (struct sockaddr *)&addr, sizeof(addr));
        write_all(client, "request body", 12);
        shutdown(client, SHUT_WR);
        char response[64];
        ssize_t n = read(client, response, sizeof(response) - 1);
        response[n] = '\0';
        printf("client read after half-close: %s\n", response);
        fflush(stdout);
        _exit(0);
    }

    int accepted = accept(server, NULL, NULL);
    char buffer[64];
    while (read(accepted, buffer, sizeof(buffer)) > 0) {
    }
    write_all(accepted, "response after EOF", 18);
    wait_for_pid(child);
    return 0;
}
