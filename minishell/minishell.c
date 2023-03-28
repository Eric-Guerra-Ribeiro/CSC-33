#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LENGTH 256
#define MAX_NUM_ARGS 16

typedef struct command command;

struct command {
    char program[MAX_LENGTH];
    int argc;
    char argv[MAX_NUM_ARGS][MAX_LENGTH];
    char input[MAX_LENGTH];
    char output[MAX_LENGTH];
};


command empty_command() {
    command cmd;
    strcpy(cmd.program, "");
    cmd.argc = 0;
    strcpy(cmd.input, "");
    strcpy(cmd.output, "");
}


command parse_cmd(char cmd_line[MAX_LENGTH]) {
    command cmd;
    char *word;
    bool start_read_args = false;
    bool finish_read_args = false;
    bool read_in = false;
    bool read_out = false;

    word = strtok(cmd_line, " \n");
    cmd.argc = 0;
    // Empty command
    if (word == NULL) {    
        return empty_command();
    }
    // Non-empty command
    strcpy(cmd.program, word);
    word = strtok(NULL, " \n");
    while (word != NULL) {
        // Reading input redirect
        if (!read_in && strcmp(word, "<") == 0) {
            // Checking if has read the arguments
            finish_read_args = start_read_args;
            // Reading input file
            read_in = true;
            word = strtok(NULL, " \n");
            // No input file given
            if (word == NULL) {
                printf("Error: No input file.\n");
                return empty_command();
            }
            strcpy(cmd.input, word);
        }
        // Reading output redirect
        else if (!read_out && strcmp(word, ">")) {
            // Checking if the arguments have been read
            finish_read_args = start_read_args;
            // Reading output file
            read_out = true;
            word = strtok(NULL, " \n");
            if (word == NULL) {
                // No output file given
                printf("Error: No output file.\n");
                return empty_command();
            }
            strcpy(cmd.output, word);
        }
        // Read arguments
        else if (!start_read_args || !finish_read_args) {
            start_read_args = true;
            strcpy(cmd.argv[cmd.argc++], word);
        }
        // Extra arguments are ignored
        else {
            break;
        }
        word = strtok(NULL, " \n");
    }
    return cmd;
}

int main() {
    char cmd_line[MAX_LENGTH];
    command cmd;
    do {
        // Reads the command line
        fgets(cmd_line, MAX_LENGTH, stdin);
        // TODO Finish shell logic
    } while (1);

    return 0;
}
