#include <stdio.h>
#include <string.h>

#include "../globals.h"

void run_echo(char *args) {
    if(args == NULL) {
        printf("\n");
        return;
    }

    char output[MAX_COMMAND_LEN];

    int i = 0; // args index
    int j = 0; // output index

    int len_args = strlen(args);

    int doubleQuoteFlag = 0;
    int oneSpace = 0;

    // the string has already been trimmed from the left and the right
    while(i < len_args) {
        if(args[i] == '"') {
            doubleQuoteFlag = (doubleQuoteFlag == 1) ? 0 : 1; // toggle the flag
            oneSpace = 0;
            i++;
            continue;
        }

        if(args[i] != ' ') {
            output[j++] = args[i++];
            oneSpace = 0;
        } else {
            if(args[i] != '\t' && (oneSpace == 0 || doubleQuoteFlag)) {
                oneSpace++;
                output[j++] = args[i++];
            } else {
                i++;
            }
        }
    }

    output[j] = '\0';
    printf("%s\n", output);
    return;
}