

struct ShellVariables {
    char *username;
    char *hostname;
    char *cwd_path;      // current working directory path
    char *prev_wd_path;  // previous working directory path
    char *home_path;     // absolute path to the home directory (where this shell is stored)

    int loop_control;
    
    char **command_queue;  // stores command history as a circular list
    int command_queue_front;
    int command_queue_rear;
};

void init_shell_variables(struct ShellVariables *sv);
void print_shell_prompt(const struct ShellVariables *sv);
void shell_warning(const char *message);

