#include "common.h"

#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MQ_NAME "/ipc_notify_queue"

static volatile sig_atomic_t notified;

static void on_notify(int signo)
{
    (void)signo;
    notified = 1;
}

int main(void)
{
    mq_unlink(MQ_NAME);
    struct mq_attr attr = {.mq_maxmsg = 8, .mq_msgsize = 64};
    mqd_t mq = mq_open(MQ_NAME, O_CREAT | O_RDWR | O_NONBLOCK, 0600, &attr);
    if (mq == (mqd_t)-1) {
        die_errno("mq_open");
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_notify;
    sigaction(SIGUSR1, &sa, NULL);

    struct sigevent sev;
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;
    if (mq_notify(mq, &sev) == -1) {
        die_errno("mq_notify");
    }

    mq_send(mq, "notify payload", 14, 7);
    while (!notified) {
        pause();
    }

    char buffer[64];
    unsigned priority;
    ssize_t n = mq_receive(mq, buffer, sizeof(buffer), &priority);
    buffer[n] = '\0';
    printf("received priority=%u text=%s\n", priority, buffer);

    mq_close(mq);
    mq_unlink(MQ_NAME);
    return 0;
}
