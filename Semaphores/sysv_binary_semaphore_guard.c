#include "common.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static void sem_op(int semid, short op)
{
    struct sembuf sb = {.sem_num = 0, .sem_op = op, .sem_flg = SEM_UNDO};
    if (semop(semid, &sb, 1) == -1) {
        die_errno("semop");
    }
}

int main(void)
{
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid == -1) {
        die_errno("semget");
    }
    union semun arg = {.val = 1};
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        die_errno("semctl SETVAL");
    }

    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    if (shmid == -1) {
        die_errno("shmget");
    }
    int *counter = shmat(shmid, NULL, 0);
    if (counter == (void *)-1) {
        die_errno("shmat");
    }
    *counter = 0;

    for (int i = 0; i < 4; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            for (int j = 0; j < 1000; j++) {
                sem_op(semid, -1);
                (*counter)++;
                sem_op(semid, 1);
            }
            _exit(0);
        }
    }

    reap_all_children();
    printf("system-v guarded counter=%d expected=4000\n", *counter);
    shmdt(counter);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}
