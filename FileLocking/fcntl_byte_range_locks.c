#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FILE_PATH "/tmp/ipc_range_lock.dat"

static void lock_range(int fd, off_t start, off_t len)
{
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = len;
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        die_errno("fcntl lock");
    }
}

int main(void)
{
    int fd = open(FILE_PATH, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("open");
    }
    ftruncate(fd, 128);

    pid_t left = fork();
    if (left == 0) {
        lock_range(fd, 0, 32);
        pwrite(fd, "left-range\n", 11, 0);
        printf("child locked bytes 0..31\n");
        fflush(stdout);
        _exit(0);
    }

    pid_t right = fork();
    if (right == 0) {
        lock_range(fd, 64, 32);
        pwrite(fd, "right-range\n", 12, 64);
        printf("child locked bytes 64..95\n");
        fflush(stdout);
        _exit(0);
    }

    wait_for_pid(left);
    wait_for_pid(right);
    unlink(FILE_PATH);
    return 0;
}
