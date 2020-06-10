#include "comm.h"

int pipe_fd;
int event_fd;
size_t len;
char buffer[MAX_PAYLOAD_LEN];
char buff[100];

void LOG(process_t *process, short p_flag, char *buffer, int len) {
    if (p_flag == 1)
        write(pipe_fd, buffer, len);
    else {
        write(1, buffer, len);
        write(event_fd, buffer, len);
    }
}

void close_logs(process_t *process) {
    close(event_fd);
    close(pipe_fd);
}

int open_pipes(process_t *process) {
    pipe_fd = open(pipes_log, O_TRUNC|O_WRONLY|O_CREAT, 0644);
    event_fd = open(events_log, O_TRUNC|O_WRONLY|O_CREAT|O_APPEND, 0644);
    for (int i = 0; i < process->number; ++i) {
        for (int j = 0; j < process->number; ++j) {
            if (i != j) {
                if (!pipe(process->pipe[i][j]))
                    write(pipe_fd, buff, sprintf(buff, open_fmt, i, j));

                fcntl(process->pipe[i][j][0], F_SETFL, O_NONBLOCK);
                fcntl(process->pipe[i][j][1], F_SETFL, O_NONBLOCK);
            }
        }
    }
    return 0;
}

int close_pipes(process_t *process) {
    for (int i = 0; i < process->number; ++i) {
        for (int j = 0; j < process->number; ++j) {
            if (i != j) {
                if (i != process->id && j != process->id) {
                    close(process->pipe[i][j][0]);
                    LOG(process, 1, buff, sprintf(buff, close_fmt, process->id, i, j, "r"));
                    close(process->pipe[i][j][1]);
                    LOG(process, 1, buff, sprintf(buff, close_fmt, process->id, i, j, "w"));
                } else if (process->id == j) {
                    close(process->pipe[i][j][1]);
                    LOG(process, 1, buff, sprintf(buff, close_fmt, process->id, i, j, "w"));
                } else {
                    close(process->pipe[i][j][0]);
                    LOG(process, 1, buff, sprintf(buff, close_fmt, process->id, i, j, "r"));    
                }          
            }
        }
    }
    return 0;
}

void log_STARTED(short flag, process_t *process) {
    if (flag == 0)
        LOG(process, 0, buffer, sprintf(buffer, log_received_all_started_fmt, get_lamport_time(), process->id));
    else {
        len = sprintf(buffer, log_received_all_started_fmt, get_lamport_time(), process->id);
        LOG(process, 0, buffer, len);
    }
}

void log_DONE(short flag, process_t *process) {
    if (flag == 0)
        LOG(process, 0, buffer, sprintf(buffer, log_received_all_done_fmt, get_lamport_time(), process->id));
    else {
        len = sprintf(buffer, log_received_all_done_fmt, get_lamport_time(), process->id);
        LOG(process, 0, buffer, len);
    }
}
