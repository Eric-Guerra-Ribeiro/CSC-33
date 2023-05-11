#if !defined(COMMAND_H)
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LENGTH 256
#define MAX_NUM_ARGS 15

typedef struct command command;

struct command {
    char program[MAX_LENGTH];
    int argc;
    char *argv[MAX_NUM_ARGS + 1];
    char *input;
    char *output;
};

command empty_command();
command parse_cmd(char cmd_line[MAX_LENGTH]);

#endif // COMMAND_H
