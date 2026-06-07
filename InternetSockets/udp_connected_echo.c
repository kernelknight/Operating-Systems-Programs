#include "common.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    int server = socket(AF_INET, SOCK_DGRAM, 0);
    int client = socket(AF_INET, SOCK_DGRAM, 0);
    if (server == -1 || client == -1) {
        die_errno("socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = 0;
    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        die_errno("bind server");
    }
    socklen_t len = sizeof(server_addr);
    getsockname(server, (struct sockaddr *)&server_addr, &len);

    if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        die_errno("connect udp");
    }
    write_all(client, "udp payload", 11);

    char buffer[64];
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    ssize_t n = recvfrom(server, buffer, sizeof(buffer), 0,
                         (struct sockaddr *)&from, &from_len);
    sendto(server, buffer, (size_t)n, 0, (struct sockaddr *)&from, from_len);

    n = read(client, buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    printf("connected udp echo: %s\n", buffer);
    return 0;
}
