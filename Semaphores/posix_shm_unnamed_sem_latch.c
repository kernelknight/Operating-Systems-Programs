#include "common.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/ipc_sem_latch"

struct shared_latch {
    sem_t ready;
    char message[64];
};

int main(void)
{
    shm_unlink(SHM_NAME);
    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("shm_open");
    }
    ftruncate(fd, (off_t)sizeof(struct shared_latch));

    struct shared_latch *shared = mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE,
                                       MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        die_errno("mmap");
    }
    sem_init(&shared->ready, 1, 0);

    pid_t child = fork();
    if (child == 0) {
        sem_wait(&shared->ready);
        printf("child observed: %s\n", shared->message);
        fflush(stdout);
        _exit(0);
    }

    snprintf(shared->message, sizeof(shared->message), "shared-memory latch released");
    sem_post(&shared->ready);
    wait_for_pid(child);
    shm_unlink(SHM_NAME);
    return 0;
}
