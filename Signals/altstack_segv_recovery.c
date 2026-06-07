#include "common.h"

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sigjmp_buf recovery;

static void on_segv(int signo)
{
    (void)signo;
    siglongjmp(recovery, 1);
}

int main(void)
{
    stack_t altstack;
    memset(&altstack, 0, sizeof(altstack));
    altstack.ss_size = SIGSTKSZ;
    altstack.ss_sp = malloc(altstack.ss_size);
    if (altstack.ss_sp == NULL) {
        die("malloc altstack");
    }
    if (sigaltstack(&altstack, NULL) == -1) {
        die_errno("sigaltstack");
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_segv;
    sa.sa_flags = SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        die_errno("sigaction");
    }

    if (sigsetjmp(recovery, 1) == 0) {
        raise(SIGSEGV);
    } else {
        printf("SIGSEGV handled on alternate signal stack\n");
    }

    free(altstack.ss_sp);
    return 0;
}
