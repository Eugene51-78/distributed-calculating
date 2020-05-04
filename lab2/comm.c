#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <stdio.h>

#include "connect.h"
#include "logging.h"
#include "pa1.h"

#define SQR(x) x*x

int close_fd(local_id id, int fd) {
    if (close(fd) == 0) {
        log_closed_fd(id, fd);
        return 0;
    }
    return -1;
}

void create_pipes(process_t* process) {
    for (local_id i = 0; i < process->process_num; i++) {
        for (local_id j = 0; j < process->process_num; j++) {
            if (i != j) {
                int pipefd[2];
                pipe2(pipefd, O_NONBLOCK);
                log_created_pipe(process->cur_id, pipefd);
                process->r_fd[j][i] = fd[0];
                process->w_fd[i][j] = fd[1];
            }
        }
    }
}

void init_parent(process_t* process, int process_num) {
    process->cur_id = PARENT_ID;
    process->process_num = process_num;
}

// Init fd
void allocate_pipes(process_t* process){

int process_number = process->process_num;
process->r_fd = malloc(sizeof(int*) * process_number);
process->w_fd = malloc(sizeof(int*) * process_number);

for (int i = 0; i < process_number; i++) {
    if (i == 0) {
    process->r_fd[i] = malloc(sizeof(int) * process_number*process_number);
    process->w_fd[i] = malloc(sizeof(int) * process_number*process_number);
    } else {
    process->r_fd[i] = (*(process->r_fd) + process_number * i);
    process->w_fd[i] = (*(process->w_fd) + process_number * i);
    }
}
} 

void close_pipes (process_t* process) {
    for (local_id i = 0; i < process->process_num; i++) {
        for (local_id j = 0; j < process->process_num; j++) {
            if (i != process->cur_id && i != j) {
                close_fd(process->cur_id, process->r_fd[i][j]);
                close_fd(process->cur_id, process->w_fd[i][j]);
            }
        }
    }
}

// fork
char * create_children (process_t* process, balance_t* balance) {
    for (local_id i = 0; i < process->process_num; i++) {
        if (i != PARENT_ID && process->cur_id == PARENT_ID) {
            pid_t pid = fork();
            if ( pid == 0 ) {
                // I AM CHILD
               process->cur_id = i;
               process->balance_state.s_balance = balance[i-1];
               process->balance_state.s_balance_pending_in = 0;
               process->balance_state.s_time = get_physical_time();
               process->balance_history.s_id = i;
               process->balance_history.s_history_len = 1;
               process->balance_history.s_history[1] = process->balance_state;

               return log_out(fd_event, log_started_fmt, get_physical_time(),
                                    process->cur_id, getpid(), getppid(), process->balance_state.s_balance);
            }
        }
    }
    return NULL;
}

void waiting_for_children(){
    int pid;
    do {
    pid = wait(NULL);
    } while (pid != -1);
}

int isNumber(char* String) {
    char str = *String;
    if (str >= '0' && str <= '9') return 1; 
    else return 0;
}
