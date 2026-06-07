#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define FILE_PATH "/tmp/ipc_mmap_counter.dat"
#define CHILDREN 4
#define ITERATIONS 1000

static void lock_region(int fd, short type)
{
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        die_errno("fcntl");
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

    for (int i = 0; i < CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            for (int j = 0; j < ITERATIONS; j++) {
                lock_region(fd, F_WRLCK);
                (*counter)++;
                lock_region(fd, F_UNLCK);
            }
            _exit(0);
        }
    }

    reap_all_children();
    printf("counter=%d expected=%d\n", *counter, CHILDREN * ITERATIONS);
    unlink(FILE_PATH);
    return 0;
}
