#include "common.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/ipc_ring_sem"
#define CAPACITY 8
#define ITEMS 20

struct ring {
    sem_t empty;
    sem_t full;
    sem_t mutex;
    int head;
    int tail;
    int slots[CAPACITY];
};

static void sem_wait_retry(sem_t *sem)
{
    while (sem_wait(sem) == -1) {
        if (errno != EINTR) {
            die_errno("sem_wait");
        }
    }
}

int main(void)
{
    shm_unlink(SHM_NAME);
    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1) {
        die_errno("shm_open");
    }
    ftruncate(fd, (off_t)sizeof(struct ring));

    struct ring *ring = mmap(NULL, sizeof(*ring), PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
    if (ring == MAP_FAILED) {
        die_errno("mmap");
    }
    memset(ring, 0, sizeof(*ring));
    sem_init(&ring->empty, 1, CAPACITY);
    sem_init(&ring->full, 1, 0);
    sem_init(&ring->mutex, 1, 1);

    pid_t child = fork();
    if (child == 0) {
        int total = 0;
        for (int i = 0; i < ITEMS; i++) {
            sem_wait_retry(&ring->full);
            sem_wait_retry(&ring->mutex);
            int value = ring->slots[ring->head];
            ring->head = (ring->head + 1) % CAPACITY;
            sem_post(&ring->mutex);
            sem_post(&ring->empty);
            total += value;
        }
        printf("consumer total=%d\n", total);
        fflush(stdout);
        _exit(0);
    }

    for (int value = 1; value <= ITEMS; value++) {
        sem_wait_retry(&ring->empty);
        sem_wait_retry(&ring->mutex);
        ring->slots[ring->tail] = value;
        ring->tail = (ring->tail + 1) % CAPACITY;
        sem_post(&ring->mutex);
        sem_post(&ring->full);
    }

    wait_for_pid(child);
    shm_unlink(SHM_NAME);
    return 0;
}
