#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>

#define FILE_PATH "/tmp/ipc_flock.dat"

int main(void)
{
    int fd = open(FILE_PATH, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("open");
    }
    if (flock(fd, LOCK_EX) == -1) {
        die_errno("flock parent");
    }

    pid_t child = fork();
    if (child == 0) {
        int fd2 = open(FILE_PATH, O_RDWR);
        if (flock(fd2, LOCK_EX | LOCK_NB) == -1) {
            printf("child could not acquire competing flock while parent holds it\n");
            fflush(stdout);
            _exit(0);
        }
        _exit(1);
    }

    wait_for_pid(child);
    flock(fd, LOCK_UN);
    unlink(FILE_PATH);
    return 0;
}
