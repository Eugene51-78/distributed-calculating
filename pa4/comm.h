#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "ipc.h"
#include "pa4.h"

static const char * const open_fmt =
    "OPEN pipe from %d to %d\n";
static const char * const close_fmt =
    "process %d CLOSED pipe from %d to %d %s\n";

typedef struct {
	int pipe[10][10][2];
	local_id number;
	local_id id;
} process_t;

int open_pipes(process_t *process);

int close_pipes(process_t *process);

void LOG(process_t *process, short p_flag, char *buffer, int len);

void close_logs(process_t *process);

void log_STARTED(short flag, process_t *process);

void log_DONE(short flag, process_t *process);

int get_lamport_time();

void increase_lamp_time(int time);

int isNumber(char* String);

void waiting_for_children();
