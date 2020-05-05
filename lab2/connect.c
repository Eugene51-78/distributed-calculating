#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "ipc.h"
#include "comm.h"
#include "logging.h"
#include "pa1.h"
#include "banking.h"
#include "transfer.h"

Message create_message(MessageType type, void* content) {

 	Message msg;
    msg.s_header.s_magic = MESSAGE_MAGIC;
    msg.s_header.s_type = type;
    msg.s_header.s_local_time = get_physical_time();
    if (type == TRANSFER){
        int sz = sizeof(TransferOrder);
        msg.s_header.s_payload_len = sz;
        memcpy(&(msg.s_payload), content, sz);
    } else if (type == BALANCE_HISTORY){
        int sz = sizeof(BalanceHistory);
        msg.s_header.s_payload_len = sz;
        memcpy(&(msg.s_payload), content, sz);
    } else{
        char* text = (char*) content;
        int sz = strlen(text);
        msg.s_header.s_payload_len = sz;          
        memcpy(&(msg.s_payload), content, sz);
    }
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
    send_multicast(process, &msg_STOP);
}

void send_ACK(process_t* process) {
    Message msg_ACK;
    msg_ACK.s_header.s_type = ACK;
    send(process, PARENT_ID, &msg_ACK); //confirmation
}

void send_BALANCE(process_t* process) {
    Message balance_history = create_message(BALANCE_HISTORY, &process->balance_history);
    send(process, PARENT_ID, &balance_history);
}

int freeze_balance(process_t* process) {
    return 0;
}

void message_handler(process_t* process) {
    int DONE_counter = 0;
    int wait_num = process->process_num - 2;
    while (DONE_counter < wait_num){

        Message msg;
        receive_any(process, &msg);
        TransferOrder* transfer_order = (TransferOrder*) msg.s_payload;
        int src = transfer_order->s_src;
        if (msg.s_header.s_type == TRANSFER && src == process->cur_id){
            transfer_src_handler(process, transfer_order, msg);
        }
        else if (msg.s_header.s_type == TRANSFER){   //уже в Cdst
            transfer_dst_handler(process, transfer_order, msg); 
        }
        else if (msg.s_header.s_type == STOP){ // 3 phase
            send_DONE(process, "hello");
        }
        else if (msg.s_header.s_type == DONE){
            DONE_counter++;
        }
    }
    log_received_all_done(process->cur_id);
    freeze_balance(process);

    send_BALANCE(process);

}
        /*
        WHAT TO DO
        1) Changing history of Ci and controlling TIME. 
        2) Remember about logging
        */

void child_existence(process_t* process) {
    send_STARTED(process);
    message_handler(process);
}

void parent_existence(process_t* process) {
    wait_STARTED(process);
    log_received_all_started(PARENT_ID);

    bank_robbery(process, process->process_num - 1);
    
    send_STOP(process);

    wait_DONE(process);
    log_received_all_done(PARENT_ID);
    //need to get Histories and make AllHistory
    int HISTORY_counter = 0;
    AllHistory allHistory;
    while(HISTORY_counter < process->process_num - 1){
        Message msg;
        for (int i = 1; i < process->process_num; i++) {
            if (receive(process, i, &msg) != 0){
                continue;
            if (msg.s_header.s_type == BALANCE_HISTORY){
                HISTORY_counter++;
                BalanceHistory* history = (BalanceHistory*) msg.s_payload;
                allHistory.s_history[history->s_id - 1] = *history;
                allHistory.s_history_len++;
            }
            }
        }
    }

    AllHistory* allHistoryPtr = &allHistory;    

    print_history(allHistoryPtr);
    
    waiting_for_children();
    close_log();
}
