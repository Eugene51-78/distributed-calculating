#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ipc.h"
#include "comm.h"
int send(void * self, local_id dst, const Message * msg){
    process_t * process = self;
    write(process->w_fd[process->cur_id][dst], msg, sizeof(MessageHeader) + msg->s_header.s_payload_len);
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
    int f = read(process->r_fd[process->cur_id][from], msg, sizeof(Message));
    if ((f == -1) || (f == 0)) return -1;
    else return 0;
}

int receive_any(void * self, Message * msg){
    process_t * process = self;
    while(1){
        for (local_id id = 0; id < process->process_num; id++){
            if (id == process->cur_id)
                continue;
            else if (receive(self, id, msg) != -1)    
                    return 0;
        }
    }
}
