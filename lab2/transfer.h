#ifndef _TRANSFER_H_
#define _TRANSFER_H_

void wait_ACK(process_t* process, local_id src, local_id dst, balance_t amount);

void transfer_src_handler(process_t* process, TransferOrder* order, Message msg);

void transfer_dst_handler(process_t* process, TransferOrder* order, Message msg);

#endif  // _TRANSFER_H_
