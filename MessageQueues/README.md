# Message Queues

Five queue-based IPC programs:

| Program | Concepts |
| --- | --- |
| `sysv_priority_workers.c` | System V typed messages and worker fan-out |
| `sysv_request_reply.c` | request/reply using process-specific message types |
| `sysv_queue_control.c` | `msgctl`, queue metadata, depth inspection |
| `posix_mq_notify_signal.c` | POSIX mqueue notification with `SIGEV_SIGNAL` |
| `posix_mq_timed_receive.c` | bounded queue receive with absolute timeout |
