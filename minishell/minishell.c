#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "command.h"

#define INPUT 0
#define OUTPUT 1

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
