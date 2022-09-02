#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "../globals.h"
#include "../utils.h"
#include "../shell_manipulation.h"
#include "commands.h"

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