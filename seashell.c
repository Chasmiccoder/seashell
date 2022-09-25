/*
TODO:
Not implemented perror and errno yet
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "globals.h"
#include "commands/commands.h"

/*
Variables
username       - 32 byte char array that stores the username
shell_prompt   - prompt that gets displayed every shell iteration
hostname       - 256 byte char array that stores the hostname
cwd_path       - current working directory path

Library Usage
stdio.h   - printf(), scanf(), perror()
stdlib.h  - malloc()
unistd.h  - getlogin(), gethostname(), chdir(), getcwd()
string.h  - strcpy(), strtok(), strcmp()
errno.h   - errno

*/

void sigint_handler() {
    
}




void sigchld_handler() {
    int pid;
    int status;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        status = WIFEXITED(status);
        for(int i = 0; i < MAX_NUM_OF_BACKGROUND_PROCESSES; i++) {
            if(sv->background_process_ids[i] == pid) {
                printf("\n[%d] exited ", pid);
                if(status) {
                    printf("normally\n");
                } else {
                    printf("abnormally\n");
                }

                remove_background_process(pid);
                break;
            }
        }
    }
}


void init_signal_handlers() {
    // signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

}

void parse_for_io_redirection(char *string, char *command, char *input_file, char *output_file) {  
    char print_string[MAX_COMMAND_LEN];
    
    char *tmp_string = strtok(string, "<");
    char *input = strtok(NULL, "");
    char *tmp_string2;

    // input file not specified, meaning '<' was not given
    if(input == NULL) {
        tmp_string2 = strtok(tmp_string, ">");
        char *output = strtok(NULL, "");
        if(output == NULL) {
            // output file not specified

            clear_string(print_string);
            trim_string(print_string, tmp_string2);
            strcpy(command, print_string);
        } else {
            clear_string(print_string);
            trim_string(print_string, tmp_string2);
            strcpy(command, print_string);

            clear_string(print_string);
            trim_string(print_string, output);
            strcpy(output_file, print_string);
        }
    } else {
        clear_string(print_string);
        trim_string(print_string, tmp_string);
        strcpy(command, print_string);

        // input file specified
        tmp_string2 = strtok(input, ">"); // this is input now

        clear_string(print_string);
        trim_string(print_string, tmp_string2);
        strcpy(input_file, print_string);

        char *output = strtok(NULL, "");
        if(output != NULL) {

            clear_string(print_string);
            trim_string(print_string, output);
            strcpy(output_file, print_string);
        }
    }

    // printf("command: %s\noutput: %s\n", command, output_file);
}

int change_output_stream_to_file(char *output_file, int flag_double_greater_than_used) {
    int file;

    if(flag_double_greater_than_used == 0) {
        file = open(output_file, O_CREAT | O_WRONLY, 0644);
        // printf("NOT HERE\n");
    }
        
    else {
        // file = open(output_file, O_APPEND, 0644);
        // printf("HERE\n");
        file = open(output_file, O_CREAT | O_WRONLY, 0644);
        lseek(file, 0, SEEK_END);
    }
        

    if(file == -1) {
        shell_warning("could not open output file");
        return -1;
    }

    int original_stdout = dup(STDOUT_FILENO);
    int old = dup2(file, STDOUT_FILENO);
    return original_stdout;
}

int change_input_stream_to_file(char *input_file) {
    int file = open(input_file, O_RDONLY);
    if(file == -1) {
        shell_warning("specified file input stream does not exist");
        return -1;
    }

    int original_stdin = dup(STDIN_FILENO);
    int old = dup2(file, STDIN_FILENO);
    return original_stdin;
}

void parse_double_greater_than(char *dest_, char *source_, int *flag_double_greater_than_used) {
    clear_string(dest_);
    int i = 0;
    int j = 0;
    int n = strlen(source_);
    while(i < n) {
        if(source_[i] != '>') {
            dest_[j++] = source_[i++];
        } else {
            if(i > 0 && source_[i-1] != '>') {
                dest_[j++] = source_[i++];
            } else {
                i++;
                *flag_double_greater_than_used = 1;
            }
        }
    }
}

