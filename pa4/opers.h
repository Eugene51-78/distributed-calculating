
Message create_message(MessageType type, void* content);

void send_STARTED(process_t *process);

void send_DONE(process_t *process);

void wait_STARTED(short type, process_t *process);

void wait_DONE(process_t *process);

void DONE_handler(process_t *process);

void init_process(process_t *process);
