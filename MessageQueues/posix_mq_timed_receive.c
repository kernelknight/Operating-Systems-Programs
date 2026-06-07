#include "common.h"

#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MQ_NAME "/ipc_timed_queue"

int main(void)
{
    mq_unlink(MQ_NAME);
    struct mq_attr attr = {.mq_maxmsg = 4, .mq_msgsize = 64};
    mqd_t mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0600, &attr);
    if (mq == (mqd_t)-1) {
        die_errno("mq_open");
    }

    mq_send(mq, "deadline aware message", 22, 3);

    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_sec += 1;

    char buffer[64];
    unsigned priority;
    ssize_t n = mq_timedreceive(mq, buffer, sizeof(buffer), &priority, &deadline);
    if (n == -1) {
        die_errno("mq_timedreceive");
    }
    buffer[n] = '\0';
    printf("timed receive priority=%u text=%s\n", priority, buffer);

    mq_close(mq);
    mq_unlink(MQ_NAME);
    return 0;
}
