#include <string.h>

void clear_string(char *str) {
    memset(str, '\0', strlen(str) * sizeof(char));
}

int is_substring(char *substring, char *string) {
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
