#define MAX_USERNAME_LEN 32
#define MAX_HOSTNAME_LEN 256
#define MAX_SHELL_PROMPT_LEN 4096
#define MAX_PATH_LEN 2048
#define MAX_COMMAND_LEN 2048
#define MAX_COMMANDS_IN_ONE_LINE 50  // for when multiple commands are used with ';'
#define MAX_COMMANDS_IN_HISTORY 20
#define MAX_NUMBER_OF_ARGS 100
#define MAX_NUM_OF_BACKGROUND_PROCESSES 32
#define STATUS_SIZE 20

#define COLOR_WHITE 0
#define COLOR_BLUE  1
#define COLOR_GREEN 2

struct ShellVariables {
    char *username;
    char *hostname;
    char *cwd_path;      // current working directory path
    char *prev_wd_path;  // previous working directory path
    char *home_path;     // absolute path to the home directory (where this shell is stored)

    int loop_control;

    struct queue **command_buffer;

    int *background_process_ids;
    int *background_process_assigned_ids;
    char **background_process_names;
    char **background_process_status; // "Running" or "Stopped"

    int background_process_count; // total number of background processes run from the beginning (used to generate the id given by the shell (assigned id))
    int num_background_processes;
};

// extern int global_number_of_processes;
// extern int global_background_pids[MAX_NUM_OF_BACKGROUND_PROCESSES];
extern struct ShellVariables *sv;

// === SHELL MANIPULATION ===
void init_shell_variables();
void print_shell_prompt();
void shell_warning(const char *message);
void add_background_process(int pid, int assigned_id, char *name, char *status);
void remove_background_process(int pid);



// === UTILITIES ===
#define COLOR_WHITE 0
#define COLOR_BLUE  1
#define COLOR_GREEN 2

void clear_string(char *str);
int is_substring(const char *substring, const char *string);

void trim_string(char *dest, const char *source);

void format_string(char *dest, const char *source);
void add_color_to_string(char *string, int color_code);
void string_to_lower(char *string);
void string_swap(char *str1, char *str2);
void int_swap(int *num1, int *num2);
int handle_system_call(int status, char *system_call);
void prepend_string(const char *prefix, char *string);

void convert_shell_path_to_absolute_path(char *target, const char *path);
void print_bg_process_details();