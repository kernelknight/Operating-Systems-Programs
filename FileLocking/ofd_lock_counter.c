#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef F_OFD_SETLKW
#define F_OFD_SETLKW 38
#endif

#define FILE_PATH "/tmp/ipc_ofd_lock_counter.dat"

static void ofd_lock(int fd, short type)
{
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    if (fcntl(fd, F_OFD_SETLKW, &fl) == -1) {
        die_errno("F_OFD_SETLKW");
    }
}

int main(void)
{
    int fd = open(FILE_PATH, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("open");
    }
    ftruncate(fd, (off_t)sizeof(int));
    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED) {
        die_errno("mmap");
    }
    *counter = 0;

    for (int i = 0; i < 3; i++) {
        if (fork() == 0) {
            for (int j = 0; j < 500; j++) {
                ofd_lock(fd, F_WRLCK);
                (*counter)++;
                ofd_lock(fd, F_UNLCK);
            }
            _exit(0);
        }
    }

    reap_all_children();
    printf("ofd-locked counter=%d expected=1500\n", *counter);
    unlink(FILE_PATH);
    return 0;
}
