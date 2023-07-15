#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "source/process.h"

#define MAX_LENGTH 256

int main() {
    char cmd_line[MAX_LENGTH];
    process* proc;
    pid_t pid;
    while (true) {
        printf("$ ");
        // Reads the command
        // Null happens when EOF
        if(fgets(cmd_line, MAX_LENGTH, stdin) == NULL) {
            printf("\n");
            break;
        }
        proc = parse_cmd(cmd_line);
        // Invalid command
        if (proc == NULL) {
            // Read next command
            continue;
        }
        // Exit
        if (strcmp(proc->argv[0], "exit") == 0) {
            // free
            free_proc(proc);
            // exit shell
            exit(0);
        }
        // Create child process to run command
        pid = fork();
        // Shell waits for child process to finish
        if (pid != 0) {
            wait(NULL);
            // free
            free_proc(proc);
        }
        // Child process runs the command
        else {
            while (proc != NULL) {
                pid = fork();
                // Child process runs the request process
                if (pid == 0) {
                    // Redirecting input
                    if (proc->infile != STDIN_FILENO) {
                        dup2(proc->infile, STDIN_FILENO);
                        close(proc->infile);
                    }
                    // Redirecting output
                    if (proc->outfile != STDOUT_FILENO) {
                        dup2(proc->outfile, STDOUT_FILENO);
                        close(proc->outfile);
                    }
                    // Executing program
                    if (execv(proc->argv[0], proc->argv) == -1) {
                        printf("Error: command not found.\n");
                        exit(0);
                    }
                }
                // Father process goes to the next
                else {
                    wait(NULL);
                    proc = proc->next;
                }
            }
            exit(0);
        }
    };

    return 0;
}
