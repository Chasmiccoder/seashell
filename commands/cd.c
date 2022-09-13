#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "../globals.h"
#include "../utils.h"
#include "../shell_manipulation.h"
#include "commands.h"

void get_resolved_path_cd(char *target_string, char *path, struct ShellVariables *sv, char *with_respect_to) {
    /*
    We can resolve the absolute path with respect to the current working
    directory ".", or the home directory (shell location) "~"
    */

    if(is_substring(sv->home_path, path)) {
        strcpy(target_string, with_respect_to);
        strcat(target_string, path+strlen(sv->home_path));
        return;
    }

    if(is_substring(with_respect_to, path)) {
        strcpy(target_string, sv->home_path);
        strcat(target_string, with_respect_to);
        strcat(target_string, path + strlen(with_respect_to));
        printf("CHECK THIS ONCE 2: %s\n", target_string);
        return;
    }
}

void convert_shell_path_to_absolute_path(char *target, const char *path, struct ShellVariables *sv) {
    if(is_substring("~", path)) {
        strcpy(target, sv->home_path);
        if(strlen(path) > 1) {
            strcat(target, path + strlen("~"));
        }
    } else {
        strcpy(target, path);
    }
}


void run_cd(const char *args, struct ShellVariables *sv) {
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

    if(args != NULL && strcmp(args, "-") == 0) {
        int status_chdir = chdir(sv->prev_wd_path);
        if(status_chdir == -1) {
            shell_warning("no such directory");
            return;
        }

        printf("%s\n", sv->prev_wd_path);

        char tmp_path[MAX_PATH_LEN];
        strcpy(tmp_path, sv->cwd_path);

        clear_string(sv->cwd_path);
        strcpy(sv->cwd_path, sv->prev_wd_path);

        clear_string(sv->prev_wd_path);
        strcpy(sv->prev_wd_path, tmp_path);
        
    } else {
        char resolved_path[MAX_PATH_LEN];
        if(args != NULL) {
            convert_shell_path_to_absolute_path(resolved_path, args, sv);
        } else {
            strcpy(resolved_path, sv->home_path);
        }

        int status_chdir = chdir(resolved_path);
        if(status_chdir == -1) {
            shell_warning("no such directory");
            return;
        }

        if(strcmp(sv->cwd_path, sv->prev_wd_path) != 0) {
            clear_string(sv->prev_wd_path);
            strcpy(sv->prev_wd_path, sv->cwd_path);
        }

        clear_string(sv->cwd_path);
        getcwd(sv->cwd_path, MAX_PATH_LEN);
    }  
}