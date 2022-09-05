#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#include "../globals.h"
#include "../shell_manipulation.h"

#define MAX_NUMBER_OF_ARGS 100

// int global_number_of_processes = 1;


void run_system_command(char *command_, struct ShellVariables *sv) {

    char **arguments = malloc(MAX_NUMBER_OF_ARGS * sizeof(char*));
    int i = 0;

    arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
    strcpy(arguments[i], command_);
    i++;

    char *arg = strtok(NULL, " ");
    while(arg != NULL) {
        
        if(strcmp(arg, "&") == 0) {

        }

        arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
        strcpy(arguments[i], arg);
        arg = strtok(NULL, " ");

        i++;
    }

    int is_background_process = 0;
    if(i != 0 && strcmp(arguments[i-1], "&") == 0) {
        is_background_process = 1;
    }

    arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
    arguments[i] = NULL;

    int number_of_args = i+1;

    time_t start_seconds = time(NULL);

    // if(global_number_of_processes > 5) {
    //     return;
    // }

    if(is_background_process) {
        // global_number_of_processes++;
        // printf("GLOB 0: %d\n", global_number_of_processes);
        int pid = fork();
        if(pid == 0) {
            int pid2 = fork();
            if(pid2 == 0) {
                int status = execvp(command_, arguments);
                if(status == -1) {
                    shell_warning("command not found");
                }
                exit(0);
                // kill(getpid(), SIGINT);
            } else {
                wait(NULL);

                // printf("\nDone!\n");
                printf("\n%s with pid [%d] exited normally\n", command_, getpid());

                print_shell_prompt(sv);
                // return;
                exit(0);
                // kill(getpid(),SIGINT);
                // printf("killed 1\n");
            }
        }

    }
    
    // if the process is a foreground process
    else {
        // global_number_of_processes++;
        // printf("GLOB 1: %d\n", global_number_of_processes);
        int pid = fork();
        if(pid == 0) {
            int status = execvp(command_, arguments);
            if(status == -1) {
                shell_warning("command not found");
            }

            // kill(getpid(),SIGINT);
            // exit(0);

        } else {
            // if it is a foreground process, wait for the child process to terminate
            wait(NULL);
        
            time_t end_seconds = time(NULL);
            int execution_time = end_seconds - start_seconds;

            if(execution_time >= 1) {
                printf("Took %d seconds\n", execution_time);
            }

            // kill(getpid(),SIGINT);
            // kill(pid, SIGTERM);
        }
    }

    for(int i = 0; i < number_of_args; i++) {
        free(arguments[i]);
    }
    free(arguments);
}
