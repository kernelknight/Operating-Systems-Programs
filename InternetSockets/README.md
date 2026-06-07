# Internet Sockets

Programs covering IPv4/IPv6 socket patterns and server design.

| Program | Concepts |
| --- | --- |
| `tcp_forking_echo_server.c` | TCP listener, loopback clients, fork-per-connection |
| `udp_connected_echo.c` | connected UDP sockets and datagram echo |
| `getaddrinfo_loopback_client.c` | protocol-independent loopback connection |
| `reuseaddr_rebind_demo.c` | `SO_REUSEADDR` and predictable rebinding |
| `tcp_shutdown_halfclose.c` | stream half-close with `shutdown(SHUT_WR)` |
