# Signals

Programs covering process signal delivery, handlers, masks, child reaping, and
Linux realtime signal behavior.

| Program | Concepts |
| --- | --- |
| `sigaction_restart_demo.c` | `sigaction`, `SA_RESTART`, interrupted blocking calls |
| `signal_mask_pending.c` | signal masks, blocked delivery, pending signal inspection |
| `sigqueue_payload.c` | queued signal delivery with `sigqueue` and `siginfo_t` |
| `sigchld_supervisor.c` | `SIGCHLD`, child exit handling, restart budget |
| `sigsuspend_wait.c` | atomic unblock-and-wait using `sigsuspend` |
| `realtime_signal_order.c` | realtime signal queuing and ordered delivery |
| `altstack_segv_recovery.c` | `sigaltstack` and `SA_ONSTACK` crash-path handling |
