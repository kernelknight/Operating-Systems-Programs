#include "common.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t got_payload;
static volatile sig_atomic_t payload_value;

static void on_usr1(int signo, siginfo_t *info, void *context)
{
    (void)signo;
    (void)context;
    payload_value = info->si_value.sival_int;
    got_payload = 1;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = on_usr1;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        die_errno("sigaction");
    }

    union sigval value;
    value.sival_int = 4096;
    if (sigqueue(getpid(), SIGUSR1, value) == -1) {
        die_errno("sigqueue");
    }

    while (!got_payload) {
        pause();
    }
    printf("sigqueue payload=%d\n", payload_value);
    return 0;
}
