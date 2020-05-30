#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "logging.h"
#include "pa3.h"
#include "common.h" 
#include "banking.h"

FILE* f_events;
FILE* f_pipes;

void log_init() {
    f_events = fopen(events_log, "w");
    f_pipes = fopen(pipes_log, "w");
}

void close_log() {
    fclose(f_events);
    fclose(f_pipes);
}

void log_STARTED(process_t* process) {
    fprintf(f_events, log_started_fmt, get_lamport_time(), process->cur_id, getpid(), getppid(), process->balance_state.s_balance);
    printf(log_started_fmt, get_lamport_time(), process->cur_id, getpid(), getppid(), process->balance_state.s_balance);
}

void log_DONE(int id, balance_t balance) {
    fprintf(f_events, log_done_fmt, get_lamport_time(), id, balance);
    printf(log_done_fmt, get_lamport_time(), id, balance);
}

void log_waited_for_all_STARTED(int id) {
    fprintf(f_events, log_received_all_started_fmt, get_lamport_time(), id);
    printf(log_received_all_started_fmt, get_lamport_time(), id);
}

void log_waited_for_all_DONE(int id) {
    fprintf(f_events, log_received_all_done_fmt, get_lamport_time(), id);
    printf(log_received_all_done_fmt, get_lamport_time(), id);
}

void log_transfer_in(int dst, balance_t amount, int src) {
    fprintf(f_events, log_transfer_in_fmt, get_lamport_time(), dst, amount, src);
    printf(log_transfer_in_fmt, get_lamport_time(), dst, amount, src);
}

void log_transfer_out(int src, balance_t amount, int dst) {
    fprintf(f_events, log_transfer_out_fmt, get_lamport_time(), src, amount, dst);
    printf(log_transfer_out_fmt, get_lamport_time(), src, amount, dst);
}

void log_create_pipe(int id, int* fd) {
    fprintf(f_pipes, log_created_pipe_fmt, id, fd[0], fd[1]);
    //printf(log_created_pipe_fmt, id, fd[0], fd[1]);
}

void log_close_fd(int id, int fd) {
    fprintf(f_pipes, log_closed_fd_fmt, id, fd);
   // printf(log_closed_fd_fmt, id, fd);
}
