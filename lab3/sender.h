#ifndef _SENDER_H_
#define _SENDER_H_

#include "comm.h"

void send_STARTED(process_t* process);

void send_DONE(process_t* process);

void send_TRANSFER(process_t* process, int src, Message msg);

void send_STOP(process_t* process);

void send_ACK(process_t* process);

void send_BALANCE(process_t* process);

#endif  // _SENDER_H_
