#include <stdio.h>
#include "connect.h"

void send_STARTED(process_t* process) {
    increase_lamp_time();
    Message msg_STARTED;
    msg_STARTED.s_header.s_type = STARTED;
    msg_STARTED.s_header.s_local_time = get_lamport_time();
    send(process, PARENT_ID, &msg_STARTED);
}

void send_DONE(process_t* process) {
    increase_lamp_time();
    Message msg_DONE;
    msg_DONE.s_header.s_type = DONE;
    msg_DONE.s_header.s_local_time = get_lamport_time();
    send_multicast(process, &msg_DONE);
}
/*
    for the following 2 functions the time has already been increased by 1 in create_msg()
*/
void send_TRANSFER(process_t* process, int src, Message msg) {
    send(process, src, &msg_TRANSFER);
}

void send_to_Cdst(process_t* process, int dst, Message msg){
    send(process, dst, &msg_TRANSFER_FRW); //moving to Cdst
  }

void send_STOP(process_t* process) {
    increase_lamp_time();
    Message msg_STOP;
    msg_STOP.s_header.s_type = STOP;
    msg_STOP.s_header.s_local_time = get_lamport_time();
    send_multicast(process, &msg_STOP);
}

void send_ACK(process_t* process) {
    increase_lamp_time();
    Message msg_ACK;
    msg_ACK.s_header.s_type = ACK;
    msg_ACK.s_header.s_local_time = get_lamport_time();
    send(process, PARENT_ID, &msg_ACK); //confirmation
    //printf("%s\n", "ACK POSLANO");
}
/*
    the time has already been increased by 1 in create_bh()
*/
void send_BALANCE(process_t* process) {
    Message balance_history = create_bh(process);
    send(process, PARENT_ID, &balance_history);
}
