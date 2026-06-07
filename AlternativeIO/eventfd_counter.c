#include "common.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <unistd.h>

int main(void)
{
    int fd = eventfd(0, 0);
    if (fd == -1) {
        die_errno("eventfd");
    }

    pid_t child = fork();
    if (child == 0) {
        uint64_t value = 5;
        write_all(fd, &value, sizeof(value));
        _exit(0);
    }

    uint64_t value;
    if (read(fd, &value, sizeof(value)) != sizeof(value)) {
        die_errno("read eventfd");
    }
    printf("eventfd counter=%llu\n", (unsigned long long)value);
    wait_for_pid(child);
    return 0;
}
