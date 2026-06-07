#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FILE_PATH "/tmp/ipc_splice_source.txt"

int main(void)
{
    int source = open(FILE_PATH, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (source == -1) {
        die_errno("open source");
    }

    const char *data = "zero-copy splice moves bytes through a pipe buffer\n";
    write_all(source, data, strlen(data));
    lseek(source, 0, SEEK_SET);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        die_errno("pipe");
    }

    ssize_t moved = splice(source, NULL, pipefd[1], NULL, 4096, 0);
    if (moved == -1) {
        die_errno("splice file to pipe");
    }
    if (splice(pipefd[0], NULL, STDOUT_FILENO, NULL, (size_t)moved, 0) == -1) {
        die_errno("splice pipe to stdout");
    }

    xclose(pipefd[0]);
    xclose(pipefd[1]);
    xclose(source);
    unlink(FILE_PATH);
    return 0;
}
