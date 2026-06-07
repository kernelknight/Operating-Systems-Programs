#include "common.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#define WORKERS 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static void change(int semid, unsigned short index, short delta)
{
    struct sembuf sb = {.sem_num = index, .sem_op = delta, .sem_flg = 0};
    if (semop(semid, &sb, 1) == -1) {
        die_errno("semop change");
    }
}

static void wait_zero(int semid, unsigned short index)
{
    struct sembuf sb = {.sem_num = index, .sem_op = 0, .sem_flg = 0};
    if (semop(semid, &sb, 1) == -1) {
        die_errno("semop wait zero");
    }
}

int main(void)
{
    int semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0600);
    if (semid == -1) {
        die_errno("semget");
    }
    unsigned short values[2] = {WORKERS, 0};
    union semun arg = {.array = values};
    semctl(semid, 0, SETALL, arg);

    for (int i = 0; i < WORKERS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("worker %d reached barrier\n", i + 1);
            fflush(stdout);
            change(semid, 0, -1);
            wait_zero(semid, 0);
            printf("worker %d released\n", i + 1);
            fflush(stdout);
            _exit(0);
        }
    }

    reap_all_children();
    semctl(semid, 0, IPC_RMID);
    return 0;
}
