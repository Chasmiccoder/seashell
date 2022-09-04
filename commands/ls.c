#include <stdio.h>
#include <string.h>
#include <time.h>

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

#include "../globals.h"
#include "../utils.h"
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
    char **colored_names;
};

void string_to_lower(char *string) {
    for(int i = 0; i < strlen(string); i++) {
        if(string[i] >= 'A' && string[i] <= 'Z') {
            string[i] = string[i] + 32;
        }
    }
}

void string_swap(char *str1, char *str2) {
    char *buffer = malloc(MAX_LS_INFO_LEN * sizeof(char));

    strcpy(buffer, str1);
    clear_string(str1);
    strcpy(str1, str2);
    clear_string(str2);
    strcpy(str2, buffer);

    free(buffer);
}

void int_swap(int *num1, int *num2) {
    int tmp = *num1;
    *num1 = *num2;
    *num2 = tmp;
}

void directory_info_swap(struct directory_info *data, int i, int j) {

    string_swap(data->permissions[i], data->permissions[j]);
    int_swap(&data->link_counts[i], &data->link_counts[j]);
    string_swap(data->owners[i], data->owners[j]);
    string_swap(data->groups[i], data->groups[j]);
    int_swap(&data->sizes[i], &data->sizes[j]);
    string_swap(data->modification_timestamps[i], data->modification_timestamps[j]);
    string_swap(data->names[i], data->names[j]);
    string_swap(data->colored_names[i], data->colored_names[j]);

}

void sort_directory_info(struct directory_info *data, int number_of_files_to_display) {
    // Simple bubble sort

    for(int i = 0; i < number_of_files_to_display - 1; i++) {
        for(int j = 0; j < number_of_files_to_display - i - 1; j++) {
            char *str1 = malloc((strlen(data->names[j])+1) * sizeof(char));
            strcpy(str1, data->names[j]);
            string_to_lower(str1);

            char *str2 = malloc((strlen(data->names[j+1])+1) * sizeof(char));
            strcpy(str2, data->names[j+1]);
            string_to_lower(str2);

            if(strcmp(str1, str2) > 0) {
                directory_info_swap(data, j, j+1);
            }

            free(str1);
            free(str2);
        }
    }
}

void init_directory_info(struct directory_info *data, int number_of_files) {
    data->total = 0;
    data->permissions = malloc(number_of_files * sizeof(char*));
    data->link_counts = malloc(number_of_files * sizeof(int));
    data->owners = malloc(number_of_files * sizeof(char*));
    data->groups = malloc(number_of_files * sizeof(char*));
    data->sizes  = malloc(number_of_files * sizeof(int));
    data->names  = malloc(number_of_files * sizeof(char*));
    data->colored_names  = malloc(number_of_files * sizeof(char*));

    data->modification_timestamps = malloc(number_of_files * sizeof(char*));

    for(int i = 0; i < number_of_files; i++) {
        data->permissions[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->owners[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->groups[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->names[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->colored_names[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
        data->modification_timestamps[i] = malloc(MAX_LS_INFO_LEN * sizeof(char));
    }
}

void free_directory_info(struct directory_info *data, int number_of_files) {
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
}

void get_permissions(struct directory_info *data, struct stat file_stats, int i) {
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
}

void ls_on_one_directory(char *argument, const int *flag_bitmap, const struct ShellVariables *sv) {
    // first check whether the argument is a file or a directory
    struct stat arg_stats;
    if(stat(argument, &arg_stats) != 0) {
        shell_warning("stat gone wrong during ls");
    } else if (st_mode_check(arg_stats, S_IFDIR) == 0) {

        char *filename = malloc(MAX_LS_INFO_LEN * sizeof(char));
        strcpy(filename, argument);
        
        if(st_mode_check(arg_stats, S_IXUSR) || st_mode_check(arg_stats, S_IXGRP) || st_mode_check(arg_stats, S_IXOTH)) 
            add_color_to_string(filename, COLOR_GREEN);

        printf("%s\n", filename);

        free(filename);
        return;
    }
    
    DIR *directory;
    directory = opendir(argument);

    char *previous_path = malloc(MAX_PATH_LEN * sizeof(char));
    strcpy(previous_path, sv->cwd_path);

    int status = chdir(argument);
    if(status == -1) {
        char *buff = malloc(MAX_PATH_LEN * sizeof(char));
        sprintf(buff, "could not open '%s'", argument);
        shell_warning(buff);
        free(buff);
    }

    printf("ls on: %s\n", argument);

    if(directory == NULL) {
        shell_warning("unable to open directory");
        return;
    }

    struct directory_info *data = malloc(sizeof(struct directory_info));

    struct dirent *file; // directory entry

    int number_of_files = 0;
    int number_of_files_to_display = 0; // this may be lesser than number_of_files if filters are applied
    while((file = readdir(directory)) != NULL) {
        number_of_files++;
    }

    rewinddir(directory);

    init_directory_info(data, number_of_files);
    
    int i = 0;

    while((file = readdir(directory)) != NULL) {

        // handling flag: -a
        if(flag_bitmap[FLAG_BITMAP_a] == 0 && file->d_name[0] == '.') {
            continue;
        }

        struct stat file_stats;

        char *file_path = malloc(MAX_PATH_LEN * sizeof(char));
        strcpy(file_path, file->d_name);

        if(stat(file_path, &file_stats) != 0) {
            shell_warning("stat gone wrong during ls");
        } else {
            
            get_permissions(data, file_stats, i);
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
            strcpy(data->colored_names[i], file->d_name);



            if(data->permissions[i][0] == 'd') {
                add_color_to_string(data->colored_names[i], COLOR_BLUE);
            } else if(data->permissions[i][3] == 'x' || data->permissions[i][6] == 'x' || data->permissions[i][9] == 'x') {
                add_color_to_string(data->colored_names[i], COLOR_GREEN);
            } else {
                add_color_to_string(data->colored_names[i], COLOR_WHITE);
            }
        }

        free(file_path);
        i++;
        number_of_files_to_display++;
    }

    sort_directory_info(data, number_of_files_to_display);

    for(int i = 0; i < number_of_files_to_display; i++) {
        // handling flag: -l
        if(flag_bitmap[FLAG_BITMAP_l] == 1) {
            printf("%s %d %s %s %d %s %s\n", data->permissions[i], data->link_counts[i], data->owners[i], data->groups[i], data->sizes[i], data->modification_timestamps[i], data->colored_names[i]);
        } else {
            printf("%s\n", data->colored_names[i]);
        }
    }

    printf("\n");

    free_directory_info(data, number_of_files);
    closedir(directory);
    free(previous_path);

    chdir(previous_path);

}


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

                flag_bitmap[FLAG_BITMAP_l] = 1;
                flag_bitmap[FLAG_BITMAP_a] = 1;
            
            } else if(strcmp(arg, "-al") == 0) {

                flag_bitmap[FLAG_BITMAP_l] = 1;
                flag_bitmap[FLAG_BITMAP_a] = 1;
            
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
    
    for(int i = 0; i < number_of_args; i++) {
        ls_on_one_directory(arguments[i], flag_bitmap, sv);
    }
    
    free(flag_bitmap);
    for(int i = 0; i < number_of_args; i++) {
        free(arguments[i]);
    }
    free(arguments);
}
