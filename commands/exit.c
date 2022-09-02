#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../shell_manipulation.h"

void run_exit() {
    char *arg = strtok(NULL, " ");
    
    if(arg != NULL) {
        shell_warning("'exit' takes no arguments");
    } else {
        printf("\nExiting \033[34mseashell\033[m :)\n\n");
        exit(0);
    }
}