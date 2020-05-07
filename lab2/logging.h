#ifndef _LOGGING__H
#define _LOGGING__H

#include "ipc.h"

extern int fd_events;
extern int fd_pipes;

static const char * const log_created_pipe_fmt =
    "Process %d CREATED pipe from fd %d to fd %d)\n";

static const char * const log_closed_fd_fmt =
    "Process %d CLOSED fd %d\n";

void log_init();

void close_log();

void log_error(int fd, const char *str);

char* log_out(int fd, const char *format,  ...);

char* log_STARTED (local_id id, pid_t pid, pid_t parent);

char* log_DONE(local_id id);

void log_waited_for_all_STARTED(local_id id);

void log_waited_for_all_DONE(local_id id);

void log_create_pipe(local_id id, int* fd);

void log_close_fd(local_id id, int fd);

#endif  // _LOGGING__H
