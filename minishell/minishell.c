#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "source/process.h"


int main() {
    char cmd_line[MAX_LENGTH];
    process proc;
    pid_t pid;

    while (true) {
        printf("$ ");
        // Reads the command
        // Null happens when EOF
        if(fgets(cmd_line, MAX_LENGTH, stdin) == NULL) {
            printf("\n");
            break;
        }
        proc = parse_proc(cmd_line);
        // Invalid command
        if (strcmp(proc.program, "") == 0) {
            continue;
        }
        // Exit
        if (strcmp(proc.program, "exit") == 0) {
            exit(0);
        }
        // Create child process to run command
        pid = fork();
        // Shell waits for child process to finish
        if (pid != 0) {
            wait(NULL);
            // free argv
            for (int i = 0; i < proc.argc; ++i) {
                free(proc.argv[i]);
            }
        }
        // Child process runs the command
        else {
            // Redirecting input
            if (proc.infile != STDIN_FILENO) {
                dup2(proc.infile, STDIN_FILENO);
                close(proc.infile);
            }
            // Redirecting output
            if (proc.outfile != STDOUT_FILENO) {
                dup2(proc.outfile, STDOUT_FILENO);
            }
            // Executing program
            if (execv(proc.program, proc.argv) == -1) {
                printf("Error: command not found.\n");
                exit(0);
            }
        }
    };

    return 0;
}
