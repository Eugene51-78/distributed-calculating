#define _BSD_SOURCE
#include <unistd.h>
#include <stdlib.h>

#include "logging.h"
#include "connect.h"
#include "transfer.h"
#include "sender.h"
#include "pa2.h"

void wait_STARTED(process_t* process) {
    int STARTED_counter = 0;
    int wait_counter = 0;
    int wait_num = process->process_num;
    
    while (wait_counter < wait_num){
        for (local_id i = 1; i < process->process_num; i++) {
            if(i != process->cur_id) {
                Message msg;
                receive(process, i, &msg);
                if (msg.s_header.s_type == STARTED) STARTED_counter++;
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
                if (msg.s_header.s_type == DONE) DONE_counter++;
                wait_counter++;
            }
        }
    }
}

void message_handler(process_t* process) {
    int DONE_counter = 0;
    int wait_num = process->process_num - 2;

    while (DONE_counter < wait_num) {
        Message msg;
        receive_any(process, &msg);
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
    timestamp_t cur_time = get_physical_time();
    log_out(fd_events, log_done_fmt, cur_time, process->cur_id, process->balance_state.s_balance);
    freeze_balance(process);
    send_BALANCE(process);
}
