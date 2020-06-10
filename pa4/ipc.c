#define _BSD_SOURCE
#include "unistd.h"
#include "comm.h"

int send(void *self, local_id dst, const Message *msg) {
    process_t *process = self;
    local_id src = process->id;

    return write(process->pipe[src][dst][1], msg, sizeof(MessageHeader) + msg->s_header.s_payload_len);
}

int send_multicast(void *self, const Message *msg) {
    process_t *process = self;

    for (local_id i = 0; i < process->number; i++) {
        if (i == process->id) {
            continue;
        } else if (send(process, i, msg) == -1)
                    return -1;
    }
    return 0;
}

int receive(void *self, local_id src, Message *msg) {
    process_t *process = self;
    local_id dst = process->id;

    while (read(process->pipe[src][dst][0], msg, sizeof(MessageHeader)) < 0) {
        usleep(1000);
    }
    if (msg->s_header.s_payload_len > 0) {
        read(process->pipe[src][dst][0], msg->s_payload, msg->s_header.s_payload_len);
    }
    increase_lamp_time(msg->s_header.s_local_time);
    return 0;
}

int receive_any(void *self, Message *msg) {
    process_t *process = self;
    local_id dst = process->id;
    int src = 0;
    while (1) {
        if (src != dst) {
            if (read(process->pipe[src][dst][0], msg, sizeof(MessageHeader)) > 0) {
                if (msg->s_header.s_payload_len > 0) {
                    read(process->pipe[src][dst][0], msg->s_payload, msg->s_header.s_payload_len);
                }
                increase_lamp_time(msg->s_header.s_local_time);
                return 0;
            }
        }
        ++src;
        if (src > process->number - 1)
            src = 0;
        usleep(1000);
    }
}
