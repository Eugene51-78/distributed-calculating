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

void wait_for_all_messages(process_t* process, MessageType type) {

    int STARTED_counter = 0;
    int DONE_counter = 0;
    int par_process = 0;
    int cur_counter = 0;
    int wait_num = process->process_num - 2;
    
    if (process->cur_id == PARENT_ID) par_process = 1;
    
    while (cur_counter < wait_num + par_process){
        for (local_id i = 0; i < process->process_num; i++) {
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

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount){

    process_t* process = parent_data;

    if (process->cur_id = PARENT_ID){ // only parent

        TransferOrder transfer_order;
        transfer_order.s_src = src;
        transfer_order.s_dst = dst;
        transfer_order.s_amount = amount;

        Message msg_out = create_message(TRANSFER, &transfer_order, sizeof(TransferOrder));
    
        send(process, src, &msg);
        log_send_transfer(&transfer_order); // что логировать?

        Message msg_in;

        while (msg_in.s_header.s_type != ACK){
        receive(process, dst, &msg_in);
        // что если придет сообщ другого типа и может ли?
        }

        log_receive_ACK(&smth);  // что логировать?
    }
}

void transfer_handler(process_t* process){
    for (int i = 0; i < process->process_num; i++){
        if (i != process->cur_id){
            Message msg;
            receive(process, i, &msg); 
            if (msg.s_header.s_type == TRANSFER && i == PARENT_ID){    // что если придет сообщ другого типа и может ли?
                int dst = msg.transfer_order.s_dst;
                  
                process.balance_state.s_balance-=msg.transfer_order.amount;  //надо изменить баланс
                process.balance_state.s_balance_pending_in = 0;
                process.balance_state.s_time;       // Что делать со временем?? get_physical_time()

                send(process, dst, &msg); //пересылка в Cdst
                }
                else if (msg.s_header.s_type == TRANSFER){   //уже в Cdst
                    process.balance_state.s_balance+=msg.transfer_order.amount;
                    process.balance_state.s_balance_pending_in = 0;       

                    Message msg_ack;
                    msg_ACK.s_header.s_type = ACK;
                    send(process, PARENT_ID, &msg_ACK); //подтверждение
                    }
            }
        }
}

void child_existence(process_t* process, char* buffer) {
    
    send_STARTED(process, buffer);
    wait_STARTED(process);
    send_DONE(process, log_out(fd_events, log_done_fmt, process->cur_id)); 
    wait_DONE(process);
}

void parent_existence(process_t* process) {
    wait_for_all_messages(process, STARTED);
    log_received_all_started(process);

    bank_robbery(process, process->process_num - 1);
    
    Message msg;
    msg_STOP.s_header.s_type = STOP;
    send_multicast(process, msg_STOP);

    wait_DONE(process);
    log_received_all_done (process);
    waiting_for_children();
    close_log();
}
