#define _BSD_SOURCE
#include <unistd.h>
#include <stdlib.h>

#include "logging.h"
#include "connect.h"
#include "transfer.h"
#include "sender.h"
#include "pa3.h"

void wait_STARTED(process_t* process) {
    int STARTED_counter = 0;
    int wait_counter = 0;
    int wait_num = process->process_num;
    
    while (wait_counter < wait_num){
        for (local_id i = 1; i < process->process_num; i++) {
            if(i != process->cur_id) {
                Message msg;
                receive(process, i, &msg);
                if (msg.s_header.s_type == STARTED) {
                    STARTED_counter++;
                    int time = msg.s_header.s_local_time;
                    change_lamp_time(time);
                }
                wait_counter++;
            }
        }
    }
}

void wait_DONE(process_t* process) {
    int DONE_counter = 0;
    int wait_counter = 0;
    int wait_num = process->process_num;
    
    while (wait_counter < wait_num){
        for (local_id i = 1; i < process->process_num; i++) {
            if(i != process->cur_id) {
                Message msg;
                receive(process, i, &msg);
                if (msg.s_header.s_type == DONE) {
                    DONE_counter++;
                    int time = msg.s_header.s_local_time;
                    change_lamp_time(time);
                }
                wait_counter++;
            }
        }
    }
    //usleep(10000);
}

void wait_ACK(process_t* process, local_id src, local_id dst, balance_t amount) {
    
    Message msg_CONFIRMATION;

    while (msg_CONFIRMATION.s_header.s_type != ACK){
       // printf("%s\n", "ACK V CYCLE POLY4ENIYA");
        receive(process, dst, &msg_CONFIRMATION);
       // if (confirmation.s_header.s_type != ACK) printf("%hd\n", confirmation.s_header.s_type);
    }

    int time = msg_CONFIRMATION.s_header.s_local_time;
    change_lamp_time(time);
    //printf("%s\n", "ACK POLY4ENO");
    log_transfer_in(dst, amount, src);
}

void message_handler(process_t* process) {
    int DONE_counter = 0;
    int wait_num = process->process_num - 2;

    while (DONE_counter < wait_num) {
        Message msg;
        receive_any(process, &msg);
        
        int time = msg.s_header.s_local_time;
        change_lamp_time(time);

        TransferOrder* transfer_order = (TransferOrder*) msg.s_payload;
        int src = transfer_order->s_src;
        int dst = transfer_order->s_dst;

        if (msg.s_header.s_type == TRANSFER && src == process->cur_id) { // in Csrc
            transfer_src_handler(process, transfer_order, msg);
        }
        else if (msg.s_header.s_type == TRANSFER && dst == process->cur_id) {   //already in Cdst
            transfer_dst_handler(process, transfer_order);
        }
        else if (msg.s_header.s_type == STOP) {
            send_DONE(process);
        }
        else if (msg.s_header.s_type == DONE) {
            DONE_counter++;
        }
    }
    
    log_DONE(process->cur_id, process->balance_state.s_balance);
    freeze_balance(process);
    send_BALANCE(process);
}
