#include "common.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define SEM_PING "/ipc_sem_ping"
#define SEM_PONG "/ipc_sem_pong"

int main(void)
{
    sem_unlink(SEM_PING);
    sem_unlink(SEM_PONG);
    sem_t *ping = sem_open(SEM_PING, O_CREAT | O_EXCL, 0600, 1);
    sem_t *pong = sem_open(SEM_PONG, O_CREAT | O_EXCL, 0600, 0);
    if (ping == SEM_FAILED || pong == SEM_FAILED) {
        die_errno("sem_open");
    }

    pid_t child = fork();
    if (child == 0) {
        for (int i = 0; i < 3; i++) {
            sem_wait(pong);
            printf("pong %d\n", i + 1);
            fflush(stdout);
            sem_post(ping);
        }
        _exit(0);
    }

    for (int i = 0; i < 3; i++) {
        sem_wait(ping);
        printf("ping %d\n", i + 1);
        sem_post(pong);
    }

    wait_for_pid(child);
    sem_close(ping);
    sem_close(pong);
    sem_unlink(SEM_PING);
    sem_unlink(SEM_PONG);
    return 0;
}
