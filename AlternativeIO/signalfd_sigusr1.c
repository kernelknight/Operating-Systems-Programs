#include "common.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/signalfd.h>
#include <unistd.h>

int main(void)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        die_errno("sigprocmask");
    }

    int fd = signalfd(-1, &mask, 0);
    if (fd == -1) {
        die_errno("signalfd");
    }

    raise(SIGUSR1);

    struct signalfd_siginfo info;
    ssize_t n = read(fd, &info, sizeof(info));
    if (n != sizeof(info)) {
        die_errno("read signalfd");
    }
    printf("signalfd received signo=%u pid=%u\n", info.ssi_signo, info.ssi_pid);
    return 0;
}
