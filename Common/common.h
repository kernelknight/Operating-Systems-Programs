#ifndef OS_PROGRAMS_COMMON_H
#define OS_PROGRAMS_COMMON_H

#include <stddef.h>
#include <sys/types.h>

void die(const char *message);
void die_errno(const char *message);
void xclose(int fd);
void wait_for_pid(pid_t pid);
void reap_all_children(void);
ssize_t write_all(int fd, const void *buffer, size_t length);
int set_nonblock(int fd);

#endif
