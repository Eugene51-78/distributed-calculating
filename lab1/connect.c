#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "ipc.h"
#include "comm.h"
#include "logging.h"
#include "pa1.h"

Message create_message (MessageType type, char* content) {
 	Message msg;
    msg.s_header.s_magic = MESSAGE_MAGIC;
   	msg.s_header.s_payload_len = strlen(content);
   	memcpy(&(msg.s_payload), content, strlen(content));
   	msg.s_header.s_type = type;

   	return msg;
}

void wait_for_all_messages(process_t* process, MessageType type) {

    int STARTED_counter = 0;
    int DONE_counter = 0;
    int par_process = 0;
    int cur_counter = 0;
    int wait_num = process->process_num - 2;
    
    if (process->cur_id == PARENT_ID) par_process = 1;
    
    while (cur_counter < wait_num + par_process){
        for (local_id i = 0; i < process->process_num + par_process; i++) {
            if(i != process->cur_id && i != PARENT_ID){
                Message msg;
                receive(process, i, &msg);
                if ((type == STARTED) && (msg.s_header.s_type == STARTED)) STARTED_counter++;
                if ((type == DONE) && (msg.s_header.s_type == DONE)) DONE_counter++;
            }
        }
        if (type == STARTED) 
            cur_counter = STARTED_counter;
        else cur_counter = DONE_counter;
    }
}

void wait_STARTED(process_t* process) {
    wait_for_all_messages(process,STARTED);
}

void wait_DONE(process_t* process) {
    wait_for_all_messages(process, DONE);
}

void send_STARTED(process_t* process, char* buffer) {
    Message msg = create_message(STARTED, buffer);
    send_multicast(process, &msg);
}

void send_DONE(process_t* process, char* buffer) {
    Message msg = create_message(DONE, buffer);
    send_multicast(process, &msg);
}

void child_work(process_t* process, char* buffer) {
    
    send_STARTED(process, buffer);

    wait_STARTED(process);

    char* buffer1 = log_output(fd_event, log_done_fmt, process->cur_id);

    send_DONE(process, buffer1); 
    free(buffer1);

    log_output(fd_event, log_received_all_started_fmt, process->cur_id);
    
    wait_DONE(process);
    
    log_output(fd_event, log_received_all_done_fmt, process->cur_id);
}

void parent_work(process_t* process) {
    wait_STARTED(process);
    wait_DONE(process);
    waiting_for_children();
    close_log();
}
