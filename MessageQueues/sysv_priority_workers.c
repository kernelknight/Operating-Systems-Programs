#include "common.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define WORKERS 3
#define JOBS 9

struct job_msg {
    long type;
    int job_id;
    int value;
};

struct result_msg {
    long type;
    int job_id;
    int value;
    int result;
    long pid;
};

static void worker(int qid, int type)
{
    for (;;) {
        struct job_msg job;
        if (msgrcv(qid, &job, sizeof(job) - sizeof(long), type, 0) == -1) {
            die_errno("msgrcv");
        }
        if (job.job_id < 0) {
            return;
        }
        struct result_msg result = {
            .type = 100,
            .job_id = job.job_id,
            .value = job.value,
            .result = job.value * job.value,
            .pid = (long)getpid(),
        };
        if (msgsnd(qid, &result, sizeof(result) - sizeof(long), 0) == -1) {
            die_errno("msgsnd result");
        }
    }
}

int main(void)
{
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid == -1) {
        die_errno("msgget");
    }

    for (int i = 1; i <= WORKERS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            worker(qid, i);
            _exit(0);
        }
    }

    for (int i = 0; i < JOBS; i++) {
        struct job_msg job = {.type = (i % WORKERS) + 1, .job_id = i + 1, .value = i + 2};
        msgsnd(qid, &job, sizeof(job) - sizeof(long), 0);
    }

    for (int i = 0; i < JOBS; i++) {
        struct result_msg result;
        msgrcv(qid, &result, sizeof(result) - sizeof(long), 100, 0);
        printf("job=%d value=%d square=%d worker=%ld\n",
               result.job_id, result.value, result.result, result.pid);
    }

    for (int i = 1; i <= WORKERS; i++) {
        struct job_msg stop = {.type = i, .job_id = -1};
        msgsnd(qid, &stop, sizeof(stop) - sizeof(long), 0);
    }
    reap_all_children();
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
