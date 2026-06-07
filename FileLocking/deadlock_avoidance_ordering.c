#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FILE_PATH "/tmp/ipc_ordered_locks.dat"

static void lock_byte(int fd, off_t byte)
{
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = byte;
    fl.l_len = 1;
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        die_errno("lock byte");
    }
}

int main(void)
{
    int fd = open(FILE_PATH, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("open");
    }
    ftruncate(fd, 2);

    for (int i = 0; i < 2; i++) {
        if (fork() == 0) {
            lock_byte(fd, 0);
            lock_byte(fd, 1);
            printf("process %ld acquired locks in global order\n", (long)getpid());
            fflush(stdout);
            _exit(0);
        }
    }

    reap_all_children();
    unlink(FILE_PATH);
    return 0;
}
