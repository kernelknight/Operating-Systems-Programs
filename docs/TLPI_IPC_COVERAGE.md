# Linux IPC Coverage Map

This repository organizes Linux IPC programs by mechanism. The topic sequence is
aligned with common Linux systems-programming coverage: IPC overview, pipes and
FIFOs, System V IPC, POSIX IPC, memory mappings, file locking, sockets, server
design, and alternative I/O models.

## Current Coverage

| Topic | Folder | Coverage |
| --- | --- | --- |
| Pipes and FIFOs | `Pipes/` | anonymous pipes, FIFO pairs, `poll`, `select`, `splice`, self-pipe signal handling |
| Unix domain sockets | `Sockets/` | stream clients/servers, datagrams, `socketpair`, descriptor passing, abstract namespace, `epoll` |
| System V message queues | `MessageQueues/` | typed messages, priority workers, request/reply, queue metadata |
| POSIX message queues | `MessageQueues/` | notification, priorities, timed receive |
| POSIX shared memory | `SharedMemory/` | `shm_open`, shared mappings, process-shared semaphores and mutexes |
| Memory mappings | `SharedMemory/` | file-backed mappings, anonymous mappings, copy-on-write |
| Linux memory file descriptors | `SharedMemory/` | `memfd_create`, file seals |
| File locking | `FileLocking/` | advisory locks, lock files, byte-range locks, OFD locks |
| Semaphores | `Semaphores/` | System V semaphores, POSIX named semaphores, timed waits, barriers |
| Internet sockets | `InternetSockets/` | TCP/UDP clients and servers, concurrent service patterns, socket options |
| Alternative I/O | `AlternativeIO/` | `select`, `poll`, `epoll`, `signalfd`, `eventfd` |
| Signals | `Signals/` | handlers, masks, pending sets, queued payloads, child notification, realtime signals |

## Documentation Rule

Program READMEs describe the system problem and API behavior in original terms.
They do not reproduce exercise text from any book.
