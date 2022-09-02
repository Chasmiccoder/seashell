/*
TODO:
Not implemented perror and errno yet
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "globals.h"
#include "utils.h"
#include "shell_manipulation.h"
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


void process_statement(struct ShellVariables *sv, const char *raw_statement) {

    char *statement = malloc((sizeof(raw_statement) + 1) * sizeof(char));
    format_string(statement, raw_statement);
    
    char *command = strtok(statement, " ");

    if(strcmp(command, "exit") == 0) {
        run_exit();
    } else if(strcmp(command, "echo") == 0) {
        run_echo();
    } else if(strcmp(command, "cd") == 0) {
        run_cd(sv);
    } else if(strcmp(command, "pwd") == 0) {
        run_pwd(sv);
    } else if(strcmp(command, "clear") == 0) {
        run_clear();
    } else {
        shell_warning("command not found");
    }
}

void process_input(struct ShellVariables *sv, char *input_string) {
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
        process_statement(sv, command_list[j]);
    }

    free(command_list);
}

int main() {

    printf("\n\033[31m---\033[m \033[34mWelcome to seashell!\033[m \033[31m---\033[m\n\n");

    struct ShellVariables *sv = malloc(sizeof(struct ShellVariables));
    init_shell_variables(sv);

    while(sv->loop_control) {
        print_shell_prompt(sv);

        char *input_string = malloc(MAX_COMMAND_LEN * sizeof(char));
        scanf("%[^\n]%*c", input_string);

        process_input(sv, input_string);
    }
    
    free(sv);
    printf("\n");
    return 0;
}
