#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../shell_manipulation.h"

void run_exit(const char *args) {
    if(args == NULL) {
        printf("\n🐚 Exiting \033[34mseashell\033[m :)\n\n");
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
        
    }
}