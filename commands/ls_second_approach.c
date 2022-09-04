#include <stdio.h>
#include <string.h>
#include <time.h>

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
// #include <unistd.h>

#include "../globals.h"
#include "../utils.h"
#include "../shell_manipulation.h"
// #include "commands.h"


#define MAX_LS_ARGS 25
#define NUM_FLAGS_SUPPORTED 2


/*
For the flag bitmap, 
Array Index to Flag Convention:
0 = -l
1 = -a
*/

#define FLAG_BITMAP_l 0
#define FLAG_BITMAP_a 1

void init_flag_bitmap(int *flag_bitmap) {
    for(int i = 0; i < NUM_FLAGS_SUPPORTED; i++) {
        flag_bitmap[i] = 0;
    }
}

int st_mode_check(struct stat stats, int code) {
    if (stats.st_mode & code) {
        return 1;
    } else {
        return 0;
    }
}

// contains all the details in the directory
// each array contains information regarding 1 file
// example, permissions[0] contains the permissions of file corresponding to names[0]
struct directory_info {
    int total;

    char **permissions;
    int *link_counts;
    
    char **owners;
    char **groups;

    int *sizes;

    char **modification_timestamps;
    char **names;
};




void run_ls(const struct ShellVariables *sv) {
    /*
    list (ls) arguments -
    None - Display the names of all files (ascending lexicographical order)
    -l   - Display extra information regarding the files
    -a   - Display the names of all the files, including hidden ones
    .    - Display the names of all files in the current working directory
    ..   - Display the names of all files in the current working directory's parent
    ~    - Dsiplay the names of all files in the home directory
    -la  - same as -l -a
    -al  - same as -l -a
    
    <directory name> - Do ls on the specified directory
    <directory path> - Do ls on the directory specified by the path
    <file name>      - Do ls on a file (prints the name of the file)
    <d1> <d2>        - Do ls on directory d1, and then on d2 (print the output separately)

    Note(s): 
    Multiple flags and argument types can be used
    The order of flags does not matter. 
    For multiple directories, do ls according to the order in which they were given.
    
    E.g: ls dir1 -l -a dir2
    The -l and -a flags apply to both dir1 and dir2. ls dir1 happens before ls dir2
    All flags start with '-'

    Flags Implemented:
    -l
    -a

    Approach:
    The string has already passed through the format_string() function
    First tokenize by ' ', to extract all arguments

    Create a bitmap out of all flags
    flag_bitmap[i] = 1 if the flag is used, and 0 if not used
    Elements that are not flags are added as arguments
    (they have to be paths on which we want to apply ls)
    */


    char *arg = strtok(NULL, " ");

    char **arguments = malloc(MAX_LS_ARGS * sizeof(char*));
    int i = 0;

    int *flag_bitmap = malloc(NUM_FLAGS_SUPPORTED * sizeof(int));
    init_flag_bitmap(flag_bitmap);


    // if no argument is given, add '.' as an argument
    if(arg == NULL) {

        arguments[i] = malloc(MAX_PATH_LEN * sizeof(char*));
        strcpy(arguments[i], ".");
        i++;

    } else {
        while(arg != NULL) {
            
            if(strcmp(arg, "-l") == 0) {
              
                flag_bitmap[FLAG_BITMAP_l] = 1;

            } else if(strcmp(arg, "-a") == 0) {
              
                flag_bitmap[FLAG_BITMAP_a] = 1;
            
            } else if(strcmp(arg, "-la") == 0) {

                flag_bitmap[FLAG_BITMAP_a] = 1;
                flag_bitmap[FLAG_BITMAP_l] = 1;
            
            } else if(strcmp(arg, "-al") == 0) {

                
                flag_bitmap[FLAG_BITMAP_a] = 1;
                flag_bitmap[FLAG_BITMAP_l] = 1;
            
            } else {
                arguments[i] = malloc(MAX_PATH_LEN * sizeof(char*));
                strcpy(arguments[i], arg);
                i++;
            }

            arg = strtok(NULL, " ");
        }
    }

    int number_of_args = i;



    ///////////////// For each argument:
    DIR *directory;
    directory = opendir("."); // CHANGE

    if(directory == NULL) {
        shell_warning("unable to open directory");
        return;
    }

    struct dirent *file; // directory entry

    int number_of_files = 0;
    while((file = readdir(directory)) != NULL) {
        number_of_files++;
    }

    rewinddir(directory);

    char **file_names = malloc(number_of_files * sizeof(char*));

    for(int i = 0; i < number_of_files; i++) {
        file_names[i] = malloc(MAX_PATH_LEN * sizeof(char));
    }



    
    // printf("Number of files: %d\n", number_of_files);
    i = 0;

    while((file = readdir(directory)) != NULL) {

        struct stat file_stats;

        char *file_path = malloc(MAX_PATH_LEN * sizeof(char));
        strcpy(file_path, "./");  // CHANGE
        strcat(file_path, file->d_name);


        if(stat(file_path, &file_stats) != 0) {
            shell_warning("stat gone wrong during ls");
        } else {
            strcpy(file_names[i], file->d_name);
        }

        free(file_path);
        i++;
    }

    // sort file names in ascending order


    rewinddir(directory);

    for(int i = 0; i < number_of_files; i++) {

        // if the -a flag is not given, replace all hidden files with '\0';
        if(flag_bitmap[FLAG_BITMAP_a] == 0) {
            for(int i = 0; i < number_of_files; i++) {
                if(file_names[i][0] == '.') {
                    clear_string(file_names[i]);
                }
            }
        }


        // file = readdir(directory);

        struct stat file_stats;

        char *file_path = malloc(MAX_PATH_LEN * sizeof(char));
        strcpy(file_path, "./");  // CHANGE
        strcat(file_path, file->d_name);


        if(stat(file_path, &file_stats) != 0) {
            shell_warning("stat gone wrong during ls");
            return;
        }

        // future feature: Create a table data structure and add rows
        // print the table at the end (instead of printing in each if-else block)
        if(flag_bitmap[FLAG_BITMAP_l] == 0) {
            // colour the filenames
            // print the filenames

            // paint it blue
            // if(st_mode_check(file_stats, S_IFDIR)) strcpy(file_names[i])


        } else {

            char permissions[] = "----------\0";
            int link_count = file_stats.st_nlink;
            char *owner_name = malloc(MAX_HOSTNAME_LEN * sizeof(char));
            char *group_name = malloc(MAX_HOSTNAME_LEN * sizeof(char));
            int size;
            char *modification_timestamp = malloc(50 * sizeof(char));

            if(st_mode_check(file_stats, S_IFDIR)) permissions[0] = 'd';

            if(st_mode_check(file_stats, S_IRUSR)) permissions[1] = 'r';
            if(st_mode_check(file_stats, S_IWUSR)) permissions[2] = 'w';
            if(st_mode_check(file_stats, S_IXUSR)) permissions[3] = 'x';

            if(st_mode_check(file_stats, S_IRGRP)) permissions[4] = 'r';
            if(st_mode_check(file_stats, S_IWGRP)) permissions[5] = 'w';
            if(st_mode_check(file_stats, S_IXGRP)) permissions[6] = 'x';

            if(st_mode_check(file_stats, S_IROTH)) permissions[7] = 'r';
            if(st_mode_check(file_stats, S_IWOTH)) permissions[8] = 'w';
            if(st_mode_check(file_stats, S_IXOTH)) permissions[9] = 'x';


            struct passwd *pw = getpwuid(file_stats.st_uid);
            if(pw != 0) {
                strcpy(owner_name, pw->pw_name);
            }

            struct group  *gp = getgrgid(file_stats.st_gid);
            if(gp != 0) {
                strcpy(group_name, gp->gr_name);
            }

            size = file_stats.st_size;

            strcpy(modification_timestamp, ctime(&file_stats.st_mtime));
            if(modification_timestamp[strlen(modification_timestamp) - 1] == '\n') {
                modification_timestamp[strlen(modification_timestamp) - 1] = '\0';
            }
            


            // colour the filenames



            free(file_path);
            free(owner_name);
            free(group_name);
            free(modification_timestamp);

        }

        // printf("%s %d %s %s %d %s %s\n", data->permissions[i], data->link_counts[i], data->owners[i], data->groups[i], data->sizes[i], data->modification_timestamps[i], data->names[i]);
    }


    for(int i = 0; i < number_of_files; i++) {
        free(file_names);
    }


    /////////////////////////////

    closedir(directory);

    free(flag_bitmap);
    for(int i = 0; i < number_of_args; i++) {
        free(arguments[i]);
    }
    free(arguments);
}
