/*
TODO:
Not implemented perror and errno yet
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

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


#define MAX_USERNAME_LEN 32
#define MAX_HOSTNAME_LEN 256
#define MAX_SHELL_PROMPT_LEN 4096
#define MAX_PATH_LEN 2048
#define MAX_COMMAND_LEN 2048
#define MAX_COMMANDS_IN_ONE_LINE 50  // for when multiple commands are used with ';'

struct ShellVariables {
    char *username;
    char *hostname;
    char *cwd_path;      // current working directory path
    char *prev_wd_path;  // previous working directory path
    char *home_path;     // absolute path to the home directory (where this shell is stored)

    int loop_control;
};

void clear_string(char *str) {
    memset(str, '\0', strlen(str) * sizeof(char));
}

int is_substring(char *substring, char *string) {
    if(strlen(substring) > strlen(string)) {
        return 0;
    }

    for(int i = 0; i < strlen(substring); i++) {
        if(string[i] != substring[i]) {
            return 0;
        }
    }

    return 1;
}


void init_shell_variables(struct ShellVariables *sv) {
    /* --- Fetch the username and the hostname --- */
    sv->username = malloc(MAX_USERNAME_LEN * sizeof(char));
    sv->username = getlogin();

    sv->hostname = malloc(MAX_HOSTNAME_LEN * sizeof(char));
    gethostname(sv->hostname, MAX_HOSTNAME_LEN);

    // start at the root directory by default (where the shell is stored)
    sv->cwd_path = malloc(MAX_PATH_LEN * sizeof(char));
    getcwd(sv->cwd_path, MAX_PATH_LEN);

    sv->prev_wd_path = malloc(MAX_PATH_LEN * sizeof(int));
    // strcpy(sv->prev_wd_path, sv->cwd_path);
    sv->prev_wd_path[0] = '\0';

    sv->home_path = malloc(MAX_PATH_LEN * sizeof(int));
    strcpy(sv->home_path, sv->cwd_path);

    sv->loop_control = 1;
}