void spacify_tokens(char *dest_, const char *source_) {
    /*
    this function adds one space between arguments and special tokens like
    &, >, <, and more. This is to make future processing easier
    */
    // char source[MAX_COMMAND_LEN];
    // strcpy(source, source_);

    // strcpy(dest_, source_);
    // return;
    clear_string(dest_);

    char *tokens[] = {">", "<", "&", "|"};
    int num_tokens = sizeof(tokens)/sizeof(tokens[0]);

    int i = 0;
    int j = 0;
    while(i < strlen(source_)) {

        // if the ith element in source is a token, then flag_token_found turns true
        int flag_token_found = 0;
        for(int j = 0; j < num_tokens; j++) {
            char tmp[5];
            // strcpy(tmp, source[i]);
            sprintf(tmp, "%c", source_[i]);
            if(strcmp(tokens[j], tmp) == 0) {
                flag_token_found = 1;
                break;
            }
        }

        if(flag_token_found) {

            if(i > 0 && source_[i-1] != ' ')
                dest_[j++] = ' ';
            
            dest_[j++] = source_[i];
            
            if(i < strlen(source_) && source_[i+1] != ' ')
                dest_[j++] = ' ';


            
        } else {
            dest_[j++] = source_[i];
        }

        i++;
    }
}

void process_command(char *raw_statement, int original_stdout, int original_stdin) {

    char statement0[MAX_COMMAND_LEN];

    trim_string(statement0, raw_statement);

    char statement[MAX_COMMAND_LEN];

    int flag_double_greater_than_used = 0;

    char statement1[MAX_COMMAND_LEN];

    parse_double_greater_than(statement1, statement0, &flag_double_greater_than_used);

    spacify_tokens(statement, statement1);

    // format_string(statement, raw_statement);
    char tmp_statement[MAX_COMMAND_LEN];

    strcpy(tmp_statement, statement);

    char command2[MAX_COMMAND_LEN];
    char input_file[MAX_COMMAND_LEN];
    char output_file[MAX_COMMAND_LEN];

    clear_string(command2);
    clear_string(input_file);
    clear_string(output_file);

    // int is_double_greater_than = 0;
    parse_for_io_redirection(tmp_statement, command2, input_file, output_file);

    char *command = strtok(command2, " ");
    char *args = strtok(NULL, "");

    // int original_stdout = STDOUT_FILENO;
    // int original_stdin = STDIN_FILENO;

    if(strlen(output_file) != 0) {
        original_stdout = change_output_stream_to_file(output_file, flag_double_greater_than_used);
    }

    if(strlen(input_file) != 0) {
        original_stdin = change_input_stream_to_file(input_file);
    }
    
    if(strcmp(command, "exit") == 0) {
        run_exit(args);
    } else if(strcmp(command, "echo") == 0) {
        run_echo(args);
    } else if(strcmp(command, "cd") == 0) {
        run_cd(args);
    } else if(strcmp(command, "pwd") == 0) {
        run_pwd(args);
    } else if(strcmp(command, "ls") == 0) {
        run_ls(args);
    } else if(strcmp(command, "history") == 0) {
        run_history(args);
    } else if(strcmp(command, "discover") == 0) {
        run_discover();
    } else if(strcmp(command, "pinfo") == 0) {
        run_pinfo();
    } else if(strcmp(command, "jobs") == 0) {
        run_jobs(args);
    } else if(strcmp(command, "sig") == 0) {
        run_sig(args);
    } else {
        run_system_command(command, args);
    }

    if(strlen(output_file) != 0 && original_stdout != -1) {
        dup2(original_stdout, STDOUT_FILENO);
    }

    if(strlen(input_file) != 0 && original_stdin != -1) {
        dup2(original_stdin, STDIN_FILENO);
    }
}

