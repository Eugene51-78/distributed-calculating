#ifndef _TRANSFER_H_
#define _TRANSFER_H_

void transfer_messaging(process_t* process, local_id src, local_id dst, balance_t amount);

void transfer_src_handler(Message msg);

void transfer_dst_handler(Message msg);

#endif  // _CONNECT_H_
