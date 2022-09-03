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
// #include "../utils.h"
#include "../shell_manipulation.h"
// #include "commands.h"


#define MAX_LS_ARGS 25
#define NUM_FLAGS_SUPPORTED 4


/*
For the flag bitmap, 
Array Index to Flag Convention:
0 = -l
1 = -a
2 = -la
3 = -al
*/

#define FLAG_BITMAP_l 0
#define FLAG_BITMAP_a 1
#define FLAG_BITMAP_la 2
#define FLAG_BITMAP_al 3

#define MAX_LS_INFO_LEN 50

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
    
    For each argument (path)
        Get all information regarding all files/directories in the folder if the path is valid
        Then apply filters based on flags (with 1) from the bitmap
    
    Why this approach:
    We're not separating ls on the basis of flags initially to avoid too many if-else blocks
    This method is more memory intensive, but makes addition of more flags and features easy.
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

                flag_bitmap[FLAG_BITMAP_la] = 1;
            
            } else if(strcmp(arg, "-al") == 0) {

                flag_bitmap[FLAG_BITMAP_al] = 1;
            
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

    struct directory_info *data = malloc(sizeof(struct directory_info));

    struct dirent *file; // directory entry

    int number_of_files = 0;
    while((file = readdir(directory)) != NULL) {
        number_of_files++;
    }

    rewinddir(directory);

    data->total = 0;
    data->permissions = malloc(number_of_files * sizeof(char*));
    data->link_counts = malloc(number_of_files * sizeof(int));
    data->owners = malloc(number_of_files * sizeof(char*));
    data->groups = malloc(number_of_files * sizeof(char*));
    data->sizes  = malloc(number_of_files * sizeof(int));
    data->names  = malloc(number_of_files * sizeof(char*));

    data->modification_timestamps = malloc(number_of_files * sizeof(char*));

    for(int i = 0; i < number_of_files; i++) {
        data->permissions[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->owners[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->groups[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->names[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->modification_timestamps[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
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
            
            strcpy(data->permissions[i], "----------");

            if(st_mode_check(file_stats, S_IFDIR)) data->permissions[i][0] = 'd';

            if(st_mode_check(file_stats, S_IRUSR)) data->permissions[i][1] = 'r';
            if(st_mode_check(file_stats, S_IWUSR)) data->permissions[i][2] = 'w';
            if(st_mode_check(file_stats, S_IXUSR)) data->permissions[i][3] = 'x';

            if(st_mode_check(file_stats, S_IRGRP)) data->permissions[i][4] = 'r';
            if(st_mode_check(file_stats, S_IWGRP)) data->permissions[i][5] = 'w';
            if(st_mode_check(file_stats, S_IXGRP)) data->permissions[i][6] = 'x';

            if(st_mode_check(file_stats, S_IROTH)) data->permissions[i][7] = 'r';
            if(st_mode_check(file_stats, S_IWOTH)) data->permissions[i][8] = 'w';
            if(st_mode_check(file_stats, S_IXOTH)) data->permissions[i][9] = 'x';

            data->link_counts[i] = file_stats.st_nlink;

            struct passwd *pw = getpwuid(file_stats.st_uid);
            if(pw != 0) {
                strcpy(data->owners[i], pw->pw_name);
            }

            struct group  *gp = getgrgid(file_stats.st_gid);
            if(gp != 0) {
                strcpy(data->groups[i], gp->gr_name);
            }

            data->sizes[i] = file_stats.st_size;

            strcpy(data->modification_timestamps[i], ctime(&file_stats.st_mtime));
            if(data->modification_timestamps[i][strlen(data->modification_timestamps[i]) - 1] == '\n') {
                data->modification_timestamps[i][strlen(data->modification_timestamps[i]) - 1] = '\0';
            }

            strcpy(data->names[i], file->d_name);
        }

        free(file_path);
        i++;
    }


    for(int i = 0; i < number_of_files; i++) {
        if(flag_bitmap[FLAG_BITMAP_a] == 0 && data->names[i][0] != '.') {
            printf("%s %d %s %s %d %s %s\n", data->permissions[i], data->link_counts[i], data->owners[i], data->groups[i], data->sizes[i], data->modification_timestamps[i], data->names[i]);
        }
        



        printf("%s %d %s %s %d %s %s\n", data->permissions[i], data->link_counts[i], data->owners[i], data->groups[i], data->sizes[i], data->modification_timestamps[i], data->names[i]);
    }


    for(int i = 0; i < number_of_files; i++) {
        free(data->permissions[i]);
        free(data->owners[i]);
        free(data->groups[i]);
        free(data->names[i]);
        free(data->modification_timestamps[i]);
    }

    free(data->permissions);
    free(data->link_counts);
    free(data->owners);
    free(data->groups);
    free(data->sizes);
    free(data->names);
    free(data->modification_timestamps);

    free(data);


    /////////////////////////////

    closedir(directory);

    free(flag_bitmap);
    for(int i = 0; i < number_of_args; i++) {
        free(arguments[i]);
    }
    free(arguments);
}
