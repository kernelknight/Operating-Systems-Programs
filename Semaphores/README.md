# Semaphores

Programs covering System V and POSIX semaphore synchronization.

| Program | Concepts |
| --- | --- |
| `sysv_binary_semaphore_guard.c` | System V binary semaphore protecting a shared counter |
| `sysv_semaphore_set_barrier.c` | semaphore sets and process rendezvous |
| `posix_named_sem_pingpong.c` | two named semaphores implementing turn-taking |
| `posix_sem_timedwait_timeout.c` | timeout-aware waits with `sem_timedwait` |
| `posix_shm_unnamed_sem_latch.c` | unnamed process-shared semaphore inside shared memory |
