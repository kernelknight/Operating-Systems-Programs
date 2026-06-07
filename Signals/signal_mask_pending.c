#include "common.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t delivered;

static void on_usr1(int signo)
{
    (void)signo;
    delivered = 1;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_usr1;
    sigaction(SIGUSR1, &sa, NULL);

    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &block, NULL) == -1) {
        die_errno("sigprocmask block");
    }

    raise(SIGUSR1);

    sigset_t pending;
    sigpending(&pending);
    printf("pending-before-unblock=%d delivered=%d\n",
           sigismember(&pending, SIGUSR1), delivered);

    if (sigprocmask(SIG_UNBLOCK, &block, NULL) == -1) {
        die_errno("sigprocmask unblock");
    }
    usleep(10000);
    printf("delivered-after-unblock=%d\n", delivered);
    return 0;
}
