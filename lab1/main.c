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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#include "ipc.h"
#include "logging.h"
#include "comm.h"
#include "connect.h"
    
int isNumber(char* String) {
    char str = *String;
    if (str >= '0' && str <= '9') return 1; 
    else return 0;
}

int main(int argc, char *argv[]) {
    int rez = 0;
    int X = 0;
    while ((rez = getopt(argc, argv, "p")) != -1) {
    switch (rez){
        case 'p': 
                if (argc == 3 && (isNumber(argv[2]) == 1)){
                    X = strtol(argv[2], NULL, 10);
                    if (X > 9){
                        fprintf(stderr, "Too many children. Up to 9 subprocesses are allowed.\n");
                        exit(1);
                    }
                }   else {
                    fprintf(stderr, "Error: '-p NUMBER_OF_CHILDREN' is mandatory\n");
                    exit(1);
                    }
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
    char * buffer = create_children(&process);

    close_pipes(&process);

    if (process.cur_id != PARENT_ID) {
        child_work(&process, buffer);
    } else parent_work(&process);

    free(buffer);

    exit(0);
}
