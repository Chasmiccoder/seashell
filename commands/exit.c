#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "../globals.h"

void kill_processes() {
    // for(int i = 0; i < global_number_of_processes; i++) {
    //     // kill(global_background_pids[i], SIGKILL);
    //     printf("BG: %d\n", global_background_pids[i]);
    // }
}

void run_exit(const char *args) {
    if(args == NULL) {
        printf("\n🐚 Exiting \033[34mseashell\033[m :)\n\n");
        // kill(getpid(), SIGKILL);
        kill_processes();
        exit(0);
    } else {
        char args_modifiable[MAX_COMMAND_LEN];
        strcpy(args_modifiable, args);

        char *arg1 = strtok(args_modifiable, " ");
        char *arg2 = strtok(NULL, " ");
        if(arg2 != NULL) {
            shell_warning("'exit' takes only 1 argument [status]");
            return;
        }
        
        printf("\n🐚 Exiting \033[34mseashell\033[m with status: %s\n\n", arg1);
        // kill(getpid(), SIGKILL);
        kill_processes();
        int status = atoi(arg1);
        exit(status);
    }
}
