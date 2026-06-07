#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOCK_PATH "/tmp/ipc_single_instance.lock"

static int try_lock(int fd)
{
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    return fcntl(fd, F_SETLK, &fl);
}

int main(void)
{
    int fd = open(LOCK_PATH, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("open");
    }
    if (try_lock(fd) == -1) {
        die_errno("initial lock");
    }
    dprintf(fd, "%ld\n", (long)getpid());

    pid_t child = fork();
    if (child == 0) {
        int competing = open(LOCK_PATH, O_RDWR);
        if (try_lock(competing) == -1) {
            printf("second instance rejected by advisory lock\n");
            fflush(stdout);
            _exit(0);
        }
        _exit(1);
    }

    wait_for_pid(child);
    unlink(LOCK_PATH);
    return 0;
}
