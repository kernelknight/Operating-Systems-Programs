# Sockets

Five Unix-domain socket programs:

| Program | Concepts |
| --- | --- |
| `unix_calc_server.c` / `unix_calc_client.c` | stream socket RPC and fork-per-client service |
| `socketpair_fd_passing.c` | `socketpair`, `sendmsg`, `SCM_RIGHTS` descriptor passing |
| `unix_dgram_logger.c` | connectionless datagram logging with filesystem socket paths |
| `epoll_unix_echo.c` | nonblocking Unix socket server using `epoll` |
| `abstract_namespace_ping.c` | Linux abstract namespace socket address |
