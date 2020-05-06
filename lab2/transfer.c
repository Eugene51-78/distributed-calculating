#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#include "ipc.h"
#include "comm.h"
#include "logging.h"
#include "pa2345.h"
#include "banking.h"
#include "transfer.h"
#include "connect.h"

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {

    process_t* process = parent_data;

    if (process->cur_id == PARENT_ID){ // only parent

        TransferOrder transfer_order;
        transfer_order.s_src = src;
        transfer_order.s_dst = dst;
        transfer_order.s_amount = amount;

        Message transfer_out = create_message(TRANSFER, &transfer_order);
        send(process, src, &transfer_out);
        log_out(fd_event, log_transfer_out_fmt, get_physical_time(), src, amount, dst);
        wait_ACK(process, src, dst, amount);
    }
}

void wait_ACK(process_t* process, local_id src, local_id dst, balance_t amount) {
    
    Message transfer_in;

    while (transfer_in.s_header.s_type != ACK){
        receive(process, dst, &transfer_in);
    }

    log_out(fd_event, log_transfer_in_fmt, get_physical_time(), dst, amount, src);
}

void transfer_src_handler(process_t* process, TransferOrder* order, Message msg) {

    TransferOrder* transfer_order = order;
    int dst = transfer_order->s_dst;
    process->balance_state.s_time = get_physical_time();
    process->balance_state.s_balance -= transfer_order->s_amount;  //need to change balance
    change_history(process);
    send(process, dst, &msg); //moving to Cdst
}

void transfer_dst_handler(process_t* process, TransferOrder* order) {

    TransferOrder* transfer_order = order;
    process->balance_state.s_time = get_physical_time();
    process->balance_state.s_balance += transfer_order->s_amount;
    change_history(process);
    send_ACK(process);
}

void change_history(process_t* process){
    if (process->balance_state.s_time >= process->balance_history.s_history_len - 1) {
        for (timestamp_t t = process->balance_history.s_history_len; t < process->balance_state.s_time; t++) {
            process->balance_history.s_history[t] = process->balance_history.s_history[t-1];
            process->balance_history.s_history[t].s_time = t;
        }
        process->balance_history.s_history[process->balance_state.s_time] = process->balance_state;
        process->balance_history.s_history_len = process->balance_state.s_time + 1;
    }
}
