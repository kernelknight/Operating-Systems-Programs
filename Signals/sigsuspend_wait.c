#include "common.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t ready;

static void on_usr1(int signo)
{
    (void)signo;
    ready = 1;
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
    sigprocmask(SIG_BLOCK, &block, NULL);

    if (fork() == 0) {
        usleep(50000);
        kill(getppid(), SIGUSR1);
        _exit(0);
    }

    sigset_t empty;
    sigemptyset(&empty);
    while (!ready) {
        sigsuspend(&empty);
    }

    printf("sigsuspend observed SIGUSR1 atomically\n");
    reap_all_children();
    return 0;
}
