#define _BSD_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#include "ipc.h"
#include "comm.h"
#include "logging.h"
#include "pa3.h"
#include "banking.h"
#include "transfer.h"
#include "connect.h"
#include "sender.h"
#include "waiter.h"

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {

    process_t* process = parent_data;

    TransferOrder transfer_order;
    transfer_order.s_src = src;
    transfer_order.s_dst = dst;
    transfer_order.s_amount = amount;

    Message msg_TRANSFER = create_msg(TRANSFER, &transfer_order);
    send_TRANSFER(process, src, msg_TRANSFER);

    log_transfer_out(src, amount, dst);
    wait_ACK(process, src, dst, amount);
}

void transfer_src_handler(process_t* process, TransferOrder* order, Message msg) {

    TransferOrder* transfer_order = order;
    int dst = transfer_order->s_dst;
    process->balance_state.s_time = get_lamport_time();
    process->balance_state.s_balance -= transfer_order->s_amount;  //need to change balance
    change_history(process);
    usleep(10000);
    send_to_Cdst(process, dst, msg);
}

void transfer_dst_handler(process_t* process, TransferOrder* order) {

    TransferOrder* transfer_order = order;
    process->balance_state.s_time = get_lamport_time();
    process->balance_state.s_balance += transfer_order->s_amount;
    change_history(process);
    usleep(10000);
    send_ACK(process);
}

void change_history(process_t* process){
    int count = process->balance_history.s_history_len;
    while (count < process->balance_state.s_time) {
        process->balance_history.s_history[count] = process->balance_history.s_history[count-1];
        process->balance_history.s_history[count].s_time = count;
        count++;
    }
    process->balance_history.s_history[process->balance_state.s_time] = process->balance_state;
    process->balance_history.s_history_len = process->balance_state.s_time + 1;
}
