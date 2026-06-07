#include "common.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t handled;

static void on_usr1(int signo)
{
    (void)signo;
    handled = 1;
}

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        die_errno("pipe");
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_usr1;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        die_errno("sigaction");
    }

    pid_t child = fork();
    if (child == 0) {
        usleep(50000);
        kill(getppid(), SIGUSR1);
        usleep(50000);
        write_all(pipefd[1], "read resumed after signal\n", 26);
        _exit(0);
    }

    char buffer[64];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
    if (n == -1) {
        die_errno("read");
    }
    buffer[n] = '\0';
    printf("handled=%d %s", handled, buffer);
    wait_for_pid(child);
    return 0;
}
