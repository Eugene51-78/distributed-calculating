#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "ipc.h"
#include "sender.h"
#include "logging.h"
#include "pa2.h"
#include "banking.h"
#include "transfer.h"
#include "waiter.h"

Message create_msg(MessageType type, void* content) {
 	Message msg;
    msg.s_header.s_magic = MESSAGE_MAGIC;
    msg.s_header.s_type = type;
    msg.s_header.s_local_time = get_physical_time();
    if (type == TRANSFER) {
        int sz = sizeof(TransferOrder);
        msg.s_header.s_payload_len = sz;
        memcpy(&(msg.s_payload), content, sz);
    } else {
        char* text = (char*) content;
        int sz = strlen(text);
        msg.s_header.s_payload_len = sz;          
        memcpy(&(msg.s_payload), content, sz);
    }
   	return msg;
}

Message create_bh(process_t* process){
    Message msg;
    msg.s_header.s_magic = MESSAGE_MAGIC;
    msg.s_header.s_type = BALANCE_HISTORY;
    msg.s_header.s_local_time = get_physical_time();
    BalanceHistory* content = &process->balance_history;

    int sz = (process->balance_history.s_history_len) * sizeof(BalanceState) + 
        sizeof(process->balance_history.s_history_len) + 
        sizeof(process->balance_history.s_id);
        msg.s_header.s_payload_len = sz;
        memcpy(&(msg.s_payload), content, sz);

    return msg;
}

void freeze_balance(process_t* process) {
    timestamp_t final_time = get_physical_time();
    // printf("%s\n", process->balance_history.s_history[process->balance_history.s_history_len - 1]);
    if (process->balance_state.s_time != final_time) {
        process->balance_state = process->balance_history.s_history[process->balance_history.s_history_len - 1];
        process->balance_state.s_time = final_time;
        //printf("%hd\n", final_time);
        change_history(process);
    };
}

AllHistory make_all_history(process_t* process) {
    //need to get Histories and make AllHistory
    int HISTORY_counter = 0;
    AllHistory allHistory;

    while(HISTORY_counter < process->process_num - 1){
        Message msg;
        receive_any(process, &msg);
        if (msg.s_header.s_type == BALANCE_HISTORY){
            HISTORY_counter++;
            BalanceHistory* history = (BalanceHistory*) msg.s_payload;
            allHistory.s_history[history->s_id - 1] = *history;
            allHistory.s_history_len++;
        }
    }
    return allHistory;
}

void child_existence(process_t* process) {
    send_STARTED(process);
    message_handler(process);
}

void parent_existence(process_t* process) {
    wait_STARTED(process);
    log_waited_for_all_STARTED(PARENT_ID);
    bank_robbery(process, process->process_num - 1);
    send_STOP(process);
    wait_DONE(process);
    log_waited_for_all_DONE(PARENT_ID);
    AllHistory allHistory = make_all_history(process);
    print_history(&allHistory);
    close_log();
    waiting_for_children();
}
