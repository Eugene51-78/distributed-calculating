#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "ipc.h"
#include "comm.h"
#include "logging.h"
#include "pa1.h"
#include "banking.h"

Message create_message(MessageType type, char* content) {
 	Message msg;
    msg.s_header.s_magic = MESSAGE_MAGIC;
    msg.s_header.s_type = type;
   	msg.s_header.s_payload_len = strlen(content);
   	memcpy(&(msg.s_payload), content, strlen(content));

   	return msg;
}

void wait_all(process_t* process, MessageType type) {

    int STARTED_counter = 0;
    int DONE_counter = 0;
    int par_process = 0;
    int cur_counter = 0;
    int wait_num = process->process_num - 2;
    
    if (process->cur_id == PARENT_ID) par_process = 1;
    
    while (cur_counter < wait_num + par_process){
        for (local_id i = 0; i < process->process_num; i++) {
            if(i != process->cur_id && i != PARENT_ID) {
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
    wait_all(process, STARTED);
}

void wait_DONE(process_t* process) {
    wait_all(process, DONE);
}

void send_STARTED(process_t* process) {
    Message msg_STARTED;
    msg_STARTED.s_header.s_type = STARTED;
    send(process, PARENT_ID, &msg_STARTED);
}

void send_DONE(process_t* process, char* buffer) {
    Message msg_DONE = create_message(DONE, buffer);
    send_multicast(process, &msg_DONE);
}

void send_STOP(process_t* process) {
    Message msg_STOP;
    msg_STOP.s_header.s_type = STOP;
    send_multicast(process, msg_STOP);
}

void send_ACK(process_t* process) {
    Message msg_ACK;
    msg_ACK.s_header.s_type = ACK;
    send(process, PARENT_ID, &msg_ACK); //confirmation
}

void message_handler(process_t* process) {
    int DONE_counter = 0;
    int wait_num = process->process_num - 2;
    while (DONE_counter < wait_num){

        Message msg;
        int src = receive_any(process, &msg);

        if (msg.s_header.s_type == TRANSFER && src == PARENT_ID){
            transfer_src_handler(&msg);
        }
        else if (msg.s_header.s_type == TRANSFER){   //уже в Cdst
            transfer_dst_handler(&msg); 
        }
        else if (msg.s_header.s_type == STOP){ // 3 phase
            send_DONE(process);
        }
        else if (msg.s_header.s_type == DONE){
            DONE_counter++;
        }
    }
}
        /*
        WHAT TO DO
        1) Changing history of Ci and controlling TIME. 
        2) Remember about logging
        */

void child_existence(process_t* process) {
    send_STARTED(process);
    message_handler(process);
    log_received_all_done(process->cur_id);
}

void parent_existence(process_t* process) {
    wait_STARTED(process);
    log_received_all_started(PARENT_ID);

    bank_robbery(process, process->process_num - 1);
    
    send_STOP(process);

    wait_DONE(process);
    log_received_all_done(PARENT_ID);
    //print_history(allhistory); here
    waiting_for_children();
    close_log();
}
