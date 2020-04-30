#include "banking.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "stdio.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#include "ipc.h"
#include "logging.h"
#include "comm.h"
#include "connect.h"
#include "banking.h"

void transfer(void * parent_data, local_id src, local_id dst,
              balance_t amount)
{
    // student, please implement me
}

int isNumber(char* String) {
    char str = *String;
    if (str >= '0' && str <= '9') return 1;
    else return 0;
}

AllHistory getAllHistory(int process_count, process_t * children){
    AllHistory allHistory;
    allHistory.s_history_len = process_count;
    for (int j = 0; j < process_count; ++j) {
        allHistory.s_history[j] = children[j].balance_history;
        }

    return allHistory;
}

int main(int argc, char * argv[])
{

    //bank_robbery(parent_data);
    //print_history(all);

    int rez = 0;
    int X = 0;
    while ((rez = getopt(argc, argv, "p")) != -1) {
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
    balance_t balance[X];
    for (int i = 0; i < X; ++i) {
        if (isNumber(argv[3+i]) == 1){
            balance[i] = strtol(argv[3+i], NULL, 10);
            if ((balance[i] < 1) || (balance[i] > 99)){
                fprintf(stderr, "Incorrect value of balance. It must be in [1;99]\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Error: '-p NUMBER_OF_ACCOUNTS BALANCE [BALANCE]' is mandatory\n");
            exit(1);
        }
    }

    local_id process_num = X + 1;

    process_t process;

    start_log();

    init_parent(&process, process_num);

    // Allocate memory for pipes
    allocate_pipes(&process);

    // Creating pipes
    create_pipes(&process);

    // fork
    process_t* children = create_children(&process, balance, process_num - 1 );

    char * buffer = NULL;

    close_pipes(&process);

    if (process.cur_id != PARENT_ID) {
        child_work(&process, buffer);
    } else parent_work(&process);

    AllHistory allHistory = getAllHistory(X, children);
    AllHistory* allHistoryPtr = &allHistory;

    print_history(allHistoryPtr);

    free(buffer);

    exit(0);
}
