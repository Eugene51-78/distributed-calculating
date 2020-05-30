#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include "connect.h"

void send_STARTED(process_t* process) {
    Message msg_STARTED;
    msg_STARTED.s_header.s_magic = MESSAGE_MAGIC;
    msg_STARTED.s_header.s_type = STARTED;
    msg_STARTED.s_header.s_local_time = get_lamport_time();
    send(process, PARENT_ID, &msg_STARTED);
}

void send_DONE(process_t* process) {
    Message msg_DONE;
    msg_DONE.s_header.s_magic = MESSAGE_MAGIC;
    msg_DONE.s_header.s_type = DONE;
    msg_DONE.s_header.s_local_time = get_lamport_time();
    send_multicast(process, &msg_DONE);
}

void send_TRANSFER(process_t* process, int src, Message msg) {
    send(process, src, &msg);
}

void send_to_Cdst(process_t* process, int dst, Message msg){
    send(process, dst, &msg); //moving to Cdst
}

void send_STOP(process_t* process) {
    Message msg_STOP;
    msg_STOP.s_header.s_magic = MESSAGE_MAGIC;
    msg_STOP.s_header.s_type = STOP;
    msg_STOP.s_header.s_local_time = get_lamport_time();
    send_multicast(process, &msg_STOP);
}

void send_ACK(process_t* process) {;
    Message msg_ACK;
    msg_ACK.s_header.s_magic = MESSAGE_MAGIC;
    msg_ACK.s_header.s_type = ACK;
    msg_ACK.s_header.s_local_time = get_lamport_time();
    send(process, PARENT_ID, &msg_ACK); //confirmation
}

void send_BALANCE(process_t* process) {
    usleep(100000);
    Message balance_history = create_bh(process);
    send(process, PARENT_ID, &balance_history);
}