void process_statement(char *raw_statement) {
    // pipe handling happens here

    int num_pipes = 0;
    for(int i = 0; i < strlen(raw_statement); i++) {
        if(raw_statement[i] == '|') {
            num_pipes++;
        }
    }

    if(num_pipes == 0) {
        process_command(raw_statement, STDOUT_FILENO, STDIN_FILENO);
        return;
    }

    char piped_input_string[MAX_COMMAND_LEN];
    trim_string(piped_input_string, raw_statement);

    char **piped_command_list = malloc(MAX_COMMANDS_IN_ONE_LINE * sizeof(char*));

    char *piped_command = strtok(piped_input_string, "|");

    int i = 0;
    while(piped_command != NULL) {
        piped_command_list[i] = malloc(MAX_COMMAND_LEN * sizeof(char));

        char tmp[MAX_COMMAND_LEN];
        trim_string(tmp, piped_command);
        strcpy(piped_command_list[i], tmp);
        piped_command = strtok(NULL, "|");
        i++;
    }

    int len_piped_command_list = i;


    char buffer_prefix[] = "pipe_tmp_buffer";
    char **output_buffers = malloc(num_pipes * sizeof(char*));
    int fds[num_pipes];
    for(int i = 0; i < num_pipes; i++) {
        output_buffers[i] = malloc(MAX_COMMAND_LEN * sizeof(char));
        sprintf(output_buffers[i], "%s_%d", buffer_prefix, i);
        fds[i] = open(output_buffers[i], O_CREAT | O_RDWR, 0644);
    }

    int commands_done = 0;
    
    // first command
    int original_stdout = dup(STDOUT_FILENO);
    int old = dup2(fds[commands_done], STDOUT_FILENO);
    process_command(piped_command_list[commands_done], old, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);

    commands_done++;

    while(commands_done < num_pipes) {

        int original_stdin = dup(STDIN_FILENO);
        lseek(fds[commands_done-1], 0, SEEK_SET);
        int old2 = dup2(fds[commands_done-1], STDIN_FILENO);

        int original_stdout = dup(STDOUT_FILENO);
        int old = dup2(fds[commands_done], STDOUT_FILENO);

        process_command(piped_command_list[commands_done], old, old2);
        dup2(original_stdin, STDIN_FILENO);
        dup2(original_stdout, STDOUT_FILENO);

        commands_done++;
    }

    // last command
    int original_stdin = dup(STDIN_FILENO);
    lseek(fds[commands_done-1], 0, SEEK_SET);
    int old2 = dup2(fds[commands_done-1], STDIN_FILENO);

    process_command(piped_command_list[commands_done], STDOUT_FILENO, old2);
    dup2(original_stdin, STDIN_FILENO);

    for(int i = 0; i < num_pipes; i++) {
        close(fds[i]);
        remove(output_buffers[i]);
        free(output_buffers[i]);
    }
    free(output_buffers);

    for(int i = 0; i < len_piped_command_list; i++) {
        free(piped_command_list[i]);
    }
    free(piped_command_list);
}

void process_input(char *input_string) {
    add_command_to_history(input_string);

    update_history_db();

    /*
    Handling each input line:
    First tokenize with respect to ; with strtok
    Then process the commands individually
    */

    char **command_list = malloc(MAX_COMMANDS_IN_ONE_LINE * sizeof(char*));
    char *command = strtok(input_string, ";");

    int i = 0;
    while(command != NULL) {
        command_list[i] = malloc(MAX_COMMAND_LEN * sizeof(char));
        strcpy(command_list[i], command);
        command = strtok(NULL, ";");
        i++;
    }

    for(int j = 0; j < i; j++) {
        process_statement(command_list[j]);
    }

    for(int j = 0; j < i; j++) {
        free(command_list[j]);
    }

    free(command_list);
    clear_string(input_string);
}

int main() {
    init_signal_handlers();

    printf("\n🐚 \033[31m---\033[m \033[34mWelcome to seashell!\033[m \033[31m---\033[m 🐚\n\n");

    sv = malloc(sizeof(struct ShellVariables));

    init_shell_variables();
    
    fetch_history_db();

    while(sv->loop_control) {
        print_shell_prompt();
        char input_string[MAX_COMMAND_LEN];
        clear_string(input_string);
        // printf("inp: %s\n", input_string);
        scanf("%[^\n]%*c", input_string);

        // if(strlen(input_string) == 0) {
        //     scanf("%s\n", input_string);
        //     continue;
        // }

        process_input(input_string);
        // fflush(stdin);
    }
    
    free(sv);
    printf("\n");
    return 0;
}
