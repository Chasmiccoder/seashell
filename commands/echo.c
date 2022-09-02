#include <stdio.h>
#include <string.h>

void run_echo() {
    char *arg = strtok(NULL, "");

    // handling double quotes in echo
    if(arg[0] == '"' && arg[strlen(arg)-1] == '"') {
        for(int i = 1; i < strlen(arg)-1; i++) {
            printf("%c", arg[i]);
        }
    } else {
        arg = strtok(arg, " ");

        while(arg != NULL) {
            printf("%s", arg);
            arg = strtok(NULL, " ");

            if(arg != NULL) {
                printf(" ");
            }
        }
    }
    printf("\n");
}