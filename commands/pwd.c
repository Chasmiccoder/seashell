#include <stdio.h>
#include <string.h>

#include "../globals.h"


void run_pwd(const char *args) {

    if(args != NULL) {
        shell_warning("'pwd' takes no arguments");
        return;
    }

    printf("%s\n", sv->cwd_path);
}
