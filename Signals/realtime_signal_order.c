#include "common.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t seen;
static volatile sig_atomic_t values[3];

static void on_rt(int signo, siginfo_t *info, void *context)
{
    (void)signo;
    (void)context;
    if (seen < 3) {
        values[seen++] = info->si_value.sival_int;
    }
}

int main(void)
{
    int signo = SIGRTMIN;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = on_rt;
    sa.sa_flags = SA_SIGINFO;
    sigaction(signo, &sa, NULL);

    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, signo);
    sigprocmask(SIG_BLOCK, &block, NULL);

    for (int i = 0; i < 3; i++) {
        union sigval value;
        value.sival_int = 100 + i;
        sigqueue(getpid(), signo, value);
    }

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    while (seen < 3) {
        pause();
    }

    printf("realtime values=%d,%d,%d\n", values[0], values[1], values[2]);
    return 0;
}
