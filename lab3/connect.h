#ifndef _CONNECT_H_
#define _CONNECT_H_

#include "comm.h"

Message create_msg(MessageType type, void* content);

Message create_bh(process_t* process);

void child_existence(process_t* process);

void parent_existence(process_t* process);

void freeze_balance(process_t* process);

#endif  // _CONNECT_H_
