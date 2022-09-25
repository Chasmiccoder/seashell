#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "../globals.h"
#include "commands.h"

void run_sig(char *args) {
    if(args == NULL) {
        shell_warning("sig takes 2 integers are arguments");
        return;
    }

    char *arg1 = strtok(args, " ");
    char *arg2 = strtok(NULL, " ");
    char *arg3 = strtok(NULL, "");

    if(arg1 == NULL || arg2 == NULL || arg3 != NULL) {
        shell_warning("sig takes 2 integers are arguments");
        return;
    }

    // sid = shell id of the process (assigned by the shell)
    int sid, code;
    sid = atoi(arg1);
    code = atoi(arg2);

    // printf("sig: %d %d\n", sid, code);

    // locate the process by the sid
    int p_index = -1;
    for(int i = 0; i < MAX_NUM_OF_BACKGROUND_PROCESSES; i++) {
        if(sv->background_process_assigned_ids[i] == sid) {
            p_index = i;
            break;
        }
    }

    if(p_index == -1) {
        shell_warning("no such job exists");
        return;
    }

    int status = kill(sv->background_process_ids[p_index], code);
    if(status == -1) {
        shell_warning("sig failed");
        return;
    }
}