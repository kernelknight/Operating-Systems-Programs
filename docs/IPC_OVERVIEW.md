# Linux IPC Overview

Linux IPC is not one API. It is a toolbox of kernel mechanisms with different
tradeoffs around lifetime, ownership, buffering, wakeups, naming, and security.

## Mechanisms Covered

| Mechanism | Kernel object | Good fit | Important concern |
| --- | --- | --- | --- |
| Unix domain socket | Socket inode or abstract endpoint | Local RPC, daemon/client protocols | Framing and lifecycle cleanup |
| POSIX shared memory | Named shared object | High-throughput shared state | Requires explicit synchronization |
| System V message queue | Kernel message queue | Typed asynchronous work dispatch | Queue removal and capacity limits |
| Named pipe/FIFO | Filesystem node | Simple shell-friendly byte streams | Open ordering can block |
| mmap file | Page cache backed mapping | Shared persistent state | Locking is separate from mapping |
| Signals | Per-process pending signal state | Process lifecycle events | Handlers must use async-signal-safe operations |

## Design Lessons Demonstrated

- Shared memory is fast because data does not cross the kernel after mapping,
  but it is unsafe without semaphores, mutexes, or lock-free protocols.
- Message queues and sockets copy data through the kernel, but provide clean
  blocking semantics and ownership boundaries.
- FIFOs are excellent for simple tools because they integrate with filesystem
  permissions and shell redirection.
- `SIGCHLD` handling is easy to get wrong. The self-pipe pattern moves complex
  logic out of the signal handler and back into the main event loop.
- `mmap` does not imply consistency. The memory mapping shares bytes, while
  `fcntl` locks coordinate who may mutate them.

## Implementation Notes

- The programs use process boundaries instead of thread-only simulations.
- Temporary kernel objects are removed after use where the API requires cleanup.
- Shared-state examples separate data sharing from synchronization primitives.
- Socket examples cover both stream and datagram semantics.
- Queue examples include both System V and POSIX interfaces.
