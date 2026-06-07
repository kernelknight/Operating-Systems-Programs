# File Locking

Programs covering Linux advisory locking patterns.

| Program | Concepts |
| --- | --- |
| `fcntl_byte_range_locks.c` | independent locks on different file byte ranges |
| `lockfile_single_instance.c` | daemon-style single-instance lock file |
| `flock_parent_child.c` | BSD `flock` inheritance and exclusive locking |
| `ofd_lock_counter.c` | Linux open-file-description locks |
| `deadlock_avoidance_ordering.c` | ordered lock acquisition to avoid deadlock |
