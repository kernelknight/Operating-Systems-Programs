#include "common.h"

#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int signal_pipe[2];

static void signal_handler(int signo)
{
    unsigned char byte = (unsigned char)signo;
    (void)write(signal_pipe[1], &byte, 1);
}

int main(void)
{
    if (pipe(signal_pipe) == -1) {
        die_errno("pipe");
    }

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_handler = signal_handler;
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &action, NULL) == -1 ||
        sigaction(SIGALRM, &action, NULL) == -1) {
        die_errno("sigaction");
    }

    raise(SIGUSR1);
    alarm(1);

    int events = 0;
    while (events < 2) {
        struct pollfd pfd = {.fd = signal_pipe[0], .events = POLLIN};
        if (poll(&pfd, 1, 1500) <= 0) {
            die("poll timed out");
        }
        unsigned char signo;
        if (read(signal_pipe[0], &signo, 1) == 1) {
            printf("main loop handled signal %u outside handler\n", signo);
            events++;
        }
    }

    xclose(signal_pipe[0]);
    xclose(signal_pipe[1]);
    return 0;
}
