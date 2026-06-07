# Alternative I/O

Programs covering readiness notification and event file descriptors.

| Program | Concepts |
| --- | --- |
| `select_timeout_pipe.c` | `select` with pipe readiness and timeout |
| `poll_many_pipes.c` | `poll` across several pipe descriptors |
| `epoll_edge_triggered_pipe.c` | edge-triggered `epoll` with nonblocking reads |
| `signalfd_sigusr1.c` | receiving signals as file-descriptor events |
| `eventfd_counter.c` | kernel counter events for process notification |
