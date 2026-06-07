# Operating Systems Programs

Linux C programs covering inter-process communication, synchronization, process
control, and kernel-backed data exchange mechanisms. The examples are organized
by IPC mechanism and build into separate executables.

## What This Demonstrates

| Area | Folder | Skills shown |
| --- | --- | --- |
| Pipes | `Pipes/` | anonymous pipes, FIFOs, `poll`, `select`, `splice`, signal self-pipe |
| Sockets | `Sockets/` | Unix stream sockets, datagrams, `socketpair`, descriptor passing, `epoll` |
| Internet sockets | `InternetSockets/` | TCP, UDP, `getaddrinfo`, half-close, address reuse, concurrent service |
| Message queues | `MessageQueues/` | System V queues, POSIX mqueues, typed requests, notification, queue stats |
| Shared memory | `SharedMemory/` | POSIX shm, `mmap`, process-shared mutexes, memfd sealing, copy-on-write |
| Semaphores | `Semaphores/` | System V semaphore sets, POSIX named/unnamed semaphores, timed waits |
| File locking | `FileLocking/` | byte-range locks, lock files, `flock`, OFD locks, ordered acquisition |
| Alternative I/O | `AlternativeIO/` | `select`, `poll`, `epoll`, `signalfd`, `eventfd` |
| Signals | `Signals/` | `sigaction`, masks, `sigqueue`, `SIGCHLD`, `sigsuspend`, realtime signals |

## Quick Start

These programs use Linux/POSIX APIs. Build them on Linux, WSL, or a Linux VM.

```bash
make
```

Run the curated demo sequence:

```bash
make demo
```

Clean generated files:

```bash
make clean
```

## Individual Runs

```bash
./bin/Pipes/anonymous_pipeline_exec
./bin/Sockets/unix_calc_server /tmp/ipc_calc.sock &
./bin/Sockets/unix_calc_client /tmp/ipc_calc.sock "12 * (3 + 4) - 5"
./bin/InternetSockets/tcp_forking_echo_server
./bin/MessageQueues/sysv_priority_workers
./bin/SharedMemory/posix_shm_ring_sem
./bin/Semaphores/posix_named_sem_pingpong
./bin/FileLocking/fcntl_byte_range_locks
```

## Repository Map

```text
.
|-- Makefile
|-- README.md
|-- Common/
|-- AlternativeIO/
|-- docs/
|-- FileLocking/
|-- InternetSockets/
|-- MessageQueues/
|-- Pipes/
|-- scripts/
|-- Semaphores/
|-- SharedMemory/
|-- Signals/
`-- Sockets/
```

## Notes

- Code is written in C11 with GNU/POSIX feature macros for Linux-specific APIs.
- Each topic folder has a README describing the programs and system calls used.
- `make` builds every source file into a matching path under `bin/`.
- GitHub Actions builds all programs and runs the curated demo sequence on Ubuntu.
