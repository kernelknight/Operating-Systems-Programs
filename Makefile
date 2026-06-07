CC ?= gcc
CFLAGS ?= -std=c11 -D_GNU_SOURCE -Wall -Wextra -Wpedantic -O2 -g -ICommon
LDFLAGS ?=
LDLIBS ?= -pthread -lrt

BIN_DIR := bin
COMMON := Common/common.c
SOURCES := $(shell find Pipes Sockets MessageQueues SharedMemory Semaphores FileLocking InternetSockets -name '*.c' 2>/dev/null | sort)
PROGRAMS := $(patsubst %.c,$(BIN_DIR)/%,$(SOURCES))

.PHONY: all demo clean

all: $(PROGRAMS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: %.c $(COMMON) Common/common.h | $(BIN_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

demo: all
	./scripts/run_demos.sh

clean:
	rm -rf $(BIN_DIR)
	rm -f /tmp/ipc_*.sock /tmp/ipc_*.fifo /tmp/ipc_*.dat /tmp/ipc_*.txt
