#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "logging.h"
#include "pa2345.h"
#include "common.h" 

int fd_events;
int fd_pipes;

int start_log() {
    fd_events = open(events_log, O_CREAT | O_TRUNC | O_WRONLY, 00744); 
    fd_pipes = open(pipes_log, O_CREAT | O_TRUNC | O_WRONLY, 00744);

    if (fd_events == -1 || fd_pipes == -1) {
        log_error(fd_events, "Can't start logging");
        log_error(fd_pipes, "Can't close logging");
        return -1;
    }
    return 0;
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
    int bufsize = sysconf(_SC_PAGESIZE);

    va_list args;
    va_start(args, format);

    char *buffer = malloc(bufsize);

    vsprintf(buffer, format, args);

    if (fd == fd_events)
    write(1, buffer, strlen(buffer));
    write(fd, buffer, strlen(buffer));
    va_end(args);

    return buffer;
}

void log_received_all_started(local_id id) {
    log_out(fd_event, log_received_all_started_fmt, get_physical_time(), id);
}

void log_received_all_done(local_id id) {
    log_out(fd_event, log_received_all_done_fmt, get_physical_time(), id);
}

void log_created_pipe(local_id id, int* fd) {
    log_out(fd_pipes, log_created_pipe_fmt, id, fd[0], fd[1]);
}

void log_closed_fd(local_id id, int fd) {
    log_out(fd_pipes, log_closed_fd_fmt, id, fd);
}
