#include "common.h"

#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define RESTART_LIMIT 2

static int signal_pipe[2];

static void on_sigchld(int signo)
{
    (void)signo;
    unsigned char byte = 1;
    (void)write(signal_pipe[1], &byte, 1);
}

static pid_t start_worker(int attempt)
{
    pid_t pid = fork();
    if (pid == -1) {
        die_errno("fork");
    }
    if (pid == 0) {
        printf("worker attempt=%d pid=%ld\n", attempt, (long)getpid());
        fflush(stdout);
        _exit(30 + attempt);
    }
    return pid;
}

int main(void)
{
    if (pipe(signal_pipe) == -1) {
        die_errno("pipe");
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    int attempt = 0;
    pid_t worker = start_worker(attempt);

    while (attempt < RESTART_LIMIT) {
        struct pollfd pfd = {.fd = signal_pipe[0], .events = POLLIN};
        if (poll(&pfd, 1, 2000) <= 0) {
            die("poll timed out waiting for SIGCHLD");
        }
        unsigned char drain[8];
        read(signal_pipe[0], drain, sizeof(drain));

        int status;
        pid_t pid = waitpid(worker, &status, WNOHANG);
        if (pid == worker) {
            printf("supervisor reaped pid=%ld status=%d\n",
                   (long)pid, WEXITSTATUS(status));
            attempt++;
            if (attempt < RESTART_LIMIT) {
                worker = start_worker(attempt);
            }
        }
    }

    return 0;
}
