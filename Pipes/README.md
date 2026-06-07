# Pipes

Five programs covering pipe-backed IPC patterns:

| Program | Concepts |
| --- | --- |
| `anonymous_pipeline_exec.c` | parent-to-child pipe, `dup2`, `exec`, close discipline |
| `full_duplex_pipe_poll.c` | two unidirectional pipes, request/reply protocol, `poll` |
| `fifo_select_chat.c` | named pipes, full-duplex FIFO pair, `select` |
| `splice_zero_copy_pipe.c` | Linux `splice` through a pipe for zero-copy transfer |
| `self_pipe_signal_reactor.c` | signal-safe notification using the self-pipe trick |
