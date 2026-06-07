# Sample Output

Actual process IDs and some ordering can vary because the programs use real
multi-process scheduling.

## Unix Domain Socket Calculator

```text
== Unix domain socket calculator ==
pid=18542 result=79.000
```

## Pipes

```text
KERNEL PIPES FEED CHILD PROCESS STANDARD INPUT
child pid=2118 processed: read inode
peer-A received: peer-B message 1
zero-copy splice moves bytes through a pipe buffer
main loop handled signal 10 outside handler
```

## Sockets

```text
pid=2190 result=79.000
child read from received fd: descriptor passed over unix socket
logger received: event=1 source=client
server received: ping abstract namespace
epoll echo saw: client-1
```

## Internet Sockets

```text
tcp worker echo: alpha
connected udp echo: udp payload
resolved loopback stream endpoints=1
SO_REUSEADDR allowed rebind on loopback port 43127
client read after half-close: response after EOF
```

## Message Queues

```text
job=1 value=2 square=4 worker=2240
sum=55 product=714 reply-type=2239
queue-id=3 messages=4 bytes=160
received priority=7 text=notify payload
timed receive priority=3 text=deadline aware message
```

## Shared Memory

```text
consumer total=210
counter=4000 expected=4000
shared balance=4000 expected=4000
child mapped sealed config:
workers=4
parent still sees: parent-before-fork
```

## Semaphores And File Locks

```text
system-v guarded counter=4000 expected=4000
ping 1
sem_timedwait expired as expected
child observed: shared-memory latch released
second instance rejected by advisory lock
ofd-locked counter=1500 expected=1500
```
