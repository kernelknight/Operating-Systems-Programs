#include "common.h"

#include <fcntl.h>
#include <linux/memfd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

static int create_memfd(const char *name)
{
    int fd = (int)syscall(SYS_memfd_create, name, MFD_ALLOW_SEALING);
    if (fd == -1) {
        die_errno("memfd_create");
    }
    return fd;
}

int main(void)
{
    int fd = create_memfd("ipc_config");
    const char *config = "workers=4\nmode=readonly-after-seal\n";
    write_all(fd, config, strlen(config));

    if (fcntl(fd, F_ADD_SEALS, F_SEAL_WRITE | F_SEAL_GROW | F_SEAL_SHRINK) == -1) {
        die_errno("F_ADD_SEALS");
    }

    pid_t child = fork();
    if (child == 0) {
        lseek(fd, 0, SEEK_SET);
        char buffer[128];
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        buffer[n] = '\0';
        printf("child mapped sealed config:\n%s", buffer);
        fflush(stdout);
        _exit(0);
    }

    wait_for_pid(child);
    printf("seals=0x%x\n", fcntl(fd, F_GET_SEALS));
    return 0;
}
