#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        die_errno("pipe");
    }

    pid_t child = fork();
    if (child == -1) {
        die_errno("fork");
    }
    if (child == 0) {
        xclose(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            die_errno("dup2");
        }
        xclose(pipefd[0]);
        execlp("tr", "tr", "a-z", "A-Z", NULL);
        die_errno("execlp tr");
    }

    xclose(pipefd[0]);
    const char *payload = "kernel pipes feed child process standard input\n";
    if (write_all(pipefd[1], payload, strlen(payload)) == -1) {
        die_errno("write pipe");
    }
    xclose(pipefd[1]);
    wait_for_pid(child);
    return 0;
}
