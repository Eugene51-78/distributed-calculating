#ifndef _COMM_H_
#define _COMM_H_

#include <sys/types.h>

#include "ipc.h"
#include "banking.h"

typedef struct {
    int  cur_id;
    int**     r_fd;
    int**     w_fd;
    int  process_num;
    BalanceState balance_state;
    BalanceHistory balance_history;
} process_t;

int close_fd(int id, int fd);

void create_pipes(process_t* process);

void allocate_pipes(process_t* process);

void close_pipes(process_t* process);

void init_parent(process_t* process, int process_num);

void create_children(process_t* process, balance_t* balance);

void waiting_for_children();

int isNumber(char* String);

#endif // _COMM_H_
