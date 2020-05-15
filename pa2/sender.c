#include <stdio.h>
#include "connect.h"

void send_STARTED(process_t* process) {
    Message msg_STARTED;
    msg_STARTED.s_header.s_type = STARTED;
    send(process, PARENT_ID, &msg_STARTED);
}

void send_DONE(process_t* process) {
    Message msg_DONE;
    msg_DONE.s_header.s_type = DONE;
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
    //printf("%s\n", "ACK POSLANO");
}

void send_BALANCE(process_t* process) {
    Message balance_history = create_bh(process);
    send(process, PARENT_ID, &balance_history);
}
