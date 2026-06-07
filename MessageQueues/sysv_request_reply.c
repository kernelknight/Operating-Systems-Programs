#include "common.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct request {
    long type;
    long reply_type;
    int a;
    int b;
};

struct reply {
    long type;
    int sum;
    int product;
};

int main(void)
{
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid == -1) {
        die_errno("msgget");
    }

    pid_t server = fork();
    if (server == 0) {
        struct request req;
        msgrcv(qid, &req, sizeof(req) - sizeof(long), 1, 0);
        struct reply rep = {
            .type = req.reply_type,
            .sum = req.a + req.b,
            .product = req.a * req.b,
        };
        msgsnd(qid, &rep, sizeof(rep) - sizeof(long), 0);
        _exit(0);
    }

    long reply_type = (long)getpid();
    struct request req = {.type = 1, .reply_type = reply_type, .a = 21, .b = 34};
    msgsnd(qid, &req, sizeof(req) - sizeof(long), 0);

    struct reply rep;
    msgrcv(qid, &rep, sizeof(rep) - sizeof(long), reply_type, 0);
    printf("sum=%d product=%d reply-type=%ld\n", rep.sum, rep.product, reply_type);
    wait_for_pid(server);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
