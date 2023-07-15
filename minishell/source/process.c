#include "process.h"

process* parse_cmd(char* cmd_line) {
    process* first_proc;
    process* proc;
    process* prev_proc;
    char *word;
    char* process_words[MAX_PIPE_DEPTH];
    int pipe_depth = 0;
    // Parse command into pipelines of processes
    word = strtok(cmd_line, "|");
    while (word != NULL) {
        process_words[pipe_depth] = (char *) malloc(sizeof(char)*(strlen(word) + 1));
        strcpy(process_words[pipe_depth++], word);
        word = strtok(NULL, "|");
    }
    // Parse each process in pipeline
    if (pipe_depth > 0) {
        first_proc = parse_process(process_words[0]);
        prev_proc = first_proc;
        proc = first_proc;
        for (int i = 1; i < pipe_depth; ++i) {
            if (proc == NULL) {
                break;
            }
            proc = parse_process(process_words[i]);
            prev_proc->next = proc;
            prev_proc = proc;
        }
    }
    else {
        first_proc = NULL;
    }
    // Free process words
    for (int i = 0; i < pipe_depth; ++i) {
        free(process_words[i]);
    }
    return first_proc;
}


process* parse_process(char* process_str) {
    char* word;
    bool start_read_args = false;
    bool finish_read_args = false;
    bool read_in = false;
    bool read_out = false;

    word = strtok(process_str, " \n");
    // Null process
    if (word == NULL) {
        return NULL;
    }
    // Non-null process
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
                return NULL;
            }
            // Reading input file
            read_in = true;
            word = strtok(NULL, " \n");
            // No input file given
            if (word == NULL) {
                printf("Error: No input file.\n");
                return NULL;
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
                return NULL;
            }
            // Reading output file
            read_out = true;
            word = strtok(NULL, " \n");
            if (word == NULL) {
                // No output file given
                printf("Error: No output file.\n");
                return NULL;
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
    process* proc;
    process* prev_proc;
    proc = first_proc;
    while (proc != NULL) {
        prev_proc = proc;
        // free argvs
        for (int i = 0; i < prev_proc->argc; ++i) {
            free(prev_proc->argv[i]);
        }
        // free process struct
        free(prev_proc);

        proc = prev_proc->next;
    }
}
