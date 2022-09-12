#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#include "../globals.h"
#include "../shell_manipulation.h"

#define MAX_NUMBER_OF_BACKGROUND_PROCESSES 15

int global_number_of_processes = 0;
char *global_background_pids[MAX_NUMBER_OF_BACKGROUND_PROCESSES];

void signal_handler(int sig) {
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) ;
    
    printf("\n[%d] exited ", global_number_of_processes);

    if(status == 0) {
        printf("normally\n\n");
    } else {
        printf("abnormally\n\n");
    }
    global_number_of_processes--;
}

void run_system_command(char *command_, struct ShellVariables *sv) {

    char **arguments = malloc(MAX_NUMBER_OF_ARGS * sizeof(char*));
    int i = 0;

    arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
    strcpy(arguments[i], command_);
    i++;

    int is_background_process = 0;

    char *arg = strtok(NULL, " ");
    while(arg != NULL) {
        
        if(strcmp(arg, "&") == 0) {
            is_background_process = 1;
            arg = strtok(NULL, " ");
            continue;
        }

        arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
        strcpy(arguments[i], arg);
        arg = strtok(NULL, " ");

        i++;
    }

    arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
    arguments[i] = NULL;

    int number_of_args = i+1;

    time_t start_seconds = time(NULL);

    if(is_background_process) {
        global_number_of_processes++;
        int pid = fork();
        int fork_status = -1;
        if(pid == 0) {
            
            // global_number_of_processes++;
            printf("\nBackground Process [%d]: %s (%d) initiated\n", global_number_of_processes, command_, getpid());
            int status = execvp(command_, arguments);
            if(status == -1) {
                shell_warning("command not found");
            }
            exit(0);

        } else {

            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_RESTART;
            sa.sa_handler = signal_handler;

            sigaction(SIGCHLD, &sa, NULL);
        }
    }
    
    // if the process is a foreground process
    else {
        int pid = fork();
        if(pid == 0) {
            int status = execvp(command_, arguments);
            if(status == -1) {
                shell_warning("command not found");
            }

        } else {
            // if it is a foreground process, wait for the child process to terminate
            wait(NULL);
        
            time_t end_seconds = time(NULL);
            int execution_time = end_seconds - start_seconds;

            if(execution_time >= 1) {
                printf("Took %d seconds\n", execution_time);
            }
        }
    }

    for(int i = 0; i < number_of_args; i++) {
        free(arguments[i]);
    }
    free(arguments);
}
