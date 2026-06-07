# Build And Run

## Requirements

- Linux, WSL, or Linux VM
- `gcc`
- `make`
- POSIX threads support from glibc

On Ubuntu or WSL:

```bash
sudo apt update
sudo apt install build-essential
```

## Build

```bash
make
```

The generated executables are placed in matching paths under `bin/`.

```text
bin/Pipes/anonymous_pipeline_exec
bin/Sockets/socketpair_fd_passing
bin/MessageQueues/sysv_priority_workers
bin/SharedMemory/process_shared_mutex
```

## Run Full Demo

```bash
make demo
```

## Run By Topic

```bash
./bin/Pipes/full_duplex_pipe_poll
./bin/Pipes/fifo_select_chat
./bin/Pipes/splice_zero_copy_pipe
./bin/Pipes/self_pipe_signal_reactor

./bin/Sockets/socketpair_fd_passing
./bin/Sockets/unix_dgram_logger
./bin/Sockets/abstract_namespace_ping
./bin/Sockets/epoll_unix_echo

./bin/MessageQueues/sysv_request_reply
./bin/MessageQueues/sysv_queue_control
./bin/MessageQueues/posix_mq_notify_signal
./bin/MessageQueues/posix_mq_timed_receive

./bin/SharedMemory/mmap_file_lock_counter
./bin/SharedMemory/process_shared_mutex
./bin/SharedMemory/memfd_sealed_config
./bin/SharedMemory/mmap_copy_on_write
```

## Output Style

You should see output similar to:

```text
== Unix domain socket calculator ==
pid=12345 result=79.000

== Pipes: anonymous pipeline with exec ==
KERNEL PIPES FEED CHILD PROCESS STANDARD INPUT

== mmap counter with fcntl locks ==
counter=4000 expected=4000
```

Process IDs and ordering may vary because the examples intentionally use real
multi-process scheduling.

## Troubleshooting

- If `bind` fails for the socket demo, remove stale files with
  `rm -f /tmp/ipc_calc.sock`.
- If a System V queue remains after interruption, inspect it with `ipcs -q` and
  remove it with `ipcrm -q <id>`.
- If POSIX shared memory remains after interruption, check `/dev/shm` for
  names beginning with `ipc_`.
