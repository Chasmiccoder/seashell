#include <stdio.h>
#include <string.h>

#include "../globals.h"
#include "../shell_manipulation.h"

void run_pwd(const char *args, const struct ShellVariables *sv) {
    if(args != NULL) {
        shell_warning("'pwd' takes no arguments");
        return;
    }

    printf("%s\n", sv->cwd_path);
}
