#include "common.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/ipc_pshared_mutex"

struct shared_state {
    pthread_mutex_t mutex;
    int balance;
};

int main(void)
{
    shm_unlink(SHM_NAME);
    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("shm_open");
    }
    ftruncate(fd, (off_t)sizeof(struct shared_state));
    struct shared_state *state = mmap(NULL, sizeof(*state), PROT_READ | PROT_WRITE,
                                      MAP_SHARED, fd, 0);
    if (state == MAP_FAILED) {
        die_errno("mmap");
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&state->mutex, &attr);
    state->balance = 0;

    for (int i = 0; i < 4; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            for (int j = 0; j < 500; j++) {
                pthread_mutex_lock(&state->mutex);
                state->balance += 2;
                pthread_mutex_unlock(&state->mutex);
            }
            _exit(0);
        }
    }

    reap_all_children();
    printf("shared balance=%d expected=4000\n", state->balance);
    shm_unlink(SHM_NAME);
    return 0;
}
