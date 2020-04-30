#ifndef _COMM_H_
#define _COMM_H_

#include "ipc.h"
#include <sys/types.h>
#include "banking.h"

typedef struct {
    local_id  cur_id;
    local_id  process_num;
    int**     r_fd;
    int**     w_fd;
    BalanceState balance_state;
    BalanceHistory balance_history;
} process_t;

int close_fd(local_id id, int fd);

int create_pipe(local_id id, int* fd);

void create_pipes(process_t* process);

void init_parent(process_t* process, int process_count);

void allocate_pipes(process_t* process);

void close_pipes(process_t* process);

process_t * create_children(process_t* process, balance_t * balance, local_id children_count);

void waiting_for_children();

#endif // _COMM_H_
