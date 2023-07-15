#if !defined(PROCESS_H)
#define PROCESS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_NUM_ARGS 15

typedef struct process process;

struct process {
    pid_t pid;
    int argc;
    char *argv[MAX_NUM_ARGS + 1];
    int infile;
    int outfile;
    process* next;
};

process* empty_process();
process* parse_cmd(char* cmd_line);
process* parse_process(char* process_line);
void free_proc(process* first_proc);

#endif // PROCESS_H
