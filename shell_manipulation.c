#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "globals.h"
#include "datastructures.h"

void init_shell_variables() {
    /* --- Fetch the username and the hostname --- */
    sv->username = malloc(MAX_USERNAME_LEN * sizeof(char));
    sv->username = getlogin();

    sv->hostname = malloc(MAX_HOSTNAME_LEN * sizeof(char));
    gethostname(sv->hostname, MAX_HOSTNAME_LEN);

    // start at the root directory by default (where the shell is stored)
    sv->cwd_path = malloc(MAX_PATH_LEN * sizeof(char));
    getcwd(sv->cwd_path, MAX_PATH_LEN);

    sv->prev_wd_path = malloc(MAX_PATH_LEN * sizeof(int));
    sv->prev_wd_path[0] = '\0';

    sv->home_path = malloc(MAX_PATH_LEN * sizeof(int));
    strcpy(sv->home_path, sv->cwd_path);

    sv->loop_control = 1;

    sv->command_buffer = malloc(sizeof(struct queue*));
    init_queue(sv->command_buffer);

    sv->background_process_ids = malloc(MAX_NUM_OF_BACKGROUND_PROCESSES * sizeof(int));
    sv->background_process_assigned_ids = malloc(MAX_NUM_OF_BACKGROUND_PROCESSES * sizeof(int));
    sv->background_process_names = malloc(MAX_NUM_OF_BACKGROUND_PROCESSES * sizeof(char*));
    sv->background_process_status = malloc(MAX_NUM_OF_BACKGROUND_PROCESSES * sizeof(char*));
    for(int i = 0; i < MAX_NUM_OF_BACKGROUND_PROCESSES; i++) {
        sv->background_process_names[i] = malloc(MAX_PATH_LEN * sizeof(char));
        sv->background_process_status[i] = malloc(STATUS_SIZE * sizeof(char));
        sv->background_process_ids[i] = -1;
        sv->background_process_assigned_ids[i] = -1;
        clear_string(sv->background_process_names[i]);
        clear_string(sv->background_process_status[i]);
    }
    sv->background_process_count = 0;
    sv->num_background_processes = 0;
}

void add_background_process(int pid, int assigned_id, char *name, char *status) {
    for(int i = 0; i < MAX_NUM_OF_BACKGROUND_PROCESSES; i++) {
        if(sv->background_process_ids[i] == -1) {
            sv->background_process_ids[i] = pid;
            sv->background_process_assigned_ids[i] = assigned_id;
            strcpy(sv->background_process_names[i], name);
            strcpy(sv->background_process_status[i], status);
            sv->num_background_processes++;
            sv->background_process_count++;
            return;
        }
    }
    shell_warning("background process limit reached!");
}

void remove_background_process(int pid) {
    int i = 0;
    while(i < MAX_NUM_OF_BACKGROUND_PROCESSES) {
        if(sv->background_process_ids[i] == pid) {
            sv->background_process_ids[i] = -1;
            sv->background_process_assigned_ids[i] = -1;
            clear_string(sv->background_process_names[i]);
            clear_string(sv->background_process_status[i]);
            sv->num_background_processes--;
            return;
        }
        i++;
    }
}


void print_shell_prompt() {
    /*
    Prints the shell prompt with the format,
    <username@system: ~ >
    We're using ANSI color codes to customize the terminal
    */
    
    char *shell_prompt = malloc(MAX_SHELL_PROMPT_LEN * sizeof(char));

    char *cwd = malloc(MAX_PATH_LEN * sizeof(char));

    // if the home path is a substring of the current path, replace the substring with '~/'
    if(is_substring(sv->home_path, sv->cwd_path)) {
        int j = 0;
        cwd[j++] = '~';
        cwd[j++] = '/';
        for(int i = strlen(sv->home_path)+1; i < strlen(sv->cwd_path); i++) {
            cwd[j++] = sv->cwd_path[i];
        }
        cwd[j] = '\0';
    } else {
        strcpy(cwd, sv->cwd_path);
    }

    sprintf(shell_prompt, "\033[37m<\033[m\033[31m%s\033[m\033[37m@\033[m\033[34m%s\033[m\033[37m: %s >\033[m ", sv->username, sv->hostname, cwd);
    printf("%s",shell_prompt);

    free(cwd);
    free(shell_prompt);
}

void shell_warning(const char *message) {
    char *buffer = malloc((strlen(message)+50) * sizeof(char));
    strcpy(buffer, "\n🐚 \033[33mseashell:\033[m ");
    strcat(buffer, message);
    strcat(buffer, "\n\n");
    printf("%s", buffer);
    // perror(buffer);
    free(buffer);
}
