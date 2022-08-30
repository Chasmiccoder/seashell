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
unistd.h  - getlogin(), gethostname()
string.h  - strcpy(), strtok(), strcmp()
errno.h   - errno

*/


#define MAX_USERNAME_LEN 32
#define MAX_HOSTNAME_LEN 256
#define MAX_SHELL_PROMPT_LEN 4096
#define MAX_PATH_LEN 2048
#define MAX_COMMAND_LEN 2048

struct ShellVariables {
    char *username;
    char *hostname;
    char *cwd_path;

    int loop_control;
};


void init_shell_variables(struct ShellVariables *sv) {
    
    /* --- Fetch the username and the hostname --- */
    sv->username = malloc(MAX_USERNAME_LEN * sizeof(char));
    sv->username = getlogin();

    sv->hostname = malloc(MAX_HOSTNAME_LEN * sizeof(char));
    gethostname(sv->hostname, MAX_HOSTNAME_LEN);

    // start at the root directory by default (where the shell is stored)
    sv->cwd_path = malloc(MAX_PATH_LEN * sizeof(char));
    strcpy(sv->cwd_path, "~");
    
    sv->loop_control = 1;
}


void printShellPrompt(const struct ShellVariables *sv) {
    /*
    Prints the shell prompt with the format,
    <username@system: ~ >
    We're using ANSI color codes to customize the terminal
    */
    char *shell_prompt = malloc(MAX_SHELL_PROMPT_LEN * sizeof(char));
    sprintf(shell_prompt, "\033[37m<\033[m\033[31m%s\033[m\033[37m@\033[m\033[34m%s\033[m\033[37m: %s >\033[m ", sv->username, sv->hostname, sv->cwd_path);
    printf("%s",shell_prompt);
    free(shell_prompt);
}

void removeUnnecessarySpaces(char *str) {
    /*
    Removes unnecessary spaces in a string.
    [1] Trims spaces from the left to the first character
    [2] Trims spaces from the last character onwards
    [3] Trims extra spaces in between words/arguments
    [4] If double quotes are used, it doesn't trim the spaces in between

    E.g.
    ` hello  world   `    -> `hello world`
    `  hello " word   !"` -> `hello " word   !"`
    */
    
    char *buff = malloc((strlen(str)+1) * sizeof(char));

    int i = 0; // str index
    int j = 0; // buffer index

    // [1]
    while(str[i] == ' ') {
        i++;
    }

    int last_char = -1;

    // [2]
    for(int k = strlen(str) - 1; k >= 0; k--) {
        if(str[k] != ' ') {
            last_char = k;
            break;
        }
    }

    // [3]
    int doubleQuoteFlag = 0; // flag for the double quote immunity
    int oneSpace = 0;        // gets incremented if 1 space is encountered
    while(i <= last_char) {
        if(str[i] == '"') {  // [4]
            doubleQuoteFlag = (doubleQuoteFlag == 1) ? 0 : 1; // toggle the flag
        }

        if(str[i] != ' ') {
            buff[j++] = str[i];
            oneSpace = 0;
        } else {
            if(oneSpace == 0 || doubleQuoteFlag) {
                oneSpace++;
                buff[j++] = str[i];
            }
        }

        i++;
    }

    strcpy(str, buff);
    free(buff);
}

void run_exit() {
    char *arg = strtok(NULL, " ");
    
    if(arg != NULL) {
        printf("\n\033[33mSeaShell:\033[m 'exit' takes no arguments\n\n");
    } else {
        printf("\nExiting \033[34mSeaShell\033[m :)\n\n");
        exit(0);
    }
}

void processStatement(char *statement) {
    char *command = strtok(statement, " ");

    if(strcmp(command, "exit") == 0) {
        run_exit();
    }

    
}

void processInput(char *input_string) {
    // first tokenize with respect to ; with strtok
    // then process the commands individually
    processStatement(input_string);
}




int main() {
    printf("\n\033[31m---\033[m \033[34mWelcome to the SeaShell!\033[m \033[31m---\033[m\n\n");

    struct ShellVariables *sv = malloc(sizeof(struct ShellVariables));

    init_shell_variables(sv);

    while(sv->loop_control) {
        printShellPrompt(sv);

        char *input_string = malloc(MAX_COMMAND_LEN * sizeof(char));

        scanf("%[^\n]%*c", input_string); 
        
        
        processInput(input_string);

        

    }
    

    free(sv);


    printf("\n");
    return 0;
}