#include "process.h"

process* empty_process() {
    process* proc = (process *) malloc(sizeof(process));
    proc->pid = (pid_t) 0;
    proc->argc = 0;
    proc->infile = STDIN_FILENO;
    proc->outfile = STDOUT_FILENO;
    proc->argv[proc->argc] = (char *) malloc(sizeof(char)*(1));
    strcpy(proc->argv[proc->argc++], "");
    proc->next = NULL;
    return proc;
}


process* parse_cmd(char* cmd_line) {
    return parse_process(cmd_line);
}

process* parse_process(char* process_line) {
    char* word;
    bool start_read_args = false;
    bool finish_read_args = false;
    bool read_in = false;
    bool read_out = false;

    word = strtok(process_line, " \n");
    // Empty process
    if (word == NULL) {
        return empty_process();
    }
    // Non-empty process
    process* proc = (process *) malloc(sizeof(process));
    proc->argc = 1;
    proc->pid = (pid_t) 0;
    proc->argv[0] = (char *) malloc(sizeof(char)*(strlen(word) + 1));
    strcpy(proc->argv[0], word);
    word = strtok(NULL, " \n");
    while (word != NULL) {
        // Reading input redirect
        if (strcmp(word, "<") == 0) {
            // Checking if the arguments have been read
            finish_read_args = start_read_args;
            // More than one input file
            if (read_in) {
                printf("Error: More than one input file.\n");
                return empty_process();
            }
            // Reading input file
            read_in = true;
            word = strtok(NULL, " \n");
            // No input file given
            if (word == NULL) {
                printf("Error: No input file.\n");
                return empty_process();
            }
            proc->infile = open(word, O_RDONLY);
        }
        // Reading output redirect
        else if (strcmp(word, ">") == 0) {
            // Checking if the arguments have been read
            finish_read_args = start_read_args;
            // More than one output file
            if (read_out) {
                printf("Error: More than one output file.\n");
                return empty_process();
            }
            // Reading output file
            read_out = true;
            word = strtok(NULL, " \n");
            if (word == NULL) {
                // No output file given
                printf("Error: No output file.\n");
                return empty_process();
            }
            proc->outfile = open(word, O_WRONLY);
        }
        // Read arguments
        else if (!finish_read_args) {
            start_read_args = true;
            proc->argv[proc->argc] = (char *) malloc(sizeof(char)*(strlen(word) + 1));
            strcpy(proc->argv[proc->argc++], word);
        }
        // Extra arguments are ignored
        else {
            printf("Warning: extra arguments were ignored.\n");
            break;
        }
        word = strtok(NULL, " \n");
    }
    // Standard I/O if input or output were read
    if (!read_in) {
        proc->infile = STDIN_FILENO;
    }
    if (!read_out) {
        proc->outfile = STDOUT_FILENO;
    }
    // End argv with NULLs
    proc->argv[proc->argc] = NULL;
    // Set next process
    proc->next = NULL;
    return proc;
}

void free_proc(process* first_proc) {
    // free argvs
    for (process* proc = first_proc; proc != NULL; proc = proc->next) {
        for (int i = 0; i < proc->argc; ++i) {
            free(proc->argv[i]);
        }
    }
    // free process structs
    for (process* proc = first_proc; proc != NULL; proc = proc->next) {
        free(proc);
    }
}
