# Shared Memory

Five shared-memory and memory-mapping programs:

| Program | Concepts |
| --- | --- |
| `posix_shm_ring_sem.c` | `shm_open`, `mmap`, process-shared semaphores |
| `mmap_file_lock_counter.c` | file-backed mapping with `fcntl` write locks |
| `process_shared_mutex.c` | `pthread_mutexattr_setpshared` in shared memory |
| `memfd_sealed_config.c` | Linux `memfd_create` and file seals |
| `mmap_copy_on_write.c` | `MAP_PRIVATE` copy-on-write behavior across fork |
