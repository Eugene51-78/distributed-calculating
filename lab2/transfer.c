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

void transfer_messaging(process_t* process, local_id src, local_id dst, balance_t amount) {

        Message transfer_out = create_message(TRANSFER, &transfer_order, sizeof(TransferOrder));
    
        send(process, src, &transfer_out);
        log_out(fd_event, log_transfer_out_fmt, get_physical_time(), transfer_order->s_src, transfer_order->s_amount, transfer_order->s_dst);

        Message transfer_in;

        while (transfer_in.s_header.s_type != ACK){
        receive(process, dst, &transfer_in);
        }

        log_out(fd_event, log_transfer_in_fmt, get_physical_time(), transfer_order->s_dst, transfer_order->s_amount, transfer_order->s_src);
}

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {

    process_t* process = parent_data;

    if (process->cur_id = PARENT_ID){ // only parent

        TransferOrder transfer_order;
        transfer_order.s_src = src;
        transfer_order.s_dst = dst;
        transfer_order.s_amount = amount;

        transfer_messaging(process, src, dst, amount);
    }
}

void transfer_src_handler(Message msg) {

    TransferOrder* transfer_order = (TransferOrder*) msg->s_payload;
    int dst = transfer_order.s_dst;

    process.balance_state.s_balance -= transfer_order.amount;  //need to change balance
    process.balance_state.s_time;       // Что делать со временем?? get_physical_time()

    send(process, dst, &msg); //пересылка в Cdst
}

void transfer_dst_handler(Message msg) {
    
    TransferOrder* transfer_order = (TransferOrder*)msg->s_payload;
    process.balance_state.s_balance += transfer_order.amount;
    send_ACK(&process);
}