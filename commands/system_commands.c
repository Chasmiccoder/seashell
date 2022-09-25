#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#include "../globals.h"

// void run_system_command(char *command, const char *args, struct ShellVariables *sv) {
void run_system_command(char *command, const char *args) {
    char **arguments = malloc(MAX_NUMBER_OF_ARGS * sizeof(char*));
    int i = 0;

    arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
    strcpy(arguments[i], command);
    i++;

    int is_background_process = 0;
    char args_modifiable[MAX_COMMAND_LEN];
    char *arg;

    if(args != NULL) {
        strcpy(args_modifiable, args);
        arg = strtok(args_modifiable, " ");
    } else {
        arg = NULL;
    }

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
        is_background_process = 0;

        int pid = fork();
        
        if(pid < 0) {
            shell_warning("fork failed");
            return;
        }

        if(pid == 0) {
            setpgid(0, 0);

            int bg_process_id = getpid();
            printf("\nBackground Process [%d]: %s (%d) initiated\n", sv->background_process_count, command, bg_process_id);


            int status = execvp(command, arguments);
            if(status == -1) {
                shell_warning("command not found");
            }
            exit(0);
        } else {
            
            // insertChild(pid, args[0]);

            // add the process to the Background Process Table
            // sv->background_process_ids[sv->num_background_processes] = pid;
            // strcpy(sv->background_process_names[sv->num_background_processes], command);
            // strcpy(sv->background_process_status[sv->num_background_processes], "Running");
            // sv->background_process_assigned_ids[sv->num_background_processes] = sv->num_background_processes;
            // sv->num_background_processes++;
            
            char process_name[MAX_COMMAND_LEN];
            strcpy(process_name, command);
            strcat(process_name, " ");
            strcat(process_name, args);
            add_background_process(pid, sv->background_process_count, process_name, "Running");



            // printf("Yo:\nid: %d\nname: %s\nnum bg: %d\n", sv->background_process_ids[sv->num_background_processes-1], sv->background_process_names[sv->num_background_processes-1], sv->num_background_processes);



            // wait(NULL);


            
                
            // printf("\n[%d] exited\n", background_process_id);
            // kill(background_process_id, SIGKILL);
        }



        ////////////////

        /*
        int pid = fork();
        int fork_status = -1;
        if(pid == 0) {
            int background_process_id = getpid();
            // send this background process id to the parent (root process)
            // close(pipefd[0]);
            // write(pipefd[1], background_process_id, 5);
            // char *test_str = "test string\0";
            // printf("Writing: %s\n", test_str);
            // write(pipefd[1], test_str, strlen(test_str));

            int my_pipe[2];
            if(pipe(my_pipe) == -1) {
                shell_warning("pipe creation failed");
                return;
            }

            int pid2 = fork();
            if(pid2 == 0) {
                printf("\nBackground Process [%d]: %s (%d) initiated\n", global_number_of_processes, command, background_process_id);

                // char transfer[MAX_COMMAND_LEN] = "test string!";
                // sprintf(transfer, "%s", background_process_id);

                // write(my_pipe[1], transfer, MAX_COMMAND_LEN);

                int status = execvp(command, arguments);
                if(status == -1) {
                    shell_warning("command not found");
                }
                exit(0);

            } else {
                wait(NULL);
                // char in_buff[MAX_COMMAND_LEN];
                // int nbytes = read(my_pipe[0], in_buff, MAX_COMMAND_LEN);

                // printf("Fetched BG PID!!: %s\n", in_buff);

                // wait(NULL);

                
                
                printf("\n[%d] exited\n", background_process_id);
                // kill(background_process_id, SIGKILL);
            }
            

            // global_number_of_processes++;
            // printf("\nBackground Process [%d]: %s (%d) initiated\n", global_number_of_processes, command, getpid());
            // int status = execvp(command, arguments);
            // if(status == -1) {
            //     shell_warning("command not found");
            // }
            // exit(0);


            */

           //////////////

        // } else {
            // struct sigaction sa;
            // sigemptyset(&sa.sa_mask);
            // sa.sa_flags = SA_RESTART;
            // sa.sa_handler = signal_handler;

            // sigaction(SIGCHLD, &sa, NULL);

            // close(pipefd[1]);
            // int background_process_id = 0;
            // read(pipefd[0], &background_process_id, 1);
            
            // global_background_pids[global_number_of_processes] = background_process_id;
            // global_number_of_processes++;
            // struct sigaction sa;
            // sigemptyset(&sa.sa_mask);
            // sa.sa_flags = SA_RESTART;
            // sa.sa_handler = signal_handler;

            // sigaction(SIGCHLD, &sa, NULL);
        // }
    }
    
    // if the process is a foreground process
    else {
        int pid = fork();
        if(pid == 0) {
            int status = execvp(command, arguments);
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
