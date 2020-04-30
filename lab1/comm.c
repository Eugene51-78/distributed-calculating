#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>

#include "connect.h"
#include "logging.h"
#include "pa1.h"

#define SQR(x) x*x

int close_fd ( local_id id, int fd ) {
    if ( close(fd) == 0 ) {
        log_closed_fd(id, fd);
        return 0;
    }

    return -1;
}

void create_pipes (process_t* process) {
    for (local_id i = 0; i < process->process_num; i++) {
        for (local_id j = 0; j < process->process_num; j++) {
            if (i!=j) {
                int fd[2];
                pipe(fd);
                log_created_pipe(process->cur_id, fd);
                process->r_fd[j][i] = fd[0];
                process->w_fd[i][j] = fd[1];
            }
        }
    }
}

void init_parent (process_t* process, int process_num) {
    process->cur_id = PARENT_ID;
    process->process_num = process_num;
}

// Init fd
void allocate_pipes (process_t* process) {
        
    int process_number = process->process_num;
    process->r_fd = (int **)malloc( sizeof(int *) * process_number);
    process->w_fd = (int **)malloc( sizeof(int *) * process_number);
    process->r_fd[0] = malloc( sizeof(int) * SQR(process_number) );
    process->w_fd[0] = malloc( sizeof(int) * SQR(process_number) );
    for (int i = 0; i < process_number; i++) {
        process->r_fd[i] = (*(process->r_fd) + process_number * i);
        process->w_fd[i] = (*(process->w_fd) + process_number * i);
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
char* create_children (process_t* process) {
    for (local_id i = 0; i < process->process_num; i++) {
        if (i != PARENT_ID && process->cur_id == PARENT_ID) {
            pid_t pid = fork();
            if ( pid == 0 ) {
                // I AM CHILD
                process->cur_id = i;
                return log_output(fd_event, log_started_fmt, process->cur_id, getpid(), getppid());
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
