#include <getopt.h>

#include "comm.h"
#include "opers.h"

int child_existence(process_t *process, int mutex);

process_t process;
char buffer[MAX_PAYLOAD_LEN];
size_t len;
Message msg;
int mutex;

int child_existence(process_t *process, int mutex) {
    
    close_pipes(process);

    send_STARTED(process);

    wait_STARTED(1, process);

    //HERE IS USEFUL WORK
    if (mutex == 1) {printf("%s\n", "HI");}

    //} else
  
    int N = process->id * 5;
    for (int i = 1; i < N; i++) {
        char s;
        sprintf(&s, log_loop_operation_fmt, process->id, i, N);
        print(&s);
    }
    //END OF USEFUL WORK

    send_DONE(process);

    DONE_handler(process);

    log_DONE(1, process);
    //printf("%d\n", get_lamport_time());

    close_logs(process);

    return 0;
}

int main(int argc, char *argv[]) {

    int rez = 0;
    int X = 0;

    const struct option long_options[] = {
        {"lmutex", no_argument, &mutex, 1},
        {NULL,0,NULL,0}
    };

    while ((rez = getopt_long(argc, argv, "p", long_options, NULL)) != -1) {
        switch (rez){
            case 'p':
                if (isNumber(argv[2]) == 1){
                    X = strtol(argv[2], NULL, 10);
                    if ((X < 2) || (X > 10)) {
                        fprintf(stderr, "incorrect number of accounts. It must be in [2;10]\n");
                        exit(1);
                    }
                }   else {
                    fprintf(stderr, "Error: '-p NUMBER_OF_ACCOUNTS' is mandatory\n");
                    exit(1);
                }
        }
    }

    process.number = X + 1;
    open_pipes(&process);

    for (int i = 1; i < process.number; ++i) {
        switch (fork()) {
            case 0:
                //I AM CHILD
                process.id = i;
                child_existence(&process, mutex);
                exit(0);
            default:
                //I AM PARENT
                continue;
        }
    }

    close_pipes(&process);

    //PARENT EXISTENCE
    wait_STARTED(0, &process);
    log_STARTED(0, &process);

    wait_DONE(&process);
    log_DONE(0, &process);

    waiting_for_children();

    close_logs(&process);

    return 0;
}
