#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>

#include "../globals.h"
#include "../shell_manipulation.h"

#define MAX_NUMBER_OF_ARGS 100


void run_system_command(char *command_, struct ShellVariables *sv) {

    // printf("\n\nbef bef bef bef\n\n");

    char **arguments = malloc(MAX_NUMBER_OF_ARGS * sizeof(char*));

    int i = 0;

    // printf("\n\nbef bef bef\n\n");

    arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
    strcpy(arguments[i], command_);
    i++;

    // printf("\n\nbef bef\n\n");

    char *arg = strtok(NULL, " ");
    while(arg != NULL) {
        
        // printf("\n\nbef\n\n");
        arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
        // printf("\n\nRECHED\n\n");
        strcpy(arguments[i], arg);
        arg = strtok(NULL, " ");

        i++;
    }

    arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
    arguments[i] = NULL;

    int number_of_args = i+1;

    // for(int i = 0; i < number_of_args-1; i++) {
    //     // printf("laknsf\n");
    //     printf("%s\n", arguments[i]);
    //     // printf("new new\n\n");
    // }

    int pid = fork();
    if(pid == 0) {
        int status = execvp(command_, arguments);
        if(status == -1) {
            shell_warning("command not found");
        }

        for(int i = 0; i < number_of_args; i++) {
            free(arguments[i]);
        }
        free(arguments);
    } else {
        wait(NULL);
    } 
    
}
