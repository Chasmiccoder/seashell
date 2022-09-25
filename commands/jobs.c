#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../globals.h"
#include "commands.h"

#define FLAG_BITMAP_JOBS_r 0
#define FLAG_BITMAP_JOBS_s 1

#define NUM_JOBS_FLAGS_SUPPORTED 2


void init_jobs_flag_bitmap(int *flag_bitmap) {
    for(int i = 0; i < NUM_JOBS_FLAGS_SUPPORTED; i++) {
        flag_bitmap[i] = 0;
    }
}

void sort_jobs() {
    // first fill all in-between gaps
    // int num = 0;
    int i = 0;
    while(i < MAX_NUM_OF_BACKGROUND_PROCESSES) {
        int j = i;
        int end_flag = 0;
        while(sv->background_process_ids[j] == -1) {
            j++;
            if(j == MAX_NUM_OF_BACKGROUND_PROCESSES) {
                end_flag = 1;
                break;
            }
        }

        if(end_flag) {
            break;
        }
        
        // we found at least one blank row
        if(i != j) {
            sv->background_process_ids[i] = sv->background_process_ids[j];
            sv->background_process_assigned_ids[i] = sv->background_process_assigned_ids[j];
            strcpy(sv->background_process_names[i], sv->background_process_names[j]);
            strcpy(sv->background_process_status[i], sv->background_process_status[j]);

            sv->background_process_ids[j] = -1;
            sv->background_process_assigned_ids[j] = -1;
            clear_string(sv->background_process_names[j]);
            clear_string(sv->background_process_status[j]);
        }
        // num++;
        i++;
    }

    int num = i;

    for(int i = 0; i < num - 1; i++) {
        for(int j = 0; j < num - i - 1; j++) {
            char *str1 = malloc((strlen(sv->background_process_names[j])+1) * sizeof(char));
            strcpy(str1, sv->background_process_names[j]);
            string_to_lower(str1);

            char *str2 = malloc((strlen(sv->background_process_names[j+1])+1) * sizeof(char));
            strcpy(str2, sv->background_process_names[j+1]);
            string_to_lower(str2);

            if(strcmp(str1, str2) > 0) {
                // int tmp = sorted_indices[j];
                // sorted_indices[j] = sorted_indices[j+1];
                // sorted_indices[j+1] = tmp;

                // swap rows j and j+1
                int id = sv->background_process_ids[j];
                int assigned_id = sv->background_process_assigned_ids[j];
                char name[MAX_COMMAND_LEN];
                char status[STATUS_SIZE];
                strcpy(name, sv->background_process_names[j]);
                strcpy(status, sv->background_process_status[j]);

                sv->background_process_ids[j] = sv->background_process_ids[j+1];
                sv->background_process_assigned_ids[j] = sv->background_process_assigned_ids[j+1];

                clear_string(sv->background_process_names[j]);
                clear_string(sv->background_process_status[j]);
                strcpy(sv->background_process_names[j], sv->background_process_names[j+1]);
                strcpy(sv->background_process_status[j], sv->background_process_status[j+1]);

                sv->background_process_ids[j+1] = id;
                sv->background_process_assigned_ids[j+1] = assigned_id;

                clear_string(sv->background_process_names[j+1]);
                clear_string(sv->background_process_status[j+1]);
                strcpy(sv->background_process_names[j+1], name);
                strcpy(sv->background_process_status[j+1], status);
            }
        }
    }
}


void run_jobs(char *args) {

    int *flag_bitmap = malloc(NUM_JOBS_FLAGS_SUPPORTED * sizeof(int));
    init_jobs_flag_bitmap(flag_bitmap);

    if(args != NULL) {
        char *arg = strtok(args, " ");
        while(arg != NULL) {
            if(strcmp(arg, "-r") == 0) {
                flag_bitmap[FLAG_BITMAP_JOBS_r] = 1;
            } else if(strcmp(arg, "-s") == 0) {
                flag_bitmap[FLAG_BITMAP_JOBS_s] = 1;
            } else {
                shell_warning("invalid arguments passed. jobs only takes flags -r and -s");
                return;
            }

            arg = strtok(NULL, " ");
        }
    } else {
        flag_bitmap[FLAG_BITMAP_JOBS_r] = 1;
        flag_bitmap[FLAG_BITMAP_JOBS_s] = 1;
    }

    // int *sorted_indices = malloc(MAX_NUM_OF_BACKGROUND_PROCESSES * sizeof(int));
    // sort_jobs(sorted_indices, sv->background_process_names, sv->background_process_assigned_ids);
    sort_jobs();

    for(int i = 0; i < MAX_NUM_OF_BACKGROUND_PROCESSES; i++) {
        // int ind = sorted_indices[i];

        if(sv->background_process_ids[i] != -1 && (flag_bitmap[FLAG_BITMAP_JOBS_r] && strcmp(sv->background_process_status[i], "Running") == 0|| flag_bitmap[FLAG_BITMAP_JOBS_s] && strcmp(sv->background_process_status[i], "Stopped") == 0))
            printf("[%d] %s %s [%d]\n", sv->background_process_assigned_ids[i], sv->background_process_status[i], sv->background_process_names[i], sv->background_process_ids[i]);
    }

    // free(sorted_indices);
    free(flag_bitmap);

}
