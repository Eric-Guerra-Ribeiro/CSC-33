#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LENGTH 256
#define MAX_NUM_ARGS 15

#define INPUT 0
#define OUTPUT 1

typedef struct command command;

struct command {
    char program[MAX_LENGTH];
    int argc;
    char *argv[MAX_NUM_ARGS + 1];
    char *input;
    char *output;
};


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


int main() {
    char cmd_line[MAX_LENGTH];
    command cmd;
    pid_t pid;
    int input_fd;
    int output_fd;

    while (true) {
        printf("$ ");
        // Reads the command
        // Null happens when EOF
        if(fgets(cmd_line, MAX_LENGTH, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd = parse_cmd(cmd_line);
        // Invalid command
        if (strcmp(cmd.program, "") == 0) {
            continue;
        }
        // Exit
        if (strcmp(cmd.program, "exit") == 0) {
            break;
        }
        // Create child process to run command
        pid = fork();
        // Shell waits for child process to finish
        if (pid != 0) {
            wait(NULL);
            // free argv
            for (int i = 0; i < cmd.argc; ++i) {
                free(cmd.argv[i]);
            }
            // free input and output string names
            if (cmd.input != NULL) {
                free(cmd.input);
            }
            if (cmd.output != NULL) {
                free(cmd.output);
            }
        }
        // Child process runs the command
        else {
            // TODO close files
            // Redirecting input
            if (cmd.input != NULL) {
                input_fd = open(cmd.input, O_RDONLY);
                if (dup2(input_fd, INPUT) == -1) {
                    close(input_fd);
                    printf("Error redirecting input.\n");
                    exit(0);
                }
                close(input_fd);
            }
            // Redirecting output
            if (cmd.output != NULL) {
                output_fd = open(cmd.output, O_WRONLY);
                if (dup2(output_fd, OUTPUT) == -1) {
                    close(output_fd);
                    printf("Error redirecting output.\n");
                    exit(0);
                }
                close(output_fd);
            }
            // Executing program
            if (execv(cmd.program, cmd.argv) == -1) {
                printf("Error: command not found.\n");
                exit(0);
            }
        }
    };

    return 0;
}
