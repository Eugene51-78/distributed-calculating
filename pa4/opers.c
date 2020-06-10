#include "comm.h"
#include "opers.h"


Message msg;
size_t len;
char buffer[MAX_PAYLOAD_LEN];

Message create_message(MessageType type, void* content) {
    Message msg;
    msg.s_header.s_magic = MESSAGE_MAGIC;
    msg.s_header.s_type = type;
    msg.s_header.s_local_time = get_lamport_time();

    char* text = (char*) content;
    int sz = strlen(text);
    msg.s_header.s_payload_len = sz;
    memcpy(&(msg.s_payload), content, sz);

    return msg;
}

int isNumber(char* String) {
    char str = *String;
    if (str >= '0' && str <= '9') return 1; 
    else return 0;
}

void waiting_for_children() {
    local_id pid;
    do pid = wait(NULL);
    while (pid != -1);
}

void send_STARTED(process_t *process) {
    increase_lamp_time(0);
    len = sprintf(buffer, log_started_fmt, get_lamport_time(), process->id, getpid(), getppid(), process->id);
    LOG(process, 0, buffer, len);
    msg = create_message(STARTED, buffer);
    send_multicast(process, &msg);
}

void send_DONE(process_t *process) {
    increase_lamp_time(0);
    len = sprintf(buffer, log_done_fmt, get_lamport_time(), process->id, process->id);
    msg = create_message(DONE, buffer);
    send_multicast(process, &msg);
    LOG(process, 0, buffer, len);
}

void wait_STARTED(short type, process_t *process) {
    int started_counter = 0;
    while (started_counter < process->number - 1) {
        if (type == 0) {
            for (int i = 1; i < process->number; i++) {
                receive(process, i, &msg);
                if (msg.s_header.s_type == STARTED) started_counter++;
            }
        } else {
            for (int i = 1; i < process->number; ++i) {
                if (i != process->id) 
                    receive(process, i, &msg);
                if (msg.s_header.s_type == STARTED) started_counter++;
            }
            log_STARTED(1, process);
        }
    }
}

void wait_DONE(process_t *process) {
    int done_counter = 0;
    while (done_counter < process->number - 1) {
        for (int i = 1; i < process->number; i++) {
            receive(process, i, &msg);
            if (msg.s_header.s_type == DONE) done_counter++;
        }
    }
}

void DONE_handler(process_t *process) {
 int num = process->number - 2;
    while(num) {
        receive_any(process, &msg);
        switch(msg.s_header.s_type) {
            case DONE:
                num--;
                break;
        }
    }
}
