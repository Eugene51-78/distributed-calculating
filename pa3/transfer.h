#ifndef _TRANSFER_H_
#define _TRANSFER_H_

void transfer_src_handler(process_t* process, TransferOrder* order, Message msg);

void transfer_dst_handler(process_t* process, TransferOrder* order);

void change_history(process_t* process);

#endif  // _TRANSFER_H_
