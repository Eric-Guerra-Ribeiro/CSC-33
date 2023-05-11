#include "command.h"

command empty_command() {
    command cmd;
    strcpy(cmd.program, "");
    cmd.argc = 0;
    cmd.input = NULL;
    cmd.output = NULL;
    return cmd;
}


command parse_cmd(char cmd_line[MAX_LENGTH]) {
    command cmd;
    char *word;
    bool start_read_args = false;
    bool finish_read_args = false;
    bool read_in = false;
    bool read_out = false;

    word = strtok(cmd_line, " \n");
    cmd.argc = 1;
    // Empty command
    if (word == NULL) {
        return empty_command();
    }
    // Non-empty command
    strcpy(cmd.program, word);
    cmd.argv[0] = (char *) malloc(sizeof(char)*(strlen(word) + 1));
    strcpy(cmd.argv[0], word);
    word = strtok(NULL, " \n");
    while (word != NULL) {
        // Reading input redirect
        if (strcmp(word, "<") == 0) {
            // Checking if the arguments have been read
            finish_read_args = start_read_args;
            // More than one input file
            if (read_in) {
                printf("Error: More than one input file.\n");
                return empty_command();
            }
            // Reading input file
            read_in = true;
            word = strtok(NULL, " \n");
            // No input file given
            if (word == NULL) {
                printf("Error: No input file.\n");
                return empty_command();
            }
            cmd.input = (char *) malloc(sizeof(char *)*(strlen(word) + 1));
            strcpy(cmd.input, word);
        }
        // Reading output redirect
        else if (strcmp(word, ">") == 0) {
            // Checking if the arguments have been read
            finish_read_args = start_read_args;
            // More than one output file
            if (read_out) {
                printf("Error: More than one output file.\n");
                return empty_command();
            }
            // Reading output file
            read_out = true;
            word = strtok(NULL, " \n");
            if (word == NULL) {
                // No output file given
                printf("Error: No output file.\n");
                return empty_command();
            }
            cmd.output = (char *) malloc(sizeof(char *)*(strlen(word) + 1));
            strcpy(cmd.output, word);
        }
        // Read arguments
        else if (!finish_read_args) {
            start_read_args = true;
            cmd.argv[cmd.argc] = (char *) malloc(sizeof(char)*(strlen(word) + 1));
            strcpy(cmd.argv[cmd.argc++], word);
        }
        // Extra arguments are ignored
        else {
            printf("Warning: extra arguments were ignored.\n");
            break;
        }
        word = strtok(NULL, " \n");
    }
    // Standard I/O if input or output are NULL
    if (!read_in) {
        cmd.input = NULL;
    }
    if (!read_out) {
        cmd.output = NULL;
    }
    // End argv with NULLs
    cmd.argv[cmd.argc] = NULL;
    return cmd;
}
