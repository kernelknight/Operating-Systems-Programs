#include "common.h"

#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void)
{
    sem_t sem;
    if (sem_init(&sem, 0, 0) == -1) {
        die_errno("sem_init");
    }

    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_nsec += 100000000;
    if (deadline.tv_nsec >= 1000000000) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000;
    }

    if (sem_timedwait(&sem, &deadline) == -1 && errno == ETIMEDOUT) {
        printf("sem_timedwait expired as expected\n");
    } else {
        die("sem_timedwait did not time out");
    }

    sem_destroy(&sem);
    return 0;
}
