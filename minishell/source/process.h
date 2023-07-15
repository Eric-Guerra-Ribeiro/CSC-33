#if !defined(PROCESS_H)
#define PROCESS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_LENGTH 256
#define MAX_NUM_ARGS 15

typedef struct process process;

struct process {
    char program[MAX_LENGTH];
    int argc;
    char *argv[MAX_NUM_ARGS + 1];
    int infile;
    int outfile;
};

process empty_command();
process parse_proc(char cmd_line[MAX_LENGTH]);

#endif // PROCESS_H
