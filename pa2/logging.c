#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "logging.h"
#include "pa2.h"
#include "common.h" 
#include "banking.h"

int fd_events;
int fd_pipes;

void log_init() {
    fd_events = open(events_log, O_CREAT | O_TRUNC | O_WRONLY, 00744);
    fd_pipes = open(pipes_log, O_CREAT | O_TRUNC | O_WRONLY, 00744);
}

void close_log() {
    close(fd_events);
    close(fd_pipes);
}

void log_error(int fd, const char *str) {
    write(2, str, strlen(str));
    write(2, "\n", 1);
    write(fd, str, strlen(str));
}

char* log_out(int fd, const char *format, ...) {
    
    va_list args;
    va_start(args, format);

    char* log_str = malloc(sizeof(format));

    vsprintf(log_str, format, args);

    if (fd == fd_events)
        write(1, log_str, strlen(log_str));
    
    write(fd, log_str, strlen(log_str));
    va_end(args);

    return log_str;
}

void log_waited_for_all_STARTED(local_id id) {
    log_out(fd_events, log_received_all_started_fmt, get_physical_time(), id);
}

void log_waited_for_all_DONE(local_id id) {
    log_out(fd_events, log_received_all_done_fmt, get_physical_time(), id);
}

void log_create_pipe(local_id id, int* fd) {
    log_out(fd_pipes, log_created_pipe_fmt, id, fd[0], fd[1]);
}

void log_close_fd(local_id id, int fd) {
    log_out(fd_pipes, log_closed_fd_fmt, id, fd);
}
