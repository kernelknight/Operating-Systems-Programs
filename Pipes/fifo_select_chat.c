#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_A "/tmp/ipc_fifo_a.fifo"
#define FIFO_B "/tmp/ipc_fifo_b.fifo"

static void mkfifo_clean(const char *path)
{
    unlink(path);
    if (mkfifo(path, 0600) == -1) {
        die_errno("mkfifo");
    }
}

static void peer(const char *name, const char *rx_path, const char *tx_path)
{
    int rx = open(rx_path, O_RDONLY | O_NONBLOCK);
    int tx = open(tx_path, O_WRONLY);
    if (rx == -1 || tx == -1) {
        die_errno("open fifo");
    }

    for (int i = 0; i < 3; i++) {
        char out[96];
        int len = snprintf(out, sizeof(out), "%s message %d\n", name, i + 1);
        write_all(tx, out, (size_t)len);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(rx, &rfds);
        if (select(rx + 1, &rfds, NULL, NULL, NULL) == -1) {
            die_errno("select");
        }

        char in[96];
        ssize_t n = read(rx, in, sizeof(in) - 1);
        if (n > 0) {
            in[n] = '\0';
            printf("%s received: %s", name, in);
        }
    }
}

int main(void)
{
    mkfifo_clean(FIFO_A);
    mkfifo_clean(FIFO_B);

    pid_t child = fork();
    if (child == -1) {
        die_errno("fork");
    }
    if (child == 0) {
        peer("peer-B", FIFO_A, FIFO_B);
        fflush(stdout);
        _exit(0);
    }

    peer("peer-A", FIFO_B, FIFO_A);
    wait_for_pid(child);
    unlink(FIFO_A);
    unlink(FIFO_B);
    return 0;
}
