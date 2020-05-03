#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ipc.h"
#include "comm.h"
int send(void * self, local_id dst, const Message * msg){
    process_t * process = self;
    write(process->w_fd[process->cur_id][dst], msg, sizeof(Message));
    return 0;
}

int send_multicast(void * self, const Message * msg){
    process_t * process = self;

    for (local_id i = 0; i < process->process_num; i++) {
        if (i == process->cur_id) {
            continue;
        } else send(process, i, msg);
    }
    return 0;
}

int receive(void * self, local_id from, Message * msg){
    process_t * process = self;
    if(read(process->r_fd[process->cur_id][from], msg, sizeof(Message)) != -1)
    return 0;
    else return 1;
}

int receive_any(void * self, Message * msg){
	return -1;
}
