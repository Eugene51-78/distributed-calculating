#ifndef _CONNECT_H_
#define _CONNECT_H_

#include "comm.h"

Message create_message(MessageType type, void* content);

void wait_all(process_t* process, MessageType status);

void transfer_handler(process_t* process);

void wait_STARTED(process_t* process);

void wait_DONE(process_t* process);

void send_STARTED(process_t* process, char* buf);

void send_DONE(process_t* process, char* buf);

void send_STOP(process_t* process);

void send_ACK(process_t* process);

void child_existence(process_t* process, char* buf);

void parent_existence(process_t* process);

#endif  // _CONNECT_H_
