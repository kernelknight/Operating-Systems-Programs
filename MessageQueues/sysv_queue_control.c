#include "common.h"

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg {
    long type;
    char text[32];
};

int main(void)
{
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid == -1) {
        die_errno("msgget");
    }

    for (int i = 0; i < 4; i++) {
        struct msg m = {.type = 10};
        snprintf(m.text, sizeof(m.text), "queued-%d", i + 1);
        msgsnd(qid, &m, sizeof(m) - sizeof(long), 0);
    }

    struct msqid_ds ds;
    if (msgctl(qid, IPC_STAT, &ds) == -1) {
        die_errno("msgctl IPC_STAT");
    }
    printf("queue-id=%d messages=%lu max-bytes=%lu\n",
           qid, (unsigned long)ds.msg_qnum, (unsigned long)ds.msg_qbytes);

    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
