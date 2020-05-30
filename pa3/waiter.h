#ifndef _WAITER__H
#define _WAITER__H

void wait_STARTED(process_t* process);

void wait_DONE(process_t* process);

void wait_ACK(process_t* process, int src, int dst, balance_t amount);

void message_handler(process_t* process);

void wait_all(process_t* process, MessageType status);

void transfer_handler(process_t* process);

#endif  // _WAITER__H
