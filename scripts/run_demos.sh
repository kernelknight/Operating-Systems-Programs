#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN_DIR="$ROOT_DIR/bin"
SOCKET_PATH="/tmp/ipc_calc.sock"

echo "== Unix domain socket calculator =="
rm -f "$SOCKET_PATH"
"$BIN_DIR/Sockets/unix_calc_server" "$SOCKET_PATH" > /tmp/ipc_calc_server.log 2>&1 &
server_pid=$!
trap 'kill "$server_pid" 2>/dev/null || true; rm -f "$SOCKET_PATH"' EXIT
sleep 0.2
"$BIN_DIR/Sockets/unix_calc_client" "$SOCKET_PATH" "12 * (3 + 4) - 5"
kill "$server_pid"
wait "$server_pid" 2>/dev/null || true
trap - EXIT
rm -f "$SOCKET_PATH"

echo
echo "== Pipes: anonymous pipeline with exec =="
"$BIN_DIR/Pipes/anonymous_pipeline_exec"

echo
echo "== System V message queue workers =="
"$BIN_DIR/MessageQueues/sysv_priority_workers"

echo
echo "== Sockets: descriptor passing =="
"$BIN_DIR/Sockets/socketpair_fd_passing"

echo
echo "== mmap counter with fcntl locks =="
"$BIN_DIR/SharedMemory/mmap_file_lock_counter"

echo
echo "== POSIX shared memory ring buffer =="
"$BIN_DIR/SharedMemory/posix_shm_ring_sem"
