#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEMP_FILE "/tmp/ipc_fd_passing.txt"

static void send_fd(int sock, int fd)
{
    char control[CMSG_SPACE(sizeof(int))];
    memset(control, 0, sizeof(control));
    struct iovec io = {.iov_base = "F", .iov_len = 1};
    struct msghdr msg = {.msg_iov = &io, .msg_iovlen = 1,
                         .msg_control = control, .msg_controllen = sizeof(control)};
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    memcpy(CMSG_DATA(cmsg), &fd, sizeof(int));
    msg.msg_controllen = cmsg->cmsg_len;
    if (sendmsg(sock, &msg, 0) == -1) {
        die_errno("sendmsg");
    }
}

static int recv_fd(int sock)
{
    char byte;
    char control[CMSG_SPACE(sizeof(int))];
    struct iovec io = {.iov_base = &byte, .iov_len = 1};
    struct msghdr msg = {.msg_iov = &io, .msg_iovlen = 1,
                         .msg_control = control, .msg_controllen = sizeof(control)};
    if (recvmsg(sock, &msg, 0) == -1) {
        die_errno("recvmsg");
    }
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    int fd = -1;
    memcpy(&fd, CMSG_DATA(cmsg), sizeof(int));
    return fd;
}

int main(void)
{
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        die_errno("socketpair");
    }

    int fd = open(TEMP_FILE, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("open");
    }
    write_all(fd, "descriptor passed over unix socket\n", 35);
    lseek(fd, 0, SEEK_SET);

    pid_t child = fork();
    if (child == -1) {
        die_errno("fork");
    }
    if (child == 0) {
        xclose(sv[0]);
        int received = recv_fd(sv[1]);
        char buf[128];
        ssize_t n = read(received, buf, sizeof(buf) - 1);
        buf[n] = '\0';
        printf("child read from received fd: %s", buf);
        fflush(stdout);
        _exit(0);
    }

    xclose(sv[1]);
    send_fd(sv[0], fd);
    wait_for_pid(child);
    xclose(fd);
    xclose(sv[0]);
    unlink(TEMP_FILE);
    return 0;
}
