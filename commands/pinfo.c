#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../globals.h"

void run_pinfo() {

    int pid = getpid();    
    char *file_name = malloc(MAX_PATH_LEN * sizeof(char));

    // proc/[pid]/stat
    sprintf(file_name, "/proc/%d/stat", pid);

    FILE *fp = fopen(file_name, "r");

    int var;
    int parent_process_id;
    char status;
    char *command = malloc(MAX_COMMAND_LEN * sizeof(char));
    fscanf(fp, "%d %s %c %d", &var, command, &status, &parent_process_id);

    clear_string(file_name);
    sprintf(file_name, "/proc/%d/statm", pid);
    int memory;
    int v1;
    int v2;
    int v3; // text
    int v4; // lib
    int v5; // data
    int v6; // dt

    FILE *fp2 = fopen(file_name, "r");

    fscanf(fp2, "%d %d %d %d %d %d %d", &memory, &v1, &v2, &v3, &v4, &v5, &v6);    
    printf("pid: %d\nprocess status: %c\nmemory: %d\ncommand: %s\n", pid, status, memory, command);

    free(command);
    free(file_name);
}