void print_shell_prompt(const struct ShellVariables *sv) {
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

void format_string(char *dest, const char *source) {
    /*
    Removes unnecessary spaces in a string.
    [1] Trims spaces from the left to the first character
    [2] Trims spaces from the last character onwards
    [3] Trims extra spaces in between words/arguments
    [4] If double quotes are used, it doesn't trim the spaces in between
    [5] Also removes the newline char at the end (if present)
    [6] Removes tabs '\t' if present (not inside double quotes)

    E.g.
    ` hello  world   `    -> `hello world`
    `  hello " word   !"` -> `hello " word   !"`
    */
    
    clear_string(dest);

    int i = 0; // source index
    int j = 0; // dest index

    // [1]
    while(source[i] == ' ') {
        i++;
    }

    int last_char = strlen(source)-1;

    // [2]
    for(int k = strlen(source) - 1; k >= 0; k--) {
        if(source[k] != ' ' && source[k] != '\n') {  // [5]
            last_char = k;
            break;
        }
    }

    // [3]
    int doubleQuoteFlag = 0; // flag for the double quote immunity
    int oneSpace = 0;        // gets incremented if 1 space is encountered

    while(i <= last_char) {
        if(source[i] == '"') {  // [4]
            doubleQuoteFlag = (doubleQuoteFlag == 1) ? 0 : 1; // toggle the flag
            oneSpace = 0;
        }

        if(source[i] != ' ') {
            dest[j++] = source[i++];
            oneSpace = 0;
        } else {
            if(oneSpace == 0 || doubleQuoteFlag) {
                oneSpace++;
                dest[j++] = source[i++];
            } else {
                i++;
            }
        }
    }
    dest[i] = '\0';
}

void shell_warning(const char *message) {
    char *buffer = malloc((strlen(message)+50) * sizeof(char));
    strcpy(buffer, "\n\033[33mseashell:\033[m ");
    strcat(buffer, message);
    strcat(buffer, "\n\n");
    printf("%s", buffer);
    free(buffer);
}

void run_exit() {
    char *arg = strtok(NULL, " ");
    
    if(arg != NULL) {
        shell_warning("'exit' takes no arguments");
    } else {
        printf("\nExiting \033[34mseashell\033[m :)\n\n");
        exit(0);
    }
}

void run_echo() {
    // TODO Handle tabs (remove them, unless in double quotes)
    char *arg = strtok(NULL, "");

    // handling double quotes in echo
    if(arg[0] == '"' && arg[strlen(arg)-1] == '"') {
        for(int i = 1; i < strlen(arg)-1; i++) {
            printf("%c", arg[i]);
        }
    } else {
        arg = strtok(arg, " ");

        while(arg != NULL) {
            printf("%s", arg);
            arg = strtok(NULL, " ");

            if(arg != NULL) {
                printf(" ");
            }
        }
    }
    printf("\n");
}

void run_cd(struct ShellVariables *sv) {
    /*
    Usual filestructure:
    ~/ stands for home directory (for the specific user)
    /  stands for root directory (topmost folder)
    ./ prefix for relative paths

    If there is no argument, go to the home directory
    If the argument is '-', go to the previous directory
    
    Change Directory Logic:
    1 - Change the directory based on the given path
    2 - Reflect those changes in the next shell prompt

    This approach is easier as we don't have to handle '.' '..' './' '/',
    as chdir does that for us. We display the path with respect to the root 
    by checking if the absolute path of the current working directory has the absolute path
    of the home directory (of the shell) as a substring.

    */

    // TODO: handle case in which more than 1 arg is passed
    char *arg = strtok(NULL, "");
    if(arg == NULL || strcmp(arg, "~") == 0 || strcmp(arg, ".") == 0) {

        if(strcmp(sv->cwd_path, sv->home_path) == 0) {
            return;
        }

        // take it to home
        int status = chdir(sv->home_path);
        if(status == -1) {
            shell_warning("no such file or directory");
            return;
        }

        if(strcmp(sv->cwd_path, sv->prev_wd_path) != 0) {
            clear_string(sv->prev_wd_path);
            strcpy(sv->prev_wd_path, sv->cwd_path);
        }

        clear_string(sv->cwd_path);
        strcpy(sv->cwd_path, sv->home_path);

    } else if(strcmp(arg, "-") == 0) {

        int status = chdir(sv->prev_wd_path);
        if(status == -1) {
            shell_warning("no such file or directory");
            return;
        }

        printf("%s\n", sv->prev_wd_path);

        if(strcmp(sv->cwd_path, sv->prev_wd_path) != 0) {
            char *tmp_path = malloc(MAX_PATH_LEN * sizeof(char));
            strcpy(tmp_path, sv->cwd_path);

            clear_string(sv->cwd_path);
            strcpy(sv->cwd_path, sv->prev_wd_path);

            clear_string(sv->prev_wd_path);
            strcpy(sv->prev_wd_path, tmp_path);

            free(tmp_path);
        }
    } else {

        int status = chdir(arg);
        if(status == -1) {
            shell_warning("no such file or directory");
            return;
        }

        if(strcmp(sv->prev_wd_path, sv->cwd_path) != 0) {
            clear_string(sv->prev_wd_path);
            strcpy(sv->prev_wd_path, sv->cwd_path);
        }

        clear_string(sv->cwd_path);
        getcwd(sv->cwd_path, MAX_PATH_LEN);
    }
}

void run_pwd(const struct ShellVariables *sv) {

    char *arg = strtok(NULL, "");
    if(arg != NULL) {
        shell_warning("'pwd' takes no arguments");
        return;
    }

    printf("%s\n", sv->cwd_path);
}

void run_clear() {
    system("clear");
}

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
    // first tokenize with respect to ; with strtok
    // then process the commands individually

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
