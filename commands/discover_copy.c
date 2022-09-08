#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include "../globals.h"
#include "../utils.h"
#include "../shell_manipulation.h"

#define MAX_DISCOVER_ARGS 25            // incase flags are repeated
#define NUM_DISCOVER_FLAGS_SUPPORTED 2  // -d, -f

/*
For the flag bitmap, 
Array Index to Flag Convention:
0 = -d
1 = -f
*/

#define FLAG_BITMAP_DISCOVER_d 0
#define FLAG_BITMAP_DISCOVER_f 1

#define MAX_DISCOVER_ENTITIES 100

struct discover_info {
    char **entities;  // could be files or directories
    int *types;       // mentions whether the ith entity is a directory (1) or a file (0)
    int size;         // holds the number of entities;
};

void init_discover_flag_bitmap(int *flag_bitmap) {
    for(int i = 0; i < NUM_DISCOVER_FLAGS_SUPPORTED; i++) {
        flag_bitmap[i] = 0;
    }
}

void scan_directory(char *targetdir, const char *targetfile, char *target_file_path, struct discover_info *data, const int *flag_bitmap, const int is_recursive){
    /*
    If is_recursive is true, then scan the directory recursively
    targetdir = directory to search under
    targetfile = file to search for
    target_file_path = char array to store the path of the target file
    */

    // char tmp[MAX_PATH_LEN];
    // getcwd(tmp, MAX_PATH_LEN);
    // printf("%s\n", tmp);

    if(targetdir == NULL) {
        clear_string(targetdir);
        strcpy(targetdir, ".");
    }

    DIR *directory = opendir(targetdir);

    if(directory == NULL) {
        // printf("here: %s %d\n", targetdir, strlen(targetdir));
        // char tmp[MAX_PATH_LEN];
        // getcwd(tmp, MAX_PATH_LEN);
        // printf("%s\n", tmp);
        shell_warning("no such directory exists");
        return;
    }

    struct dirent *file;

    file = readdir(directory);
    while(file != NULL) {
        if(targetfile != NULL && target_file_path != NULL && strcmp(targetfile, file->d_name) == 0) {
            
            char tmp[MAX_PATH_LEN];
            getcwd(tmp, MAX_PATH_LEN);
            strcpy(target_file_path, tmp);
            strcat(target_file_path, "/");
            strcat(target_file_path, targetfile);

            return;
        }

        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || strcmp(file->d_name, ".git") == 0 || strcmp(file->d_name, ".vscode") == 0) {  // removing .git and .vscode folders because they contain too many sub structures. Later on, we can use an array of strings named 'outliers' to handle this
            file = readdir(directory);
            continue;
        }

        if(targetfile == NULL && data->size < MAX_DISCOVER_ENTITIES) {
            int added = 0;
            if(flag_bitmap[FLAG_BITMAP_DISCOVER_d] && file->d_type == DT_DIR) {
                data->entities[data->size] = malloc(MAX_PATH_LEN * sizeof(char));
                strcpy(data->entities[data->size], file->d_name);
                data->size++;
                added = 1;
            }

            if(added == 0 && flag_bitmap[FLAG_BITMAP_DISCOVER_f] && file->d_type != DT_DIR) {
                data->entities[data->size] = malloc(MAX_PATH_LEN * sizeof(char));
                strcpy(data->entities[data->size], file->d_name);
                data->size++;
                added = 1;
            }

            if(added == 0 && flag_bitmap[FLAG_BITMAP_DISCOVER_d] == 0 && flag_bitmap[FLAG_BITMAP_DISCOVER_f] == 0) {
                data->entities[data->size] = malloc(MAX_PATH_LEN * sizeof(char));
                strcpy(data->entities[data->size], file->d_name);
                data->size++;
            }
        }


        if(is_recursive && file->d_type == DT_DIR && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
            char path[MAX_PATH_LEN];
            strcpy(path, targetdir);
            strcat(path, "/");
            strcat(path, file->d_name);

            scan_directory(path, targetfile, target_file_path, data, flag_bitmap, is_recursive);
        }

        file = readdir(directory);
    }
    closedir(directory);
}

void run_discover(const struct ShellVariables *sv) {
    /*
    Usage:
    discover <target_dir> <type_flags> <file_name>

    Flags:
    -d    - searches the target directory and its sub directories
    -f    - searches for all files

    Note:
    If no paths are given, this will print all files in the current directory
    If 1 path is given, it will be assumed to be the target directory
    
    arguments[0] is the target directory, and arguments[1] is the target file

    */
    char *previous_path = malloc(MAX_PATH_LEN * sizeof(char));
    strcpy(previous_path, sv->cwd_path);
    

    char *arg = strtok(NULL, " ");

    char **arguments = malloc(MAX_DISCOVER_ARGS * sizeof(char*));
    int i = 0;

    int *flag_bitmap = malloc(NUM_DISCOVER_FLAGS_SUPPORTED * sizeof(int));
    init_discover_flag_bitmap(flag_bitmap);

    if(arg == NULL) {
        arguments[i] = malloc(MAX_PATH_LEN * sizeof(char));
        strcpy(arguments[i], ".");
        i++;
    } else {
        while(arg != NULL) {
            if(strcmp(arg, "-d") == 0) {

                flag_bitmap[FLAG_BITMAP_DISCOVER_d] = 1;

            } else if(strcmp(arg, "-f") == 0) {

                flag_bitmap[FLAG_BITMAP_DISCOVER_f] = 1;
            
            } else {
                arguments[i] = malloc(MAX_PATH_LEN * sizeof(char*));
                strcpy(arguments[i], arg);
                i++;
            }

            arg = strtok(NULL, " ");
        }
    }

    // no path given, but at least one flag is given
    if(i == 0) {
        arguments[0] = malloc(MAX_PATH_LEN * sizeof(char*));
        strcpy(arguments[0], "./");
        i++;
    }

    int number_of_args = i;

    // number of paths specified
    if(number_of_args > 2) {
        shell_warning("discover takes at most 2 paths as arguments");
        return;
    }

    struct discover_info *data = malloc(sizeof(struct discover_info));
    data->entities = malloc(MAX_DISCOVER_ENTITIES * sizeof(char*));
    data->types    = malloc(MAX_DISCOVER_ENTITIES * sizeof(int));

    data->size = 0;
    char target_file_path[MAX_PATH_LEN] = "\0";

    chdir(arguments[0]);

    if(number_of_args == 1) {
        // scan_directory(arguments[0], data, NULL, NULL, flag_bitmap[FLAG_BITMAP_DISCOVER_d]);
        scan_directory(NULL, NULL, NULL, data, flag_bitmap, 0); // targetfile not given
    } else {
        // scan_directory(arguments[0], data, arguments[1], target_file_path, flag_bitmap[FLAG_BITMAP_DISCOVER_d]);
        scan_directory(arguments[0], arguments[1], target_file_path, data, flag_bitmap, 1);
        printf("%s\n", target_file_path);
    }

    for(int i = 0; i < data->size; i++) {
        printf("%s\n", data->entities[i]);
    }

    for(int i = 0; i < data->size; i++) {
        free(data->entities[i]);
    }

    free(data->entities);
    free(data->types);
    free(data);

    chdir(previous_path);
    free(previous_path);
}
