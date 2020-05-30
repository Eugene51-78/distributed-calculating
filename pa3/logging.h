#ifndef _LOGGING__H
#define _LOGGING__H

#include "ipc.h"
#include "banking.h"
#include "comm.h"

extern int fd_events;
extern int fd_pipes;

static const char * const log_created_pipe_fmt =
    "Process %d CREATED pipe from fd %d to fd %d\n";

static const char * const log_closed_fd_fmt =
    "Process %d CLOSED fd %d\n";

void log_init();

void close_log();

void log_STARTED(process_t* process);

void log_DONE(int id, balance_t balance);

void log_waited_for_all_STARTED(int id);

void log_waited_for_all_DONE(int id);

void log_transfer_in(int dst, balance_t amount, int src);

void log_transfer_out(int src, balance_t amount, int dst);

void log_create_pipe(int id, int* fd);

void log_close_fd(int id, int fd);

#endif  // _LOGGING__H
