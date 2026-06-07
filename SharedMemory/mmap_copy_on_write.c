#include "common.h"

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void)
{
    char *page = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
        die_errno("mmap");
    }
    strcpy(page, "parent-before-fork");

    pid_t child = fork();
    if (child == 0) {
        strcpy(page, "child-private-copy");
        printf("child sees: %s\n", page);
        fflush(stdout);
        _exit(0);
    }

    wait_for_pid(child);
    printf("parent still sees: %s\n", page);
    return 0;
}
