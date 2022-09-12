#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// #include "globals.h"
#include "shell_manipulation.h"

#define COLOR_WHITE 0
#define COLOR_BLUE  1
#define COLOR_GREEN 2

long long max(long long num1, long long num2) {
    if(num1 > num2) {
        return num1;
    }
    return num2;
}

void clear_string(char *str) {
    memset(str, '\0', strlen(str) * sizeof(char));
}

int is_substring(const char *substring, const char *string) {
    if(strlen(substring) > strlen(string)) {
        return 0;
    }

    for(int i = 0; i < strlen(substring); i++) {
        if(string[i] != substring[i]) {
            return 0;
        }
    }

    return 1;
}

void trim_string(char *dest, const char *source) {
    int i = 0; // source index
    int j = 0; // dest index

    int len_source = strlen(source);
    while(i < len_source && source[i] == ' ') {
        i++;
    }

    int last_char = len_source - 1;

    for(int k = len_source - 1; k >= 0; k--) {
        if(source[k] != ' ' && source[k] != '\n') {  // [5]
            last_char = k;
            break;
        }
    }

    while(i <= last_char) {
        dest[j++] = source[i++];
    }

    dest[j] = '\0';
}

void format_string(char *dest, const char *source) {
    /*
    Removes unnecessary spaces in a string.
    [1] Trims spaces from the left to the first character
    [2] Trims spaces from the last character onwards
    [3] Trims extra spaces in between words/arguments
    [4] If double quotes are used, it doesn't trim the spaces in between
    [5] Also removes the newline char at the end (if present)
    [6] Removes tabs '\t' if present (not inside double quotes)

    E.g.
    ` hello  world   `    -> `hello world`
    `  hello " word   !"` -> `hello " word   !"`
    */
    
    clear_string(dest);

    int i = 0; // source index
    int j = 0; // dest index

    // [1]
    while(source[i] == ' ' || source[i] == '\t' || source[i] == '\n') {
        i++;
    }

    int last_char = strlen(source)-1;

    // [2]
    for(int k = strlen(source) - 1; k >= 0; k--) {
        if(source[k] != ' ' && source[k] != '\n' && source[k] != '\t') {  // [5]
            last_char = k;
            break;
        }
    }

    // [3]
    int doubleQuoteFlag = 0; // flag for the double quote immunity
    int oneSpace = 0;        // gets incremented if 1 space/tab is encountered

    while(i <= last_char) {
        if(source[i] == '"') {  // [4]
            doubleQuoteFlag = (doubleQuoteFlag == 1) ? 0 : 1; // toggle the flag
            oneSpace = 0;
        }

        if(source[i] != ' ' && source[i] != '\t') {
            dest[j++] = source[i++];
            oneSpace = 0;
        } else {
            if(source[i] != '\t' && (oneSpace == 0 || doubleQuoteFlag)) { // [6]
                oneSpace++;
                dest[j++] = source[i++];
            } else {
                i++;
            }
        }
    }
    dest[i] = '\0';
}

void add_color_to_string(char *string, int color_code) {
    char *buffer = malloc((strlen(string)+15) * sizeof(char));

    if(color_code == COLOR_WHITE) {
        sprintf(buffer, "\033[37m%s\033[m", string);
        clear_string(string);
        strcpy(string, buffer);
    } else if(color_code == COLOR_BLUE) {
        sprintf(buffer, "\033[34m%s\033[m", string);
        clear_string(string);
        strcpy(string, buffer);
    } else if(color_code == COLOR_GREEN) {
        sprintf(buffer, "\033[32m%s\033[m", string);
        clear_string(string);
        strcpy(string, buffer);
    }

    free(buffer);
}

void string_to_lower(char *string) {
    for(int i = 0; i < strlen(string); i++) {
        if(string[i] >= 'A' && string[i] <= 'Z') {
            string[i] = string[i] + 32;
        }
    }
}

void string_swap(char *str1, char *str2) {
    int len = max(strlen(str1) + 1, strlen(str2) + 1);
    char *buffer = malloc(len * sizeof(char));

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

int handle_system_call(int status, char *system_call) {
    if(strcmp(system_call, "chdir") == 0) {
        if(status != 0) {
            shell_warning("runtime error at chdir()\n");
            return -1;
        }
    }
    // success
    return 0;
}

void prepend_string(const char *prefix, char *string) {

    char *tmp = malloc((strlen(string) + strlen(prefix) + 1) * sizeof(char));

    printf("pre, str: %s %s\n", prefix, string);
    strcpy(tmp, prefix);
    strcat(tmp, string);
    clear_string(string);
    strcpy(string, tmp);
    printf("str str: %s\n", string);
    free(tmp);
}
